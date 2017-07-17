 #
 # This file is part of libhistoryrpki
 #
 # Author: Samir Al-Sheikh (Freie Universitaet, Berlin)
 #         s.al-sheikh@fu-berlin.de
 #
 # MIT License
 #
 # Copyright (c) 2017 The Libhistoryrpki authors
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

SUBDIRS= lib

AM_CPPFLAGS = 	-I$(top_srcdir) \
		-I$(top_srcdir)/src/lib

lib_LTLIBRARIES = libhistoryrpki.la

libincludesubdir=$(pkgincludedir)/lib

pkginclude_HEADERS = libhistoryrpki.h		\
	rpki.h

libincludesub_HEADERS = lib/rpki_config.h  \
  lib/broker.h    \
  lib/constants.h \
  lib/elem.h      \
  lib/khash.h     \
  lib/live_validation.h \
  lib/historical_validation.h

libhistoryrpki_la_SOURCES = 	\
	rpki.h		\
	rpki.c		\
	libhistoryrpki.h

libhistoryrpki_la_CFLAGS = -Wall -lrtr -lwandio

libhistoryrpki_la_LIBADD = $(top_builddir)/src/lib/libutils.la

ACLOCAL_AMFLAGS = -I m4

CLEANFILES = *~
