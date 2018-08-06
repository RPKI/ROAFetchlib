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

#include <arpa/inet.h>
#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "utils.h"
#include "constants.h"
#include "debug.h"
#include "validation.h"
#include "rpki_config.h"
#include "wandio.h"

rpki_cfg_t *cfg_create(char *projects_collectors, char *time_intervals,
                       int unified, int mode, char *broker_url,
                       char *ssh_options)
{
  /* Create Config */
  rpki_cfg_t *cfg = NULL;
  if ((cfg = (rpki_cfg_t *)malloc(sizeof(rpki_cfg_t))) == NULL) {
    return NULL;
  } else {
    memset(cfg, 0, sizeof(rpki_cfg_t));
  }

  /* Set the Broker request URL for the default and info service */
  utils_cfg_set_broker_urls(cfg, broker_url);

  /* Allocate memory for the ROA URLs */
  config_broker_t *broker = &cfg->cfg_broker;
  broker->roa_urls_count = BROKER_ROA_URLS_COUNT;
  broker->roa_urls = malloc(BROKER_ROA_URLS_COUNT * sizeof(char *));
  for (int i = 0; i < BROKER_ROA_URLS_COUNT; i++) {
    broker->roa_urls[i] = malloc(BROKER_ROA_URLS_LEN);
  }
  broker->broker_khash_init = 0;

  /* Allocate memory for the Prefix Tables */
  config_validation_t *val = &cfg->cfg_val;
  val->pfxt = NULL;
  val->pfxt = malloc(MAX_RPKI_COUNT * sizeof(struct pfx_table));
  for (int i = 0; i < MAX_RPKI_COUNT; i++) {
    pfx_table_init(&val->pfxt[i], NULL);
  }
  val->pfxt_count = 0;

  /* Set up the RTR manager config */
  val->rtr_mgr_cfg = NULL;
  for (int i = 0; i < sizeof(val->rtr_allocs)/sizeof(val->rtr_allocs[0]); i++) {
    val->rtr_allocs[i] = 0;
  }

  /* Check and add the ssh options and flags */
  if(utils_cfg_check_ssh_options(cfg, ssh_options) != 0) {
    return NULL;
  }
  if(utils_cfg_check_flags(cfg, unified, mode) != 0) {
    return NULL;
  }

  /* Check and add the intervals */
  if(utils_cfg_check_intervals(cfg, time_intervals) != 0) {
    cfg_destroy(cfg);
    return NULL;
  }

  /* Check and add the projects and collectors */
  if(utils_cfg_check_collectors(cfg, projects_collectors, mode) != 0) {
    cfg_destroy(cfg);
    return NULL;
  }

  return cfg;
}

int cfg_destroy(rpki_cfg_t *cfg)
{

  if (cfg == NULL) {
    return -1;
  }

  /* Destroy the ROA URLs */
  config_broker_t *broker = &cfg->cfg_broker;
  for (int i = 0; i < broker->roa_urls_count; i++) {
    free(broker->roa_urls[i]);
  }
  free(broker->roa_urls);

  /* Destroy the Prefix Tables */
  for (int i = 0; i < MAX_RPKI_COUNT; i++) {
    pfx_table_free(&cfg->cfg_val.pfxt[i]);
  }
  //pfx_table_free(cfg->cfg_val.pfxt);
  free(cfg->cfg_val.pfxt);

  /* Destroy the KHASH */
  kh_destroy(broker_result, broker->broker_kh);
  broker->broker_kh = NULL;

  /* Destroy the live connection to the RTR socket */
  if (!cfg->cfg_input.mode) {
    validation_close_connection(cfg);
  }

  free(cfg);

  return 0;
}

