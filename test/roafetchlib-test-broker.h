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

#ifndef __ROAFETCHLIB_TEST_BROKER_H
#define __ROAFETCHLIB_TEST_BROKER_H

#include "roafetchlib.h"

#define TEST_BUF_LEN 2048
#define TEST_PROJECT "FU-Berlin"
#define TEST_COLLECTOR "CC06(RTR)"
#define TEST_TIMEWDW "1000000000-1000000000"
#define TEST_KHASH_CNT 1

#define TEST_BROKER_CNN_PROJECT_ERR "ERR_PROJECT"
#define TEST_BROKER_CNN_PROJECT_ERR_URL "Broken URL"
#define TEST_BROKER_CNN_TIMEWDW_ERR "0800000000-0800000000"

#define TEST_BROKER_PARSE_PRJ "BROKER_FU-Berlin"
#define TEST_BROKER_PARSE_CC "BROKER_CC06(RTR)"
#define TEST_BROKER_PARSE_INV "BROKER_1511960400-1511960800"
#define TEST_BROKER_PARSE_KHASH_CNT 3
#define TEST_BROKER_PARSE_START 1511960400
#define TEST_BROKER_PARSE_END 1511960800
#define TEST_BROKER_URL_1                                                      \
  "http://roa-broker.realmv6.org:443/CC01/2017.11/"                            \
  "vrp.20171129.1300.csv.gz"
#define TEST_BROKER_URL_2                                                      \
  "http://roa-broker.realmv6.org:443/CC01/2017.11/"                            \
  "vrp.20171129.1303.csv.gz"
#define TEST_BROKER_URL_3                                                      \
  "http://roa-broker.realmv6.org:443/CC01/2017.11/"                            \
  "vrp.20171129.1306.csv.gz"
#define TEST_BROKER_URL_4                                                      \
  "http://roa-broker.realmv6.org:443/CC06(RTR)/"                               \
  "2001.09/vrp.20010909.0146.csv.gz"

#define TEST_JSON_FILE                                                         \
  "{\"projects\": \"BROKER_FU-Berlin\", \"collectors\": "                      \
  "\"BROKER_CC06(RTR)\", "                                                     \
  "\"interval\": \"BROKER_1511960400-1511960800\", \"start\": "                \
  "\"1511960400\", "                                                           \
  "\"max_end\": \"1511960800\", \"data\": {\"1511960400\": "                   \
  "\"http://roa-broker.realmv6.org:443/CC01/2017.11/"                          \
  "vrp.20171129.1300.csv.gz\","                                                \
  "\"1511960580\": \"http://roa-broker.realmv6.org:443/CC01/2017.11/"          \
  "vrp.20171129.1303.csv.gz\", \"1511960760\": "                               \
  "\"http://roa-broker.realmv6.org:443/CC01/2017.11/"                          \
  "vrp.20171129.1306.csv.gz\"}}"

#define TEST_JSON_FILE_ERR_REP                                                 \
  "{\"projects\": \"BROKER_FU-Berlin\", \"collectors\": "                      \
  "\"BROKER_CC06(RTR)\", \"interval\": \"BROKER_1511960400-1511960800\", "     \
  "\"start\": \"1511960400\", \"max_end\": \"1511960800\"}"

#define TEST_JSON_FILE_ERR_FMT "{\\ä}"

#endif /* __ROAFETCHLIB_TEST_BROKER_H */
