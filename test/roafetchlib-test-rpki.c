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

#pragma GCC optimize ("O0")
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "roafetchlib-test.h"
#include "roafetchlib-test-rpki.h"

int test_rpki_status(rpki_cfg_t *cfg, char* type, char* buf, char* result) {

  rpki_validate(cfg, TEST_TIMESTAMP, TEST1_O_ASN, TEST1_PFX, TEST1_MSKL, result,
                TEST_BUF_LEN);
  snprintf(buf, TEST_BUF_LEN, "%s,%s,%s,%i,%s/%i-%i;", TEST_PROJECT,
           TEST_COLLECTOR, "valid",
           TEST1_O_ASN, TEST1_PFX, TEST1_MSKL, TEST1_MSKL);
  CHECK_RESULT("for valid ROA Beacon", type, !strcmp(result, buf));

  rpki_validate(cfg, TEST_TIMESTAMP, TEST2_O_ASN, TEST2_PFX, TEST2_MSKL, result,
                TEST_BUF_LEN);
  snprintf(buf, TEST_BUF_LEN, "%s,%s,%s,%i,%s/%i-%i;", TEST_PROJECT,
           TEST_COLLECTOR, "valid",
           TEST2_O_ASN, TEST2_PFX, TEST2_MSKL, TEST2_MSKL);
  CHECK_RESULT("for valid ROA Beacon", type, !strcmp(result, buf));

  rpki_validate(cfg, TEST_TIMESTAMP, TEST3_O_ASN, TEST3_PFX, TEST3_MSKL, result,
                TEST_BUF_LEN);
  snprintf(buf, TEST_BUF_LEN, "%s,%s,%s,%i,%s/%i-%i;", TEST_PROJECT,
           TEST_COLLECTOR, "invalid",
           TEST3_V_O_ASN, TEST3_PFX, TEST3_MSKL, TEST3_MSKL);
  CHECK_RESULT("for invalid ROA Beacon", type, !strcmp(result, buf));

  rpki_validate(cfg, TEST_TIMESTAMP, TEST4_O_ASN, TEST4_PFX, TEST4_MSKL, result,
                TEST_BUF_LEN);
  snprintf(buf, TEST_BUF_LEN, "%s,%s,%s,%i,%s/%i-%i;", TEST_PROJECT,
           TEST_COLLECTOR, "invalid",
           TEST4_V_O_ASN, TEST4_PFX, TEST4_MSKL, TEST4_MSKL);
  CHECK_RESULT("for invalid ROA Beacon", type, !strcmp(result, buf));

  rpki_validate(cfg, TEST_TIMESTAMP, TEST5_O_ASN, TEST5_PFX, TEST5_MSKL, result,
                TEST_BUF_LEN);
  snprintf(buf, TEST_BUF_LEN, "%s,%s,%s;", TEST_PROJECT, TEST_COLLECTOR,
           "notfound");
  CHECK_RESULT("for non-existing ROA Beacon", type, !strcmp(result, buf));

  rpki_validate(cfg, TEST_TIMESTAMP, TEST6_O_ASN, TEST6_PFX, TEST6_MSKL, result,
                TEST_BUF_LEN);
  snprintf(buf, TEST_BUF_LEN, "%s,%s,%s;", TEST_PROJECT, TEST_COLLECTOR,
           "notfound");
  CHECK_RESULT("for non-existing ROA Beacon", type, !strcmp(result, buf));
  return 0;
}

int test_rpki(char* buf, char* result) {

  // Check Live Mode
  rpki_cfg_t* cfg = rpki_set_config(TEST_PROJECT_COLLECTOR, "0-0", 0, 0,
                                    NULL, NULL);
  CHECK_SUBSECTION("Live mode", 1, !test_rpki_status(cfg, "Live ", buf,result));

  // Check History Mode
  cfg = rpki_set_config(TEST_PROJECT_COLLECTOR, TEST_HISTORY_TIMEWDW, 0, 1,
                        NULL, NULL);
  CHECK_SUBSECTION("History mode", 0, !test_rpki_status(cfg, "History ", buf,
                   result));
  cfg_destroy(cfg);

  return 0;
}

int main()
{
  char buf[TEST_BUF_LEN];
  char result[TEST_BUF_LEN];

  CHECK_SECTION("RPKI", !test_rpki(buf, result));

  return 0;
}
