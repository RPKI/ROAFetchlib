 #
 # This file is part of ROAFetchlib
 #
 # Author: Samir Al-Sheikh (Freie Universitaet, Berlin)
 #         s.al-sheikh@fu-berlin.de
 #
 # MIT License
 #
 # Copyright (c) 2017 The ROAFetchlib authors
 #
 # Permission is hereby granted, free of charge, to any person obtaining a copy
 # of this software and associated documentation files (the "Software"), to deal
 # in the Software without restriction, including without limitation the rights
 # to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 # copies of the Software, and to permit persons to whom the Software is
 # furnished to do so, subject to the following conditions:
 # 
 # The above copyright notice and this permission notice shall be included in all
 # copies or substantial portions of the Software.
 # 
 # THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 # IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 # FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 # AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 # LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 # OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 # SOFTWARE.
 #

AM_CPPFLAGS =	                                   \
	-I$(top_srcdir)                                \
	-I$(top_srcdir)/src                            \
	-I$(top_srcdir)/src/lib

AM_CFLAGS = -c -Wall -Iinclude

noinst_LTLIBRARIES=libroafetch-utils.la

include_HEADERS=                                 \
  utils.h                                        \
  utils_broker.h                                 \
  utils_cfg.h                                    \
  utils_elem.h                                   \
  utils_rpki.h

libroafetch_utils_la_SOURCES =                   \
  utils.h                                        \
  utils_broker.c                                 \
  utils_broker.h                                 \
  utils_cfg.c                                    \
  utils_cfg.h                                    \
  utils_elem.c                                   \
  utils_elem.h                                   \
  utils_rpki.c                                   \
  utils_rpki.h

libroafetch_utils_la_LIBADD = $(CONDITIONAL_LIBS)

ACLOCAL_AMFLAGS = -I m4

CLEANFILES = *~
