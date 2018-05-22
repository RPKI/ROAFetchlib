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

#ifndef __UTILS_CFG_H
#define __UTILS_CFG_H

#include <stdint.h>

#include "constants.h"
#include "rpki_config.h"
#include "debug.h"
#include "rtrlib/rtrlib.h"

/** Check whether the projects/collectors are valid, if so add it
 *
 * @param[in] cfg                  Pointer to the configuration struct
 * @param[in] projects_collectors  Pointer to the projects/collectors
 * @param[in] mode                 Passed parameter for the mode
 * @return                         0 if the check process was valid,otherwise -1
 */
int utils_cfg_check_collectors(rpki_cfg_t *cfg, char *projects_collectors,
                               int mode);

/** Check whether the intervals are valid, if so add it
 *
 * @param[in] cfg              Pointer to the configuration struct
 * @param[in] time_intervals   Pointer to the string containing the intervals
 * @return                     0 if the check process was valid, otherwise -1
 */
int utils_cfg_check_intervals(rpki_cfg_t *cfg, char* time_intervals);

/** Check whether the parameter flags are valid, if so add it
 *
 * @param[in] cfg              Pointer to the configuration struct
 * @param[in] unified          Passed parameter for the unified mode
 * @param[in] mode             Passed parameter for the mode
 * @return                     0 if the check process was valid, otherwise -1
 */
int utils_cfg_check_flags(rpki_cfg_t *cfg, int unified, int mode);

/** Check whether the SSH parameters are valid, if so add it
 *
 * @param[in] cfg              Pointer to the configuration struct
 * @param[in] ssh_options      Passed SSH parameters
 * @return                     0 if the check process was valid, otherwise -1
 */
int utils_cfg_check_ssh_options(rpki_cfg_t *cfg, char* ssh_options);

/** Set the broker if a custom broker is set otherwise use the default broker
 *
 * @param[in] cfg              Pointer to the configuration struct
 * @param[in] broker_url       Pointer to the custom broker url
 * @return                     0 if the check process was valid, otherwise -1
 */
int utils_cfg_set_broker_urls(rpki_cfg_t *cfg, char* broker_url);

/** Check whether a input matches its specifications
 *
 * @param[in] input            Pointer to the input
 * @param[in] input_max_size   Maximum size of the input
 * @param[in] del              Delimiter for splitting the input
 * @param[in] item_max_count   Maximum number of input items
 * @param[in] input_cpy        Pointer to a duplicate of the input
 * @return                     0 if the check process was valid, otherwise -1
 */
int utils_cfg_check_input(char* input, size_t input_max_size, char* del,
                          size_t item_max_count, char* input_cpy);

/** Check whether a numberic field (ASN, Max-Len) of a ROA record is valid
 *
 * @param[in]  val             Pointer to the value field of the ROA record
 * @param[out] rst_val         Pointer to uintX_t to which the value is stored
 * @param[in]  unsigned_len    Size of the fixed unsignet integer type (8, 32)
 * @return                     0 if the check process was valid, otherwise -1
 */
int utils_cfg_validity_check_val(char* val, void *rst_val, int unsigned_len);


/** Check whether a prefix of a ROA record is valid
 *
 * @param[in]  prefix          Prefix of the ROA record
 * @param[out] address         Pointer to char array to which the addr is stored
 * @param[out] min_len         Pointer to uint8_t to which the min-len is stored
 * @return                     0 if the check process was valid, otherwise -1
 */
int utils_cfg_validity_check_prefix(char* prefix, char* address,
                                    uint8_t *min_len);

/** Add the projects/collectors parameter to the config struct
 *
 * @param[in]  input           Pointer to the passed input parameter
 * @param[in]  input_max_size  Maximum size of the input
 * @param[in]  item_max_size   Maximum size of the input items
 * @param[in]  item_max_count  Maximum number of input items
 * @param[in]  del             Delimiter for splitting the input
 * @param[out] cfg_str_concat  Pointer to a char array - sizeof(cfg_storage)
 * @param[out] cc              Pointer to a 2D array of fixed size (collector)
 * @param[out] proj            Pointer to a 2D array of fixed size (project)
 * @param[in]  cfg             Pointer to the RPKI configuration
 * @return                     0 if the add-process was valid, otherwise -1
 */
int utils_cfg_add_collectors(char*input, size_t input_max_size, 
                            size_t item_max_size, int item_max_count, char* del,
                            char* cfg_str_concat, char (*cc)[MAX_INPUT_LENGTH],
                            char (*proj)[MAX_INPUT_LENGTH], rpki_cfg_t *cfg);

/** Add the time intervals parameter to the config struct
 *
 * @param[in]  input           Pointer to the passed input parameter
 * @param[in]  input_max_size  Maximum size of the input
 * @param[in]  del             Delimiter for splitting the input
 * @param[in]  item_max_count  Maximum number of input items
 * @param[in]  item_max_size   Maximum size of the input items
 * @param[out] cfg_num         Pointer to the uint32_t array
 * @param[out] cfg_str_concat  Pointer to a char array holding the concatenation
 * @return                     0 if the add-process was valid, otherwise -1
 */
int utils_cfg_add_intervals(char *input, size_t input_max_size, char *del,
                            size_t item_max_count, size_t item_max_size,
                            uint32_t *cfg_num, char* cfg_str_concat);

/** Print a pfx_record
 *
 * @param[in]  pfx_record      Pfx_record which will be printed
 * @param[out] data            Call-back function
 */
void utils_cfg_print_record(const struct pfx_record *pfx_record, void *data);

#endif /* __UTILS_CFG_H */
