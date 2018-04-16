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
#include <stddef.h>
#include <ctype.h>
#include <errno.h>
#include <inttypes.h>
#include <arpa/inet.h>

#include "constants.h"
#include "debug.h"
#include "live_validation.h"
#include "rpki_config.h"
#include "wandio.h"

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

  // Add intervals
  if(time_intervals != NULL) {
    snprintf(input->time_intervals, sizeof(input->time_intervals),
             "%s", time_intervals);
    char time_window[MAX_TIME_WINDOWS * MAX_INTERVAL_SIZE];
    strncpy(time_window, time_intervals, sizeof(time_window));
    char *time = strtok(time_window, ",-");
    while(time != NULL) {
      cfg_validity_check_val(time,
        &input->time_intervals_window[input->time_intervals_count++], 32);
      time = strtok(NULL, ",-");
    }
  } else if(input->mode) {
      std_print("%s", "Error: For historical mode a valid interval is needed\n");
      cfg_destroy(cfg);
      return NULL;
  }

  // Add projects and collectors
  strcpy(input->broker_projects, projects);
  char (*proj)[MAX_INPUT_LENGTH] = input->projects;
  size_t input_max_size = sizeof(input->broker_collectors);
  input->collectors_count = add_input_to_cfg(projects, input_max_size,
                              MAX_INPUT_LENGTH, MAX_RPKI_COUNT, proj, ", ");
  strcpy(input->broker_collectors, collectors);
  char (*coll)[MAX_INPUT_LENGTH] = input->collectors;
  input->collectors_count = add_input_to_cfg(collectors, input_max_size,
                               MAX_INPUT_LENGTH, MAX_RPKI_COUNT, coll, ", ");
  
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
          if(cfg_validity_check_val(arg + strlen("AS"), &asn, 32) != 0) {
            return -1;
          }
        } else if(cfg_validity_check_val(arg, &asn, 32) != 0) {
            return -1;
        } break;
      case 1:
        if(cfg_validity_check_prefix(arg, address, &min_len) != 0) {
          return -1;
        } break;
      case 2:
        if(cfg_validity_check_val(arg, &max_len, 8) != 0) {
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

int cfg_validity_check_val(char* val, void *rst_val, int unsigned_len) {

  // Check whether the value is well-formed
  for (int i = 0; i < strlen(val); i++) {
    if(!isdigit(val[i])) {
      std_print("%s", "Error: value is malformed\n");
      return -1;
    }
  }

  // Check whether the value is valid
  int err = 0; errno = 0;
  switch(unsigned_len) {
    default: std_print("%s", "Error: invalid int type\n"); return -1; 
    case 8: {
      uint64_t rst_chk = strtoumax(val, NULL, 10);
      if(rst_chk > UINT8_MAX) { err = 1; errno = ERANGE; }
      uint8_t rst = strtoumax(val, NULL, 10);
      if (rst == UINT8_MAX && errno == ERANGE) { err = 1; }
      *((uint8_t*)rst_val) = rst;
    } break;
    case 32: {
      uint64_t rst_chk = strtoumax(val, NULL, 10);
      if(rst_chk > UINT32_MAX) { err = 1; errno = ERANGE; }
      uint32_t rst = strtoumax(val, NULL, 10);
      if (rst == UINT32_MAX && errno == ERANGE) { err = 1; }
      *((uint32_t*)rst_val) = rst;
    } break;
  }
  if(err != 0) {
    std_print("Error: %s\n", strerror(errno));
    errno = 0; return -1;
  }

  return 0;
}

int cfg_validity_check_prefix(char* prefix, char* address, uint8_t *min_len) {

  // Check whether the prefix is formatted validly
  char ip_address[INET6_ADDRSTRLEN] = {0};
  char *token = strchr(prefix, '/');
  if(token == NULL || (int)(token - prefix) > sizeof(ip_address)) {
    std_print("%s", "Error: Prefix invalid format\n");   
    return -1;
  }
  strncpy(ip_address, prefix, (int)(token - prefix));

  // Check whether the IP address is valid  
  if(inet_pton(strchr(ip_address, ':') == NULL ? AF_INET : AF_INET6,
                                     ip_address, address) != 1) {
    std_print("%s", "Error: IP address of prefix is invalid\n");
    return -1;
  }
  strncpy(address, ip_address, INET6_ADDRSTRLEN);

  // Check whether the minimal length of the prefix is well-formed
	// null-terminated IPv6 + "/128"
	size_t ipv6_prefix_len = INET6_ADDRSTRLEN + 4;
  char prefix_dup[ipv6_prefix_len];
  snprintf(prefix_dup, sizeof(prefix_dup), "%s", prefix);
  char *minlen = strtok(prefix_dup, "/"); minlen = strtok(NULL, "/");
  if(cfg_validity_check_val(minlen, min_len, 8) != 0) {
    std_print("%s", "Error: Min length of prefix is invalid\n");
    return -1;
  } else if(*min_len > (strchr(ip_address, ':') == NULL ? 32 : 128)) {
    std_print("%s", "Error: Min length of prefix is invalid\n");
    return -1;
  }

  return 0;
}

void cfg_print_record(const struct pfx_record *pfx_record, void *data) {
  char ip_pfx[INET6_ADDRSTRLEN];
  lrtr_ip_addr_to_str(&(*pfx_record).prefix, ip_pfx, sizeof(ip_pfx));
  debug_print("%"PRIu32",%s/%"PRIu8",%"PRIu8"\n",(*pfx_record).asn, ip_pfx, (*pfx_record).min_len, (*pfx_record).max_len);
}

int add_input_to_cfg(char* input, size_t input_max_size, size_t item_max_size,
          int item_max_count, char (*cfg_storage)[MAX_INPUT_LENGTH], char* del)
{
  int count = 0; char input_cpy[input_max_size];

  // Check if the input lengths is valid
  if(!strlen(input) || strlen(input) > input_max_size) {
    std_print("%s", "Error: Input length exceeds limits\n");
    return -1;
  }

  memset(input_cpy, 0, input_max_size);
  strncpy(input_cpy, input, input_max_size);

  // Check if the input exceeds the maximum
  for (size_t i = 0; i < strlen(input); count += input[i++] == ',' ? 1 : 0);
  count++;
  if(count > item_max_count) {
    std_print("%s", "Error: Number of input elements invalid\n");
    return -1;
  }

  // Extract every input element if it's length is valid
  count = 0;
  char *arg = strtok(input_cpy, del);
  while(arg != NULL) {
    if(strlen(arg) >= item_max_size) {
      std_print("%s", "Error: Input element length invalid\n");
      return -1;
    }
    snprintf(cfg_storage[count++], item_max_size, "%s", arg);
    arg = strtok(NULL, del);
  }

  return count;
}
