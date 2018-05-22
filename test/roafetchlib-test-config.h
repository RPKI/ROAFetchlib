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

#ifndef __ROAFETCHLIB_TEST_CONFIG_H
#define __ROAFETCHLIB_TEST_CONFIG_H

#include "roafetchlib.h"
#include "roafetchlib-test.h"
#include "utils.h"

#define TEST_BUF_LEN 4096

/** Testcases for determining the timestamps */
#define TEST_TS_COUNT 6

#define TEST_TS_PROJECT_COLLECTOR "FU-Berlin:CC01"
#define TEST_TS_PROJECT "FU-Berlin"
#define TEST_TS_COLLECTOR "CC01"
#define TEST_TS_TIMESTAMP 1000000000
#define TEST_TS_HISTORY_TIMEWDW "1000000000-1000000000"

#define TEST_TS_1 999999960
#define TEST_TS_2 1000000140
#define TEST_TS_3 1000000320
#define TEST_TS_4 1000000500
#define TEST_TS_5 1000000680
#define TEST_TS_6 1000000860

#define TEST_TS_1_URL                                                          \
  "http://roa-broker.realmv6.org:443/CC01/2001.09/"                            \
  "vrp.20010909.0146.csv.gz"
#define TEST_TS_2_URL                                                          \
  "http://roa-broker.realmv6.org:443/CC01/2001.09/"                            \
  "vrp.20010909.0149.csv.gz"
#define TEST_TS_3_URL                                                          \
  "http://roa-broker.realmv6.org:443/CC01/2001.09/"                            \
  "vrp.20010909.0152.csv.gz"
#define TEST_TS_4_URL                                                          \
  "http://roa-broker.realmv6.org:443/CC01/2001.09/"                            \
  "vrp.20010909.0155.csv.gz"
#define TEST_TS_5_URL                                                          \
  "http://roa-broker.realmv6.org:443/CC01/2001.09/"                            \
  "vrp.20010909.0158.csv.gz"
#define TEST_TS_6_URL                                                          \
  "http://roa-broker.realmv6.org:443/CC01/2001.09/"                            \
  "vrp.20010909.0201.csv.gz"

#define TEST_TS                                                                \
  (uint32_t[TEST_TS_COUNT])                                                    \
  {                                                                            \
    TEST_TS_1, TEST_TS_2, TEST_TS_3, TEST_TS_4, TEST_TS_5, TEST_TS_6           \
  }

#define TEST_TS_URL                                                            \
  (char * [TEST_TS_COUNT])                                                     \
  {                                                                            \
    TEST_TS_1_URL, TEST_TS_2_URL, TEST_TS_3_URL, TEST_TS_4_URL, TEST_TS_5_URL, \
      TEST_TS_6_URL                                                            \
  }

#define TEST_TS_NTS                                                            \
  (uint32_t[TEST_TS_COUNT - 2])                                                \
  {                                                                            \
    TEST_TS_1, TEST_TS_2, TEST_TS_5, TEST_TS_6                                 \
  }

/** Testcases for the addition checks **/
#define TEST_ADD_COUNT 5

#define TEST_ADD_1_MINL 24
#define TEST_ADD_1_MAXL 24
#define TEST_ADD_1_ASN 12654
#define TEST_ADD_1_ADDR "93.175.146.0"
#define TEST_ADD_1_REC "12654,93.175.146.0/24,24"

#define TEST_ADD_2_MINL 48
#define TEST_ADD_2_MAXL 48
#define TEST_ADD_2_ASN 12654
#define TEST_ADD_2_ADDR "2001:7fb:fd02::"
#define TEST_ADD_2_REC "12654,2001:7fb:fd02::/48,48"

#define TEST_ADD_3_MINL 24
#define TEST_ADD_3_MAXL 24
#define TEST_ADD_3_ASN 196615
#define TEST_ADD_3_ADDR "93.175.147.0"
#define TEST_ADD_3_REC "196615,93.175.147.0/24,24"

