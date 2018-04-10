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

#include <stddef.h>
#include <stdlib.h>

#include "broker.h"
#include "debug.h"
#include "rpki_config.h"
#include "constants.h"
#include "jsmn/jsmn.h"
#include "wandio.h"

int broker_connect(rpki_cfg_t* cfg, char* project, char* collector, char* time_intervals){

  // Build broker request URL
	io_t *jsonfile;
	char broker_url[RPKI_BROKER_URL_LEN];
  snprintf(broker_url, sizeof(broker_url), "%sproject=%s&collector=%s&interval=%s",
           cfg->cfg_broker.broker_url, project, collector, time_intervals);

  // Get the broker reponse and check for errors
	io_t *json_chk_err = wandio_create(broker_url);
	char broker_err_check[80] = "";
	if (json_chk_err == NULL) {
	  std_print("ERROR: Could not open %s for reading\n", broker_url);
	  wandio_destroy(json_chk_err);
    return -1;
	}
	wandio_read(json_chk_err, broker_err_check, sizeof(broker_err_check));
	if(!strncmp(broker_err_check, "Error:", strlen("Error:")) || 
     !strncmp(broker_err_check, "Malformed", strlen("Malformed"))) {
    broker_err_check[strlen(broker_err_check)] = '\0';
	  std_print("%s\n", broker_err_check);
	  wandio_destroy(json_chk_err);
    return -1;
	}
	wandio_destroy(json_chk_err);
  return broker_json_buf(cfg, broker_url);
}

int broker_json_buf(rpki_cfg_t* cfg, char *broker_url){

  io_t *file_io = wandio_create(broker_url);
  int length = 0, ret = 0;
  char *json_file = NULL;
  char *buf = (char *) malloc (JSON_BUF_SIZE * sizeof(char));

  if(!buf) {
    std_print("%s", "Error: Could not allocate enough memory\n");
    return -1;
  } 

  // Read the JSON file into a corresponding buffer
  while(1) {
    ret = wandio_read(file_io, buf, JSON_BUF_SIZE);
    if (ret < 0) {
      std_print("%s", "ERROR: Could not read JSON file from broker\n");
      return -1;
    }
    if (!ret) {break;}
    if (!(json_file = realloc(json_file, length + ret + 2))) {
      std_print("%s", "ERROR: Could not realloc JSON string\n");
      return -1;
    }
    strncpy(json_file + length, buf, ret);
    length += ret;
  }
  json_file[length] = '\0';
  free(buf);
  wandio_destroy(file_io);
  ret = broker_parse_json(cfg, json_file);
  free(json_file);
  return ret;
}

