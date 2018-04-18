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

#include "roafetchlib-test-config.h"
#include "roafetchlib-test.h"

/** Utility functions **/
void cfg_print_pfxt(const struct pfx_record *pfx_record, void *data)
{
  char ip_pfx[INET6_ADDRSTRLEN];
  char rec[TEST_BUF_LEN];
  lrtr_ip_addr_to_str(&(*pfx_record).prefix, ip_pfx, sizeof(ip_pfx));
  snprintf(rec, sizeof(rec), "%" PRIu32 ",%s/%" PRIu8 ",%" PRIu8 "\n",
           (*pfx_record).asn, ip_pfx, (*pfx_record).min_len,
           (*pfx_record).max_len);
  strcat((char *)data, rec);
}

void cfg_print_record(const struct pfx_record *pfx_record, void *data)
{
  char ip_pfx[INET6_ADDRSTRLEN];
  lrtr_ip_addr_to_str(&(*pfx_record).prefix, ip_pfx, sizeof(ip_pfx));
  snprintf(data, TEST_BUF_LEN, "%" PRIu32 ",%s/%" PRIu8 ",%" PRIu8,
           (*pfx_record).asn, ip_pfx, (*pfx_record).min_len,
           (*pfx_record).max_len);
}

void create_dummy_broker_kh(rpki_cfg_t *cfg, uint32_t timestamps[], int size)
{
  config_broker_t *broker = &cfg->cfg_broker;
  broker->broker_kh = kh_init(broker_result);
  broker->broker_khash_init = 1;
  int kh_ret = 0;
  khiter_t k;

  for (int i = 0; i < size; i++) {
    k = kh_put(broker_result, broker->broker_kh, timestamps[i], &kh_ret);
    kh_val(broker->broker_kh, k) = '\0';
    kh_val(broker->broker_kh, k) = TEST_TS_URL[i];
    broker->broker_khash_count++;
  }
}
/** Utility functions - END **/

int test_rpki_config_validity_check_val(rpki_cfg_t *cfg)
{
  /** cfg_validity_check_val **/
  char testcase[TEST_BUF_LEN];
  int ret = -1;
  uint8_t rst_val_8 = 0;
  uint32_t rst_val_32 = 0;

  PRINT_SSECTION("UINT8_T");
  for (int i = 0; i < TEST_VAL_COUNT; i++) {
    if (i == TEST_VAL_COUNT - 1) {
      PRINT_INTENDED_ERR;
    }
    ret = cfg_validity_check_val(TEST_VAL_8[i], &rst_val_8, 8);
    snprintf(testcase, sizeof(testcase), "#%i - Validity check value: %s",
             i + 1, TEST_VAL_8[i]);
    CHECK_RESULT("", testcase,
                 rst_val_8 == TEST_VAL_RST_8[i] && i == TEST_VAL_COUNT - 1
                   ? ret == -1
                   : !ret);
  }
  PRINT_SSECTION("UINT32_T");
  for (int i = 0; i < TEST_VAL_COUNT; i++) {
    if (i == TEST_VAL_COUNT - 1) {
      PRINT_INTENDED_ERR;
    }
    ret = cfg_validity_check_val(TEST_VAL_32[i], &rst_val_32, 32);
    snprintf(testcase, sizeof(testcase), "#%i - Validity check value: %s",
             i + 1, TEST_VAL_32[i]);
    CHECK_RESULT("", testcase,
                 rst_val_32 == TEST_VAL_RST_32[i] && i == TEST_VAL_COUNT - 1
                   ? ret == -1
                   : !ret);
  }

  return 0;
}

int test_rpki_config_validity_check_prefix(rpki_cfg_t *cfg)
{
  /** cfg_validity_check_prefix **/
  char testcase[TEST_BUF_LEN] = {0};
  char address[TEST_BUF_LEN] = {0};
  int ret = -1;
  uint8_t min_len = 0;

  PRINT_SSECTION("IPv4");
  for (int i = 0; i < TEST_PFX_IPv4_COUNT; i++) {
    if (i > 2) {
      PRINT_INTENDED_ERR;
    }
    ret = cfg_validity_check_prefix(TEST_PFX_IPv4[i], &address[0], &min_len);
    snprintf(testcase, sizeof(testcase), "#%i - Prefix: %s", i + 1,
             TEST_PFX_IPv4[i]);
    if (i == TEST_PFX_IPv4_COUNT - 1) {
      CHECK_RESULT("", testcase, ret == -1);
    } else {
      CHECK_RESULT("", testcase, (i > 2 ? ret == -1 : !ret) &&
                                   !strcmp(address, TEST_PFX_ADDR_IPv4[i]) &&
                                   min_len == TEST_PFX_MINL_IPv4[i]);
    }
  }
  PRINT_SSECTION("IPv6");
  for (int i = 0; i < TEST_PFX_IPv6_COUNT; i++) {
    if (i > 2) {
      PRINT_INTENDED_ERR;
    }
    ret = cfg_validity_check_prefix(TEST_PFX_IPv6[i], &address[0], &min_len);
    snprintf(testcase, sizeof(testcase), "#%i - Prefix: %s", i + 1,
             TEST_PFX_IPv6[i]);
    if (i == TEST_PFX_IPv6_COUNT - 1) {
      CHECK_RESULT("", testcase, ret == -1);
    } else {
      CHECK_RESULT("", testcase, (i > 2 ? ret == -1 : !ret) &&
                                   !strcmp(address, TEST_PFX_ADDR_IPv6[i]) &&
                                   min_len == TEST_PFX_MINL_IPv6[i]);
    }
  }

  return 0;
}

