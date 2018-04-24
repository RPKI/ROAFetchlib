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

#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "utils.h"
#include "validation.h"
#include "constants.h"
#include "debug.h"
#include "rpki_config.h"
#include "rtrlib/rtrlib.h"
#include "wandio.h"

int live_validation_set_config(char *project, char *collector, rpki_cfg_t *cfg,
                               char *ssh_options)
{

  /* Check if the requested collector is a RTR-collector */
  if (strstr(collector, "RTR") == NULL) {
    std_print("Error: Collector not allowed: %s (only RTR-Server)\n",
              collector);
    return -1;
  }

  /* Build the info request URL */
  config_broker_t *broker = &cfg->cfg_broker;
  char info_url[BROKER_INFO_REQ_URL_LEN] = {0};
  snprintf(info_url, sizeof(info_url), "%sproject=%s&collector=%s",
           broker->info_url, project, collector);

  /* Check if the broker reports errors, if so stop the process */
  char info_check_rst[BROKER_ERR_MSG_LEN] = {0}; 
  if(utils_broker_check_url(info_url, info_check_rst, BROKER_ERR_MSG_LEN) 
     != 0) {
    return -1;
  }

  /* Extract the host and port information from the broker response */
  char *host = broker->info_host; char *port = broker->info_port;
  snprintf(host, sizeof(broker->info_host), "%s", strtok(info_check_rst, ":"));
  snprintf(port, sizeof(broker->info_port), "%s", strtok(NULL, ":"));

  /* If no SSH options are passed, start the RTR connection over TCP */
  config_validation_t *val = &cfg->cfg_val;
  if (ssh_options == NULL) {
    val->rtr_mgr_cfg = live_validation_start_connection(cfg, host, port,
                                                        NULL, NULL, NULL);
    return (val->rtr_mgr_cfg == NULL ? -1 : 0);
  }

  /* If SSH options are given, extract them & start the RTR connection (SSH) */
  char ssh_options_cpy[MAX_SSH_LEN] = {0};
  if (strlen(ssh_options) > MAX_SSH_LEN) {
    std_print("%s", "Error: SSH options exceed maximum length\n");
    return -1;
  }
  size_t ssh_size = sizeof(val->ssh_user);
  snprintf(ssh_options_cpy, sizeof(ssh_options_cpy), "%s", ssh_options);
  snprintf(val->ssh_user, ssh_size, "%s", strtok(ssh_options_cpy, ","));
  snprintf(val->ssh_hostkey, ssh_size, "%s", strtok(NULL, ","));
  snprintf(val->ssh_privkey, ssh_size, "%s", strtok(NULL, ","));
  val->rtr_mgr_cfg = live_validation_start_connection(cfg, host, port, 
                             val->ssh_user, val->ssh_hostkey, val->ssh_privkey);

  return (val->rtr_mgr_cfg == NULL ? -1 : 0);
}

struct rtr_mgr_config *live_validation_start_connection(rpki_cfg_t *cfg,
                                                        char *host, char *port,
                                                        char *ssh_user,
                                                        char *ssh_hostkey,
                                                        char *ssh_privkey)
{
  struct tr_socket *tr = malloc(sizeof(struct tr_socket));
  config_validation_t *val = &cfg->cfg_val;
  val->rtr_allocs[0] = tr;
  debug_print("Live mode (host: %s, port: %s)\n", host, port);

  /* If all SSH options are given and the ROAFetchlib was configured with SSH,
     set up a SSH config */
  if (ssh_user != NULL && ssh_hostkey != NULL && ssh_privkey != NULL) {
#ifdef WITH_SSH
    int ssh_port = strtol(port, NULL, 10);
    struct tr_ssh_config config = {host,     ssh_port,    NULL,
                                   ssh_user, ssh_hostkey, ssh_privkey};
    tr_ssh_init(&config, tr);

    if (tr_open(tr) == TR_ERROR) {
      std_print("%s", "Error: SSH socket not initialized, wrong SSH options\n");
      return NULL;
    }
#else
    std_print("%s", "Error: The library was not configured with SSH\n");
    return NULL;
#endif

  /* If not all SSH options are given set up a TCP config */
  } else {
    struct tr_tcp_config config = {host, port, NULL};
    tr_tcp_init(&config, tr);
  }

  /* Integrate the configuration into the socket*/
  struct rtr_socket *rtr = malloc(sizeof(struct rtr_socket));
  val->rtr_allocs[1] = rtr;
  rtr->tr_socket = tr;
  struct rtr_mgr_group groups[1];
  groups[0].sockets = malloc(sizeof(struct rtr_socket *));
  val->rtr_allocs[2] = groups[0].sockets;
  groups[0].sockets_len = 1;
  groups[0].sockets[0] = rtr;
  groups[0].preference = 1;
  val->rtr_socket = rtr;

  /* Initialize the RTR manager and and wait for a synchronized state */  
  struct rtr_mgr_config *conf;
  int ret = rtr_mgr_init(&conf, groups, 1, 30, 600, 600, NULL, NULL, NULL,NULL);
  rtr_mgr_start(conf);
  while (!rtr_mgr_conf_in_sync(conf))
    sleep(1);

  return conf;
}

