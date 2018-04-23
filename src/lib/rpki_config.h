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

#ifndef __CONFIG_H
#define __CONFIG_H

#include <stdint.h>

#include "khash.h"
#include "constants.h"
#include "rtrlib/rtrlib.h"

/** Initialising the Broker result khash */
KHASH_INIT(broker_result, khint64_t, char *, 1, kh_int64_hash_func,
           kh_int64_hash_equal)

/** A RPKI broker result object */
typedef struct struct_config_broker_t {

  /** Broker URL
   *
   * Broker URL (default or passed)
   */
  char broker_url[BROKER_MAX_MAIN_URL_LEN];

  /** Info URL
   *
   * Info URL
   */
  char info_url[BROKER_MAX_MAIN_URL_LEN];

  /** Info host
   *
   * Info host - response of the broker info server
   */
  char info_host[BROKER_INFO_RESP_URL_LEN];

  /** Info port
   *
   * Info port - response of the broker info server
   */
  char info_port[BROKER_INFO_RESP_PORT_LEN];

  /** Broker result khash
   *
   * Broker result hashtable (UTC epoch timestamp -> ROA-URLS)
   */
  khash_t(broker_result) * broker_kh;

  /** Broker khash count
   *
   * Number of different entries in the broker khash
   */
  int broker_khash_count;

  /** Broker khash used
   *
   * Number of entries in the broker khash already used for validation
   */
  int broker_khash_used;

  /** Broker khash init
   *
   * Initialization status of the broker result hashtable
   */
  int broker_khash_init;

  /** Configuration ROA URLs
   *
   * All ROA URLs - response of the broker service
   */
  char **roa_urls;

  /** ROA URLs count
   *
   * Number of initialized ROA URLs
   */
  int roa_urls_count;

} config_broker_t;

/** A RPKI config input object */
typedef struct struct_config_input_t {

  /** Configuration mode
   *
   * Indicator for a live or historical validation (0 = Live, 1 = Historical)
   */
  int mode;

  /** Unified or discrete flag
   *
   * Indicator for an unified or discrete validation (0 = discrete, 1 = unified)
   */
  int unified;

  /** SSH options
   *
   * SSH user, SSH privkey, SSH hostkey
   */
  char ssh_options[MAX_INPUT_LENGTH];

  /** Projects
   *
   * All projects (stored separately) for the validation output 
   */
  char projects[MAX_RPKI_COUNT][MAX_INPUT_LENGTH];

  /** Projects for broker request
   *
   * All projects concatenated for the broker request
   */
  char broker_projects[MAX_RPKI_COUNT * MAX_INPUT_LENGTH];

  /** Projects count
   *
   * Number of projects
   */
  int projects_count;

  /** Collectors
   *
   * All collector (stored separately) for the validation output 
   */
  char collectors[MAX_RPKI_COUNT][MAX_INPUT_LENGTH];

  /** Collectors for broker request
   *
   * All collectors concatenated for the broker request
   */
  char broker_collectors[MAX_RPKI_COUNT * MAX_INPUT_LENGTH];

  /** Collectors count
   *
   * Number of collectors
   */
  int collectors_count;

  /** Input Intervals
   *
   * All time intervals (two consecutive values form an interval)
   */
  uint32_t intervals[MAX_TIME_WINDOWS];

  /** Broker intervals
   *
   * All time intervals concatenated for the broker request
   */
  char broker_intervals[MAX_TIME_WINDOWS * MAX_INPUT_LENGTH];

  /** Configuration time intervals count
   *
   * Time interval count
   */
  int intervals_count;

} config_input_t;

/** A RPKI config time object */
typedef struct struct_config_time_t {

  /** Timestamp of the current ROA file
  *
  * Timestamp of the current ROA file (UTC epoch timestamp)
  */
  uint32_t current_roa_timestamp;

  /** Timestamp of the next ROA file
  *
  * Timestamp of the next ROA file (UTC epoch timestamp)
  */
  uint32_t next_roa_timestamp;

  /** Start timestamp
   *
   * First timestamp of the broker response
   */
  uint32_t start;

  /** Max end timestamp
   *
   * Latest timestamp of the broker response
   */
  uint32_t max_end;

  /** Current gap
   *
   * If there are no ROA files for a period of time -> flag is set
   */
  int current_gap;

} config_time_t;

