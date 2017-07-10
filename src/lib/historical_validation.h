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

#ifndef __HISTORICAL_VALIDATION_H
#define __HISTORICAL_VALIDATION_H

#include "constants.h"
#include "rtrlib/rtrlib.h"

/** Valdation result object */
struct reasoned_result {
  struct pfx_record *reason;
  enum pfxv_state result;
  unsigned int reason_len;
};

/** Validates the origin of a BGP-Route and returns the reason for a certain time in the past (historical Validation)
 *  
 * @param asn             autonomous system number of the origin as of the prefix
 * @param prefix          announced network prefix
 * @param mask_len        length of the network mask of the announced prefix  
 * @param pfxt            pointer to the prefix tables        
 * @return                result of the validation and the reason
                          BGP_PFXV_STATE_VALID, BGP_PFXV_STATE_NOT_FOUND, BGP_PFXV_STATE_INVALID
 */
struct reasoned_result historical_validate_reason(uint32_t asn, char prefix[], uint8_t mask_len, struct pfx_table * pfxt);

/** @} */

#endif /*__HISTORICAL_VALIDATION_H*/
