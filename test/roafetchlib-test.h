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

#ifndef __ROAFETCHLIB_TEST_H
#define __ROAFETCHLIB_TEST_H

#include "roafetchlib.h"

#define RESULT_LEN 40

#define sep "--------------------------------------------------------"         \

#define PRINT_SSECTION(name)                                                   \
  do {                                                                         \
    fprintf(stderr, "*  " name "\n");                                          \
  } while (0)

#define PRINT_INTENDED_ERR                                                     \
  do {                                                                         \
    fprintf(stderr, "*   Intended ");                                          \
  } while (0)

#define CHECK_SECTION(name, check)                                             \
  do {                                                                         \
    int s = RESULT_LEN - strlen("Test-Section: ") - strlen(name);              \
    int r = RESULT_LEN - strlen("Result for section ") - strlen(name);         \
    fprintf(stderr, "* " sep "\n");                                            \
    fprintf(stderr, "* %*c Test-Section: " name "\n", s/2, ' ');               \
    fprintf(stderr, "* " sep "\n");                                            \
    if (!(check)) {                                                            \
      fprintf(stderr, "* " sep "\n");                                          \
      fprintf(stderr, "* %*c Result for section " name ": FAILED\n", r/2, ' ');\
      fprintf(stderr, "* " sep "\n");                                          \
      return -1;                                                               \
    } else {                                                                   \
      fprintf(stderr, "* " sep "\n");                                          \
      fprintf(stderr, "* %*c Result for section " name ": OK\n", r/2, ' ');    \
      fprintf(stderr, "* " sep "\n\n");                                        \
    }                                                                          \
  } while (0)

#define CHECK_SUBSECTION(name, first, check)                                   \
  do {                                                                         \
    if (first) {                                                               \
      fprintf(stderr, "* ");                                                   \
    } else {                                                                   \
      fprintf(stderr, "*\n* ");                                                \
    }                                                                          \
    fprintf(stderr, "Checking " name ":\n");                                   \
    if (!(check)) {                                                            \
      fprintf(stderr, "* " name ": FAILED\n");                                 \
      return -1;                                                               \
    } else {                                                                   \
      fprintf(stderr, "* " name ": OK\n");                                     \
    }                                                                          \
  } while (0)

#define CHECK_RESULT(test, type, check)                                        \
  do {                                                                         \
    int s = RESULT_LEN - strlen(type) - strlen(test);                          \
    if (!(check)) {                                                            \
      fprintf(stderr, "*   Test: %s%s ... %*c FAILED\n", type, test, s, ' ');  \
      return -1;                                                               \
    }                                                                          \
    fprintf(stderr, "*   Test: %s%s ... %*c OK\n", type, test, s, ' ');        \
  } while (0)

#endif /* __ROAFETCHLIB_TEST_H */