/** A RPKI config for RTRLib object */
typedef struct struct_config_rtr_t {

  /** All prefix tables for the current RPKI elem
   *
   * Prefix-tables used for unified or discrete validation
   */
  struct pfx_table *pfxt;

  /** Prefix table count
   *
   * Number of prefix tables used for unified or discrete validation
   */
  int pfxt_count;

  /** Active prefix table flags (existing ROA files)
   *
   * Whether a collector has a matching ROA file and prefix table
   */
  int pfxt_active[MAX_RPKI_COUNT];

  /** RTR manager configuration of the RTRLib
   *
   * Pointer to the RTR manager configuration of the RTRLib
   */
  struct rtr_mgr_config *rtr_mgr_cfg;

  /** RTR socket of the RTRLib
   *
   * Pointer to the RTR socket of the RTRLib
   */
  struct rtr_socket *rtr_socket;

  /** Allocations for the RTR manager configuration of the RTRLib
   *
   * Allocations made for the RTR manager
   */
  void *rtr_allocs[3];

  /** SSH user - to close the RTR socket properly
   *
   * SSH user
   */
  char ssh_user[MAX_SSH_LEN];

  /** SSH hostkey - to close the RTR socket properly
   *
   * SSH hostkey
   */
  char ssh_hostkey[MAX_SSH_LEN];

  /** SSH privkey - to close the RTR socket properly
   *
   * SSH privkey
   */
  char ssh_privkey[MAX_SSH_LEN];

} config_rtr_t;

/** A RPKI Configuration object */
typedef struct struct_rpki_config_t {

  /** Config Broker */
  config_broker_t cfg_broker;

  /** Config input */
  config_input_t cfg_input;

  /** Config Timestamps */
  config_time_t cfg_time;

  /** Prefix tables and RTR configuration */
  config_rtr_t cfg_rtr;

} rpki_cfg_t;

/** Create a configuration for the RPKI validation
 *
 * @param[in] projects        All projects (comma-separated list)
 * @param[in] collectors      All collectors (comma-separated list)
 * @param[in] time_intervals  Time intervals as UTC epoch timestamps
 * @param[in] unified         Distinct validation (0) or unified validation (1) 
 * @param[in] mode            Mode of the validation - live (0) or historical (1)
 * @param[in] broker_url      Broker url if it another broker should be used
 * @param[in] ssh_options     SSH user, SSH hostkey, SSH privkey
 * @return                    Pointer to RPKI configuration
 */
rpki_cfg_t *cfg_create(char *projects, char *collectors, char *time_intervals,
                       int unified, int mode, char *broker_url,
                       char *ssh_options);

/** Destroy a configuration after the RPKI validation finished
 *
 * @param[in] cfg            Pointer to the configuration struct
 * @return                   0 if the config was destroyed, otherwise -1
 */
int cfg_destroy(rpki_cfg_t *cfg);

/** Get the current and next timestamp and the matching ROA URLs for the current
 * timestamp
 *
 * @param[in]  cfg           Pointer to the configuration struct
 * @param[in]  timestamp     Timestamp which will be searched
 * @param[out] dest          Pointer to the URL buffer where URLs will be saved
 * @return                   0 if the timestamps and the URLs were selected, 
 *                           otherwise -1
 */
int cfg_get_timestamps(rpki_cfg_t *cfg, uint32_t timestamp, char *dest);

/** Get the next timestamp if there is any (or end of time interval)
 *
 * @param[in] cfg            Pointer to the configuration struct
 * @param[in] current_ts     Current active timestamp (key of the broker Kh)
 * @return                   Next timestamp, 0 if there is no ROA file
 */
uint32_t cfg_next_timestamp(rpki_cfg_t *cfg, uint32_t current_ts);

/** Parse a string containing different ROA URLs and parse the corresponding
 * files
 *
 * @param[in] cfg            Pointer to the configuration struct
 * @param[in] url            String containing ROA URLs (delimiter: ",")
 * @return                   0 if the parsing was valid, otherwise -1
 */
int cfg_parse_urls(rpki_cfg_t *cfg, char *url);

/** Parse a ROA file and import all records to a prefix table
 *
 * @param[in]  roa_file      Path to the ROA file which will be imported
 * @param[out] pfxt          Corresponding Prefix Table
 * @return                   0 if the import was successful, otherwise -1
 */
int cfg_import_roa_file(char *roa_path, struct pfx_table *pfxt);

/** Add an ROA record of a ROA file to the prefix table
 *
 * @param[in]  asn           ASN value of the ROA record
 * @param[in]  address       IP adress of the prefix of the ROA record
 * @param[in]  min_len       Min length of the prefix of the ROA record
 * @param[in]  max_len       Max length of the prefix of the ROA record
 * @param[out] pfxt          Prefix table to which the record is added
 * @return                   0 if the add-process was valid, otherwise -1
 */
int cfg_add_record_to_pfx_table(uint32_t asn, char *address, uint8_t min_len,
                                uint8_t max_len, struct pfx_table *pfxt);

#endif /* __CONFIG_H */