int test_rpki_config_import_roa_file(rpki_cfg_t *cfg)
{
  /** cfg_import_roa_file **/
  char ip_v4[TEST_BUF_LEN] = {0};
  char ip_v6[TEST_BUF_LEN] = {0};
  char ip_v4_s[TEST_BUF_LEN] = {0};
  char ip_v6_s[TEST_BUF_LEN] = {0};
  struct pfx_table *pfxt = NULL;
  pfxt = malloc(sizeof(struct pfx_table));
  pfx_table_init(pfxt, NULL);

  int ret = cfg_import_roa_file(TEST_IMP_URL, pfxt);

  pfx_table_for_each_ipv4_record(pfxt, cfg_print_pfxt, &ip_v4);
  elem_sort_result(ip_v4, TEST_BUF_LEN, ip_v4_s, "\n");
  CHECK_RESULT("", "Import all IPv4 ROA Records",
               !strcmp(TEST_IMP_IPv4, ip_v4_s) && !ret);

  pfx_table_for_each_ipv6_record(pfxt, cfg_print_pfxt, &ip_v6);
  elem_sort_result(ip_v6, TEST_BUF_LEN, ip_v6_s, "\n");
  CHECK_RESULT("", "Import all IPv6 ROA Records",
               !strcmp(TEST_IMP_IPv6, ip_v6_s) && !ret);

  return 0;
}

int test_rpki_config_add_input_to_cfg()
{
  /** add_input_to_cfg **/
  char projects[MAX_RPKI_COUNT][MAX_INPUT_LENGTH] = {0};
  char concat_projects[MAX_RPKI_COUNT * MAX_INPUT_LENGTH] = {0};
  int cnt = 0;
  char testcase[TEST_BUF_LEN];
  char rst[TEST_BUF_LEN] = {0};

  for (int i = 0; i < TEST_ADD_INP_COUNT; i++) {
    if (i >= TEST_ADD_INP_COUNT - 3) {
      PRINT_INTENDED_ERR;
    }
    cnt = add_input_to_cfg(TEST_ADD_INP_PROJ[i], TEST_BUF_LEN, MAX_INPUT_LENGTH,
                           MAX_RPKI_COUNT, concat_projects, projects, ", ");
    snprintf(testcase, sizeof(testcase), "#%i - Count added input elements",
             i + 1);
    CHECK_RESULT("", testcase, cnt == TEST_ADD_INP_CNT[i]);

    if (i < TEST_ADD_INP_COUNT - 3) {
      int chk = 0;
      for (int j = 0; j < cnt; j++) {
        snprintf(rst, sizeof(rst), "PJ%i", j + 1);
        chk += strcmp(rst, projects[j]);
      }
      snprintf(testcase, sizeof(testcase),
               "#%i - Addition of input to cfg (values)", i + 1);
      CHECK_RESULT("", testcase, !chk &&
									 !strcmp(TEST_ADD_INP_BROKER_PROJ[i], concat_projects));
    }
  }

  return 0;
}

int test_rpki_config_add_record_to_pfx_table()
{
  /** cfg_add_record_to_pfx_table **/
  char rst[TEST_BUF_LEN];
  char rec[TEST_BUF_LEN];
  char testcase[TEST_BUF_LEN];
  struct pfx_table *pfxt = NULL;
  pfxt = malloc(sizeof(struct pfx_table));
  pfx_table_init(pfxt, NULL);
  int ret = TEST_BUF_LEN;

  for (int i = 0; i < TEST_ADD_COUNT; i++) {
    memset(&rst[0], 0, sizeof(rst));
    snprintf(rec, sizeof(rec), "%s", TEST_ADD_REC[i]);

    if (i == TEST_ADD_COUNT - 1) {
      PRINT_INTENDED_ERR;
      ret =
        cfg_add_record_to_pfx_table(TEST_ADD_ASN[i], TEST_ADD_ADDR[i],
                                    TEST_ADD_MINL[i], TEST_ADD_MAXL[i], pfxt);
      snprintf(testcase, sizeof(testcase),
               "#%i - Do not add incomplete records", i + 1);
      CHECK_RESULT("", testcase, ret == -1);
      return 0;
    }
    ret = cfg_add_record_to_pfx_table(TEST_ADD_ASN[i], TEST_ADD_ADDR[i],
                                      TEST_ADD_MINL[i], TEST_ADD_MAXL[i], pfxt);
    pfx_table_for_each_ipv4_record(pfxt, cfg_print_record, &rst);
    if (strlen(rst) > 0) {
      snprintf(testcase, sizeof(testcase), "#%i - Add %s", i + 1, rst);
      CHECK_RESULT("", testcase, !strcmp(rec, rst) && !ret);
    } else {
      pfx_table_for_each_ipv6_record(pfxt, cfg_print_record, &rst);
      snprintf(testcase, sizeof(testcase), "#%i - Add %s", i + 1, rst);
      CHECK_RESULT("", testcase, !strcmp(rec, rst) && !ret);
    }
    pfx_table_src_remove(pfxt, NULL);
  }
  return 0;
}

