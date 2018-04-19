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

AC_PREREQ([2.69])
AC_INIT([roafetchlib], [0.1.0], [s.al-sheikh@fu-berlin.de])
AM_INIT_AUTOMAKE()
AC_CONFIG_MACRO_DIR([m4])
AC_PROG_RANLIB
AC_CONFIG_SRCDIR([src/rpki.h])
AC_CONFIG_HEADERS([config.h])

LT_INIT([disable-static])

AC_PROG_CXX
AC_PROG_CC
AC_PROG_CC_STDC

AC_C_INLINE
AC_TYPE_SIZE_T
AC_TYPE_UINT32_T
AC_TYPE_UINT64_T
AC_TYPE_UINT8_T

AC_FUNC_MALLOC
AC_FUNC_REALLOC
AC_CHECK_FUNCS([memset strchr strcspn])

# Debug mode
AC_MSG_NOTICE([------ Debug configuration  ------])
AC_MSG_CHECKING([whether to build with debug information])
AC_ARG_ENABLE([debug],
              [AS_HELP_STRING([--enable-debug],
              [enable debug data generation (def=no)])],
              [debugval="$enableval"],
              [debugval=no])
AC_MSG_RESULT([$debugval])
AS_IF([test x"$debugval" = x"yes"],
	[
	  AC_DEFINE_UNQUOTED([RPKI_DEBUG],[1],
		[Enable debugging output])
	])

## Wandio configuration
AC_MSG_NOTICE([------ Wandio configuration  ------])
AC_SEARCH_LIBS([wandio_create], [wandio], [with_wandio=yes],
               [AC_MSG_ERROR(
  [libwandio required (http://research.wand.net.nz/software/libwandio.php)]
)])
AM_CONDITIONAL([WITH_WANDIO], [test "x$with_wandio" == xyes])

## RTR configuration
AC_MSG_NOTICE([------ RTRLib configuration  ------])
AC_CHECK_LIB([rtr], [rtr_mgr_init], [with_rtr=yes],
               [AC_MSG_ERROR(
  [rtrlib is required (http://rtrlib.realmv6.org)]
)])
AM_CONDITIONAL([WITH_RTR], [test "x$with_rtr" == xyes])

## SSH configuration
AC_MSG_CHECKING([whether the RTR library is compiled with SSH])
AC_ARG_WITH([ssh],
	[AS_HELP_STRING([--without-ssh],
	  [do not compile with ssh support])],
	  [],
	  [with_ssh=yes])
AM_CONDITIONAL([WITH_ssh], [test "x$with_ssh" != xno])
if test x"$with_ssh" = xyes; then
AC_LINK_IFELSE([AC_LANG_PROGRAM([[#include "rtrlib/rtrlib.h"]],
                        [[struct tr_ssh_config config;]])],
      [AC_MSG_RESULT(yes)
       AC_DEFINE([WITH_SSH],[1],[Building with SSH support])],
       AC_MSG_RESULT(no)
  )
else
AC_MSG_RESULT([no])
fi

## Doxygen
AC_MSG_NOTICE([------ Doxygen configuration ------])
AC_CHECK_PROGS([DOXYGEN], [doxygen])
if test -z "$DOXYGEN";
   then AC_MSG_WARN([Doxygen not found - continuing without Doxygen support])
fi
AM_CONDITIONAL([HAVE_DOXYGEN], 
[test -n "$DOXYGEN"])AM_COND_IF([HAVE_DOXYGEN], [AC_CONFIG_FILES([doc/Doxyfile])])

AC_CHECK_HEADERS([limits.h stddef.h stdint.h stdlib.h string.h unistd.h])
AC_CONFIG_FILES([Makefile
                 doc/Makefile
                 man/Makefile
                 src/Makefile
                 src/lib/Makefile
                 src/lib/utils/Makefile
                 src/lib/jsmn/Makefile
                 test/Makefile])
AC_OUTPUT
