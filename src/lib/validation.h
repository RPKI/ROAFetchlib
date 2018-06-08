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

#ifndef __VALIDATION_H
#define __VALIDATION_H

#include "constants.h"
#include "rtrlib/rtrlib.h"

/** A RPKI config for RTRLib object */
typedef struct struct_config_validation_t {

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

} config_validation_t;

/** Valdation result object */
struct reasoned_result {

  /** Valdation result reason
   *
   * Valdation result reason
   */
  struct pfx_record *reason;

  /** Valdation result result
   *
   * Valdation result result
   */
  enum pfxv_state result;

  /** Valdation result reason length
   *
   * Valdation result reason length
   */
  unsigned int reason_len;
};

/* Forward declaration */
typedef struct struct_rpki_config_t rpki_cfg_t;

/** Set the configuration for a connection to a RTR-Server for live validation
 *
 * @param[in] broker_collectors  RTR collector for live validation
 * @param[in] cfg                Pointer to the configuration struct
 * @param[in] ssh_options        SSH user, SSH hostkey, SSH privkey
 * @return                       0 if the configuration was created,otherwise -1
 */
int validation_set_live_config(char *broker_collectors, rpki_cfg_t *cfg,
                               char *ssh_options);

/** Start a connection to a RTR-Server over SSH or TCP
 *
 * @param[in] cfg            Pointer to the configuration struct
 * @param[in] host           Host address
 * @param[in] port           Port number of the RTR server
 * @param[in] ssh_user       SSH username to be used
 * @param[in] ssh_hostkey    SSH hostkey to be used
 * @param[in] ssh_privkey    SSH private key to be used
 * @return                   Pointer to the RTR manager configuration
 */
struct rtr_mgr_config *validation_start_connection(rpki_cfg_t *cfg, char *host,
                                          char *port, char *ssh_user,
                                          char *ssh_hostkey, char *ssh_privkey);

/** Stop a connection to a desired RTR-Server over SSH or TCP
 *
 * @param[in] cfg            Pointer to the configuration struct
 */
void validation_close_connection(rpki_cfg_t *cfg);

/** Validate the origin of a BGP-Route and returns the reason for the validation
 *  result (Live- and Historical-Validation)
 *
 * @param[in]  cfg           Pointer to the configuration struct
 * @param[in]  asn           Origin ASN of the prefix
 * @param[in]  prefix        Announced network prefix
 * @param[in]  mask_len      Length of the network mask of the announced prefix
 * @param[in]  pfxt          Pointer to the prefix Tables (Historical)
 * @param[out] reason        Result of the validation and the reason
 * @return                   0 if the validation process was valid, otherwise -1
 */
int validation_validate(rpki_cfg_t *cfg, uint32_t asn, char *prefix,
                        uint8_t mask_len, struct pfx_table *pfxt,
                        struct reasoned_result *reason);

/** @} */

#endif /*__VALIDATION_H*/
