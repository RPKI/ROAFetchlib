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

/** @file rpki.h
 *  @brief ROAFetchlib API functions
 */

#ifndef __RPKI_H
#define __RPKI_H

#include <stdint.h>

#include "lib/elem.h"
#include "lib/rpki_config.h"

/** Create a configuration and sets values for the RPKI validation
 *
 * @param[in] project_collectors  All RPKI projects and collectors
 *                                PJ_1:(*|CC_1,CC_2);PJ_2:(*|CC_1,CC_2)
 * @param[in] time_intervals      Time intervals as UTC epoch timestamps
 *                                (start_1-end_1[,start_n-end_n]*)
 * @param[in] unified             Distinct (0) or unified validation (1)
 * @param[in] mode                Validation mode - live (0) or historical (1)
 * @param[in] broker_url          RPKI broker url
 * @param[in] ssh_options         SSH user, SSH hostkey, SSH privkey
 * @return                        Pointer to RPKI configuration
 */
rpki_cfg_t *rpki_set_config(char *project_collectors, char *time_intervals,
                            int unified, int mode, char *broker_url,
                            char *ssh_options);

/** Validate a BGP element with RPKI and stores the result in the given buffer 
 *
 * @param[in]  cfg           Pointer to the RPKI configuration
 * @param[in]  timestamp     UTC epoch timestamp of the BGP elem
 * @param[in]  asn           Origin ASN of the BGP elem
 * @param[in]  prefix        BGP elem prefix
 * @param[in]  mask_len      Mask-len of the BGP prefix
 * @param[out] result        Pointer to a buffer where the result will be stored
 * @param[in]  size          Size of the result buffer
 * @return                   0 if the RPKI validation was valid, otherwise -1
 */
int rpki_validate(rpki_cfg_t *cfg, uint32_t timestamp, uint32_t asn,
                  char *prefix, uint8_t mask_len, char *result, size_t size);

/** Destroy a configuration
 *
 * @param[in] cfg            Pointer to the RPKI configuration
 * @return                   0 if the RPKI configuration was destroyed,
 *                           otherwise -1
 */
int rpki_destroy_config(rpki_cfg_t *cfg);

#endif /* __RPKI_H */
