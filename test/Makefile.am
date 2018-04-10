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

AM_CPPFLAGS = -I$(top_srcdir)   \
	 	-I$(top_srcdir)/src         \
	 	-I$(top_srcdir)/src/lib

TESTS =				                  \
  roafetchlib-test-config       \
  roafetchlib-test-validation   \
	roafetchlib-test-broker       \
	roafetchlib-test-rpki

check_PROGRAMS =				        \
  roafetchlib-test-config       \
  roafetchlib-test-validation   \
	roafetchlib-test-broker       \
  roafetchlib-test-rpki

roafetchlib_test_config_CFLAGS = -Wall -lrtr -lwandio
roafetchlib_test_config_SOURCES = roafetchlib-test-config.c roafetchlib-test-config.h
roafetchlib_test_config_LDADD = $(top_builddir)/src/libroafetch.la

roafetchlib_test_validation_SOURCES = roafetchlib-test-validation.c roafetchlib-test-validation.h
roafetchlib_test_validation_LDADD = $(top_builddir)/src/libroafetch.la

roafetchlib_test_broker_SOURCES = roafetchlib-test-broker.c roafetchlib-test-broker.h
roafetchlib_test_broker_LDADD = $(top_builddir)/src/libroafetch.la

roafetchlib_test_rpki_SOURCES = roafetchlib-test-rpki.c roafetchlib-test-rpki.h
roafetchlib_test_rpki_LDADD = $(top_builddir)/src/libroafetch.la

ACLOCAL_AMFLAGS = -I m4

CLEANFILES = *~
