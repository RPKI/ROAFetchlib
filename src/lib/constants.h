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

#ifndef _CONSTANTS_H
#define _CONSTANTS_H

/** RPKI validation result max length */
#define RPKI_RST_MAX_LEN 2048

/** RPKI validation result max ROA entries */
#define RPKI_MAX_ROA_ENT 256

/** Max number of RPKI collectors */
#define MAX_RPKI_COUNT 32

/** Max size of the input arguments */
#define MAX_INPUT_LENGTH 64

/** Max size of ROA URLs */
#define UTILS_ROA_STR_NAME_LEN 2048

/** Max number of ROA entries in JSON */
#define MAX_BROKER_RESPONSE_ENT 2048

/** Max size of the broker URL buffer */
#define RPKI_BROKER_URL_BUFLEN 4096

/** Max size of the broker URL */
#define RPKI_BROKER_URL_LEN 4096

/** Max size of the JSON buffer */
#define JSON_BUF_SIZE 6144

/** Time interval of the ROA files */
#define ROA_INTERVAL 180

/** Max size of a interval timestamp */
#define MAX_INTERVAL_SIZE 24

/** Max number of time windows */
#define MAX_TIME_WINDOWS 1024

/** Max length of SSH options */
#define MAX_SSH_LEN 1024

/** RPKI historical validation broker URL*/
#define URL_HISTORY_VALIDATION_BROKER "http://roa-broker.realmv6.org/broker?"

/** RPKI historical validation broker URL*/
#define URL_LIVE_VALIDATION_INFO_BROKER "http://roa-broker.realmv6.org/info?"

/** @} */

#endif /* _CONSTANTS_H */
