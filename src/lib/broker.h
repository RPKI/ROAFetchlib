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

#ifndef __BROKER_H
#define __BROKER_H

#include <stdint.h>

#include "rpki_config.h"
#include "elem.h"

/** Connects to RPKI broker for the validation
 *
 * @param cfg             pointer to the configuration struct
 * @param projects        all projects of the RPKI collectors
 * @param collectors      all RPKI collectors
 * @param start           start time as UTC epoch timestamp
 * @param end             end time as UTC epoch timestamp
 */
int broker_connect(rpki_cfg_t* cfg, char* project, char* collector, char* time_intervals);

/** Reads the broker response (JSON) into a buffer
 *
 * @param cfg             pointer to the configuration struct
 * @param broker_url      broker URL containing the necessary parameters (projects, collectors, time-window)
 */
int broker_json_buf(rpki_cfg_t* cfg, char* broker_url);

/** Parses the JSON buffer and stores all values in the broker result khash table
 *
 * @param cfg             pointer to the configuration struct
 * @param js              pointer to the JSON buffer
 * @return                0 if the JSON parsing was valid, otherwise -1
 */
int broker_parse_json(rpki_cfg_t* cfg, char* js);

/** Prints all entries of the broker result khash
 *
 * @param cfg           pointer to the configuration 
 */
void broker_print_debug(rpki_cfg_t* cfg);

/** @} */

#endif /* __BROKER_H */
