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

#ifndef __ROAFETCHLIB_TEST_VALIDATION_H
#define __ROAFETCHLIB_TEST_VALIDATION_H

#include "roafetchlib.h"

#define TEST_BUF_LEN 4096
#define TEST_PROJECT "FU-Berlin,FU-Berlin"
#define TEST_CC                                                                \
  (const char * [2])                                                           \
  {                                                                            \
    "CC01", "CC06(RTR)"                                                        \
  }
#define TEST_COLLECTOR "CC01,CC06(RTR)"
#define TEST_PROJECT_COLLECTOR "FU-Berlin:CC01,CC06(RTR)"
#define TEST_TIMESTAMP 1000000000
#define TEST_HISTORY_TIMEWDW "1000000000-1000000000"

/* Testcase for checking the setup of the Live validation configuration */
#define TEST_LIVE_COUNT 5

#define TEST_LIVE_1 "(valid)"
#define TEST_LIVE_1_PJ "FU-Berlin"
#define TEST_LIVE_1_CC "CC06(RTR)"
#define TEST_LIVE_1_PJ_CC "FU-Berlin:CC06(RTR)"
#define TEST_LIVE_1_SSH NULL
#define TEST_LIVE_1_MOD 0
#define TEST_LIVE_1_RST 0

#define TEST_LIVE_2 "(no interval)"
#define TEST_LIVE_2_PJ "FU-Berlin"
#define TEST_LIVE_2_CC "CC06(RTR)"
#define TEST_LIVE_2_PJ_CC "FU-Berlin:CC06(RTR)"
#define TEST_LIVE_2_SSH NULL
#define TEST_LIVE_2_MOD 1
#define TEST_LIVE_2_RST -1

#define TEST_LIVE_3 "(not a RTR-Server)"
#define TEST_LIVE_3_PJ "FU-Berlin"
#define TEST_LIVE_3_CC "CC01"
#define TEST_LIVE_3_PJ_CC "FU-Berlin:CC01"
#define TEST_LIVE_3_SSH NULL
#define TEST_LIVE_3_MOD 0
#define TEST_LIVE_3_RST -1

#define TEST_LIVE_4 "(unknown CC)"
#define TEST_LIVE_4_PJ "FU-Berlin"
#define TEST_LIVE_4_CC "FAKE_CC(RTR)"
#define TEST_LIVE_4_PJ_CC "FU-Berlin:FAKE_CC(RTR)"
#define TEST_LIVE_4_SSH NULL
#define TEST_LIVE_4_MOD 0
#define TEST_LIVE_4_RST -1

#define TEST_LIVE_5 "(SSH options)"
#define TEST_LIVE_5_PJ "FU-Berlin"
#define TEST_LIVE_5_CC "CC06(RTR)"
#define TEST_LIVE_5_PJ_CC "FU-Berlin:CC06(RTR)"
#define TEST_LIVE_5_SSH "ssh_user,ssh_hostkey_path,ssh_privkey_path"
#define TEST_LIVE_5_MOD 0
#define TEST_LIVE_5_RST -1

#define TEST_LIVE                                                              \
  (char * [TEST_LIVE_COUNT])                                                   \
  {                                                                            \
    TEST_LIVE_1, TEST_LIVE_2, TEST_LIVE_3, TEST_LIVE_4, TEST_LIVE_5            \
  }

#define TEST_LIVE_PJ_CC                                                        \
  (char * [TEST_LIVE_COUNT])                                                   \
  {                                                                            \
    TEST_LIVE_1_PJ_CC, TEST_LIVE_2_PJ_CC, TEST_LIVE_3_PJ_CC,                   \
      TEST_LIVE_4_PJ_CC, TEST_LIVE_5_PJ_CC                                     \
  }

#define TEST_LIVE_PJ                                                           \
  (char * [TEST_LIVE_COUNT])                                                   \
  {                                                                            \
    TEST_LIVE_1_PJ, TEST_LIVE_2_PJ, TEST_LIVE_3_PJ, TEST_LIVE_4_PJ,            \
      TEST_LIVE_5_PJ                                                           \
  }

#define TEST_LIVE_CC                                                           \
  (char * [TEST_LIVE_COUNT])                                                   \
  {                                                                            \
    TEST_LIVE_1_CC, TEST_LIVE_2_CC, TEST_LIVE_3_CC, TEST_LIVE_4_CC,            \
      TEST_LIVE_5_CC                                                           \
  }

