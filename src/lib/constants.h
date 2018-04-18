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

#define STRLEN(s) (sizeof(s)/sizeof(s[0]))

/* -------------------- Input ------------------------- */

/** Max number of RPKI collectors */
#define MAX_RPKI_COUNT 32

/** Max size of a single input arguments */
#define MAX_INPUT_LENGTH 64

/** Max size of a interval timestamp */
#define MAX_INTERVAL_SIZE 24

/** Max number of time windows */
#define MAX_TIME_WINDOWS 1024

/** Max length of SSH options */
#define MAX_SSH_LEN 1024

/* -------------------- Validation -------------------- */

/** Length of a valid asn entry (project,collector,status,ASN) */
#define VALID_ASN_LEN 64

/** Length of a valid prefix entry (all prefixes related to an ASN) */
#define VALID_PFX_LEN 64

/** Number of a valid reasons */
#define VALID_REASONS_SIZE 5 * MAX_RPKI_COUNT

/** Size of the validation result buffer */
#define VALIDATION_MAX_RESULT_LEN 4096

#define VALIDATION_MAX_SINGLE_RESULT_LEN 512

/* -------------------- Broker ------------------------ */

/** Max size of the error message of the broker */
#define BROKER_ERR_MSG_LEN 80

/** Size of a string representation of a port number */
#define BROKER_INFO_RESP_URL_LEN 64

/** Size of a string representation of a port number */
#define BROKER_INFO_RESP_PORT_LEN 6

/** Initial count of ROA dump URLs in JSON */
#define BROKER_ROA_URLS_COUNT 2048

/** Max size of the broker JSON buffer */
#define BROKER_JSON_BUF_SIZE 6144

/** Max size of the broker ROA dump buffer */
#define BROKER_ROA_DUMP_BUFLEN 6144

/** Max size of the broker URL without arguments */
#define BROKER_MAX_MAIN_URL_LEN 1024

/** Max size of the requested info URL 
		project, collector (64) */
#define BROKER_INFO_REQ_URL_LEN BROKER_MAX_MAIN_URL_LEN + 64

/** Max size of all ROA dump URLs for one timestamp */
#define BROKER_ROA_URLS_LEN 128 * MAX_RPKI_COUNT

/** Max size of the requested broker URL 
		project (32 + 1), collector (9 + 1), interval length (24 + 1) */
#define BROKER_REQUEST_URL_LEN \
				STRLEN(BROKER_HISTORY_VALIDATION_URL) + 30 + \
        ROA_ARCHIVE_PJ_MAX_LEN * MAX_RPKI_COUNT + ROA_ARCHIVE_CC_MAX_LEN * \
        MAX_RPKI_COUNT + (MAX_INTERVAL_SIZE + 1) * MAX_RPKI_COUNT

/** RPKI historical validation broker URL*/
#define BROKER_HISTORY_VALIDATION_URL "http://roa-broker.realmv6.org/broker?"

/** RPKI historical validation broker URL*/
#define BROKER_LIVE_VALIDATION_INFO_URL "http://roa-broker.realmv6.org/info?"

/* -------------------- ROA Archive ------------------- */

/** Time interval of the dump files in the ROA archive */
#define ROA_ARCHIVE_INTERVAL 180

/** Max length of a project name */
#define ROA_ARCHIVE_PJ_MAX_LEN 33

/** Max length of a collector name - CCXX(RTR)*/
#define ROA_ARCHIVE_CC_MAX_LEN 10

/** @} */

#endif /* _CONSTANTS_H */
