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

#ifndef __UTILS_BROKER_H
#define __UTILS_BROKER_H

#include "debug.h"
#include "rpki_config.h"
#include "wandio.h"

/** Print all entries of the broker result khash
 *
 * @param[in] cfg              Pointer to the configuration 
 */
void utils_broker_print_debug(rpki_cfg_t *cfg);

/** Check whether the broker service URL works and throws no error message
 *
 * @param[in] broker_url       Broker request/info URL
 * @param[in] result           Buffer where the response is stored
 * @param[in] size             Size of the buffer
 * @return                     0 if the check process was valid, otherwise -1
 */
int utils_broker_check_url(char *broker_url, char* result, size_t size);

/** Add the projects/collectors response of the broker to the configuration
 *
 * @param[in]  input           Pointer to the broker response
 * @param[out] cfg_str_concat  Pointer to a char array holding the concatenation
 * @param[in]  del             Delimiter
 * @param[out] cfg_str         Pointer to a 2D array of fixed size
 * @return                     0 if the add process was valid, otherwise -1
 */
int utils_broker_add_projects_collectors(char* input, char* cfg_str_concat,
                                  char* del, char (*cfg_str)[MAX_INPUT_LENGTH]);

#endif /* __UTILS_BROKER_H */
