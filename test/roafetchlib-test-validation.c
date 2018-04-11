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
 * all
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
#include <unistd.h>

#include "roafetchlib-test-validation.h"
#include "roafetchlib-test.h"

int test_live_validation_set_config()
{
  /* live_validation_set_config */
  int ret = 0;
  char testcase[TEST_BUF_LEN];
  for (int i = 0; i < TEST_LIVE_COUNT; i++) {
    snprintf(testcase, sizeof(testcase), "#%i - Live Validation %s", i + 1,
             TEST_LIVE[i]);
    if (i == 1) {
      PRINT_INTENDED_ERR;
      rpki_cfg_t *cfg = cfg_create(TEST_LIVE_PJ[i], TEST_LIVE_CC[i], NULL, 0,
                                   TEST_LIVE_MOD[i], NULL, TEST_LIVE_SSH[i]);
      CHECK_RESULT("", testcase, cfg == NULL);
    } else {
      if (TEST_LIVE_RST[i] == -1) {
        PRINT_INTENDED_ERR;
      }
      rpki_cfg_t *cfg = cfg_create(TEST_LIVE_PJ[i], TEST_LIVE_CC[i], NULL, 0,
                                   TEST_LIVE_MOD[i], NULL, TEST_LIVE_SSH[i]);
      ret = live_validation_set_config(TEST_LIVE_PJ[i], TEST_LIVE_CC[i], cfg,
                                       TEST_LIVE_SSH[i]);
      CHECK_RESULT("", testcase, TEST_LIVE_RST[i] == ret);
    }
  }
  return 0;
}
int test_rpki_validation_status(rpki_cfg_t *cfg, char *result)
{
  /* elem_get_rpki_validation_result */

  char testcase[TEST_BUF_LEN];
  for (int j = 0; j < TEST_COUNT - 1; j++) {

    elem_t *elem = elem_create();
    config_rtr_t *rtr = &cfg->cfg_rtr;
    config_input_t *input = &cfg->cfg_input;

    snprintf(testcase, sizeof(testcase), "BGP Beacon #%i - ", j + 1);
    for (int i = 0; i < rtr->pfxt_count; i++) {
      elem_get_rpki_validation_result(cfg, NULL, elem, TEST_PFX[j],
                                      TEST_O_ASN[j], TEST_MSKL[j],
                                      &rtr->pfxt[i], i);
    }

    // Notfound
    if (elem->rpki_validation_status[0] ==
          ELEM_RPKI_VALIDATION_STATUS_NOTFOUND &&
        elem->rpki_validation_status[1] ==
          ELEM_RPKI_VALIDATION_STATUS_NOTFOUND) {
      for (int k = 0; k < rtr->pfxt_count; k++) {
        snprintf(result, TEST_BUF_LEN, "%s,%s,notfound;", input->projects[k],
                 input->collectors[k]);
        CHECK_RESULT(TEST_CC[k], testcase,
                     !strcmp(result, TEST_RST[j * 2]) ||
                       !strcmp(result, TEST_RST[j * 2 + 1]));
      }
      elem_destroy(elem);
      continue;
    }

    // Valid or invalid
    const char *key, *val;
    int i = 0;
    kh_foreach(elem->rpki_kh, key, val,
               snprintf(result, TEST_BUF_LEN, "%s,%s", key, val);
               CHECK_RESULT(TEST_CC[i], testcase,
                            !strcmp(result, TEST_RST[j * 2]) ||
                              !strcmp(result, TEST_RST[j * 2 + 1]));
               i++;);
    elem_destroy(elem);
  }
  return 0;
}

int test_rpki_validation_output(rpki_cfg_t *cfg, char *result)
{
  /* elem_get_rpki_validation_result_snprintf */

  char testcase[TEST_BUF_LEN];
  for (int j = 0; j < TEST_COUNT; j++) {
    elem_t *elem = elem_create();
    config_rtr_t *rtr = &cfg->cfg_rtr;
    config_input_t *input = &cfg->cfg_input;
    snprintf(testcase, sizeof(testcase), "BGP Beacon #%i", j + 1);

    for (int i = 0; i < rtr->pfxt_count; i++) {
      elem_get_rpki_validation_result(cfg, NULL, elem, TEST_PFX[j],
                                      TEST_O_ASN[j], TEST_MSKL[j],
                                      &rtr->pfxt[i], i);
    }

    if (input->unified) {
      elem_get_rpki_validation_result_snprintf(cfg, result, TEST_BUF_LEN, elem);
      CHECK_RESULT("", testcase, !strcmp(result, TEST_UNIFIED[j]));
    } else {
      elem_get_rpki_validation_result_snprintf(cfg, result, TEST_BUF_LEN, elem);
      CHECK_RESULT("", testcase, !strcmp(result, TEST_DISC[j]));
    }
    elem_destroy(elem);
  }
  return 0;
}

int test_rpki_validation(char *result)
{
  // Build up the configuration, URLs and prefix tables
  char url[RPKI_BROKER_URL_LEN] = "";
  rpki_cfg_t *cfg = rpki_set_config(TEST_PROJECT, TEST_COLLECTOR,
                                    TEST_HISTORY_TIMEWDW, 0, 1, NULL, NULL);
  cfg_get_timestamps(cfg, TEST_TIMESTAMP, url);
  cfg_parse_urls(cfg, url);

  CHECK_SUBSECTION("Live Validation config setup", 1,
                   !test_live_validation_set_config());

  CHECK_SUBSECTION("Validation result for BGP Beacons", 0,
                   !test_rpki_validation_status(cfg, result));

  CHECK_SUBSECTION("Validation output for discrete BGP Beacons", 0,
                   !test_rpki_validation_output(cfg, result));

  cfg = rpki_set_config(TEST_PROJECT, TEST_COLLECTOR, TEST_HISTORY_TIMEWDW, 1,
                        1, NULL, NULL);

  cfg_get_timestamps(cfg, TEST_TIMESTAMP, url);
  cfg_parse_urls(cfg, url);
  cfg->cfg_input.unified = 1;
  CHECK_SUBSECTION("Validation output for unified BGP Beacons", 0,
                   !test_rpki_validation_output(cfg, result));

  cfg_destroy(cfg);

  return 0;
}

int main()
{
  char result[TEST_BUF_LEN];

  CHECK_SECTION("Validation", !test_rpki_validation(result));

  return 0;
}