int cfg_get_timestamps(rpki_cfg_t *cfg, uint32_t timestamp, char *dest)
{
  /* Get the current timestamp by downgrading the timestamp minute-wise */
  khash_t(broker_result) *broker_kh = cfg->cfg_broker.broker_kh;
  uint32_t current_ts = timestamp - (timestamp % 60);
  while (kh_get(broker_result, broker_kh, current_ts) == kh_end(broker_kh)) {
    current_ts -= 60;
  }
  strcpy(dest, kh_value(broker_kh, kh_get(broker_result,broker_kh,current_ts)));
  cfg->cfg_time.current_roa_timestamp = current_ts;

  /* If there are more than one entry in the hash table */
  if (kh_size(broker_kh) > 1) {
    cfg->cfg_time.next_roa_timestamp = cfg_next_timestamp(cfg, current_ts);
  } else {
    cfg->cfg_time.next_roa_timestamp = 0;
  }

  return 0;
}

uint32_t cfg_next_timestamp(rpki_cfg_t *cfg, uint32_t current_ts)
{
  uint32_t next_ts;
  config_broker_t *broker = &cfg->cfg_broker;
  khash_t(broker_result) *broker_kh = cfg->cfg_broker.broker_kh;

  /* If there are more than the current timestamp left, get the next one */
  if ((broker->broker_khash_count - broker->broker_khash_used) > 1) {
    next_ts = current_ts + ROA_ARCHIVE_INTERVAL;
    while (kh_get(broker_result, broker_kh, next_ts) == kh_end(broker_kh)) {
      next_ts += ROA_ARCHIVE_INTERVAL;
    }

  /* If no timestamp is left, set the next timestamp to 0 */
  } else {
    next_ts = 0;
  }

  return next_ts;
}

