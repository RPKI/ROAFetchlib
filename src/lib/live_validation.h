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

#ifndef __LIVE_VALIDATION_H
#define __LIVE_VALIDATION_H

#include "rpki_config.h"
#include "constants.h"
#include "historical_validation.h"
#include "rtrlib/rtrlib.h"

/** Set the configuration for a connection to a RTR-Server for live validation
 *
 * @param collector       RTR collector for live validation
 * @param cfg             pointer to the configuration struct
 * @param ssh_options     SSH user, SSH hostkey, SSH privkey
 * @return                pointer to the configuration of the rtr-socket-manager
 */
int live_validation_set_config(char* collector, rpki_cfg_t* cfg, char* ssh_options);


/** Start a connection to a desired RTR-Server over SSH or TCP
 *
 * @param host            string of the host-address
 * @param port            specific port should be used (string of the port-number)
 * @param polling_period  specific polling period should be used in seconds
 * @param cache_timeout   specific cache_timeout should be used in seconds
 * @param ssh_user        ssh username to be used
 * @param ssh_hostkey     ssh hostkey to be used
 * @param ssh_privkey     ssh private key to be used
 * @return                struct consisting of the configuration and the address of the transport-socket
 */
struct rtr_mgr_config *live_validation_start_connection(char *host, char *port, 
                              char *ssh_user, char *ssh_hostkey, char *ssh_privkey);


/** Stop a connection to a desired RTR-Server over SSH or TCP
 *
 * @param mgr_cfg         pointer to the RTR manager configuration
 */
void live_validation_close_connection(struct rtr_mgr_config *mgr_cfg);


/** Validates the origin of a BGP-Route and returns the reason (live Validation)
 *  
 * @param mgr_cfg         pointer to the RTR manager configuration
 * @param asn             autonomous system number of the origin as of the prefix
 * @param prefix          announced network prefix
 * @param mask_len        length of the network mask of the announced prefix          
 * @return                result of the validation and the reason
                          BGP_PFXV_STATE_VALID, BGP_PFXV_STATE_NOT_FOUND, BGP_PFXV_STATE_INVALID
 */
struct reasoned_result live_validate_reason(struct rtr_mgr_config *mgr_cfg, uint32_t asn,
                                            char prefix[], uint8_t mask_len);

/** @} */

#endif /*__LIVE_VALIDATION_H*/