#define TEST_LIVE_SSH                                                          \
  (char * [TEST_LIVE_COUNT])                                                   \
  {                                                                            \
    TEST_LIVE_1_SSH, TEST_LIVE_2_SSH, TEST_LIVE_3_SSH, TEST_LIVE_4_SSH,        \
      TEST_LIVE_5_SSH                                                          \
  }

#define TEST_LIVE_MOD                                                          \
  (int[TEST_LIVE_COUNT])                                                       \
  {                                                                            \
    TEST_LIVE_1_MOD, TEST_LIVE_2_MOD, TEST_LIVE_3_MOD, TEST_LIVE_4_MOD,        \
      TEST_LIVE_5_MOD                                                          \
  }

#define TEST_LIVE_RST                                                          \
  (int[TEST_LIVE_COUNT])                                                       \
  {                                                                            \
    TEST_LIVE_1_RST, TEST_LIVE_2_RST, TEST_LIVE_3_RST, TEST_LIVE_4_RST,        \
      TEST_LIVE_5_RST                                                          \
  }

/* Testcase for checking the validation result */

// Validate RPKI-enabled BGP Beacons from RIPE
// see:
// https://www.ripe.net/analyse/internet-measurements/routing-information-service-ris/current-ris-routing-beacons

#define TEST_COUNT 7

#define TEST1_O_ASN 12654
#define TEST1_PFX "93.175.146.0"
#define TEST1_MSKL 24
#define TEST1_RST_1 "FU-Berlin,CC01,valid,12654,93.175.146.0/24-24"
#define TEST1_RST_2 "FU-Berlin,CC06(RTR),valid,12654,93.175.146.0/24-24"
#define TEST1_UNIFIED                                                          \
  "FU-Berlin\\CC01 FU-Berlin\\CC06(RTR),valid,"                                \
  "12654,93.175.146.0/24-24;"
#define TEST1_DISC                                                             \
  "FU-Berlin,CC01,valid,12654,93.175.146.0/24-24;"                             \
  "FU-Berlin,CC06(RTR),valid,12654,93.175.146.0/24-24;"

#define TEST2_O_ASN 12654
#define TEST2_PFX "2001:7fb:fd02::"
#define TEST2_MSKL 48
#define TEST2_RST_1 "FU-Berlin,CC01,valid,12654,2001:7fb:fd02::/48-48"
#define TEST2_RST_2 "FU-Berlin,CC06(RTR),valid,12654,2001:7fb:fd02::/48-48"
#define TEST2_UNIFIED                                                          \
  "FU-Berlin\\CC01 FU-Berlin\\CC06(RTR),valid,"                                \
  "12654,2001:7fb:fd02::/48-48;"
#define TEST2_DISC                                                             \
  "FU-Berlin,CC01,valid,12654,2001:7fb:fd02::/48-48;"                          \
  "FU-Berlin,CC06(RTR),valid,12654,2001:7fb:fd02::/48-48;"

#define TEST3_O_ASN 12654
#define TEST3_PFX "93.175.147.0"
#define TEST3_MSKL 24
#define TEST3_V_O_ASN 196615
#define TEST3_RST_1 "FU-Berlin,CC01,invalid,196615,93.175.147.0/24-24"
#define TEST3_RST_2 "FU-Berlin,CC06(RTR),invalid,196615,93.175.147.0/24-24"
#define TEST3_UNIFIED                                                          \
  "FU-Berlin\\CC01 FU-Berlin\\CC06(RTR),invalid,"                              \
  "196615,93.175.147.0/24-24;"
#define TEST3_DISC                                                             \
  "FU-Berlin,CC01,invalid,196615,93.175.147.0/24-24;"                          \
  "FU-Berlin,CC06(RTR),invalid,196615,93.175.147.0/24-24;"

#define TEST4_O_ASN 12654
#define TEST4_PFX "2001:7fb:fd03::"
#define TEST4_MSKL 48
#define TEST4_V_O_ASN 196615
#define TEST4_RST_1 "FU-Berlin,CC01,invalid,196615,2001:7fb:fd03::/48-48"
#define TEST4_RST_2 "FU-Berlin,CC06(RTR),invalid,196615,2001:7fb:fd03::/48-48"
#define TEST4_UNIFIED                                                          \
  "FU-Berlin\\CC01 FU-Berlin\\CC06(RTR),invalid,"                              \
  "196615,2001:7fb:fd03::/48-48;"
#define TEST4_DISC                                                             \
  "FU-Berlin,CC01,invalid,196615,2001:7fb:fd03::/48-48;"                       \
  "FU-Berlin,CC06(RTR),invalid,196615,2001:7fb:fd03::/48-48;"