#define TEST_ADD_4_MINL 48
#define TEST_ADD_4_MAXL 48
#define TEST_ADD_4_ASN 196615
#define TEST_ADD_4_ADDR "2001:7fb:fd03::"
#define TEST_ADD_4_REC "196615,2001:7fb:fd03::/48,48"

#define TEST_ADD_5_ASN 196615
#define TEST_ADD_5_MINL 48
#define TEST_ADD_5_MAXL 48
#define TEST_ADD_5_ADDR "?2001:7fb:fd03::"

#define TEST_ADD_ASN                                                           \
  (int[TEST_ADD_COUNT])                                                        \
  {                                                                            \
    TEST_ADD_1_ASN, TEST_ADD_2_ASN, TEST_ADD_3_ASN, TEST_ADD_4_ASN,            \
      TEST_ADD_5_ASN                                                           \
  }

#define TEST_ADD_MINL                                                          \
  (int[TEST_ADD_COUNT])                                                        \
  {                                                                            \
    TEST_ADD_1_MINL, TEST_ADD_2_MINL, TEST_ADD_3_MINL, TEST_ADD_4_MINL,        \
      TEST_ADD_5_MINL                                                          \
  }

#define TEST_ADD_MAXL                                                          \
  (int[TEST_ADD_COUNT])                                                        \
  {                                                                            \
    TEST_ADD_1_MAXL, TEST_ADD_2_MAXL, TEST_ADD_3_MAXL, TEST_ADD_4_MAXL,        \
      TEST_ADD_5_MAXL                                                          \
  }

#define TEST_ADD_ADDR                                                          \
  (char * [TEST_ADD_COUNT])                                                    \
  {                                                                            \
    TEST_ADD_1_ADDR, TEST_ADD_2_ADDR, TEST_ADD_3_ADDR, TEST_ADD_4_ADDR,        \
      TEST_ADD_5_ADDR                                                          \
  }

#define TEST_ADD_REC                                                           \
  (char * [TEST_ADD_COUNT])                                                    \
  {                                                                            \
    TEST_ADD_1_REC, TEST_ADD_2_REC, TEST_ADD_3_REC, TEST_ADD_4_REC             \
  }

/** Testcases for the numberic validity checks **/
#define TEST_VAL_COUNT 3

#define TEST_VAL_1 "0"
#define TEST_VAL_1_RST 0
#define TEST_VAL_2 "128"
#define TEST_VAL_2_RST 128
#define TEST_VAL_3 "256"
#define TEST_VAL_3_RST 0

#define TEST_VAL_4 "0"
#define TEST_VAL_4_RST 0
#define TEST_VAL_5 "8192"
#define TEST_VAL_5_RST 8192
#define TEST_VAL_6 "4294967296"
#define TEST_VAL_6_RST 0

#define TEST_VAL_8                                                             \
  (char * [TEST_VAL_COUNT])                                                    \
  {                                                                            \
    TEST_VAL_1, TEST_VAL_2, TEST_VAL_3                                         \
  }

#define TEST_VAL_RST_8                                                         \
  (int[TEST_VAL_COUNT])                                                        \
  {                                                                            \
    TEST_VAL_1_RST, TEST_VAL_2_RST, TEST_VAL_3_RST                             \
  }

#define TEST_VAL_32                                                            \
  (char * [TEST_VAL_COUNT])                                                    \
  {                                                                            \
    TEST_VAL_4, TEST_VAL_5, TEST_VAL_6                                         \
  }

#define TEST_VAL_RST_32                                                        \
  (int[TEST_VAL_COUNT])                                                        \
  {                                                                            \
    TEST_VAL_4_RST, TEST_VAL_5_RST, TEST_VAL_6_RST                             \
  }

/** Testcases for the prefix validity checks **/
// IPv4
#define TEST_PFX_IPv4_COUNT 6

#define TEST_PFX_1_PFX "0.0.0.0/0"
#define TEST_PFX_1_ADDR "0.0.0.0"
#define TEST_PFX_1_MINL 0

#define TEST_PFX_2_PFX "10.10.10.10/10"
#define TEST_PFX_2_ADDR "10.10.10.10"
#define TEST_PFX_2_MINL 10

