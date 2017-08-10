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

SUBDIRS = jsmn

AM_CPPFLAGS = -I$(top_srcdir) \
	-I$(top_srcdir)/srclib/jsmn

noinst_LTLIBRARIES=libutils.la

libutils_la_SOURCES = \
  broker.c \
  broker.h \
  rpki_config.c \
  rpki_config.h \
  constants.h \
  debug.h \
  elem.c \
  elem.h \
  historical_validation.c \
  historical_validation.h \
  live_validation.c \
  live_validation.h \
  rtr_cache_server.h \
  khash.h 

libutils_la_LIBADD = $(top_builddir)/src/lib/jsmn/libjsmn.la \
			$(CONDITIONAL_LIBS)

ACLOCAL_AMFLAGS = -I m4

CLEANFILES = *~
