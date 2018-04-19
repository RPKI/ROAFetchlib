/*
 * This file is part of ROAFetchlib
 *
 * Author: Samir Al-Sheikh (Freie Universitaet, Berlin)
 *         s.al-sheikh@fu-berlin.de
 *
 * MIT License
 *
 * Copyright (c) 2017 The ROAFetchlib authors
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include <stdio.h> 
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>
#include <arpa/inet.h>

#include "constants.h"
#include "debug.h"
#include "live_validation.h"
#include "rpki_config.h"
#include "wandio.h"
#include "utils.h"

rpki_cfg_t* cfg_create(char* projects, char* collectors, char* time_intervals,
                       int unified, int mode, char* broker_url, char* ssh_options) {

  // Create Config
  rpki_cfg_t *cfg = NULL;
  if((cfg = (rpki_cfg_t *) malloc(sizeof(rpki_cfg_t))) == NULL) {
    return NULL;
  } else {
    memset(cfg, 0, sizeof(rpki_cfg_t));
  }

  // Create RPKI Config URLs
  config_broker_t *broker = &cfg->cfg_broker;
  if(broker_url != NULL) {
    snprintf(broker->broker_url, sizeof(broker->broker_url), "%s", broker_url);
  } else {
    snprintf(broker->broker_url, sizeof(broker->broker_url), "%s", BROKER_HISTORY_VALIDATION_URL); 
  }
  snprintf(broker->info_url, sizeof(broker->info_url), "%s", BROKER_LIVE_VALIDATION_INFO_URL);
  broker->roa_urls_count = BROKER_ROA_URLS_COUNT;
  broker->roa_urls = malloc(BROKER_ROA_URLS_COUNT * sizeof(char*));
  for(int i = 0; i < BROKER_ROA_URLS_COUNT; i++) { 
    broker->roa_urls[i] = malloc(BROKER_ROA_URLS_LEN * sizeof(char));
  }

  // Create Prefix Tables
  config_rtr_t *rtr = &cfg->cfg_rtr;
  rtr->pfxt = NULL;
  rtr->pfxt = malloc (MAX_RPKI_COUNT * sizeof(struct pfx_table));
  for (int i = 0; i < MAX_RPKI_COUNT; i++) {
    pfx_table_init(&rtr->pfxt[i], NULL);
  }
  rtr->pfxt_count = 0;
  rtr->rtr_mgr_cfg = NULL;
  broker->broker_khash_init = 0;
  for (int i = 0; i < sizeof(rtr->rtr_allocs)/sizeof(rtr->rtr_allocs); i++) {
    rtr->rtr_allocs[i] = 0;
  }

  // Add unified, mode and ssh options
  config_input_t *input = &cfg->cfg_input;
  if(ssh_options != NULL) {
    snprintf(input->ssh_options, sizeof(input->ssh_options), "%s", ssh_options);
  }
  input->unified = unified;
  input->mode = mode;

  /* Add intervals */
  if(time_intervals != NULL) {
    size_t input_time_size = sizeof(input->broker_intervals);
    input->intervals_count = utils_cfg_add_input(time_intervals, 
                         input_time_size, MAX_INTERVAL_SIZE, MAX_RPKI_COUNT,
                         ",-", input->broker_intervals, NULL, input->intervals);

  } else if(input->mode) {
      std_print("%s\n", "Error: Historical mode needs a valid interval");
      cfg_destroy(cfg);
      return NULL;
  }

  if(input->intervals_count == -1) {
    cfg_destroy(cfg);
    return NULL;
  }

  /* Add projects and collectors */
  size_t input_max_size = sizeof(input->broker_projects);
  input->projects_count = utils_cfg_add_input(projects, input_max_size, 
                                 MAX_INPUT_LENGTH, MAX_RPKI_COUNT, ", ",
                                 input->broker_projects, input->projects, NULL);

  input->collectors_count = utils_cfg_add_input(collectors, input_max_size,
                             MAX_INPUT_LENGTH, MAX_RPKI_COUNT, ", ",
                             input->broker_collectors, input->collectors, NULL);

  if(input->projects_count == -1 || input->collectors_count == -1) {
    cfg_destroy(cfg);
    return NULL;
  }

  if(input->projects_count != input->collectors_count) {
    std_print("%s\n", "Error: Number of collectors and projects doesn't match");
    cfg_destroy(cfg);
    return NULL;
  }

  return cfg;
}

