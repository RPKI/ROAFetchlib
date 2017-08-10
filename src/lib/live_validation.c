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
#include <netinet/in.h>

int live_validation_set_config(char* collector, rpki_cfg_t* cfg, char* ssh_options){

  char url[RPKI_RST_MAX_LEN] = {0};
  size_t size = sizeof(rpki_collector)/sizeof(rpki_collector[0]);
  for(int i = 0; i < size; i++) {
    if(!strcmp(collector, rpki_collector[i])) {
      strcpy(url, rpki_cache_server[i]);
      break;
    }
  }
  if(!strlen(url)){
    debug_err_print("Error: Collector: %s not found for live mode (only RTR-Server allowed)\n",
                    collector);
    exit(-1);
  }

  char *host = strtok(url, ":");
  char *port = strtok(NULL, ":");

  if(ssh_options == NULL) {
    cfg->cfg_rtr.rtr_mgr_cfg = live_validation_start_connection(host, port, NULL, NULL, NULL);
    return 0;
  } 

  char* ssh_user = strtok(ssh_options, ",");
  char* ssh_hostkey = strtok(NULL, ",");
  char* ssh_privkey = strtok(NULL, ",");
  cfg->cfg_rtr.rtr_mgr_cfg = live_validation_start_connection(host, port, ssh_user, ssh_hostkey, ssh_privkey);
  return 0;
}

struct rtr_mgr_config *live_validation_start_connection(char *host, char *port, 
                              char *ssh_user, char *ssh_hostkey, char *ssh_privkey){

  struct tr_socket *tr = malloc(sizeof(struct tr_socket));
  debug_print("Live mode (host: %s, port: %s)\n", host, port);
  if (ssh_user != NULL && ssh_hostkey != NULL && ssh_privkey != NULL) {
#ifdef WITH_SSH
    int ssh_port = atoi(port);
    struct tr_ssh_config config = {host, ssh_port, NULL, ssh_user, ssh_hostkey, ssh_privkey};
    tr_ssh_init(&config, tr);

    if (tr_open(tr) == TR_ERROR) {
      debug_err_print("%s", "ERROR: Could not initialising the SSH socket, invalid SSH options\n");
      exit(-1);
    }
#else
    debug_err_print("%s", "ERROR: The library was not configured with SSH\n");
    exit(-1);
#endif

  } else {
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
  int ret = rtr_mgr_init(&conf, groups, 1, 30, 600, 600, NULL, NULL, NULL, NULL);

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
                                            char* prefix, uint8_t mask_len){
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
