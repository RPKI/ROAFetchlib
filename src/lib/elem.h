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

#ifndef __ELEM_H
#define __ELEM_H

#include "khash.h"
#include "constants.h"
#include "rpki_config.h"
#include "rtrlib/rtrlib.h"

/** Validation types */
typedef enum {

  /** Valid */
  VALID = 1,

  /** Invalid */
  INVALID = 0,

  /** Not found */
  NOTFOUND = -1,

  /** Not validated */
  NOTVALIDATED = 2,

} elem_validation_status_t;

/** Initialising the RPKI result khash */
KHASH_INIT(rpki_result, kh_cstr_t, char *, 1, kh_str_hash_func,
           kh_str_hash_equal)

/** A RPKI Elem object */
typedef struct struct_elem_t {

  /** RPKI validation status
   *
   * RPKI validation status for a given prefix
   */
  elem_validation_status_t rpki_validation_status[MAX_RPKI_COUNT];

  /** RPKI validation result khash
   *
   * RPKI validation result hashtable (ASN -> prefixes)
   */
  khash_t(rpki_result) * rpki_kh;

  /** RPKI validation result khash init status
   *
   *  1 - RPKI hashtable is already initialized, 0 - otherwise
   */
  int khash_init;

  /** RPKI validation khash count
   *
   * Number of different entries in the RPKI khash
   */
  int khash_count;

  /** RPKI valid ASN
   *
   * All valid ASN for one validation (Project,Collector,Status,ASN)
   */
  char valid_asn[VALID_REASONS_SIZE][VALID_ASN_LEN];

  /** RPKI valid prefixes
   *
   * All valid prefixes for one validation
   */
  char valid_prefix[VALID_REASONS_SIZE][VALID_PFX_LEN];

} elem_t;

/** Create an element for the RPKI validation
 *
 * @return                pointer to the RPKI element
 */
elem_t *elem_create();

/** Destroy an element after the RPKI validation
 *
 * @param elem            pointer to the RPKI element
 */
void elem_destroy(elem_t *elem);

/** Write the string representation of the RPKI validation result of an elem
 *
 * @param cfg             pointer to the configuration struct
 * @param buf             buffer the validation result will be printed into
 * @param len             available size for validation result output
 * @param elem            elem whose RPKI validation result will be printed
 * @return                0 if the print process was valid, otherwise -1
 */
int elem_get_rpki_validation_result_snprintf(rpki_cfg_t *cfg, char *buf,
                                             size_t len, elem_t const *elem);

/** Get the result of the RPKI-Validation for the elem
 *
 * @param cfg             pointer to the configuration struct
 * @param rtr_mgr_config  pointer to the rtr_mgr_config struct
 * @param elem            elem which will be validated
 * @param prefix          BGP prefix which will be validated
 * @param origin_asn      origin ASN of the BGP elem
 * @param mask_len        mask_len of the prefix
 * @param pfxt            pointer to all prefix tables
 * @param pfxt_count      the number of used prefix tables
 */
void elem_get_rpki_validation_result(rpki_cfg_t *cfg,
                                     struct rtr_mgr_config *rtr_cfg,
                                     elem_t *elem, char *prefix,
                                     uint32_t origin_asn, uint8_t mask_len,
                                     struct pfx_table *pfxt, int pfxt_count);

/** @} */

#endif /* __ELEM_H */
