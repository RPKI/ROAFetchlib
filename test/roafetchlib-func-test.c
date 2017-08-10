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

#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "roafetchlib.h"
#include "roafetchlib-func-test.h"

int print_test_results(char* test, char* type, int check) {

  if (!(check)) {
    fprintf(stderr, "Test: %s%s ... FAILED\n", type, test);
    return 1;
  }
  fprintf(stderr, "Test: %s%s ... OK\n", type, test);
  return 0;
}

int test_rpki(rpki_cfg_t *cfg, char* type, char* buf, char* result) {

  int rst = 0;
  rpki_validate(cfg, TEST_TIMESTAMP, TEST1_O_ASN, TEST1_PFX, TEST1_MSKL, result, TEST_BUF_LEN);
  snprintf(buf, TEST_BUF_LEN, "%s,%s,%s,%i,%s/%i-%i;", TEST_PROJECT, TEST_COLLECTOR, "valid",
           TEST1_O_ASN, TEST1_PFX, TEST1_MSKL, TEST1_MSKL);
  rst |= print_test_results("-RPKI-Testcase 1: Validation status for valid ROA Beacon", type, !strcmp(result, buf));

  rpki_validate(cfg, TEST_TIMESTAMP, TEST2_O_ASN, TEST2_PFX, TEST2_MSKL, result, TEST_BUF_LEN);
  snprintf(buf, TEST_BUF_LEN, "%s,%s,%s,%i,%s/%i-%i;", TEST_PROJECT, TEST_COLLECTOR, "valid",
           TEST2_O_ASN, TEST2_PFX, TEST2_MSKL, TEST2_MSKL);
  rst |= print_test_results("-RPKI-Testcase 2: Validation status for valid ROA Beacon", type, !strcmp(result, buf));

  rpki_validate(cfg, TEST_TIMESTAMP, TEST3_O_ASN, TEST3_PFX, TEST3_MSKL, result, TEST_BUF_LEN);
  snprintf(buf, TEST_BUF_LEN, "%s,%s,%s,%i,%s/%i-%i;", TEST_PROJECT, TEST_COLLECTOR, "invalid",
           TEST3_V_O_ASN, TEST3_PFX, TEST3_MSKL, TEST3_MSKL);
  rst |= print_test_results("-RPKI-Testcase 3: Validation status for invalid ROA Beacon", type, !strcmp(result, buf));

  rpki_validate(cfg, TEST_TIMESTAMP, TEST4_O_ASN, TEST4_PFX, TEST4_MSKL, result, TEST_BUF_LEN);
  snprintf(buf, TEST_BUF_LEN, "%s,%s,%s,%i,%s/%i-%i;", TEST_PROJECT, TEST_COLLECTOR, "invalid",
           TEST4_V_O_ASN, TEST4_PFX, TEST4_MSKL, TEST4_MSKL);
  rst |= print_test_results("-RPKI-Testcase 4: Validation status for invalid ROA Beacon", type, !strcmp(result, buf));

  rpki_validate(cfg, TEST_TIMESTAMP, TEST5_O_ASN, TEST5_PFX, TEST5_MSKL, result, TEST_BUF_LEN);
  snprintf(buf, TEST_BUF_LEN, "%s,%s,%s;", TEST_PROJECT, TEST_COLLECTOR, "notfound");
  rst |= print_test_results("-RPKI-Testcase 5: Validation status for non-existing ROA Beacon", type, !strcmp(result, buf));

  rpki_validate(cfg, TEST_TIMESTAMP, TEST6_O_ASN, TEST6_PFX, TEST6_MSKL, result, TEST_BUF_LEN);
  snprintf(buf, TEST_BUF_LEN, "%s,%s,%s;", TEST_PROJECT, TEST_COLLECTOR, "notfound");
  rst |= print_test_results("-RPKI-Testcase 6: Validation status for non-existing ROA Beacon", type, !strcmp(result, buf));
  return rst;
}

int main()
{
  int rst = 0;
  char buf[TEST_BUF_LEN];
  char result[TEST_BUF_LEN];

  // Check Live Validation
  rpki_cfg_t *cfg = rpki_set_config(TEST_PROJECT, TEST_COLLECTOR, "0-0", 0, 0, NULL, NULL);
  rst |= test_rpki(cfg, "Live", buf, result);
  cfg_destroy(cfg);

  // Check History Validation
  cfg = rpki_set_config(TEST_PROJECT, TEST_COLLECTOR, TEST_HISTORY_TIMEWDW, 0, 1, NULL, NULL);
  rst |= test_rpki(cfg, "History", buf, result);
  cfg_destroy(cfg);
  return rst;
}
