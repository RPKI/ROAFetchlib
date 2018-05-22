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
 * The above copyright notice and this permission notice shall be included in 
 * all copies or substantial portions of the Software.
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

#include "utils_cfg.h"
#include "debug.h"

int utils_cfg_check_collectors(rpki_cfg_t *cfg, char *projects_collectors,
                               int mode)
{
  config_input_t *input = &cfg->cfg_input;
  size_t input_max_size = sizeof(input->broker_collectors);

  /* Check whether collectors are passed, otherwise use the default collector */
  if(!strlen(projects_collectors) || !projects_collectors) {
    char default_cc[MAX_INPUT_LENGTH] = {0};
    if(mode) {
      snprintf(default_cc, sizeof(default_cc), "%s:%s",
               VALIDATION_DEFAULT_HISTORY_PJ, VALIDATION_DEFAULT_HISTORY_CC);
    } else {
      snprintf(default_cc, sizeof(default_cc), "%s:%s",
               VALIDATION_DEFAULT_LIVE_PJ, VALIDATION_DEFAULT_LIVE_CC);
    }
   projects_collectors = default_cc;
  }

  /* Add the projects/collectors to the configuration struct */
  int rst = utils_cfg_add_collectors(projects_collectors, input_max_size,
               MAX_INPUT_LENGTH, MAX_RPKI_COUNT, ";:", input->broker_collectors,
               input->collectors, input->projects, cfg);

  return rst;
}

int utils_cfg_add_collectors(char*input, size_t input_max_size, 
                            size_t item_max_size, int item_max_count, char* del,
                            char* cfg_str_concat, char (*cc)[MAX_INPUT_LENGTH],
                            char (*proj)[MAX_INPUT_LENGTH], rpki_cfg_t *cfg)
{
  /* Note: cc and proj are 2D char arrays of size: [X][MAX_INPUT_LENGTH] */
  int count = 0;
  char input_cpy[input_max_size];
  memset(input_cpy, 0, input_max_size);
  strncpy(input_cpy, input, input_max_size);

  /* Check whether the input is valid */
  if(utils_cfg_check_input(input, input_max_size, del, item_max_count,
                           input_cpy) != 0){
    return -1;
  }
  
  /* Iterate over projects/collectors */
  char *arg = strtok(input_cpy, del);
  int cc_cnt = 0, proj_cnt = 0;
  while(arg != NULL) {
    if (strlen(arg) >= item_max_size) {
      std_print("%s", "Error: Input element length invalid\n");
      return -1;
    }
    if (count % 2 == 0) {
      snprintf(proj[proj_cnt++], item_max_size, "%s", arg);
    } else {
      snprintf(cc[cc_cnt++], item_max_size, "%s", arg);
    }
    count++;
    arg = strtok(NULL, del);
  }

  /* Check whether the projects match the collectors */
  if (proj_cnt != cc_cnt) {
    std_print("%s\n", "Error: Number of collectors and projects doesn't match");
    return -1;
  } else {
    cfg->cfg_input.projects_count = proj_cnt;
    cfg->cfg_input.collectors_count = cc_cnt;
  }

  /* Concatenate all projects and collectors and store it */
  size_t concat_size = item_max_size * item_max_count;
  memset(cfg_str_concat, 0, concat_size);
  for (int i = 0; i < count; i++) {
    if(!(i % 2)) {
      snprintf(cfg_str_concat + strlen(cfg_str_concat), concat_size - 
           strlen(cfg_str_concat), i < count - 1 ? "%s:" : "%s", proj[i/2]);
    } else {
      snprintf(cfg_str_concat + strlen(cfg_str_concat), concat_size - 
           strlen(cfg_str_concat), i < count - 1 ? "%s;" : "%s", cc[i/2]);
    }
  }

  return 0;
}

char *utils_cfg_trim_whitespace(char *delimiter)
{
  /* Trims a delimiter to cut off the whitespaces
     Note: Modifies the delimiter string */
  char *end;
  while(isspace((unsigned char)*delimiter)) { delimiter++; }
  if(!(*delimiter)) { return delimiter; }
  end = delimiter + strlen(delimiter) - 1;
  while(end > delimiter && isspace((unsigned char)*end)) { end--; }
  *(end + 1) = 0;

  return delimiter;
}