int broker_parse_json(rpki_cfg_t* cfg, char *js){

  jsmn_parser parser;
  jsmn_init(&parser);

  // Parse the JSON file and realloc the tokens if necessary
  int tok_nr = 256;
  jsmntok_t *tokens = malloc(sizeof(jsmntok_t) * tok_nr);
  if(!tokens) {
    std_print("%s", "Error: Could not allocate enough memory\n");
    return -1;
  }
  int ret = jsmn_parse(&parser, js, strlen(js), tokens, tok_nr);
  while (ret == JSMN_ERROR_NOMEM)
  {
      tokens = realloc(tokens, sizeof(jsmntok_t) * (tok_nr *= 2));
      if(!tokens) {
        std_print("%s", "Error: Could not allocate enough memory\n");
        return -1;
      }
      ret = jsmn_parse(&parser, js, strlen(js), tokens, tok_nr);
  }
  if (ret == JSMN_ERROR_INVAL || ret == JSMN_ERROR_PART) {
    std_print("%s", "Error: invalid JSON format\n");
    return -1;
  }

  if (ret < 12) {
    std_print("%s", "Error: invalid response of the broker\n");
    return -1;
  }

  if (ret == 13) {
    std_print("%s", "Info: There are no ROA dumps for the interval\n");
  }

  // Broker Response Scheme:
  // 01 : Projects   - 02 Value
  // 03 : Collectors - 04 Value
  // 05 : Interval   - 06 Value
  // 07 : start      - 08 Value
  // 09 : max_end    - 10 Value
  // 11 : data       - 12 object
  // 13 : #1 TS      - 14 #1 URL <-- Start
  // 15 : #2 TS      - 16 #2 URL

  // Initialize the broker result khash table
  config_broker_t *broker = &cfg->cfg_broker;
  if(!broker->broker_khash_init) {
    broker->broker_kh = kh_init(broker_result);
    broker->broker_khash_init = 1;
  } else {
    kh_clear(broker_result, broker->broker_kh);
  }
  broker->broker_khash_count = 0;
  int kh_ret = RPKI_MAX_ROA_ENT;
  khiter_t k;

  // Realloc RPKI config URLs if necessary
  if(ret > MAX_BROKER_RESPONSE_ENT) {
    broker->roa_urls = realloc(broker->roa_urls, sizeof(char*) * ret);
    for(int i = 0; i < ret; i++) { 
      broker->roa_urls[i] = malloc(RPKI_BROKER_URL_LEN * sizeof(char));
    }
    broker->init_roa_urls_count = ret;
  }

  // Add projects and collectors in broker sorted order
  config_input_t *input = &cfg->cfg_input;
  jsmntok_t value = tokens[2];
  int length = value.end - value.start;
  char projects[length + 1];    
  memcpy(projects, &js[value.start], length);
  projects[length] = '\0';
  strcpy(input->broker_projects, projects);
  char (*proj)[MAX_INPUT_LENGTH] = input->projects;
  //add_input_to_cfg(projects, proj, ", ");
  add_input_to_cfg(projects, sizeof(input->broker_projects), MAX_INPUT_LENGTH,
                   MAX_RPKI_COUNT, proj, ", ");
  value = tokens[4];
  length = value.end - value.start;
  char collectors[length + 1];    
  memcpy(collectors, &js[value.start], length);
  collectors[length] = '\0';
  strcpy(input->broker_collectors, collectors);
  char (*coll)[MAX_INPUT_LENGTH] = input->collectors;
  //add_input_to_cfg(collectors, coll, ", ");
  add_input_to_cfg(collectors, sizeof(input->broker_collectors), MAX_INPUT_LENGTH,
                   MAX_RPKI_COUNT, coll, ", ");
  
  // Add first timestamp of broker response
  value = tokens[8];
  length = value.end - value.start;
  char timestamp[length + 1];    
  memcpy(timestamp, &js[value.start], length);
  timestamp[length] = '\0';
  cfg->cfg_time.start = atoi(timestamp);

  // Add latest timestamp of broker response
  value = tokens[10];
  length = value.end - value.start;   
  memcpy(timestamp, &js[value.start], length);
  timestamp[length] = '\0';
  cfg->cfg_time.max_end = atoi(timestamp);

  // Add Timestamp to Khash as key, URL to Khash as value and config_urls
  for (int i = 13; i < ret; i += 2) {
    jsmntok_t key = tokens[i];
    int length = key.end - key.start;
    char ts[length + 1];    
    memcpy(ts, &js[key.start], length);
    ts[length] = '\0';
    uint64_t timestamp = atoi(ts);
    k = kh_put(broker_result, broker->broker_kh, timestamp, &kh_ret);
    kh_val(broker->broker_kh, k) = '\0';

    jsmntok_t value = tokens[i+1];
    length = value.end - value.start;
    char url[length + 1];    
    memcpy(url, &js[value.start], length);
    url[length] = '\0';
    sprintf(broker->roa_urls[broker->broker_khash_count], "%s", url);
    kh_val(broker->broker_kh, k) = broker->roa_urls[broker->broker_khash_count];
    broker->broker_khash_count++;
  }

  broker_print_debug(cfg);
  free(tokens);
  return 0;
}

void broker_print_debug(rpki_cfg_t* cfg){
  uint64_t key1;
  char *val;
  debug_print("%s", "\n----------- Broker Khash -------------------\n");
  kh_foreach(cfg->cfg_broker.broker_kh, key1, val, 
  debug_print("Key: %"PRIu64", Value: %s\n", key1, val));
  debug_print("%s", "\n");
}
