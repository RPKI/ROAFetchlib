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

#ifndef __ROAFETCHLIB_TEST_RPKI_H
#define __ROAFETCHLIB_TEST_RPKI_H

#include "roafetchlib.h"

// Validate RPKI-enabled BGP Beacons from RIPE
// see: https://www.ripe.net/analyse/internet-measurements/routing-information-service-ris/current-ris-routing-beacons

#define TEST1_O_ASN 12654
#define TEST1_PFX "93.175.146.0"
#define TEST1_MSKL 24

#define TEST2_O_ASN 12654
#define TEST2_PFX "2001:7fb:fd02::"
#define TEST2_MSKL 48

#define TEST3_O_ASN 12654
#define TEST3_PFX "93.175.147.0"
#define TEST3_MSKL 24
#define TEST3_V_O_ASN 196615

#define TEST4_O_ASN 12654
#define TEST4_PFX "2001:7fb:fd03::"
#define TEST4_MSKL 48
#define TEST4_V_O_ASN 196615

#define TEST5_O_ASN 0
#define TEST5_PFX "84.205.83.0"
#define TEST5_MSKL 24

#define TEST6_O_ASN 0
#define TEST6_PFX "2001:7fb:ff03::"
#define TEST6_MSKL 48

#define TEST_BUF_LEN 2048
#define TEST_PROJECT "FU-Berlin"
#define TEST_COLLECTOR "CC06(RTR)"
#define TEST_TIMESTAMP 1000000000

#define TEST_HISTORY_TS 1000000000
#define TEST_HISTORY_TIMEWDW "1000000000-1000000000"

#endif /* __ROAFETCHLIB_TEST_RPKI_H */
