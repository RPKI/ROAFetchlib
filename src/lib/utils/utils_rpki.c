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

#include "utils_rpki.h"
#include "debug.h"

void utils_rpki_print_config_debug(rpki_cfg_t* cfg){
  debug_print("%s", "----------- Library Input ------------------\n");
  debug_print("Projects:        %s\n",      cfg->cfg_input.broker_projects);
  debug_print("Collectors:      %s\n",      cfg->cfg_input.broker_collectors);
  debug_print("Unified:         %i\n",      cfg->cfg_input.unified);
  debug_print("Mode:            %i\n",      cfg->cfg_input.mode);
  debug_print("Interval:        %s\n\n",    cfg->cfg_input.broker_intervals);
  debug_print("%s", "----------- Hashtable ----------------------\n");
  debug_print("Khash Count:     %i\n",      cfg->cfg_broker.broker_khash_count);
  debug_print("First Timestamp: %"PRIu32"\n",cfg->cfg_time.start);
  debug_print("Last Timestamp:  %"PRIu32"\n\n",cfg->cfg_time.max_end);
  debug_print("%s", "----------- Sorted Broker Array ------------\n");
  for(int i = 0; i < cfg->cfg_broker.broker_khash_count; i++)
  debug_print("Url: %s\n", cfg->cfg_broker.roa_urls[i]);
  debug_print("%s", "--------------------------------------------\n");
}