#define TEST5_O_ASN 0
#define TEST5_PFX "84.205.83.0"
#define TEST5_MSKL 24
#define TEST5_RST_1 "FU-Berlin,CC01,notfound;"
#define TEST5_RST_2 "FU-Berlin,CC06(RTR),notfound;"
#define TEST5_UNIFIED "FU-Berlin\\CC01 FU-Berlin\\CC06(RTR),notfound;"
#define TEST5_DISC "FU-Berlin,CC01,notfound;FU-Berlin,CC06(RTR),notfound;"

#define TEST6_O_ASN 0
#define TEST6_PFX "2001:7fb:ff03::"
#define TEST6_MSKL 48
#define TEST6_RST_1 "FU-Berlin,CC01,notfound;"
#define TEST6_RST_2 "FU-Berlin,CC06(RTR),notfound;"
#define TEST6_UNIFIED "FU-Berlin\\CC01 FU-Berlin\\CC06(RTR),notfound;"
#define TEST6_DISC "FU-Berlin,CC01,notfound;FU-Berlin,CC06(RTR),notfound;"

/* Testcase for checking the output representation of the validation result */
#define TEST7_O_ASN 11
#define TEST7_PFX "80.128.0.0"
#define TEST7_MSKL 11
#define TEST7_UNIFIED                                                          \
  "FU-Berlin\\CC01 FU-Berlin\\CC06(RTR),invalid,0,80.128.0.0/11-11 "           \
	"80.128.0.1/11-11;FU-Berlin\\CC01 FU-Berlin\\CC06(RTR),invalid,2792,"        \
	"80.128.0.0/11-11;FU-Berlin\\CC01 FU-Berlin\\CC06(RTR),invalid,3320,"        \
	"80.128.0.0/11-11;"
#define TEST7_DISC                                                             \
  "FU-Berlin,CC01,invalid,0,80.128.0.0/11-11 80.128.0.1/11-11;"                \
  "FU-Berlin,CC01,invalid,2792,80.128.0.0/11-11;"                              \
  "FU-Berlin,CC01,invalid,3320,80.128.0.0/11-11;"                              \
  "FU-Berlin,CC06(RTR),invalid,0,80.128.0.0/11-11 80.128.0.1/11-11;"           \
  "FU-Berlin,CC06(RTR),invalid,2792,80.128.0.0/11-11;"                         \
  "FU-Berlin,CC06(RTR),invalid,3320,80.128.0.0/11-11;"

#define TEST_O_ASN                                                             \
  (int[TEST_COUNT])                                                            \
  {                                                                            \
    TEST1_O_ASN, TEST2_O_ASN, TEST3_O_ASN, TEST4_O_ASN, TEST5_O_ASN,           \
      TEST6_O_ASN, TEST7_O_ASN                                                 \
  }

#define TEST_PFX                                                               \
  (char * [TEST_COUNT])                                                        \
  {                                                                            \
    TEST1_PFX, TEST2_PFX, TEST3_PFX, TEST4_PFX, TEST5_PFX, TEST6_PFX,          \
      TEST7_PFX                                                                \
  }

#define TEST_MSKL                                                              \
  (int[TEST_COUNT])                                                            \
  {                                                                            \
    TEST1_MSKL, TEST2_MSKL, TEST3_MSKL, TEST4_MSKL, TEST5_MSKL, TEST6_MSKL,    \
      TEST7_MSKL                                                               \
  }

#define TEST_RST                                                               \
  (char * [(TEST_COUNT - 1) * 2])                                              \
  {                                                                            \
    TEST1_RST_1, TEST1_RST_2, TEST2_RST_1, TEST2_RST_2, TEST3_RST_1,           \
      TEST3_RST_2, TEST4_RST_1, TEST4_RST_2, TEST5_RST_1, TEST5_RST_2,         \
      TEST6_RST_1, TEST6_RST_2,                                                \
  }

#define TEST_UNIFIED                                                           \
  (char * [TEST_COUNT])                                                        \
  {                                                                            \
    TEST1_UNIFIED, TEST2_UNIFIED, TEST3_UNIFIED, TEST4_UNIFIED, TEST5_UNIFIED, \
      TEST6_UNIFIED, TEST7_UNIFIED                                             \
  }

#define TEST_DISC                                                              \
  (char * [TEST_COUNT])                                                        \
  {                                                                            \
    TEST1_DISC, TEST2_DISC, TEST3_DISC, TEST4_DISC, TEST5_DISC, TEST6_DISC,    \
      TEST7_DISC                                                               \
  }

#endif /* __ROAFETCHLIB_TEST_VALIDATION_H */
