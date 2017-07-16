/*
 * This file is part of libhistoryrpki
 *
 * Author: Samir Al-Sheikh (Freie Universitaet, Berlin)
 *         s.al-sheikh@fu-berlin.de
 *
 * MIT License
 *
 * Copyright (c) 2017 The Libhistoryrpki authors
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

  /** @file rpki.h 
   *  @brief LibhistoryRPKI API functions
   */

#ifndef __RPKI_H
#define __RPKI_H

#include <stdint.h>

#include "lib/elem.h"
#include "lib/rpki_config.h"

/** Public Functions */

/** Creates a configuration and sets values for the RPKI validation
 *
 * @param projects       All RPKI projects  (comma-separated list)
 * @param collectors     All RPKI collectors (comma-separated list)
 * @param time_intervals Time intervals as UTC epoch timestamps 
 *                       (start_1-end_1[,<start_n>-<end_n>]*)
 * @param unified        Whether the validation is distinct(0) or unified(1) 
 * @param mode           Mode of the validation - live(0) or historical(1)
 * @param ssh_options    SSH user, SSH hostkey, SSH privkey
 * @return               Pointer to RPKI configuration
 */
rpki_cfg_t* rpki_set_config(char* projects, char* collectors, char* time_intervals, 
                            int unified, int mode, char* ssh_options);

/** Validates a BGP element with RPKI
 *
 * @param cfg           Pointer to the RPKI configuration
 * @param timestamp     UTC epoch timestamp of the BGP elem
 * @param asn           Origin ASN of the BGP elem
 * @param prefix        BGP elem prefix
 * @param mask_len      Mask-len of the BGP prefix
 * @param result        Pointer to a buffer where the result will be stored
 * @param size          Size of the result buffer
 * @return              0 if the RPKI validation was valid, otherwise -1
 */
int rpki_validate(rpki_cfg_t* cfg, uint32_t timestamp, uint32_t asn, 
                  char* prefix, uint8_t mask_len, char* result, size_t size);

/** Prints informations about the RPKI configuration struct
 *
 * @param cfg           pointer to the configuration 
 */
void print_config_debug(rpki_cfg_t* cfg);

#endif /* __RPKI_H */