void live_validation_close_connection(rpki_cfg_t *cfg)
{
  /* Close the RTR manager config (RTRlib) if it is initialized */
  config_validation_t *val = &cfg->cfg_val;
  if (val->rtr_mgr_cfg != NULL) {
    rtr_mgr_stop(val->rtr_mgr_cfg);
    rtr_mgr_free(val->rtr_mgr_cfg);
  }

  /* Close the Transport socket (RTRlib) */
  if (val->rtr_allocs[0] != NULL) {
    free(val->rtr_allocs[0]);
  }

  /* Close the RTR socket (RTRlib) */
  if (val->rtr_allocs[1] != NULL) {
    free(val->rtr_allocs[1]);
  }

  /* Close the Group socket (RTRlib) */
  if (val->rtr_allocs[2] != NULL) {
    free(val->rtr_allocs[2]);
  }
}

int live_validate_reason(rpki_cfg_t *cfg, uint32_t asn, char *prefix,
                         uint8_t mask_len, struct reasoned_result *reason)
{
  /* Convert the prefix in an RTRlib address */
  struct lrtr_ip_addr pref;
  if (lrtr_ip_str_to_addr(prefix, &pref) != 0) {
    std_print("%s", "Error: Address not interpretable\n");
    return -1;
  }
  enum pfxv_state result;
  struct pfx_record *pfx_reason = NULL;
  unsigned int reason_len = 0;

  /* Validate the BGP record with the current state of the given RTR server */
  if(pfx_table_validate_r(cfg->cfg_val.rtr_socket->pfx_table, &pfx_reason, 
                     &reason_len, asn, &pref, mask_len, &result) == PFX_ERROR) {
    std_print("%s\n", "Error: COuld not validate the record");
    return -1;
  }

  /* Return the RTRlib reasons for the validation */
  reason->reason = pfx_reason;
  reason->reason_len = reason_len;
  reason->result = result;

  return 0;
}


int historical_validate_reason(uint32_t asn, char *prefix, uint8_t mask_len,
                       struct pfx_table *pfxt, struct reasoned_result *reason)
{
  /* Convert the prefix in an RTRlib address */
  struct lrtr_ip_addr pref;
  if (lrtr_ip_str_to_addr(prefix, &pref) != 0) {
    std_print("%s", "Error: Address not interpretable\n");
    return -1;
  }
  enum pfxv_state result;
  struct pfx_record *pfx_reason = NULL;
  unsigned int reason_len = 0;

  /* Validate the BGP record with the given prefix table */
  if(pfx_table_validate_r(pfxt, &pfx_reason, &reason_len, asn, &pref, mask_len,
                          &result) == PFX_ERROR) {
    std_print("%s\n", "Error: COuld not validate the record");
    return -1;
  }

  /* Return the RTRlib reasons for the validation */
  reason->reason = pfx_reason;
  reason->reason_len = reason_len;
  reason->result = result;

  return 0;
}