#define TEST_PFX_3_PFX "255.255.255.255/32"
#define TEST_PFX_3_ADDR "255.255.255.255"
#define TEST_PFX_3_MINL 32

#define TEST_PFX_4_PFX "256.256.256.256/32"
#define TEST_PFX_4_ADDR "255.255.255.255"
#define TEST_PFX_4_MINL 32

#define TEST_PFX_5_PFX "255.255.255.255/33"
#define TEST_PFX_5_ADDR "255.255.255.255"
#define TEST_PFX_5_MINL 33

#define TEST_PFX_6_PFX "?.10.10.10/10"

// IPv6
#define TEST_PFX_IPv6_COUNT 5

#define TEST_PFX_7_PFX "::/0"
#define TEST_PFX_7_ADDR "::"
#define TEST_PFX_7_MINL 0

#define TEST_PFX_8_PFX "10:10:10:10:10:10:10:10/10"
#define TEST_PFX_8_ADDR "10:10:10:10:10:10:10:10"
#define TEST_PFX_8_MINL 10

#define TEST_PFX_9_PFX "ffff:ffff:ffff:ffff:ffff:ffff:ffff:ffff/128"
#define TEST_PFX_9_ADDR "ffff:ffff:ffff:ffff:ffff:ffff:ffff:ffff"
#define TEST_PFX_9_MINL 128

#define TEST_PFX_10_PFX "ffff:ffff:ffff:ffff:ffff:ffff:ffff:ffff/129"
#define TEST_PFX_10_ADDR "ffff:ffff:ffff:ffff:ffff:ffff:ffff:ffff"
#define TEST_PFX_10_MINL 129

#define TEST_PFX_11_PFX "?::/10"

#define TEST_PFX_IPv4                                                          \
  (char * [TEST_PFX_IPv4_COUNT])                                               \
  {                                                                            \
    TEST_PFX_1_PFX, TEST_PFX_2_PFX, TEST_PFX_3_PFX, TEST_PFX_4_PFX,            \
      TEST_PFX_5_PFX, TEST_PFX_6_PFX                                           \
  }

#define TEST_PFX_ADDR_IPv4                                                     \
  (char * [TEST_PFX_IPv4_COUNT])                                               \
  {                                                                            \
    TEST_PFX_1_ADDR, TEST_PFX_2_ADDR, TEST_PFX_3_ADDR, TEST_PFX_4_ADDR,        \
      TEST_PFX_5_ADDR                                                          \
  }

#define TEST_PFX_MINL_IPv4                                                     \
  (uint8_t[TEST_PFX_IPv4_COUNT])                                               \
  {                                                                            \
    TEST_PFX_1_MINL, TEST_PFX_2_MINL, TEST_PFX_3_MINL, TEST_PFX_4_MINL,        \
      TEST_PFX_5_MINL                                                          \
  }

#define TEST_PFX_IPv6                                                          \
  (char * [TEST_PFX_IPv6_COUNT])                                               \
  {                                                                            \
    TEST_PFX_7_PFX, TEST_PFX_8_PFX, TEST_PFX_9_PFX, TEST_PFX_10_PFX,           \
      TEST_PFX_11_PFX                                                          \
  }

#define TEST_PFX_ADDR_IPv6                                                     \
  (char * [TEST_PFX_IPv6_COUNT])                                               \
  {                                                                            \
    TEST_PFX_7_ADDR, TEST_PFX_8_ADDR, TEST_PFX_9_ADDR, TEST_PFX_10_ADDR        \
  }

#define TEST_PFX_MINL_IPv6                                                     \
  (uint8_t[TEST_PFX_IPv6_COUNT])                                               \
  {                                                                            \
    TEST_PFX_7_MINL, TEST_PFX_8_MINL, TEST_PFX_9_MINL, TEST_PFX_10_MINL        \
  }

#define TEST_IMP_URL                                                           \
  "http://roa-broker.realmv6.org:443/CC01/2001.09/"                            \
  "vrp.20010909.0146.csv.gz"

