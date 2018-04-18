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

#ifndef __CONFIG_H
#define __CONFIG_H

#include <stdint.h>

#include "constants.h"
#include "khash.h"
#include "rtrlib/rtrlib.h"

/** Initialising the Broker result khash */
KHASH_INIT(broker_result, khint64_t, char*, 1, kh_int64_hash_func, kh_int64_hash_equal)


/** A RPKI broker result object */
typedef struct struct_config_broker_t {

  /** RPKI Broker URL
   *
   * RPKI Broker url 
   */
  char broker_url[BROKER_MAX_MAIN_URL_LEN];

  /** RPKI Info URL
   *
   * RPKI Info url 
   */
  char info_url[BROKER_MAX_MAIN_URL_LEN];

  /** RPKI Info host
   *
   * RPKI Info host 
   */
  char info_host[BROKER_INFO_RESP_URL_LEN];

  /** RPKI Info port
   *
   * RPKI Info port 
   */
  char info_port[BROKER_INFO_RESP_PORT_LEN];

  /** RPKI Broker result khash
   *
   * RPKI Broker result hashtable (UTC epoch timestamp -> ROA-URLS)
   */
  khash_t(broker_result) *broker_kh;

  /** RPKI broker khash count
   *
   * Number of different entries in the RPKI broker khash
   */
  int broker_khash_count;

  /** RPKI broker khash count
   *
   * Number of entries in the RPKI broker khash which are already used
   */
  int broker_khash_used;

  /** RPKI broker khash init
   *
   * Initialization status of the broker result hashtable
   */
  int broker_khash_init;

  /** RPKI configuration ROA URLs
   *
   * RPKI configuration ROA URLS
   */
  char** roa_urls;

  /** Number of initialized ROA URLs
   *
   * Number of initialized ROA URLs
   */
  int roa_urls_count;

} config_broker_t;

