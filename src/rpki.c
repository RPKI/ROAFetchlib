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
#include <time.h>

#include "lib/broker.h"
#include "lib/rpki_config.h"
#include "lib/debug.h"
#include "lib/elem.h"
#include "lib/live_validation.h"
#include "config.h"
#include "rpki.h"
#include "rtrlib/rtrlib.h"

rpki_cfg_t* rpki_set_config(char* projects, char* collectors, char* time_intervals, int unified, int mode){

  rpki_cfg_t *cfg;
  if((cfg = cfg_create(projects, collectors, time_intervals, unified, mode)) == NULL){
    debug_err_print("%s", "Error: Could not create RPKI config\n");
    exit(-1);
  }

  // Configuration of live mode
  if(!mode){
    debug_print("%s", "Info: Only the first collector will be taken for Live RPKI Validation\n");
    live_validation_set_config(cfg->cfg_input.collectors[0], cfg);
    return cfg;
  }

  config_input_t *input = &cfg->cfg_input;
  broker_connect(cfg, input->broker_projects, input->broker_collectors, input->time_intervals);
  print_config_debug(cfg);
  return cfg;
}

int rpki_validate(rpki_cfg_t* cfg, uint32_t timestamp, uint32_t asn, char* prefix, uint8_t mask_len,
                  char* result, size_t size){

  elem_t *elem;
  if((elem = elem_create()) == NULL){
    debug_err_print("%s", "Error: Could not allocate memory for RPKI elem\n");
    return -1;
  }

  // Validate with live mode
  config_rtr_t *rtr = &cfg->cfg_rtr;
  if(!cfg->cfg_input.mode && rtr->rtr_mgr_cfg != NULL) {
      elem_get_rpki_validation_result(cfg, rtr->rtr_mgr_cfg, elem, prefix, asn, mask_len, NULL, 0);
      elem_get_rpki_validation_result_snprintf(cfg, result, size, elem);
      elem_destroy(elem);
      return 0;
  }

  // Check whether the timestamp is in the time interval
  int check = 0;
  config_input_t *input = &cfg->cfg_input;
  for (int i = 0; i < input->time_intervals_count; i = i+2){
    if((timestamp >= input->time_intervals_window[i] && timestamp <= input->time_intervals_window[i+1]) ||
       (timestamp >= input->time_intervals_window[i] && input->time_intervals_window[i+1] == 0)){
        check = 1;
    }
  }
  if(!check) {
    debug_err_print("%s", "Error: The timestamp is not in the configuration time interval\n");
    return -1;
  }

  char url_buf[UTILS_ROA_STR_NAME_LEN];
  char* url = url_buf;

  // If the current timestamp is empty -> get it, parse URLs and import ROAs
  config_time_t *cfg_time = &cfg->cfg_time;
  if(!cfg_time->current_roa_timestamp && !cfg_time->next_roa_timestamp){
    if (cfg_get_timestamps(cfg, timestamp, url)){
      debug_err_print("%s", "Error: Could not find current and next timestamp");
      return -1;
    }
    cfg_parse_urls(cfg, url);
  }

  // Validate with hybrid mode
  config_broker_t *broker = &cfg->cfg_broker;
  if(input->mode && !cfg_time->max_end && timestamp > cfg_time->current_roa_timestamp + ROA_INTERVAL &&
     !cfg_time->next_roa_timestamp) {
      debug_print("%s", "Info: Entering hybrid mode\n");
      if(cfg_time->current_roa_timestamp < (uint32_t)time(NULL) - ROA_INTERVAL) {
          char time_inv[MAX_INTERVAL_SIZE];
          snprintf(time_inv, MAX_INTERVAL_SIZE, "%"PRIu32",%"PRIu32, timestamp, cfg_time->max_end);
          broker_connect(cfg, input->broker_projects, input->broker_collectors,
                         time_inv);
          broker->broker_khash_used = 0;
          rtr->pfxt_count = 0;
          cfg_get_timestamps(cfg, timestamp, url);
          cfg_parse_urls(cfg, url);
      } else {
        debug_print("%s", "Info: Entering live mode\n");
        input->mode = 0;
        live_validation_set_config(cfg->cfg_input.collectors[0], cfg);
        elem_get_rpki_validation_result(cfg, rtr->rtr_mgr_cfg, elem, prefix, asn, mask_len, NULL, 0);
        elem_get_rpki_validation_result_snprintf(cfg, result, size, elem);
        elem_destroy(elem);
        return 0;
      }
  }

  // If the timestamp is equal to the next timestamp, clean the pfxt and the next timestamp will be set
  // next_roa_timestamp =  0 ->  there is no next ROA file, next_roa_timestamp = -1 ->  live mode active
  if(timestamp >= cfg_time->next_roa_timestamp && cfg_time->next_roa_timestamp != 0){
    broker->broker_khash_used++;
    cfg_time->current_roa_timestamp = cfg_time->next_roa_timestamp;
    cfg_time->next_roa_timestamp = cfg_next_timestamp(cfg, cfg_time->current_roa_timestamp);
    strcpy(url, kh_value(broker->broker_kh, kh_get(broker_result, broker->broker_kh, cfg_time->current_roa_timestamp)));
    for (int i = 0; i < MAX_RPKI_COUNT; pfx_table_src_remove(&rtr->pfxt[i++], NULL)){};
    rtr->pfxt_count = 0;
    cfg_parse_urls(cfg, url);
  }

  // Validation the prefix, mask_len and ASN with Historical RPKI Validation
  for (int i = 0; i < rtr->pfxt_count; i++){
    elem_get_rpki_validation_result(cfg, NULL, elem, prefix, asn, mask_len, &rtr->pfxt[i], i);
  }

  // Validation output
  elem_get_rpki_validation_result_snprintf(cfg, result, size, elem);

  // Clear elem
  elem_destroy(elem);
  return 0;
}

void print_config_debug(rpki_cfg_t* cfg){
  debug_print("%s", "--- LibHistoryRPKI Input ---\n");
  debug_print("Projects:    %s, %s\n",      cfg->cfg_input.projects[0], cfg->cfg_input.projects[1]);
  debug_print("Collectors:  %s, %s\n",      cfg->cfg_input.collectors[0], cfg->cfg_input.collectors[1]);
  debug_print("Unified:     %i\n",          cfg->cfg_input.unified);
  debug_print("Mode:        %i\n",          cfg->cfg_input.mode);
  debug_print("Interval:    %s\n",          cfg->cfg_input.time_intervals);
  debug_print("%s", "-------- Hashtable ---------\n");
  debug_print("Khash Count:    %i\n",       cfg->cfg_broker.broker_khash_count);
  debug_print("Last Timestamp: %"PRIu32"\n",cfg->cfg_time.max_end);
  debug_print("%s", "------- BROKER ARRAY -------\n");
  for(int i = 0; i < cfg->cfg_broker.broker_khash_count; i++) {
  debug_print("url: %s\n", cfg->cfg_broker.roa_urls[i]);
  }
  debug_print("%s", "----------------------------\n");
}