int utils_cfg_check_intervals(rpki_cfg_t *cfg, char* time_intervals)
{
  config_input_t *input = &cfg->cfg_input;
  if (time_intervals != NULL) {
    size_t input_time_size = sizeof(input->broker_intervals);
    input->intervals_count = utils_cfg_add_intervals(time_intervals,
                                          input_time_size, ",-", MAX_RPKI_COUNT,
                                          MAX_INTERVAL_SIZE, input->intervals,
                                          input->broker_intervals);

  } else if (input->mode) {
    std_print("%s\n", "Error: Historical mode needs a valid interval");
    return -1;
  }
  if (input->intervals_count == -1) {
    return -1;
  }
  return 0;
}

int utils_cfg_add_intervals(char *input, size_t input_max_size, char *del,
                            size_t item_max_count, size_t item_max_size,
                            uint32_t *cfg_num, char* cfg_str_concat)
{
  int count = 0;
  char input_cpy[input_max_size];
  memset(input_cpy, 0, input_max_size);
  strncpy(input_cpy, input, input_max_size);

  /* Check whether the input is valid */
  if(utils_cfg_check_input(input, input_max_size, del, item_max_count,
                           input_cpy) != 0) {
    return -1;
  }
  
  /* Iterate over the intervals and parse it */
  char *arg = strtok(input_cpy, del);
  while(arg != NULL) {
    if(utils_cfg_validity_check_val(arg, &cfg_num[count++], 32) != 0) {
      std_print("%s", "Error: Min length of prefix is invalid\n");
      return -1;
    }
    arg = strtok(NULL, del);
  }

  /* Concatenate all intervals and store it */
  size_t concat_size = item_max_size * item_max_count;
  memset(cfg_str_concat, 0, concat_size);
  for (int i = 0; i < count; i++) {
    if(!(i % 2)) {
      snprintf(cfg_str_concat + strlen(cfg_str_concat), concat_size - 
           strlen(cfg_str_concat), "%"PRIu32"-", cfg_num[i]);
    } else {
      if(i == count - 1) {
        snprintf(cfg_str_concat + strlen(cfg_str_concat), concat_size - 
             strlen(cfg_str_concat),  "%"PRIu32, cfg_num[i]);
      } else {
        snprintf(cfg_str_concat + strlen(cfg_str_concat), concat_size - 
             strlen(cfg_str_concat),  "%"PRIu32",", cfg_num[i]);
      }
    }
  }

  return count;
}

int utils_cfg_check_input(char* input, size_t input_max_size, char* del,
                          size_t item_max_count, char* input_cpy)
{
  int count = 0, del_size = 3;

  /* Check if the input lengths is valid */
  if(input == NULL || !strlen(input) || strlen(input) > input_max_size) {
    std_print("%s", "Error: Input length exceeds limits\n");
    return -1;
  }

  /* Check if the delimiter length is valid and trim the spaces */
  if(strlen(del) > del_size - 1) {
    std_print("%s", "Error: Delimiter length exceeds limits\n");
    return -1;
  }
  char del_cpy[del_size];
  snprintf(del_cpy, sizeof(del_cpy), "%s", del);
  if(strstr(del_cpy, " ")) { utils_cfg_trim_whitespace(del_cpy); }

  /* Check if input items exceeds the limits */
  char *ptr = input;
  while((ptr = strpbrk(ptr, del_cpy)) != NULL) { count++; ptr++; }
  count++;
  if(count > item_max_count) {
    std_print("%s", "Error: Number of input elements invalid\n");
    return -1;
  }

  return 0;
}

int utils_cfg_check_flags(rpki_cfg_t *cfg, int unified, int mode)
{
  /* Check whether the parameter flags are valid, if so add it */
  if (unified != 0 && unified != 1) {
    std_print("%s", "Error: Unified parameter does not match specifications\n");
    return -1;    
  }
  if (mode != 0 && mode != 1) {
    std_print("%s", "Error: Mode parameter does not match specifications\n");
    return -1;    
  }
  cfg->cfg_input.unified = unified;
  cfg->cfg_input.mode = mode;

  return 0;
}

