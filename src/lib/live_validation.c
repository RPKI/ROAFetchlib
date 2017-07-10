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

#include "rtrlib/rtrlib.h"
#include "constants.h"
#include "debug.h"
#include "live_validation.h"
#include "rtr_cache_server.h"
#include "rpki_config.h"

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int live_validation_set_config(char* collector, rpki_cfg_t* cfg){
  char url[RPKI_RST_MAX_LEN] = {0};
  char *cs_url = url;
  size_t size = sizeof(rpki_collector) / sizeof(rpki_collector[0]);
  for(int i = 0; i < size; i++) {
    if(!strcmp(collector, rpki_collector[i])) {
      strcpy(cs_url,rpki_cache_server[i]);
      break;
    }
  }
  if(!strlen(cs_url)){
    debug_err_print("%s", "Error: Collector not found for live mode (only RTR-Server allowed)\n");
    exit(-1);
  }

  char *host = strtok(cs_url, ":");
  char *port = strtok(NULL, ":");
  debug_print("Live mode (host: %s, port: %s)\n", host, port);

  // ToDo: SSH Support
  cfg->cfg_rtr.rtr_mgr_cfg = live_validation_start_connection(host, port, NULL, NULL, NULL, NULL, NULL, NULL);
  return 0;
}

struct rtr_mgr_config *live_validation_start_connection( char *host, char *port, uint32_t *polling_period, uint32_t *cache_timeout,
                                              uint32_t *retry_inv, char *ssh_user, char *ssh_hostkey, char *ssh_privkey)
{

  uint32_t pp = 30;
  if (polling_period == NULL) {
    polling_period = &pp;
  }

  uint32_t ct = 600;
  if (cache_timeout == NULL) {
    cache_timeout = &ct;
  }

  uint32_t rt = 600;
  if (retry_inv == NULL) {
    retry_inv = &rt;
  }

  struct tr_socket *tr = malloc(sizeof(struct tr_socket));
  if (host != NULL && ssh_user != NULL && ssh_hostkey != NULL &&
      ssh_privkey != NULL) {
/*
    int port = port;
    struct tr_ssh_config config = {
        host, port, NULL, ssh_user, ssh_hostkey, ssh_privkey,
    };
    tr_ssh_init(&config, tr);

    if (tr_open(tr) == TR_ERROR) {
      fprintf(stderr, "ERROR: The SSH-values entered caused an error while "
                      "initialising the transport-socket\n");
      exit(1);
    }
*/
  }

  else if (host != NULL && port != NULL) {
    struct tr_tcp_config config = {host, port, NULL};
    tr_tcp_init(&config, tr);
  }

  struct rtr_socket *rtr = malloc(sizeof(struct rtr_socket));
  rtr->tr_socket = tr;

  struct rtr_mgr_group groups[1];
  groups[0].sockets = malloc(sizeof(struct rtr_socket *));
  groups[0].sockets_len = 1;
  groups[0].sockets[0] = rtr;
  groups[0].preference = 1;

  struct rtr_mgr_config *conf;
  int ret = rtr_mgr_init(&conf, groups, 1, *polling_period, *cache_timeout,
                         *retry_inv, NULL, NULL, NULL, NULL);

  rtr_mgr_start(conf);

  while (!rtr_mgr_conf_in_sync(conf))
    sleep(1);

  return conf;
}

void live_validation_close_connection(struct rtr_mgr_config *mgr_cfg){

  struct tr_socket *tr = mgr_cfg->groups[0].sockets[0]->tr_socket;
  struct rtr_socket *rtr = mgr_cfg->groups[0].sockets[0];
  struct rtr_socket **socket = mgr_cfg->groups[0].sockets;
  rtr_mgr_stop(mgr_cfg);
  rtr_mgr_free(mgr_cfg);
  tr_free(tr);
  free(tr);
  free(rtr);
  free(socket);
}

struct reasoned_result live_validate_reason(struct rtr_mgr_config *mgr_cfg, uint32_t asn,
                                            char prefix[], uint8_t mask_len){
  struct lrtr_ip_addr pref;
  lrtr_ip_str_to_addr(prefix, &pref);
  enum pfxv_state result;
  struct pfx_record *reason = NULL;
  unsigned int reason_len = 0;
  pfx_table_validate_r(mgr_cfg->groups[0].sockets[0]->pfx_table, &reason,
                       &reason_len, asn, &pref, mask_len, &result);

  struct reasoned_result reasoned_res;
  reasoned_res.reason = reason;
  reasoned_res.reason_len = reason_len;
  reasoned_res.result = result;

  return (reasoned_res);
}
