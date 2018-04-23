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

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "config.h"
#include "utils.h"
#include "broker.h"
#include "debug.h"
#include "live_validation.h"
#include "rpki.h"
#include "rtrlib/rtrlib.h"

rpki_cfg_t *rpki_set_config(char *projects, char *collectors,
                            char *time_intervals, int unified, int mode,
                            char *broker_url, char *ssh_options)
{
  /* Create the configuration */
  rpki_cfg_t *cfg;
  if ((cfg = cfg_create(projects, collectors, time_intervals, unified, mode,
                        broker_url, ssh_options)) == NULL) {
    std_print("%s", "Error: Could not create RPKI config\n");
    exit(-1);
  }

  /* Configuration of live mode */
  config_input_t *input = &cfg->cfg_input;
  if (!mode) {
    debug_print("%s", "Info: For Live RPKI Validation only the first collector "
                      "will be taken\n");
    if (live_validation_set_config(input->projects[0], input->collectors[0],
                                   cfg, ssh_options) != 0) {
      rpki_destroy_config(cfg);
      exit(-1);
    }
    return cfg;
  }

  /* Configuration of historical mode */
  if (broker_connect(cfg, input->broker_projects, input->broker_collectors,
                     input->broker_intervals) != 0) {
    rpki_destroy_config(cfg);
    exit(-1);
  }
  utils_rpki_print_config_debug(cfg);
  return cfg;
}