int utils_cfg_check_ssh_options(rpki_cfg_t *cfg, char* ssh_options)
{
  if (ssh_options == NULL) {
    return 0;
  }

  /* Check whether the SSH parameter list exceed the maximum length */
  char* tmp = strdup(ssh_options);
  if (strlen(tmp) > MAX_SSH_LEN) {
    std_print("%s", "Error: SSH parameter exceed maximum length\n");
    free(tmp);
    return -1;
  }

  /* Check whether the SSH parameter list contain three parameter and add it */
  int count_ssh_options = 0;
  while((tmp = strstr(tmp, ",")) != NULL) { count_ssh_options++; tmp++; }
  count_ssh_options++;
  if(count_ssh_options != MAX_SSH_CNT) {
    std_print("%s", "Error: SSH parameter do not match the parameter count\n");
    free(tmp);
    return -1;
  }
  config_input_t *input = &cfg->cfg_input;
  snprintf(input->ssh_options, sizeof(input->ssh_options), "%s", ssh_options);
  free(tmp);

  return 0;
}

int utils_cfg_set_broker_urls(rpki_cfg_t *cfg, char* broker_url)
{
  /* If a custom broker URL is set, use it otherwise use the default broker */
  config_broker_t *broker = &cfg->cfg_broker;
  if (broker_url != NULL) {
    snprintf(broker->broker_url, sizeof(broker->broker_url), "%s/broker?",
             broker_url);
    snprintf(broker->info_url, sizeof(broker->info_url), "%s/info?",
             broker_url);
  } else {
    snprintf(broker->broker_url, sizeof(broker->broker_url), "%s",
             BROKER_HISTORY_VALIDATION_URL);
    snprintf(broker->info_url, sizeof(broker->info_url), "%s",
             BROKER_LIVE_VALIDATION_INFO_URL);
  }

  return 0;
}

int utils_cfg_validity_check_val(char* val, void* rst_val, int unsigned_len)
{
  /* Check whether the value is well-formed */
  for (int i = 0; i < strlen(val); i++) {
    if(!isdigit(val[i])) {
      std_print("%s", "Error: value is malformed\n");
      return -1;
    }
  }

  /* Check whether the value is valid */
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

int utils_cfg_validity_check_prefix(char* prefix, char* address,
                                    uint8_t* min_len)
{
  /* Check whether the prefix is formatted validly */
  char ip_address[INET6_ADDRSTRLEN] = {0};
  char *token = strchr(prefix, '/');
  if(token == NULL || (int)(token - prefix) > sizeof(ip_address)) {
    std_print("%s", "Error: Prefix invalid format\n");   
    return -1;
  }
  strncpy(ip_address, prefix, (int)(token - prefix));

  /* Check whether the IP address is valid */ 
  if(inet_pton(strchr(ip_address, ':') == NULL ? AF_INET : AF_INET6,
                                     ip_address, address) != 1) {
    std_print("%s", "Error: IP address of prefix is invalid\n");
    return -1;
  }
  strncpy(address, ip_address, INET6_ADDRSTRLEN);

  /* Check whether the minimal length of the prefix is well-formed
     null-terminated IPv6 + "/128" */
  size_t ipv6_prefix_len = INET6_ADDRSTRLEN + 4;
  char prefix_dup[ipv6_prefix_len];
  snprintf(prefix_dup, sizeof(prefix_dup), "%s", prefix);
  char *minlen = strtok(prefix_dup, "/"); minlen = strtok(NULL, "/");
  if(utils_cfg_validity_check_val(minlen, min_len, 8) != 0) {
    std_print("%s", "Error: Min length of prefix is invalid\n");
    return -1;
  } else if(*min_len > (strchr(ip_address, ':') == NULL ? 32 : 128)) {
    std_print("%s", "Error: Min length of prefix is invalid\n");
    return -1;
  }

  return 0;
}

void utils_cfg_print_record(const struct pfx_record *pfx_record, void *data)
{
  /* Print a single ROA dump record */
  char ip_pfx[INET6_ADDRSTRLEN];
  lrtr_ip_addr_to_str(&(*pfx_record).prefix, ip_pfx, sizeof(ip_pfx));
  debug_print("%"PRIu32",%s/%"PRIu8",%"PRIu8"\n", (*pfx_record).asn, ip_pfx, 
              (*pfx_record).min_len, (*pfx_record).max_len);
}