/** A RPKI config input object */
typedef struct struct_config_input_t {

  /** RPKI configuration time
   *
   * Time window
   */
  char time_intervals[MAX_TIME_WINDOWS * MAX_INPUT_LENGTH];

  /** RPKI configuration time windows
   *
   * Time windows
   */
  uint32_t time_intervals_window[MAX_TIME_WINDOWS];

  /** RPKI configuration time intervals count
   *
   * Time intervl count
   */
  int time_intervals_count;

  /** RPKI configuration mode
   *
   * 0 = Live, 1 = Historical
   */
  int mode;

  /** RPKI unified or discrete validation
   *
   * RPKI unified or discrete validation indicator
   */
  int unified;

  /** RPKI SSH options
   *
   * SSH user, SSH privkey, SSH hostkey
   */
  char ssh_options[MAX_INPUT_LENGTH];

  /** RPKI validation projects count
   *
   * Number of RPKI validation projects
   */
  int projects_count;

  /** RPKI validation projects
   *
   * RPKI project names for distinct validation ouput
   */
  char projects[MAX_RPKI_COUNT][MAX_INPUT_LENGTH];

  /** RPKI validation projects for broker request
   *
   * RPKI validation projects for broker request
   */
  char broker_projects[MAX_RPKI_COUNT * MAX_INPUT_LENGTH];

  /** RPKI validation collectors count
   *
   * Number of RPKI validation collectors
   */
  int collectors_count;

  /** RPKI validation collectors
   *
   * RPKI collector names for distinct validation ouput
   */
  char collectors[MAX_RPKI_COUNT][MAX_INPUT_LENGTH];

  /** RPKI validation collectors for broker request
   *
   * RPKI validation collectors for broker request
   */
  char broker_collectors[MAX_RPKI_COUNT * MAX_INPUT_LENGTH];

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

  /** RPKI Start
   *
   * First timestamp of the broker response
   */
  uint32_t start;

  /** RPKI max end
   *
   * Latest timestamp of the broker response
   */
  uint32_t max_end;

    /** RPKI current gap
   *
   * If there are no ROA files for a gap in the interval -> flag is set
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

  /** RPKI validation prefix-table count
   *
   * Number of prefix-tables used for unified or discrete validation
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
  struct rtr_mgr_config* rtr_mgr_cfg;

  /** RTR socket of the RTRLib
   *
   * Pointer to the RTR socket of the RTRLib
   */
	struct rtr_socket* rtr_socket;

  /** Allocations for the RTR manager configuration of the RTRLib
   *
   * Allocations made for the RTR manager
   */
  void* rtr_allocs[3];

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

/** Creates a configuration for the RPKI validation
 *
 * @param projects        All RPKI projects   (comma-separated list)
 * @param collectors      All RPKI collectors (comma-separated list)
 * @param time_intervals  Time intervals as UTC epoch timestamps (start_1,end_1[;<start_n>,<end_n>]*)
 * @param unified         Whether the validation should validate unified (1) or distinct (0)
 * @param mode            Mode of the current validation - live (0) or historical (1)
 * @param broker_url      RPKI broker url
 * @param ssh_options     SSH user, SSH hostkey, SSH privkey
 * @return                Pointer to rpki configuration
 */
rpki_cfg_t* cfg_create(char* projects, char* collectors, char* time_intervals,
                       int unified, int mode, char* broker_url, char* ssh_options);


/** Destroys a configuration after the RPKI validation finished
 *
 * @param cfg             Pointer to the configuration struct
 * @return                0 if the config was destroyes, otherwise -1
 */
int cfg_destroy(rpki_cfg_t *cfg);


/** Get the current and next timestamp and the matching ROA URLs for the current timestamp
 *
 * @param cfg             Pointer to the configuration struct
 * @param timestamp       Timestamp which will be searched
 * @param dest            Pointer to the URL buffer where the urls will be saved
 * @return                0 if the timestamps and the URLs were valid, otherwise -1
 */
int cfg_get_timestamps(rpki_cfg_t *cfg, uint32_t timestamp, char* dest);


/** Get the next timestamp if there is any (or end of time interval)
 *
 * @param cfg             Pointer to the configuration struct
 * @param current_ts      Current active timestamp (key of the broker Kh)
 * @return                Next timestamp, 0 if there is no ROA file
 */
uint32_t cfg_next_timestamp(rpki_cfg_t* cfg, uint32_t current_ts);


/** Parse a string containing different ROA URLs and parse the corresponding files
 *
 * @param cfg             Pointer to the configuration struct
 * @param url             String containing ROA URLs (delimiter:",")
 * @return                0 if the parsing was valid, otherwise -1
 */
int cfg_parse_urls(rpki_cfg_t *cfg, char* url);


/** Parse a ROA file and import all records to a prefix table
 *
 * @param roa_file        Path to the roa file which will be imported
 * @param pfxt            Corresponding prefix table
 * @return                0 if the import was valid, otherwise -1
 */
int cfg_import_roa_file(char *roa_path, struct pfx_table* pfxt);


/** Print a pfx_record
 *
 * @param pfx_record      Pfx_record which will be printed
 * @param data            Call-back function
 */
void cfg_print_record(const struct pfx_record *pfx_record, void *data);


/** Add an ROA record of a ROA file to the prefix table
 *
 * @param asn             ASN value of the ROA record
 * @param address         IP adress of the prefix of the ROA record
 * @param min_len         min length of the prefix of the ROA record
 * @param max_len         max length of the prefix of the ROA record
 * @param pfxt            prefix table to which the record is added
 * @return                0 if the add process was valid, otherwise -1
 */
int cfg_add_record_to_pfx_table(uint32_t asn, char *address,  uint8_t min_len,
                                uint8_t max_len, struct pfx_table * pfxt);

/** Check if a numberic field (ASN, Max-Len) of a ROA record is valid
 *
 * @param val             pointer to the value field of the ROA record
 * @param rst_val         pointer to uintX_t to which the value is stored
 * @param unsigned_len    size of the fixed unsignet integer type (8, 32)
 * @return                0 if the check process was valid, otherwise -1
 */
int cfg_validity_check_val(char* val, void *rst_val, int unsigned_len);

/** Check if a prefix of a ROA record is valid
 *
 * @param prefix          prefix of the ROA record
 * @param address         pointer to char array to which the address is stored
 * @param min_len         pointer to an uint8_t to which the min-len is stored
 * @return                0 if the check process was valid, otherwise -1
 */
int cfg_validity_check_prefix(char* prefix, char* address, uint8_t *min_len);

/** Add an input argument (fix length) to the config struct
 *
 * @param input           pointer to the passed input parameter
 * @param input_max_size  maximum size of the input
 * @param item_max_size   maximum size of the input items
 * @param item_max_count  maximum number of input items
 * @param concat_storage  pointer to a char array - sizeof(cfg_storage)
 * @param cfg_storage     pointer to a two dimensional char array of fixed size
 * @param del             delimiter for splitting the input
 * @return                number of items added to the config storage
 */
int add_input_to_cfg(char* input, size_t input_max_size, size_t item_max_size,
          					 int item_max_count, char* concat_storage,
										 char (*cfg_storage)[MAX_INPUT_LENGTH], char* del);

#endif /* __CONFIG_H */
