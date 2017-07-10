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
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <arpa/inet.h>

#include "config.h"
#include "constants.h"
#include "elem.h"
#include "historical_validation.h"
#include "live_validation.h"

elem_t *elem_create(){

  // Create elem
  elem_t *elem = NULL;
  if((elem = (elem_t *) malloc(sizeof(elem_t))) == NULL){
    return NULL;
  } else {
    memset(elem, 0, sizeof(elem_t));
  }

  // Create Validation Status
  for (int i = 0; i < MAX_RPKI_COUNT; i++) {
    elem->rpki_validation_status[i] = ELEM_RPKI_VALIDATION_STATUS_NOTVALIDATED;
  }
  
  return elem;
}

void elem_destroy(elem_t *elem){

  if(elem == NULL){
    return;
  }

  // Destroy hashtable (khash) and elem
  kh_destroy(rpki_result, elem->rpki_kh);
  free(elem);
}

int elem_get_rpki_validation_result_snprintf(rpki_cfg_t *cfg, char *buf, size_t len, elem_t const *elem){

  char *val;
  char *asn = NULL;
  const char *key = '\0';
  char last_key[RPKI_RST_MAX_LEN]={0};
  char valid_prefixes[RPKI_RST_MAX_LEN]={0};
  char result_output[UTILS_ROA_STR_NAME_LEN]={0};

  // If the unified flag is set, the following output will be generated :
  // Output: Proj01\Coll01 Proj02\Coll02,Validation_status[,ASN1,Prefix1 ASN2,Prefix]; || notfound;
  config_rtr_t *rtr = &cfg->cfg_rtr;
  config_input_t *input = &cfg->cfg_input;
  if(input->unified) {
    if (elem->rpki_validation_status[0] != ELEM_RPKI_VALIDATION_STATUS_NOTFOUND) {
      char proj_coll[UTILS_ROA_STR_NAME_LEN];
      for (int k = 0; k < rtr->pfxt_count; k++) {
        snprintf(proj_coll, sizeof(proj_coll), k != rtr->pfxt_count - 1 ? "%s\\%s " : "%s\\%s,",
                 input->projects[k], input->collectors[k]);
        strcat(result_output, proj_coll);
      }
      strcat(result_output, elem->rpki_validation_status[0] == 
             ELEM_RPKI_VALIDATION_STATUS_INVALID ? "invalid," : "valid,");

      kh_foreach(elem->rpki_kh, key, val,
        asn = strrchr(key, ',') + 1;
        snprintf(valid_prefixes, sizeof(valid_prefixes), "%s,%s;", asn, val);
        strcat(result_output, valid_prefixes);
      );
    } else {
      strcat(result_output, "notfound;");
    }
  // If the discrete flag is set, the following output will be generated :
  // Output: Output: Project,Collector,Validation_status[,ASN1,Prefix1,ASN2,Prefix]; || notfound;
  } else {
    if(elem->rpki_kh != NULL) {
      if(!kh_size(elem->rpki_kh)) {
        strcat(result_output, "notfound;");
      } else {
        kh_foreach(elem->rpki_kh, key, val,
          if(strstr(key, last_key)) {
            snprintf(valid_prefixes, sizeof(valid_prefixes), "%s,%s;", key, val);
          } else {
            asn = strrchr(key, ',') + 1;
            snprintf(valid_prefixes, sizeof(valid_prefixes), "%s,%s;", key, val);
          }
          strcat(result_output, valid_prefixes);
          char *pcs = strrchr(key, ',');
          pcs = '\0';
          (void)pcs;
          strncpy(last_key, key, sizeof(last_key));
        );
      }
    } else {
      strcat(result_output, "notfound;");
    }
  }
  return snprintf(buf, len, "%s", result_output);
}

void elem_get_rpki_validation_result(rpki_cfg_t* cfg, struct rtr_mgr_config *rtr_cfg, elem_t *elem,
                                     char *prefix, uint32_t origin_asn, uint8_t mask_len, 
                                     struct pfx_table * pfxt, int pfxt_count)
{

