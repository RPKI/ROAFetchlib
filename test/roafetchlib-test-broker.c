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

#include "roafetchlib-test-broker.h"
#include "roafetchlib-test.h"

int test_rpki_broker_connect(rpki_cfg_t *cfg, char *collector,
                             char *interval, char *type, char *buf,
                             char *result)
{
  int ret = -1;

  // invalid connection
  if (!strcmp(result, "invalid")) {
    PRINT_INTENDED_ERR;
    ret = broker_connect(cfg, collector, interval);
    CHECK_RESULT("of an invalid URL", type, ret != 0);
    return 0;
  }

  // valid connection but no ROA dumps
  if (!strcmp(result, "semi-valid")) {
    PRINT_INTENDED_ERR;
    ret = broker_connect(cfg, collector, interval);
    CHECK_RESULT("Khash Count", type, !cfg->cfg_broker.broker_khash_count);
    return 0;
  }

  // valid connection, parameters and ROA dumps
  ret = broker_connect(cfg, collector, interval);

  CHECK_RESULT("valid URL", type, !ret);
  CHECK_RESULT("Broker-URL", type,
               !strcmp(cfg->cfg_broker.roa_urls[0], TEST_BROKER_URL_4));
  return 0;
}
int test_rpki_broker_parse(rpki_cfg_t *cfg, char *type, char *json, char *buf,
                           char *result)
{

  int all_in = 0;
  int ret = -1;

  // invalid parsing
  if (!strcmp(result, "invalid")) {
    PRINT_INTENDED_ERR;
    ret = broker_parse_json(cfg, json);
    CHECK_RESULT("of an invalid format", type, ret != 0);
    return 0;
  }

  // valid parsing
  ret = broker_parse_json(cfg, json);
  CHECK_RESULT("of a valid format", type, !ret);
  CHECK_RESULT("of the projects", type,
               !strcmp(cfg->cfg_input.broker_projects, TEST_BROKER_PARSE_PRJ));
  CHECK_RESULT("of the collectors", type,
               !strcmp(cfg->cfg_input.broker_collectors, TEST_BROKER_PARSE_CC));
  CHECK_RESULT("of the Khash-Count", type, cfg->cfg_broker.broker_khash_count ==
                                             TEST_BROKER_PARSE_KHASH_CNT);
  CHECK_RESULT("of the start timestamp", type,
               cfg->cfg_time.start == TEST_BROKER_PARSE_START);
  CHECK_RESULT("of the end timestamp", type,
               cfg->cfg_time.max_end == TEST_BROKER_PARSE_END);

  for (int i = 0; i < cfg->cfg_broker.broker_khash_count; i++) {
    if (!strcmp(cfg->cfg_broker.roa_urls[i], TEST_BROKER_URL_1) ||
        !strcmp(cfg->cfg_broker.roa_urls[i], TEST_BROKER_URL_2) ||
        !strcmp(cfg->cfg_broker.roa_urls[i], TEST_BROKER_URL_3)) {
      all_in++;
    }
  }

  CHECK_RESULT("of the broker URL", type,
               all_in == TEST_BROKER_PARSE_KHASH_CNT);
  return 0;
}

int test_rpki_broker(char *buf, char *result)
{

  rpki_cfg_t *cfg =
    cfg_create(TEST_PROJECT_COLLECTOR, TEST_TIMEWDW, 0, 1, NULL, NULL);

  // Check broker parsing
  CHECK_SUBSECTION("Broker Parsing - valid format", 1,
                   !test_rpki_broker_parse(cfg, "Broker parsing ",
                                           TEST_JSON_FILE, buf, "valid"));
  CHECK_SUBSECTION("Broker Parsing - invalid format", 0,
                   !test_rpki_broker_parse(cfg, "Broker parsing ",
                                           TEST_JSON_FILE_ERR_REP, buf,
                                           "invalid"));
  CHECK_SUBSECTION("Broker Parsing - invalid format", 0,
                   !test_rpki_broker_parse(cfg, "Broker parsing ",
                                           TEST_JSON_FILE_ERR_FMT, buf,
                                           "invalid"));

  // Check broker connection
  CHECK_SUBSECTION(
    "Broker Connection - valid URL", 0,
    !test_rpki_broker_connect(cfg, TEST_PROJECT_COLLECTOR, TEST_TIMEWDW,
                              "Broker connection - ", buf, "valid"));
  CHECK_SUBSECTION("Broker Connection - malformed parameters", 0,
                   !test_rpki_broker_connect(
                     cfg, TEST_BROKER_CNN_PROJECT_ERR,
                     TEST_TIMEWDW, "Broker connection - ", buf, "invalid"));
  CHECK_SUBSECTION("Broker Connection - invalid URL", 0,
                   !test_rpki_broker_connect(
                     cfg, TEST_BROKER_CNN_PROJECT_ERR_URL,
                     TEST_TIMEWDW, "Broker connection - ", buf, "invalid"));
  CHECK_SUBSECTION("Broker Connection - no ROA dumps", 0,
                   !test_rpki_broker_connect(cfg, TEST_PROJECT_COLLECTOR,
                                             TEST_BROKER_CNN_TIMEWDW_ERR,
                                             "Broker connection - ", buf,
                                             "semi-valid"));
  cfg_destroy(cfg);
  return 0;
}

int main()
{
  char buf[TEST_BUF_LEN];
  char result[TEST_BUF_LEN];

  CHECK_SECTION("Broker", !test_rpki_broker(buf, result));

  return 0;
}