int rpki_validate(rpki_cfg_t *cfg, uint32_t timestamp, uint32_t asn,
                  char *prefix, uint8_t mask_len, char *result, size_t size)
{
  elem_t *elem;
  if ((elem = elem_create()) == NULL) {
    debug_err_print("%s", "Error: Could not allocate memory for RPKI elem\n");
    return -1;
  }

  /* Validate with live mode -> if the flag is set */
  config_rtr_t *rtr = &cfg->cfg_rtr;
  if (!cfg->cfg_input.mode && rtr->rtr_mgr_cfg != NULL) {
    elem_get_rpki_validation_result(cfg, rtr->rtr_mgr_cfg, elem, prefix, asn,
                                    mask_len, NULL, 0);
    rtr->pfxt_count = 1;
    elem_get_rpki_validation_result_snprintf(cfg, result, size, elem);
    elem_destroy(elem);
    return 0;
  }

  /* No validation if the timestamp is not in the time interval */
  int check = 0;
  config_input_t *input = &cfg->cfg_input;
  for (int i = 0; i < input->intervals_count; i = i + 2) {
    if ((timestamp >= input->intervals[i] &&
         timestamp <= input->intervals[i + 1]) ||
        (timestamp >= input->intervals[i] && input->intervals[i + 1] == 0)) {
      check = 1;
    }
  }
  if (!check) {
    debug_err_print("%s%" PRIu32 "%s\n", "Error: The timestamp: ", timestamp,
                    " is not in the configuration time interval\n");
    elem_destroy(elem);
    return -1;
  }

  /* No validation if the timestamp is older than the current ROA timestamp */
  config_time_t *cfg_time = &cfg->cfg_time;
  if (input->mode && timestamp < cfg_time->current_roa_timestamp) {
    debug_err_print("Info: No ROA dumps for the interval %" PRIu32
                    " - next available timestamp: %" PRIu32 "\n",
                    timestamp, cfg_time->current_roa_timestamp);
    snprintf(result, size, "%s", "");
    elem_destroy(elem);
    return 0;
  }

  /* No validation if no ROA entries exist for the time interval */
  if (!cfg->cfg_broker.broker_khash_count) {
    elem_destroy(elem);
    return -1;
  }

  /* If the current timestamp is empty -> get it, parse URLs and import ROAs */
  char current_urls[BROKER_ROA_URLS_LEN] = {0};
  if (!cfg_time->current_roa_timestamp && !cfg_time->next_roa_timestamp) {
    if (cfg_get_timestamps(cfg, timestamp, current_urls)) {
      debug_err_print("%s", "Error: Could not find current and next timestamp");
      elem_destroy(elem);
      return -1;
    }
    if (cfg_parse_urls(cfg, current_urls) != 0) {
      return -1;
    }
    debug_print("Current ROA Timestamp: %" PRIu32 "\n",
                cfg->cfg_time.current_roa_timestamp);
    debug_print("Next ROA Timestamp:    %" PRIu32 "\n",
                cfg->cfg_time.next_roa_timestamp);
  }

  /* Switch the mode if the timestamp is newer than the last cached ROA dump */
  config_broker_t *broker = &cfg->cfg_broker;
  if (input->mode && !cfg_time->max_end &&
      timestamp >= cfg_time->current_roa_timestamp + ROA_ARCHIVE_INTERVAL &&
      !cfg_time->next_roa_timestamp) {

    /* Hybrid mode if timestamp is older than current time - ROA interval */
    if (cfg_time->current_roa_timestamp <
        (uint32_t)time(NULL) - ROA_ARCHIVE_INTERVAL) {
      debug_print("%s", "Info: Entering hybrid mode\n");
      char current_interval[MAX_INTERVAL_SIZE];
      snprintf(current_interval, sizeof(current_interval),
               "%" PRIu32 "-%" PRIu32, timestamp, cfg_time->max_end);
      if (broker_connect(cfg, input->broker_projects, input->broker_collectors,
                         current_interval) != 0) {
        return -1;
      }
      broker->broker_khash_used = 0;
      rtr->pfxt_count = 0;
      cfg_get_timestamps(cfg, timestamp, current_urls);
      if (cfg_parse_urls(cfg, current_urls) != 0) {
        return -1;
      }

      /* Live mode if timestamp is newer than current time - ROA interval */
    } else {
      std_print("%s", "Info: Entering live mode\n");
      input->mode = 0;
      live_validation_set_config(input->projects[0], input->collectors[0], cfg,
                                 input->ssh_options);
      elem_get_rpki_validation_result(cfg, rtr->rtr_mgr_cfg, elem, prefix, asn,
                                      mask_len, NULL, 0);
      elem_get_rpki_validation_result_snprintf(cfg, result, size, elem);
      elem_destroy(elem);
      return 0;
    }

    /* No validation if there is a gap between two ROA dumps */
  } else if (input->mode &&
             timestamp >=
               cfg_time->current_roa_timestamp + ROA_ARCHIVE_INTERVAL &&
             timestamp < cfg_time->next_roa_timestamp &&
             cfg_time->next_roa_timestamp != 0) {
    if (cfg->cfg_time.current_gap) {
      debug_err_print("Info: No ROA dumps for the interval %" PRIu32
                      " - next available timestamp: %" PRIu32 "\n",
                      timestamp, cfg_time->next_roa_timestamp);
      cfg->cfg_time.current_gap = 0;
    }
    snprintf(result, size, "%s", "");
    elem_destroy(elem);
    return 0;
  }

  /* If the timestamp equals next timestamp, clean pfxt and set next timestamp
     next_roa_timestamp =  0 -> There is no next ROA file
     next_roa_timestamp = -1 -> Live mode active */
  if (timestamp >= cfg_time->next_roa_timestamp &&
      cfg_time->next_roa_timestamp != 0) {
    broker->broker_khash_used++;
    cfg_time->current_roa_timestamp = cfg_time->next_roa_timestamp;
    cfg_time->next_roa_timestamp =
      cfg_next_timestamp(cfg, cfg_time->current_roa_timestamp);
    strcpy(current_urls, kh_value(broker->broker_kh,
                                  kh_get(broker_result, broker->broker_kh,
                                         cfg_time->current_roa_timestamp)));
    if (cfg_parse_urls(cfg, current_urls) != 0) {
      return -1;
    }
    debug_print("Current ROA Timestamp: %" PRIu32 "\n",
                cfg->cfg_time.current_roa_timestamp);
    debug_print("Next ROA Timestamp:    %" PRIu32 "\n",
                cfg->cfg_time.next_roa_timestamp);
  }

  /* Validation the prefix, mask_len and ASN with Historical RPKI Validation */
  for (int i = 0; i < rtr->pfxt_count; i++) {
    elem_get_rpki_validation_result(cfg, NULL, elem, prefix, asn, mask_len,
                                    &rtr->pfxt[i], i);
  }

  /* Validation output */
  elem_get_rpki_validation_result_snprintf(cfg, result, size, elem);

  /* Clear elem */
  elem_destroy(elem);
  if (!cfg->cfg_time.current_gap) {
    cfg->cfg_time.current_gap = 1;
  }

  return 0;
}

int rpki_destroy_config(rpki_cfg_t *cfg)
{
  /* Destroy the RPKI configuration */
  return (cfg_destroy(cfg) != 0 ? -1 : 0);
}
