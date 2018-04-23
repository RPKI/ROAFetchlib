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

#ifndef __BROKER_H
#define __BROKER_H

#include <stdint.h>

#include "elem.h"
#include "rpki_config.h"

/** Connecting to RPKI broker for the validation
 *
 * @param[in] cfg            Pointer to the configuration struct
 * @param[in] projects       All projects of the RPKI collectors
 * @param[in] collectors     All RPKI collectors
 * @param[in] start          Start time as UTC epoch timestamp
 * @param[in] end            End time as UTC epoch timestamp
 * @return                   0 if broker connection was successful, otherwise -1
 */
int broker_connect(rpki_cfg_t *cfg, char *project, char *collector,
                   char *time_intervals);

/** Read the broker response (JSON) into a buffer
 *
 * @param[in] cfg            Pointer to the configuration struct
 * @param[in] broker_url     Broker request URL
 * @return                   0 if the JSON read-in was successful, otherwise -1
 */
int broker_json_buf(rpki_cfg_t *cfg, char *broker_url);

/** Parse the JSON buffer and stores all values in the broker result khash
 * table
 *
 * @param[in/out] cfg        Pointer to the configuration struct
 * @param[in] js             Pointer to the JSON buffer
 * @return                   0 if the JSON parsing was valid, otherwise -1
 */
int broker_parse_json(rpki_cfg_t *cfg, char *js);

/** @} */

#endif /* __BROKER_H */