int cfg_destroy(rpki_cfg_t *cfg) {

  if(cfg == NULL) {
    return -1;
  }

  // Destroy RPKI config URLs
  config_broker_t *broker = &cfg->cfg_broker;
  for(int i = 0; i < broker->roa_urls_count; i++) { 
    free(broker->roa_urls[i]);
  }  
  free(broker->roa_urls);

  // Destroy prefix tables
  for (int i = 0; i < MAX_RPKI_COUNT; i++) {
    pfx_table_free(&cfg->cfg_rtr.pfxt[i]);
  }
  pfx_table_free(cfg->cfg_rtr.pfxt);
  free(cfg->cfg_rtr.pfxt);

  // Destroy KHASH
  kh_destroy(broker_result, broker->broker_kh);
  broker->broker_kh = NULL;

  // Destroy the live connection to the RTR socket
  if(!cfg->cfg_input.mode){
    live_validation_close_connection(cfg);
  }

  free(cfg);
  return 0;
}

int cfg_get_timestamps(rpki_cfg_t *cfg, uint32_t timestamp, char* dest){
  
  // Get the current timestamp by downgrading the timestamp minute-wise
  config_broker_t *broker = &cfg->cfg_broker;
  uint32_t current_ts = timestamp - (timestamp % 60);
  while(kh_get(broker_result, broker->broker_kh, current_ts) == kh_end(broker->broker_kh)) {
    current_ts -= 60;
  }
  strcpy(dest, kh_value(broker->broker_kh, kh_get(broker_result, broker->broker_kh, current_ts)));
  config_time_t *cfg_time = &cfg->cfg_time;
  cfg_time->current_roa_timestamp = current_ts;

  // If there are more than one entries in the hash table
  if(kh_size(broker->broker_kh) > 1) {
    cfg_time->next_roa_timestamp = cfg_next_timestamp(cfg, current_ts);
  } else {
    cfg_time->next_roa_timestamp = 0;
  }
  return 0;
}

uint32_t cfg_next_timestamp(rpki_cfg_t* cfg, uint32_t current_ts) {

  // If there is only one ROA file left in the hash table -> next timestamp is set to 0
  uint32_t next_ts;
  config_broker_t *broker = &cfg->cfg_broker;
  if((broker->broker_khash_count - broker->broker_khash_used) > 1) {
    next_ts = current_ts + ROA_ARCHIVE_INTERVAL;
    while(kh_get(broker_result, broker->broker_kh, next_ts) == kh_end(broker->broker_kh)) {
      next_ts += ROA_ARCHIVE_INTERVAL;
    }
  } else {
    next_ts = 0;
  }
  return next_ts;
}

