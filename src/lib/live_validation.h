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

#ifndef __LIVE_VALIDATION_H
#define __LIVE_VALIDATION_H

#include "constants.h"
#include "historical_validation.h"
#include "rpki_config.h"
#include "rtrlib/rtrlib.h"

/** Set the configuration for a connection to a RTR-Server for live validation
 *
 * @param project         RTR project for live validation
 * @param collector       RTR collector for live validation
 * @param cfg             Pointer to the configuration struct
 * @param ssh_options     SSH user, SSH hostkey, SSH privkey
 * @return                Pointer to the configuration of the rtr-socket-manager
 */
int live_validation_set_config(char *project, char *collector, rpki_cfg_t *cfg,
                               char *ssh_options);

/** Start a connection to a RTR-Server over SSH or TCP
 *
 * @param cfg             Pointer to the configuration struct
 * @param host            Host address
 * @param port            Port number of the RTR server
 * @param ssh_user        SSH username to be used
 * @param ssh_hostkey     SSH hostkey to be used
 * @param ssh_privkey     SSH private key to be used
 * @return                RTR manager configuration
 */
struct rtr_mgr_config *live_validation_start_connection(rpki_cfg_t *cfg,
                                                        char *host, char *port,
                                                        char *ssh_user,
                                                        char *ssh_hostkey,
                                                        char *ssh_privkey);

/** Stop a connection to a desired RTR-Server over SSH or TCP
 *
 * @param cfg             Pointer to the configuration struct
 */
void live_validation_close_connection(rpki_cfg_t *cfg);

/** Validates the origin of a BGP-Route and returns the reason (live Validation)
 *
 * @param cfg             Pointer to the configuration struct
 * @param asn             Origin ASN of the prefix
 * @param prefix          Announced network prefix
 * @param mask_len        Length of the network mask of the announced prefix
 * @return                Result of the validation and the reason
 *                        BGP_PFXV_STATE_VALID, BGP_PFXV_STATE_NOT_FOUND,
 *                        BGP_PFXV_STATE_INVALID
 */
struct reasoned_result live_validate_reason(rpki_cfg_t *cfg, uint32_t asn,
                                            char *prefix, uint8_t mask_len);

/** @} */

#endif /*__LIVE_VALIDATION_H*/
