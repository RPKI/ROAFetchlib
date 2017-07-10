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

## This is a script to clean up the libhistoryrpki folder for GitHub

## Clean make and all deps
make distclean

## Delete all Makefile.in files
rm -rf "src/lib/Makefile.in"
rm -rf "src/Makefile.in"
rm -rf "doc/Makefile.in"
rm -rf "man/Makefile.in"
rm -rf "script/Makefile.in"
rm -rf "src/lib/jsmn/Makefile.in"

## Delete all Autotools files
rm -rf "stamp-h1"
rm -rf "autom4te.cache/"
rm -rf "config.guess"
rm -rf "config.h"
rm -rf "config.h.in"
rm -rf "config.log"
rm -rf "config.status"
rm -rf "config.sub"
rm -rf "configure"
rm -rf "depcomp"
rm -rf "configure"
rm -rf "install-sh"
rm -rf "libtool"
rm -rf "ltmain.sh"
rm -rf "Makefile"
rm -rf "Makefile.in"
rm -rf "missing"
rm -rf "aclocal.m4"
rm -rf "compile"

## Delete Doxygen files
rm -rf "src/doc/Doxyfile"
