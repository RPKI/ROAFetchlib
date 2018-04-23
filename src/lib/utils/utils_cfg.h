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
#include "debug.h"
#include "rtrlib/rtrlib.h"

/** Check if a numberic field (ASN, Max-Len) of a ROA record is valid
 *
 * @param val             Pointer to the value field of the ROA record
 * @param rst_val         Pointer to uintX_t to which the value is stored
 * @param unsigned_len    Size of the fixed unsignet integer type (8, 32)
 * @return                0 if the check process was valid, otherwise -1
 */
int utils_cfg_validity_check_val(char* val, void *rst_val, int unsigned_len);


/** Check if a prefix of a ROA record is valid
 *
 * @param prefix          Prefix of the ROA record
 * @param address         Pointer to char array to which the address is stored
 * @param min_len         Pointer to an uint8_t to which the min-len is stored
 * @return                0 if the check process was valid, otherwise -1
 */
int utils_cfg_validity_check_prefix(char* prefix, char* address, uint8_t *min_len);

/** Add an input argument to the config struct
 *
 * @param input           Pointer to the passed input parameter
 * @param input_max_size  Maximum size of the input
 * @param item_max_size   Maximum size of the input items
 * @param item_max_count  Maximum number of input items
 * @param del             Delimiter for splitting the input
 * @param cfg_str_concat  Pointer to a char array - sizeof(cfg_storage)
 * @param cfg_str         Pointer to a two dimensional char array of fixed size
 * @param cfg_num         Pointer to a uint32_t array if the input is time-based
 * @return                number of items added to the config storage
 */
int utils_cfg_add_input(char*input, size_t input_max_size, size_t item_max_size,
                        int item_max_count, char* del, char* cfg_str_concat,
                        char (*cfg_str)[MAX_INPUT_LENGTH], uint32_t *cfg_num);

/** Print a pfx_record
 *
 * @param pfx_record      Pfx_record which will be printed
 * @param data            Call-back function
 */
void utils_cfg_print_record(const struct pfx_record *pfx_record, void *data);

#endif /* __UTILS_H */
