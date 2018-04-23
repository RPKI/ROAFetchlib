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

int utils_cfg_validity_check_val(char* val, void* rst_val, int unsigned_len) {

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
                                    uint8_t* min_len) {

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

void utils_cfg_print_record(const struct pfx_record *pfx_record, void *data) {
  char ip_pfx[INET6_ADDRSTRLEN];
  lrtr_ip_addr_to_str(&(*pfx_record).prefix, ip_pfx, sizeof(ip_pfx));
  debug_print("%"PRIu32",%s/%"PRIu8",%"PRIu8"\n", (*pfx_record).asn, ip_pfx, 
              (*pfx_record).min_len, (*pfx_record).max_len);
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

int utils_cfg_add_input(char*input, size_t input_max_size, size_t item_max_size,
                     int item_max_count, char* del, char* cfg_str_concat,
                     char (*cfg_str)[MAX_INPUT_LENGTH], uint32_t *cfg_num)
{
  /* Note: cfg_str is a 2D char array of size: cfg_str[X][MAX_INPUT_LENGTH] */
  int count = 0; int del_size = 3; int mode = -1;
  char input_cpy[input_max_size]; 

  /* Check if only one input type is passed */
  if(cfg_num != NULL && cfg_str == NULL) { mode = 1; }
  else if (cfg_num == NULL && cfg_str != NULL) { mode = 0; }
  else {
     std_print("%s", "Error: Only one input type\n");
     return -1;
   }

  /* Check if the input lengths is valid */
  if(!strlen(input) || strlen(input) > input_max_size) {
    std_print("%s", "Error: Input length exceeds limits\n");
    return -1;
  }

  memset(input_cpy, 0, input_max_size);
  strncpy(input_cpy, input, input_max_size);

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

  /* Extract every input element if it's length is valid */
  count = 0;
  char *arg = strtok(input_cpy, del);
  while(arg != NULL) {

    /* Iterate over the time-based input if it is not empty */
    if(mode) {
      int ret = utils_cfg_validity_check_val(arg, &cfg_num[count++], 32);
      if(ret != 0) {
        std_print("%s", "Error: Min length of prefix is invalid\n");
        return -1;
      }

    /* Iterate over string-based input if it is not empty */
    } else {
      if(strlen(arg) >= item_max_size) {
        std_print("%s", "Error: Input element length invalid\n");
        return -1;
      }
      snprintf(cfg_str[count++], item_max_size, "%s", arg);
    }
    arg = strtok(NULL, del);
  }

  /* Concatenate all inputs and store it */
  size_t concat_size = item_max_size * item_max_count;
  memset(cfg_str_concat, 0, concat_size);
  for (int i = 0; i < count; i++) {
    if(!mode) {
    snprintf(cfg_str_concat + strlen(cfg_str_concat), concat_size - 
             strlen(cfg_str_concat), i < count - 1 ? "%s," : "%s", cfg_str[i]);
    } else {
      if(!(i % 2)) {
        snprintf(cfg_str_concat + strlen(cfg_str_concat), concat_size - 
             strlen(cfg_str_concat), "%"PRIu32"-", cfg_num[i]);
      } else {
        if(i == count - 1) { 
          snprintf(cfg_str_concat + strlen(cfg_str_concat), concat_size - 
               strlen(cfg_str_concat), "%"PRIu32, cfg_num[i]);
        } else {
          snprintf(cfg_str_concat + strlen(cfg_str_concat), concat_size - 
             strlen(cfg_str_concat), "%"PRIu32",", cfg_num[i]);
        }
      }
    }
  }

  return count;
}
