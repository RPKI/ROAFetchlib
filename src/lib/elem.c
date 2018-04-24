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

#include <arpa/inet.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "config.h"
#include "utils.h"
#include "constants.h"
#include "debug.h"
#include "elem.h"
#include "validation.h"

elem_t *elem_create()
{

  /* Create elem */
  elem_t *elem = NULL;
  if ((elem = (elem_t *)malloc(sizeof(elem_t))) == NULL) {
    return NULL;
  } else {
    memset(elem, 0, sizeof(elem_t));
  }

  /* Create Validation Status */
  for (int i = 0; i < MAX_RPKI_COUNT; i++) {
    elem->rpki_validation_status[i] = NOTVALIDATED;
  }

  return elem;
}

void elem_destroy(elem_t *elem)
{

  if (elem == NULL) {
    return;
  }

  /* Destroy hashtable (khash) and elem */
  kh_destroy(rpki_result, elem->rpki_kh);
  free(elem);
}

int elem_get_rpki_validation_result_snprintf(rpki_cfg_t *cfg, char *buf,
                                             size_t len, elem_t const *elem)
{

  char *val; char *asn = NULL; const char *key = '\0';
  char last_key[VALIDATION_MAX_SINGLE_RESULT_LEN] = {0};
  char result[VALIDATION_MAX_RESULT_LEN] = {0};
  size_t size = MAX_RPKI_COUNT * ROA_ARCHIVE_CC_MAX_LEN;
  char pj_cc[size]; memset(pj_cc, 0, size);

  /* For the unified validation the following outputs are generated: */
  config_input_t *input = &cfg->cfg_input;
  if (input->unified) {

    /* Output: (PJ_1\CC_1( PJ_2\CC_2)*,validation_status,ASN,Prefix1
               (Prefix2)*;)+
       PJ,CC,ASNs are separated in multiple output statements (divided by ;) */
    elem_validation_status_t unified_status = elem->rpki_validation_status[0];
    if (unified_status != NOTFOUND) {

      /* Build the concatenation of all projects and collectors */
      for (int k = 0; k < input->collectors_count; k++) {
        snprintf(pj_cc + strlen(pj_cc), sizeof(pj_cc) - strlen(pj_cc),
                 k < input->collectors_count - 1 ? "%s\\%s " : "%s\\%s,",
                 input->projects[k], input->collectors[k]);
      }

      /* For every different Khash entry, add the ASN and the prefixes */
      kh_foreach(elem->rpki_kh, key, val, asn = strrchr(key, ',') + 1;
              snprintf(result + strlen(result), sizeof(result) - strlen(result),
                       "%s%s,%s,%s;", pj_cc, unified_status == INVALID ? 
                                      "invalid" : "valid", asn, val););

      /* Output: PJ_1\CC_1( PJ_2\CC_2)*,notfound; */
    } else {
      for (int k = 0; k < input->collectors_count; k++) {
        snprintf(pj_cc + strlen(pj_cc), sizeof(pj_cc) - strlen(pj_cc),
          k < input->collectors_count - 1 ? "%s\\%s " : "%s\\%s,notfound;",
          input->projects[k], input->collectors[k]);
      }
      strncat(result, pj_cc, sizeof(result));
    }

    /* For the discrete validation the following outputs are generated: */
  } else {

    /* Output: (PJ_1,CC_1,validation_status,ASN,Prefix1( Prefix2)*;)+ */
    if (elem->rpki_kh != NULL) {
      kh_foreach(elem->rpki_kh, key, val,
              snprintf(result + strlen(result), sizeof(result) - strlen(result),
                       "%s,%s;", key, val););
    }

    /* Add all remaining notfounds
       Output: PJ_1,CC_1,notfound;(PJ_2,CC_2,notfound;)* */
    for (int k = 0; k < cfg->cfg_val.pfxt_count; k++) {
      if (elem->rpki_validation_status[k] == NOTFOUND) {
        snprintf(result + strlen(result), sizeof(result) - strlen(result),
                 "%s,%s,notfound;", input->projects[k], input->collectors[k]);
      }
    }

    /* Sort the discrete validation result lexicographically */
    char sorted_result[VALIDATION_MAX_RESULT_LEN] = {0};
    utils_elem_sort_result(result, VALIDATION_MAX_RESULT_LEN,sorted_result,";");
    strncpy(result, sorted_result, sizeof(result));
  }

  return snprintf(buf, len, "%s", result);
}

