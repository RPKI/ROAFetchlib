/*
 * This file is part of libhistoryrpki
 *
 * Author: Samir Al-Sheikh (Freie Universitaet, Berlin)
 *         s.al-sheikh@fu-berlin.de
 *
 * MIT License
 *
 * Copyright (c) 2017 The Libhistoryrpki authors
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
#include <arpa/inet.h>

#include "constants.h"
#include "debug.h"
#include "live_validation.h"
#include "rpki_config.h"
#include "wandio.h"

rpki_cfg_t* cfg_create(char* projects, char* collectors, char* time_intervals, int unified, int mode) {

  // Create Config
  rpki_cfg_t *cfg = NULL;
  if((cfg = (rpki_cfg_t *) malloc(sizeof(rpki_cfg_t))) == NULL) {
    return NULL;
  } else {
    memset(cfg, 0, sizeof(rpki_cfg_t));
  }

  // Create RPKI Config URLs
  config_broker_t *broker = &cfg->cfg_broker;
  broker->init_roa_urls_count = MAX_BROKER_RESPONSE_ENT;
  broker->roa_urls = malloc(MAX_BROKER_RESPONSE_ENT * sizeof(char*));
  for(int i = 0; i < MAX_BROKER_RESPONSE_ENT; i++) { 
    broker->roa_urls[i] = malloc(UTILS_ROA_STR_NAME_LEN * sizeof(char));
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

  // Add unified and mode
  config_input_t *input = &cfg->cfg_input;
  input->unified = unified;
  input->mode = mode;

  // Add intervals
  input->time_intervals = time_intervals;
  char time_window[MAX_TIME_WINDOWS * MAX_INTERVAL_SIZE];
  strncpy(time_window, time_intervals, sizeof(time_window));
  int time_intervals_count = 0;
  char *time = strtok(time_window, ",;");
  while(time != NULL) {
    input->time_intervals_window[time_intervals_count++] = atoi(time);
    time = strtok(NULL, ",;");
  }
  input->time_intervals_count = time_intervals_count;

  // Add projects and collectors
  char pros[UTILS_ROA_STR_NAME_LEN] = "";
  char cols[UTILS_ROA_STR_NAME_LEN] = "";
  strcpy(pros, projects);
  char* broker_proj = input->broker_projects;
  strcpy(broker_proj, projects);
  strcpy(cols, collectors);
  char* broker_coll = input->broker_collectors;
  strcpy(broker_coll, collectors);
  char *proj_coll_arg = strtok(pros, ", ");
  int proj_coll_arg_count = 0;
  while(proj_coll_arg != NULL) {
    strcpy(input->projects[proj_coll_arg_count++],proj_coll_arg);
    proj_coll_arg = strtok(NULL, ", ");
  }
  proj_coll_arg_count = 0;
  proj_coll_arg = strtok(cols, ", ");
  while(proj_coll_arg != NULL) {
    strcpy(input->collectors[proj_coll_arg_count++],proj_coll_arg);
    proj_coll_arg = strtok(NULL, ", ");
  }
 
  return cfg;
}

void cfg_destroy(rpki_cfg_t *cfg) {

  if(cfg == NULL) {
    return;
  }

  // Destroy RPKI config URLs
  config_broker_t *broker = &cfg->cfg_broker;
  for(int i = 0; i < broker->init_roa_urls_count; i++) { 
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
    live_validation_close_connection(cfg->cfg_rtr.rtr_mgr_cfg);
  }

  free(cfg);
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
    next_ts = current_ts + ROA_INTERVAL;
    while(kh_get(broker_result, broker->broker_kh, next_ts) == kh_end(broker->broker_kh)) {
      next_ts += ROA_INTERVAL;
    }
  } else {
    next_ts = 0;
  }
  return next_ts;
}

int cfg_parse_urls(rpki_cfg_t* cfg, char* url) {
  char *end_roa_arg;
  char *urls = strdup(url);
  char *roa_arg = strtok_r(urls, ",", &end_roa_arg);

  config_rtr_t *rtr = &cfg->cfg_rtr;
  config_input_t *input = &cfg->cfg_input;

  // Split the URL string in chunks and import the matching ROA file
  while(roa_arg != NULL) {
    if (strlen(roa_arg) > 0) {
        if(!strstr(roa_arg, input->collectors[rtr->pfxt_count])) {
          debug_err_print("%s", "The order of the URLs is wrong\n");
          exit(-1);
        }
        if(!input->unified) {
          cfg_import_roa_file(roa_arg, &rtr->pfxt[rtr->pfxt_count]);
        } else {
          cfg_import_roa_file(roa_arg, &rtr->pfxt[0]); 
        } 
        rtr->pfxt_count++;  
    }
   	roa_arg = strtok_r(NULL, ",", &end_roa_arg);
  }
  free(urls);
  return 0;
}
int cfg_import_roa_file(char* roa_path, struct pfx_table * pfxt){

  uint32_t asn = 0;
  uint8_t max_len = 0;
  char* roa_file = NULL;
  char* ip_prefix = "";
  char* trustanchor = "";
  int length = 0, ret = 0, roa_fields_cnt = 0, line_cnt = 0;
  char *buf = (char *) malloc (RPKI_BROKER_URL_BUFLEN * sizeof(char));

  // Read ROA file to buffer
  io_t *file_io = wandio_create(roa_path);
  while(1) {
    ret = wandio_read(file_io, buf, RPKI_BROKER_URL_BUFLEN);
    if (ret < 0) {
      debug_err_print("%s", "ERROR: Reading ROA-File from broker failed\n");
      exit(-1);
    }
    if (!ret) {break;}
    if (!(roa_file = realloc(roa_file, length + ret + 2))) {
      debug_err_print("%s", "ERROR: Could not realloc roa string\n");
      exit(-1);
    }
    strncpy(roa_file + length, buf, ret);
    length += ret;
  }
  roa_file[length] = '\0';
  free(buf);
  wandio_destroy(file_io);

  // Parse the ROA file and add every record to the prefix table
  for (size_t i = 0; i < strcspn(roa_file, "\n"); roa_fields_cnt += roa_file[i++] == ',' ? 1 : 0);
  roa_fields_cnt++;
  char *arg_end = NULL;
  char *arg = strtok_r(roa_file, ",\n", &arg_end);
  while (arg != NULL) {
    switch(line_cnt) {
      case 0: asn = strcmp(arg,"ASN")!=0?(strstr(arg,"AS")?atoi(arg+strlen("AS")):atoi(arg)):0;break;
      case 1: ip_prefix = strcmp(arg, "IP Prefix") != 0 ? arg : NULL; break;
      case 2: max_len = strcmp(arg, "Max Length") != 0 ? atoi(arg) : 0; break;
      case 3: trustanchor = strcmp(arg, "Trust Anchor") != 0 ? arg : NULL; break;
    }
    if (line_cnt == roa_fields_cnt - 1) {
      line_cnt = 0;
      if(ip_prefix && trustanchor) {
        cfg_add_record_to_pfx_table(max_len, asn, ip_prefix, trustanchor, pfxt);
      }
    } else {
      line_cnt++;
    }     
    arg = strtok_r(NULL, ",\n", &arg_end);
  }

  free(roa_file);
  return 0;
}

void cfg_print_record(const struct pfx_record *pfx_record, void *data) {
  char ip_pfx[INET6_ADDRSTRLEN];
  lrtr_ip_addr_to_str(&(*pfx_record).prefix, ip_pfx, sizeof(ip_pfx));
  debug_print("%"PRIu32",%s/%"PRIu8",%"PRIu8"\n",(*pfx_record).asn, ip_pfx, (*pfx_record).min_len, (*pfx_record).max_len);
}

int cfg_add_record_to_pfx_table (uint8_t max_len, uint32_t asn, char *ip_prefix,
                                 char *trustanchor, struct pfx_table * pfxt) {
  struct pfx_record pfx;
  char prefix[80] = "";
  char * token = strchr(ip_prefix, '/');
  if((int)(token - ip_prefix) > sizeof(prefix)) {
      fprintf(stderr, "Error: The prefix could not added to the prefix table\n"); 
      exit(-1);
  }
  strncpy(prefix, ip_prefix, (int)(token - ip_prefix));
  lrtr_ip_str_to_addr(prefix, &pfx.prefix);
  pfx.min_len = atoi(token + 1);
	pfx.max_len = max_len;
  pfx.asn = asn;
  pfx.socket = NULL;
  pfx_table_add(pfxt, &pfx);
  return 0;
}
