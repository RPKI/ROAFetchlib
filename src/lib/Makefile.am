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

SUBDIRS = jsmn utils

AM_CPPFLAGS =	                                        \
	-I$(top_srcdir)                                     \
	-I$(top_srcdir)/src/lib/utils                       \
	-I$(top_srcdir)/src/lib/jsmn

noinst_LTLIBRARIES=libroafetch-lib.la

libutilsincludesubdir=$(pkgincludedir)/lib/utils/

libutilsincludesub_HEADERS =                          \
  utils/utils.h                                       \
  utils/utils_broker.h                                \
  utils/utils_cfg.h                                   \
  utils/utils_elem.h                                  \
  utils/utils_rpki.h

libroafetch_lib_la_SOURCES =	                        \
	broker.c                                            \
	broker.h                                            \
	rpki_config.c                                       \
	rpki_config.h                                       \
	constants.h                                         \
	debug.h                                             \
	elem.c                                              \
	elem.h                                              \
	validation.c                                        \
	validation.h                                        \
	khash.h

libroafetch_lib_la_LIBADD =	                          \
	$(top_builddir)/src/lib/jsmn/libjsmn.la             \
	$(top_builddir)/src/lib/utils/libroafetch-utils.la  \
	$(CONDITIONAL_LIBS)

ACLOCAL_AMFLAGS = -I m4

CLEANFILES = *~