int elem_get_rpki_validation_result(rpki_cfg_t *cfg,
                                     struct rtr_mgr_config *rtr_cfg,
                                     elem_t *elem, char *prefix,
                                     uint32_t asn, uint8_t mask_len,
                                     struct pfx_table *pfxt, int pfxt_count)
{

  /* Only validate if the elem was not validated already, the prefix table
     is active (Historical) or NULL (Live validation) */
  if (elem->rpki_validation_status[pfxt_count] == NOTVALIDATED &&
      (cfg->cfg_val.pfxt_active[pfxt_count] || pfxt == NULL)) {

    /* Validate with the corresponding validation */
    struct reasoned_result reason;
    if (pfxt != NULL) {
      if(historical_validate_reason(asn, prefix, mask_len, pfxt, &reason) !=0) {
        return -1;
      }
    } else {
      if(live_validate_reason(cfg, asn, prefix, mask_len, &reason) != 0) {
        return -1;
      }
    }

    /* Take over the validation status */
    elem_validation_status_t *status = elem->rpki_validation_status;
    switch(reason.result) {
	    case BGP_PFXV_STATE_VALID: status[pfxt_count] = VALID; break;
	    case BGP_PFXV_STATE_NOT_FOUND: status[pfxt_count] = NOTFOUND; break;
	    case BGP_PFXV_STATE_INVALID: status[pfxt_count] = INVALID; break;
	    default: 
        std_print("%s\n","Error: Invalid validation result from RTRlib"); break;
    }

    /* If the reason is not Notfound -> Store the result in the Khash table */
    if (status[pfxt_count] != NOTFOUND) {
      int ret = 0; khiter_t k; int *k_c = &elem->khash_count;
      char reason_prefix[INET6_ADDRSTRLEN];

      /* If the Khash is not initialized already, do so */
      if (elem->khash_init != 1) {
        elem->rpki_kh = kh_init(rpki_result);
        elem->khash_init = 1;
        elem->khash_count = 0;
      }

      khash_t(rpki_result) *rpki_kh = elem->rpki_kh;
      config_input_t *input = &cfg->cfg_input;

      /* Iterate over all reasons provided by the RTRlib */
      for (int i = 0; i < reason.reason_len; i++) {

        /* Build the Khash key (PJ,CC,VS,ASN) for a single validation result */
        snprintf(elem->valid_asn[*k_c], sizeof(elem->valid_asn[*k_c]),
                 "%s,%s,%s,%" PRIu8, input->projects[pfxt_count],
                 input->collectors[pfxt_count],
                 status[pfxt_count] == INVALID ? "invalid" : "valid",
                 reason.reason[i].asn);

        /* Check if the key already exists in the Khash -> if not create one and
           add the prefix of the reason as value to the Khash table */
        if (kh_get(rpki_result, rpki_kh, elem->valid_asn[*k_c]) ==
            kh_end(rpki_kh)) {
          k = kh_put(rpki_result, rpki_kh, elem->valid_asn[*k_c], &ret);
          kh_val(rpki_kh, k) = '\0';
          lrtr_ip_addr_to_str(&(reason.reason[i].prefix), reason_prefix,
                              sizeof(reason_prefix));
          snprintf(elem->valid_prefix[*k_c], sizeof(elem->valid_prefix[*k_c]),
                   "%s/%" PRIu8 "-%" PRIu8, reason_prefix,
                   reason.reason[i].min_len, reason.reason[i].max_len);
          kh_val(rpki_kh, k) = elem->valid_prefix[*k_c];
          elem->khash_count++;

        /* If the key already exists, get the khash key and the index and 
           append the new prefix (divided by whitespace) */
        } else {
          k = kh_get(rpki_result, rpki_kh, elem->valid_asn[*k_c]);
          for (int i = 0; i < *k_c; ++i) {
            if (!strcmp(elem->valid_asn[i], elem->valid_asn[*k_c])) {
              ret = i;
            }
          }
          char v_prefix[VALID_PFX_LEN] = {0};
          lrtr_ip_addr_to_str(&(reason.reason[i].prefix), reason_prefix,
                              sizeof(reason_prefix));
          snprintf(v_prefix, sizeof(v_prefix), "%s %s/%" PRIu8 "-%" PRIu8,
                   kh_val(rpki_kh, k), reason_prefix, reason.reason[i].min_len,
                   reason.reason[i].max_len);
          strncpy(elem->valid_prefix[ret], v_prefix, 
                  sizeof(elem->valid_prefix[ret]));
          kh_val(rpki_kh, k) = elem->valid_prefix[ret];
        }
      }
    }
    free(reason.reason);
  }
  return 0;
}