int test_rpki_config_next_timestamp(rpki_cfg_t *cfg)
{
  /* cfg_next_timestamp */
  config_time_t *cfg_time = &cfg->cfg_time;
  char testcase[TEST_BUF_LEN];
  int skipped_cnt = TEST_TS_COUNT - 2;
  create_dummy_broker_kh(cfg, TEST_TS_NTS, skipped_cnt);
  for (int i = 0; i < skipped_cnt; i++) {
    cfg->cfg_broker.broker_khash_used++;
    cfg_time->next_roa_timestamp = cfg_next_timestamp(cfg, TEST_TS_NTS[i]);
    snprintf(testcase, sizeof(testcase), "#%i - %10" PRIu32 " - Next timestamp",
             i + 1, TEST_TS_NTS[i]);
    if (i < skipped_cnt - 1) {
      CHECK_RESULT("", testcase,
                   cfg_time->next_roa_timestamp == TEST_TS_NTS[i + 1]);
    } else {
      CHECK_RESULT("", testcase, cfg_time->next_roa_timestamp == 0);
    }
  }
  return 0;
}

int test_rpki_config_get_timestamps(rpki_cfg_t *cfg)
{
  /* cfg_get_timestamps */
  config_time_t *cfg_time = &cfg->cfg_time;
  char url[TEST_BUF_LEN];
  char testcase[TEST_BUF_LEN];
  char rst_url[TEST_BUF_LEN];

  create_dummy_broker_kh(cfg, TEST_TS, TEST_TS_COUNT);

  for (int i = 0; i < TEST_TS_COUNT; i++) {
    cfg->cfg_broker.broker_khash_used++;
    cfg_get_timestamps(cfg, TEST_TS[i], url);
    snprintf(testcase, sizeof(testcase), "#%i - %10" PRIu32 " - URL", i + 1,
             TEST_TS[i]);
    snprintf(rst_url, sizeof(rst_url), "%s", TEST_TS_URL[i]);
    CHECK_RESULT("", testcase, !strcmp(url, rst_url));
    snprintf(testcase, sizeof(testcase),
             "#%i - %10" PRIu32 " - Current timestamp", i + 1, TEST_TS[i]);
    CHECK_RESULT("", testcase, cfg_time->current_roa_timestamp == TEST_TS[i]);
    snprintf(testcase, sizeof(testcase), "#%i - %10" PRIu32 " - Next timestamp",
             i + 1, TEST_TS[i]);
    if (i < TEST_TS_COUNT - 1) {
      CHECK_RESULT("", testcase,
                   cfg_time->next_roa_timestamp == TEST_TS[i + 1]);
    } else {
      CHECK_RESULT("", testcase, cfg_time->next_roa_timestamp == 0);
    }
  }
  return 0;
}

int test_rpki_config()
{
  // Build up the configuration
  rpki_cfg_t *cfg = rpki_set_config(TEST_TS_PROJECT, TEST_TS_COLLECTOR,
                                    TEST_TS_HISTORY_TIMEWDW, 0, 1, NULL, NULL);

  CHECK_SUBSECTION("Addition of input to config", 1,
                   !test_rpki_config_add_input_to_cfg());

  CHECK_SUBSECTION("Import ROA dump file", 0,
                   !test_rpki_config_import_roa_file(cfg));

  CHECK_SUBSECTION("Validity check for prefix of a ROA record", 0,
                   !test_rpki_config_validity_check_prefix(cfg));

  CHECK_SUBSECTION("Validity check for ASN/MIN-LEN of a ROA record", 0,
                   !test_rpki_config_validity_check_val(cfg));

  CHECK_SUBSECTION("Addition of a ROA record to the prefix table", 0,
                   !test_rpki_config_add_record_to_pfx_table());

  CHECK_SUBSECTION("Next Timestamp Determination (skipped)", 0,
                   !test_rpki_config_next_timestamp(cfg));

  CHECK_SUBSECTION("Current & Next Timestamp Determination", 0,
                   !test_rpki_config_get_timestamps(cfg));

  cfg_destroy(cfg);

  return 0;
}

int main()
{
  CHECK_SECTION("Configuration", !test_rpki_config());

  return 0;
}