  if (elem->rpki_validation_status[pfxt_count] == ELEM_RPKI_VALIDATION_STATUS_NOTVALIDATED) {
    struct reasoned_result res_reasoned;

    // Validate with the corresponding validation
    if(pfxt != NULL) {
      res_reasoned = historical_validate_reason(origin_asn, prefix, mask_len, pfxt);
    } else {
      res_reasoned = live_validate_reason(rtr_cfg, origin_asn, prefix, mask_len);
    }

    if (res_reasoned.result == BGP_PFXV_STATE_VALID) { 
      elem->rpki_validation_status[pfxt_count] = ELEM_RPKI_VALIDATION_STATUS_VALID;
    }
    if (res_reasoned.result == BGP_PFXV_STATE_NOT_FOUND) {
      elem->rpki_validation_status[pfxt_count] = ELEM_RPKI_VALIDATION_STATUS_NOTFOUND;
    }
    if (res_reasoned.result == BGP_PFXV_STATE_INVALID) {
      elem->rpki_validation_status[pfxt_count] = ELEM_RPKI_VALIDATION_STATUS_INVALID;
    }
    
    // If the reason is not "Notfound" -> store the result in rpki_kh
    if (elem->rpki_validation_status[pfxt_count] != ELEM_RPKI_VALIDATION_STATUS_NOTFOUND) {
      int ret = RPKI_MAX_ROA_ENT;
      khiter_t k;
      char reason_prefix[INET6_ADDRSTRLEN];

      if(elem->khash_init != 1) {
        elem->rpki_kh = kh_init(rpki_result);
        elem->khash_init = 1; 
        elem->khash_count = 0;
      }
  
      int *k_c = &elem->khash_count;
      khash_t(rpki_result) *rpki_kh = elem->rpki_kh;
      config_input_t *input = &cfg->cfg_input;
      for (int i = 0; i < res_reasoned.reason_len; i++) {
        snprintf(elem->valid_asn[*k_c], sizeof(elem->valid_asn[*k_c]), "%s,%s,%s,%"PRIu8,
                 input->projects[pfxt_count], 
                 input->collectors[pfxt_count], 
                 elem->rpki_validation_status[pfxt_count] == ELEM_RPKI_VALIDATION_STATUS_INVALID
                 ? "invalid" : "valid", res_reasoned.reason[i].asn);

	      if(kh_get(rpki_result, rpki_kh, elem->valid_asn[*k_c]) == kh_end(rpki_kh)){
       	  k = kh_put(rpki_result, rpki_kh, elem->valid_asn[*k_c], &ret);
	        kh_val(rpki_kh, k) = '\0';
          lrtr_ip_addr_to_str(&(res_reasoned.reason[i].prefix), reason_prefix, sizeof(reason_prefix));
          snprintf(elem->valid_prefix[*k_c], RPKI_RST_MAX_LEN, "%s/%" PRIu8 "-%"PRIu8, 
                   reason_prefix, res_reasoned.reason[i].min_len, res_reasoned.reason[i].max_len);
          kh_val(rpki_kh, k) = elem->valid_prefix[*k_c];
          elem->khash_count++;
        } else {
          char v_prefix[RPKI_RST_MAX_LEN];
          k = kh_get(rpki_result, rpki_kh, elem->valid_asn[*k_c]);
          for(int i = 0; i < *k_c; ++i) {if(!strcmp(elem->valid_asn[i], elem->valid_asn[*k_c])) {ret = i;}}
          lrtr_ip_addr_to_str(&(res_reasoned.reason[i].prefix), reason_prefix, sizeof(reason_prefix));
          snprintf(v_prefix, sizeof(v_prefix), "%s %s/%" PRIu8 "-%"PRIu8, kh_val(rpki_kh, k), reason_prefix, 
                   res_reasoned.reason[i].min_len, res_reasoned.reason[i].max_len);
          strncpy(elem->valid_prefix[ret], v_prefix, sizeof(elem->valid_prefix[ret]));
          kh_val(rpki_kh, k) = elem->valid_prefix[ret];
        }
      }
    }
    free(res_reasoned.reason);
  }
}