int cfg_parse_urls(rpki_cfg_t* cfg, char* url) {

  // Clean all pfxt and flags before parsing new urls
  config_rtr_t *rtr = &cfg->cfg_rtr;
  memset(rtr->pfxt_active, 0, sizeof(rtr->pfxt_active));
  for (int i = 0; i < MAX_RPKI_COUNT; i++) {
    pfx_table_src_remove(&rtr->pfxt[i], NULL);
  }
  rtr->pfxt_count = 0;

  // Split the URL string in chunks and import the matching ROA file
  char *end_roa_arg;
  char *urls = strdup(url);
  config_input_t *input = &cfg->cfg_input;
  char *roa_arg = strtok_r(urls, ",", &end_roa_arg);
  while(roa_arg != NULL) {
    if (strlen(roa_arg) > 1) {
        if(!strstr(roa_arg, input->collectors[rtr->pfxt_count])) {
          std_print("%s", "The order of the URLs is wrong\n");
          std_print("%s %s\n", roa_arg, input->collectors[rtr->pfxt_count]);
          return -1;
        }
        if(!input->unified) {
          if(cfg_import_roa_file(roa_arg, &rtr->pfxt[rtr->pfxt_count]) != 0) {
            return -1;
          }
        } else {
          if(cfg_import_roa_file(roa_arg, &rtr->pfxt[0]) != 0) {
            return -1;
          }
        } 
        rtr->pfxt_active[rtr->pfxt_count] = 1;
        rtr->pfxt_count++;
    } else {
      rtr->pfxt_active[rtr->pfxt_count] = 0;
      rtr->pfxt_count++;
    }
     roa_arg = strtok_r(NULL, ",", &end_roa_arg);
  }
  free(urls);
  return 0;
}
int cfg_import_roa_file(char* roa_path, struct pfx_table * pfxt){

  // Read the whole ROA dump in and store it in `roa_file`
  char* roa_file = NULL;
  int length = 0, ret = 0;
  char *buf = (char *) malloc (BROKER_ROA_DUMP_BUFLEN * sizeof(char));
  io_t *file_io = wandio_create(roa_path);
  if(file_io == NULL) {
    std_print("ERROR: Could not open %s for reading\n", roa_path);
    return -1;   
  }
  while(1) {
    ret = wandio_read(file_io, buf, BROKER_ROA_DUMP_BUFLEN);
    if (ret < 0) {
      std_print("%s", "ERROR: Reading ROA file from broker failed\n");
      return -1;
    }
    if (!ret) { break; }
    if (!(roa_file = realloc(roa_file, length + ret + 2))) {
      std_print("%s", "ERROR: Could not realloc roa string\n");
      return -1;
    }
    strncpy(roa_file + length, buf, ret);
    length += ret;
  }
  roa_file[length] = '\0';
  free(buf);
  wandio_destroy(file_io);

  // Check whether the ROA dump format contains the trustanchor information
  int roa_fields_cnt = 0, line_cnt = 0, dbg_line = 0;
  for (size_t i = 0; i < strcspn(roa_file, "\n"); roa_fields_cnt += roa_file[i++] == ',' ? 1 : 0);
  roa_fields_cnt++;

  // Skip the header
  char *arg_end = NULL;
  char *arg = strtok_r(roa_file, ",\n", &arg_end);
  for (size_t i = 0; i < roa_fields_cnt; i++) {
    arg = strtok_r(NULL, ",\n", &arg_end);
  }

  // Parse the ROA file and add every record to the prefix table
  // ASN,IP Prefix, Max Length (, Trustanchor)?
  uint32_t asn = 0; uint8_t min_len = 0, max_len = 0;
  char address[INET6_ADDRSTRLEN] = {0};
  while (arg != NULL) {
    switch(line_cnt) {
      case 0:
        if(strstr(arg, "AS")) {
          if(utils_cfg_validity_check_val(arg + strlen("AS"), &asn, 32) != 0) {
            return -1;
          }
        } else if(utils_cfg_validity_check_val(arg, &asn, 32) != 0) {
            return -1;
        } break;
      case 1:
        if(utils_cfg_validity_check_prefix(arg, address, &min_len) != 0) {
          return -1;
        } break;
      case 2:
        if(utils_cfg_validity_check_val(arg, &max_len, 8) != 0) {
          return -1;
        } break;
      case 3: break;
    }

    // Add Record if all fields were extracted
    if (line_cnt == roa_fields_cnt - 1) {
      line_cnt = 0; dbg_line++;
      if(cfg_add_record_to_pfx_table(asn, address, min_len, max_len, pfxt) != 0) {
        std_print("Error: Record is corrupt at line: %i\n", dbg_line/roa_fields_cnt);
        return -1;
      }
    } else {
      line_cnt++; dbg_line++;
    }     
    arg = strtok_r(NULL, ",\n", &arg_end);
  }

  debug_print("Imported ROA dump: %s\n", roa_path);
  free(roa_file);
  return 0;
}

int cfg_add_record_to_pfx_table(uint32_t asn, char *address,  uint8_t min_len,
                                uint8_t max_len, struct pfx_table * pfxt) {

  struct pfx_record pfx;

  // Check if the IP address could be interpreted by the RTRlib 
  if(lrtr_ip_str_to_addr(address, &pfx.prefix) != 0) {
    std_print("%s", "Error: Address not interpretable\n");   
    return -1;
  }
  pfx.min_len = min_len;
  pfx.max_len = max_len;
  pfx.asn = asn;
  pfx.socket = NULL;

  // Check the record could be added by the RTRlib
  if(pfx_table_add(pfxt, &pfx) == PFX_ERROR) {
    std_print("%s", "Error: Record could not be added\n"); 
    return -1;
  }
  return 0;
}