#define TEST_IMP_IPv4                                                          \
  "0,80.128.0.0/11,11\n0,80.128.0.1/11,11\n12654,93.175.146.0/24,24\n"         \
  "196615,93.175.147.0/24,24\n2792,80.128.0.0/11,11\n"                         \
  "3320,80.128.0.0/11,11\n"

#define TEST_IMP_IPv6                                                          \
  "12654,2001:7fb:fd02::/48,48\n"                                              \
  "196615,2001:7fb:fd03::/48,48\n"

/** Testcases for the input to config addition **/
#define TEST_ADD_INP_COUNT 5

#define TEST_ADD_INP_1_PROJ "PJ1,PJ2,PJ3"
#define TEST_ADD_INP_1_CNT 3

#define TEST_ADD_INP_2_PROJ                                                    \
  "PJ1,PJ2,PJ3,PJ4,PJ5,PJ6,PJ7,PJ8,PJ9,PJ10,PJ11, PJ12,PJ13,PJ14,PJ15,PJ16,"   \
	"PJ17,PJ18,PJ19,PJ20,PJ21,PJ22,PJ23 ,PJ24,PJ25,PJ26,PJ27,PJ28,PJ29,PJ30,PJ31"
#define TEST_ADD_INP_2_BROKER_PROJ                                             \
  "PJ1,PJ2,PJ3,PJ4,PJ5,PJ6,PJ7,PJ8,PJ9,PJ10,PJ11,PJ12,PJ13,PJ14,PJ15,PJ16,"    \
	"PJ17,PJ18,PJ19,PJ20,PJ21,PJ22,PJ23,PJ24,PJ25,PJ26,PJ27,PJ28,PJ29,PJ30,PJ31"
#define TEST_ADD_INP_2_CNT 31

#define TEST_ADD_INP_3_PROJ                                                    \
  "1rOcqulNMrXUqXeK9jKqvFCi6tZU30TF69e3KPJrLPOocNUL"                           \
  "5vuonvKuLUUgTsvX"
#define TEST_ADD_INP_3_CNT -1

#define TEST_ADD_INP_4_PROJ                                                    \
  "PJ1,PJ2,PJ3,PJ4,PJ5,PJ6,PJ7,PJ8,PJ9,PJ10,PJ11,"                             \
  "PJ12,PJ13,PJ14,PJ15,PJ16,PJ17,PJ18, PJ19,PJ20,"                             \
  "PJ21,PJ22,PJ23,PJ24 ,PJ25 , PJ26, PJ27, PJ28, PJ29,"                        \
  "PJ30, PJ31 ,PJ32, PJ33"
#define TEST_ADD_INP_4_CNT -1

#define TEST_ADD_INP_5_PROJ ""
#define TEST_ADD_INP_5_CNT -1

#define TEST_ADD_INP_PROJ                                                      \
  (char * [TEST_ADD_INP_COUNT])                                                \
  {                                                                            \
    TEST_ADD_INP_1_PROJ, TEST_ADD_INP_2_PROJ, TEST_ADD_INP_3_PROJ,             \
      TEST_ADD_INP_4_PROJ, TEST_ADD_INP_5_PROJ                                 \
  }

#define TEST_ADD_INP_BROKER_PROJ                                               \
  (char * [TEST_ADD_INP_COUNT])                                                \
  {                                                                            \
    TEST_ADD_INP_1_PROJ, TEST_ADD_INP_2_BROKER_PROJ, TEST_ADD_INP_3_PROJ,      \
      TEST_ADD_INP_4_PROJ, TEST_ADD_INP_5_PROJ                                 \
  }

#define TEST_ADD_INP_CNT                                                       \
  (int[TEST_ADD_INP_COUNT])                                                    \
  {                                                                            \
    TEST_ADD_INP_1_CNT, TEST_ADD_INP_2_CNT, TEST_ADD_INP_3_CNT,                \
      TEST_ADD_INP_4_CNT, TEST_ADD_INP_5_CNT                                   \
  }

#endif /* __ROAFETCHLIB_TEST_CONFIG_H */
