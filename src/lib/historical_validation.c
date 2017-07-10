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

#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>

#include "rtrlib/rtrlib.h"
#include "historical_validation.h"

struct reasoned_result historical_validate_reason(uint32_t asn, char prefix[], uint8_t mask_len, struct pfx_table * pfxt)
{
  struct lrtr_ip_addr pref;
  lrtr_ip_str_to_addr(prefix, &pref);
  enum pfxv_state result;
  struct pfx_record *reason = NULL;
  unsigned int reason_len = 0;

  pfx_table_validate_r(pfxt, &reason, &reason_len, asn, &pref, mask_len, &result);

  struct reasoned_result reasoned_res;
  reasoned_res.reason = reason;
  reasoned_res.reason_len = reason_len;
  reasoned_res.result = result;
 
  return (reasoned_res);
}
