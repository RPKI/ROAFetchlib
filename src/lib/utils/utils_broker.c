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

#include "utils_broker.h"

void utils_broker_print_debug(rpki_cfg_t *cfg)
{
  uint64_t key1;
  char *val;
  debug_print("%s", "\n----------- Broker Khash -------------------\n");
  kh_foreach(cfg->cfg_broker.broker_kh, key1, val, 
  debug_print("Key: %"PRIu64", Value: %s\n", key1, val));
  debug_print("%s", "\n");
}

int utils_broker_check_url(char *broker_url, char* result, size_t size)
{
  /* Get the broker reponse and check if it is reachable */
  io_t *json_chk_err = wandio_create(broker_url);
  if (json_chk_err == NULL) {
    std_print("ERROR: Could not open %s for reading\n", broker_url);
    wandio_destroy(json_chk_err);
    return -1;
  }

  /* Check if the broker reports errors, if so stop the process */
  wandio_read(json_chk_err, result, size);
  if (!strncmp(result, "Error:", strlen("Error:")) ||
      !strncmp(result, "Malformed", strlen("Malformed"))) {
    result[strlen(result)] = '\0';
    std_print("%s\n", result);
    wandio_destroy(json_chk_err);
    return -1;
  }
  wandio_destroy(json_chk_err);

  return 0;
}

int utils_broker_add_projects_collectors(char* input, char* cfg_str_concat,
                                   char* del, char (*cfg_str)[MAX_INPUT_LENGTH])
{
  char input_cpy[MAX_INPUT_LENGTH];
  memset(input_cpy, 0, sizeof(input_cpy));
  strncpy(input_cpy, input, sizeof(input_cpy));

  /* Extract every input element if it's length is valid */
  int count = 0;
  char *arg = strtok(input_cpy, del);
  while(arg != NULL) {
    snprintf(cfg_str[count++], MAX_INPUT_LENGTH, "%s", arg);
    arg = strtok(NULL, del);
  }

  /* Concatenate all inputs and store it */
  size_t concat_size = sizeof(input_cpy) * MAX_RPKI_COUNT;
  memset(cfg_str_concat, 0, concat_size);
  for (int i = 0; i < count; i++) {
    snprintf(cfg_str_concat + strlen(cfg_str_concat), concat_size - 
             strlen(cfg_str_concat), i < count - 1 ? "%s," : "%s", cfg_str[i]);
  }
  return count;
}
