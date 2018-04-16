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
#include "rpki_config.h"
#include "wandio.h"

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>

int live_validation_set_config(char* project, char* collector, rpki_cfg_t* cfg, char* ssh_options){

  // Check if the requested collector is a RTR-collector
  if(strstr(collector, "RTR") == NULL) {
    std_print("Error: Collector not allowed: %s (only RTR-Server)\n", collector);
    return -1;
  }

  // Build the info request URL
  config_broker_t *broker = &cfg->cfg_broker;
	char info_url[BROKER_INFO_REQ_URL_LEN] = {0};
  snprintf(info_url, sizeof(info_url), "%sproject=%s&collector=%s",
           broker->info_url, project, collector);

  // Get the broker reponse and check for errors
	io_t *info_chk_err = wandio_create(info_url);
	char info_check_rst[BROKER_INFO_REQ_URL_LEN] = {0};
	if (info_chk_err == NULL) {
	  std_print("ERROR: Could not open %s for reading\n", info_url);
	  wandio_destroy(info_chk_err);
    return -1;
	}
	wandio_read(info_chk_err, info_check_rst, sizeof(info_check_rst));
	if(!strncmp(info_check_rst, "Error:", strlen("Error:")) || 
     !strncmp(info_check_rst, "Malformed", strlen("Malformed"))) {
    info_check_rst[strlen(info_check_rst)] = '\0';
	  std_print("%s\n", info_check_rst);
	  wandio_destroy(info_chk_err);
    return -1;
	}

	wandio_destroy(info_chk_err);
  strncpy(broker->info_host, strtok(info_check_rst, ":"), sizeof(broker->info_host));
  strncpy(broker->info_port, strtok(NULL, ":"), sizeof(broker->info_port)); 

  // Start the RTR connection (with SSH if required)
  config_rtr_t *rtr = &cfg->cfg_rtr;
  if(ssh_options == NULL) {
    rtr->rtr_mgr_cfg = live_validation_start_connection(cfg, broker->info_host, broker->info_port, 
                                                        NULL, NULL, NULL);
    return (rtr->rtr_mgr_cfg == NULL ? -1 : 0);
  }
  char ssh_options_cpy[MAX_SSH_LEN] = {0};
  if(strlen(ssh_options) > MAX_SSH_LEN) {
    std_print("%s", "Error: SSH options exceed maximum length\n");
    return -1;
  }
  snprintf(ssh_options_cpy, sizeof(ssh_options_cpy), "%s", ssh_options);
  snprintf(rtr->ssh_user, sizeof(rtr->ssh_user), "%s",
                                                  strtok(ssh_options_cpy, ","));
  snprintf(rtr->ssh_hostkey, sizeof(rtr->ssh_hostkey), "%s", strtok(NULL, ","));
  snprintf(rtr->ssh_privkey, sizeof(rtr->ssh_privkey), "%s", strtok(NULL, ","));
  rtr->rtr_mgr_cfg = live_validation_start_connection(cfg, broker->info_host, broker->info_port,
                                             rtr->ssh_user, rtr->ssh_hostkey, rtr->ssh_privkey);
  return (rtr->rtr_mgr_cfg == NULL ? -1 : 0);
}

struct rtr_mgr_config *live_validation_start_connection(rpki_cfg_t* cfg, char *host, char *port, 
                              char *ssh_user, char *ssh_hostkey, char *ssh_privkey){

  struct tr_socket *tr = malloc(sizeof(struct tr_socket));
  cfg->cfg_rtr.rtr_allocs[0] = tr;
  debug_print("Live mode (host: %s, port: %s)\n", host, port);

  // If all SSH options are syntactically valid, build a SSH config else build a TCP config
  if (ssh_user != NULL && ssh_hostkey != NULL && ssh_privkey != NULL) {
#ifdef WITH_SSH
    int ssh_port = strtol(port, NULL, 10);
    struct tr_ssh_config config = {host, ssh_port, NULL, ssh_user, ssh_hostkey, ssh_privkey};
    tr_ssh_init(&config, tr);

    if (tr_open(tr) == TR_ERROR) {
      std_print("%s", "ERROR: SSH socket could not be initialized, invalid SSH options\n");
      return NULL;
    }
#else
    std_print("%s", "ERROR: The library was not configured with SSH\n");
    return NULL;
#endif
  } else {
    struct tr_tcp_config config = {host, port, NULL};
    tr_tcp_init(&config, tr);
  }

  // Integrate the configuration into the socket and start the RTR MGR
  struct rtr_socket *rtr = malloc(sizeof(struct rtr_socket));
  cfg->cfg_rtr.rtr_allocs[1] = rtr;
  rtr->tr_socket = tr;

  struct rtr_mgr_group groups[1];
  groups[0].sockets = malloc(sizeof(struct rtr_socket *));
  cfg->cfg_rtr.rtr_allocs[2] = groups[0].sockets;
  groups[0].sockets_len = 1;
  groups[0].sockets[0] = rtr;
  groups[0].preference = 1;
	cfg->cfg_rtr.rtr_socket = rtr;

  struct rtr_mgr_config *conf;
  int ret = rtr_mgr_init(&conf, groups, 1, 30, 600, 600, NULL, NULL, NULL, NULL);

  rtr_mgr_start(conf);

  while (!rtr_mgr_conf_in_sync(conf))
    sleep(1);

  return conf;
}

void live_validation_close_connection(rpki_cfg_t* cfg)
{
  // Close the RTR MGR CONF (RTRlib) if it is initialized
  config_rtr_t *rtr = &cfg->cfg_rtr;
  if(rtr->rtr_mgr_cfg != NULL) {
    rtr_mgr_stop(rtr->rtr_mgr_cfg);
    rtr_mgr_free(rtr->rtr_mgr_cfg);
  }
  // Close the Transport socket (RTRlib)
  if(rtr->rtr_allocs[0] != NULL) {
    free(rtr->rtr_allocs[0]);
  }
  // Close the RTR socket (RTRlib)
  if(rtr->rtr_allocs[1] != NULL) {
    free(rtr->rtr_allocs[1]);
  }
  // Close the Group socket (RTRlib)
  if(rtr->rtr_allocs[2] != NULL) {
    free(rtr->rtr_allocs[2]);
  }
}

struct reasoned_result live_validate_reason(rpki_cfg_t* cfg, uint32_t asn,
                                            char* prefix, uint8_t mask_len){
  struct lrtr_ip_addr pref;
  lrtr_ip_str_to_addr(prefix, &pref);
  enum pfxv_state result;
  struct pfx_record *reason = NULL;
  unsigned int reason_len = 0;

  pfx_table_validate_r(cfg->cfg_rtr.rtr_socket->pfx_table, &reason,
                       &reason_len, asn, &pref, mask_len, &result);

  struct reasoned_result reasoned_res;
  reasoned_res.reason = reason;
  reasoned_res.reason_len = reason_len;
  reasoned_res.result = result;

  return (reasoned_res);
}
