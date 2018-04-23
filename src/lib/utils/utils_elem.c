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

#include <stdio.h> 
#include <string.h>
#include <stdlib.h>

#include "utils_elem.h"

int utils_elem_sort_result(char* result, size_t size,
                           char* sorted_result, char* del)
{
  /* Count delimiter occurrences */
  char result_cpy[size]; char *r_c = result_cpy;
  char temp[size]; int cnt = 0; int del_c = 0;
  strcpy(result_cpy, result);
  for (cnt = 0; r_c[cnt]; r_c[cnt] == del[0] ? cnt++ : *r_c++);
 
  /* Split result based on delimiter, sort lexicographically and concatenate*/
  char str[cnt][size];
  char *ptr = strtok(result_cpy, del);
  while(ptr != NULL) { strcpy(str[del_c++], ptr); ptr = strtok(NULL, del); }
  for(int i = 0; i < cnt - 1; i++) {
    for(int j = i + 1; j < cnt ; j++) {
      if(strcmp(str[i], str[j]) > 0) {
          strcpy(temp, str[i]); strcpy(str[i], str[j]); strcpy(str[j], temp);
      }
    }
  }
  for (int x = 0; x < cnt; x++) {
    snprintf(temp, sizeof(temp), "%s%s", str[x], del);
    strncat(sorted_result, temp, size);
  }

  return 0;
}