int cfg_parse_urls(rpki_cfg_t *cfg, char *url)
{

  /* Clean all Prefix Tables and flags before parsing new URLs */
  config_validation_t *val = &cfg->cfg_val;
  memset(val->pfxt_active, 0, sizeof(val->pfxt_active));
  for (int i = 0; i < MAX_RPKI_COUNT; i++) {
    pfx_table_src_remove(&val->pfxt[i], NULL);
  }
  val->pfxt_count = 0;

  /* Split the URL string in chunks and import the matching ROA file */
  config_input_t *input = &cfg->cfg_input;
  char *end_roa_arg; char *urls = strdup(url);
  char *roa_arg = strtok_r(urls, ",", &end_roa_arg);
  while (roa_arg != NULL) {

    /* If the broker passed an URL (ROA dump) for the current collector import 
       the ROA dump and set the Prefix Table as active */
    if (strlen(roa_arg) > 1) {
      if (!strstr(roa_arg, input->collectors[val->pfxt_count])) {
        std_print("%s", "The order of the URLs is wrong\n");
        std_print("%s %s\n", roa_arg, input->collectors[val->pfxt_count]);
        return -1;
      }
      /* If unified flag isn't set, import ROA dumps in diff. Prefix Tables else
         import all ROA dumps in a single Prefix Table */
      if (!input->unified) {
        if (cfg_import_roa_file(roa_arg, &val->pfxt[val->pfxt_count]) != 0) {
          return -1;
        }
      } else {
        if (cfg_import_roa_file(roa_arg, &val->pfxt[0]) != 0) {
          return -1;
        }
      }
      val->pfxt_active[val->pfxt_count] = 1;
      val->pfxt_count++;

    /* If the broker didn't pass an URL the Prefix Table is empty and skipped */
    } else {
      val->pfxt_active[val->pfxt_count] = 0;
      val->pfxt_count++;
    }
    roa_arg = strtok_r(NULL, ",", &end_roa_arg);
  }
  free(urls);

  return 0;
}
int cfg_import_roa_file(char *roa_path, struct pfx_table *pfxt)
{

  /* Read the whole ROA dump in, reallocate memory if necessary and store it 
     in `roa_file` */
  char *roa_file = NULL;
  int length = 0, ret = 0;
  char *buf = (char *) malloc(BROKER_ROA_DUMP_BUFLEN);
  io_t *file_io = wandio_create(roa_path);
  if (file_io == NULL) {
    std_print("Error: Could not open %s for reading\n", roa_path);
    return -1;
  }
  while (1) {
    ret = wandio_read(file_io, buf, BROKER_ROA_DUMP_BUFLEN);
    if (ret < 0) {
      std_print("%s", "Error: Could not read ROA file from broker\n");
      return -1;
    }
    if (!ret) {
      break;
    }
    if (!(roa_file = realloc(roa_file, length + ret + 2))) {
      std_print("%s", "Error: Could not realloc roa string\n");
      return -1;
    }
    strncpy(roa_file + length, buf, ret);
    length += ret;
  }
  roa_file[length] = '\0';
  free(buf);
  wandio_destroy(file_io);

  /* Check whether the ROA dump contains header information, abort if not */
  if(strstr(roa_file, "ASN,IP Prefix,Max Length") == NULL && 
     strstr(roa_file, "ASN,IP Prefix,Max Length,Trust Anchor") == NULL) {
    std_print("%s", "Error: Could not read ROA file from broker\n");
    return -1;
  }
  
  /* Check whether the ROA dump format contains the trustanchor information */
  int roa_fields_cnt = 0, line_cnt = 0, dbg_line = 0;
  for (size_t i = 0; i < strcspn(roa_file, "\n"); i++) {
    roa_fields_cnt += (roa_file[i] == ',' ? 1 : 0);
  }
  roa_fields_cnt++;

  /* Skip the header */
  char *arg_end = NULL;
  char *arg = strtok_r(roa_file, ",\n", &arg_end);
  for (size_t i = 0; i < roa_fields_cnt; i++) {
    arg = strtok_r(NULL, ",\n", &arg_end);
  }

  /* Parse the ROA file and add every record to the prefix table
     Format: ASN,IP Prefix,Max Length(, Trustanchor)?*/
  uint32_t asn = 0;
  uint8_t min_len = 0, max_len = 0;
  char addr[INET6_ADDRSTRLEN] = {0};
  while (arg != NULL) {
    switch (line_cnt) {
    case 0:
      /* Bypass the different notations for the ASN (e.g. 718 || AS718) */
      if (strstr(arg, "AS")) {
        if (utils_cfg_validity_check_val(arg + strlen("AS"), &asn, 32) != 0) {
          return -1;
        }
      } else if (utils_cfg_validity_check_val(arg, &asn, 32) != 0) {
        return -1;
      }
      break;
    case 1:
      if (utils_cfg_validity_check_prefix(arg, addr, &min_len) != 0) {
        return -1;
      }
      break;
    case 2:
      if (utils_cfg_validity_check_val(arg, &max_len, 8) != 0) {
        return -1;
      }
      break;
    case 3:
      break;
    }

    /* Add record if all fields were extracted and are valid */
    if (line_cnt == roa_fields_cnt - 1) {
      line_cnt = 0;
      dbg_line++;
      if (cfg_add_record_to_pfx_table(asn, addr, min_len, max_len, pfxt) != 0) {
        std_print("Error: Record is corrupt at line: %i\n",
                  dbg_line / roa_fields_cnt);
        return -1;
      }
    } else {
      line_cnt++;
      dbg_line++;
    }
    arg = strtok_r(NULL, ",\n", &arg_end);
  }

  debug_print("Imported ROA dump: %s\n", roa_path);
  free(roa_file);

  return 0;
}

int cfg_add_record_to_pfx_table(uint32_t asn, char *address, uint8_t min_len,
                                uint8_t max_len, struct pfx_table *pfxt)
{

  struct pfx_record pfx;

  /* Check if the IP address could be interpreted by the RTRlib */
  if (lrtr_ip_str_to_addr(address, &pfx.prefix) != 0) {
    std_print("%s", "Error: Address not interpretable\n");
    return -1;
  }
  pfx.min_len = min_len;
  pfx.max_len = max_len;
  pfx.asn = asn;
  pfx.socket = NULL;

  /* Check if the record could be added by the RTRlib */
  if (pfx_table_add(pfxt, &pfx) == PFX_ERROR) {
    std_print("%s", "Error: Record could not be added\n");
    return -1;
  }

  return 0;
}
