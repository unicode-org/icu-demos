#! /bin/sh
# © 2016 and later: Unicode, Inc. and others.
# License & terms of use: http://www.unicode.org/copyright.html

# Build script for http://demo.icu-project.org/icu-bin/collation.html

# To update the index.html file for the demo:
# 1. "make install" ICU, for example with
#   .../runConfigureICU Linux --prefix=/usr/local/google/home/mscherer/svn.icu/trunk/inst/icu4c
# 2. Adjust the local-machine variables below, and then do this:
#   ~/svn.icuapps/trunk/src$ webdemo/collation/build.sh
# 3. Check the output from "available":
#   ~/svn.icuapps/trunk/src$ meld webdemo/collation/index.html ../available-collators.txt
#   Compare the part of index.html between the "begin output" and "end output" lines
#   with the newly generated HTML snippet.
# 4. Fix problems & regenerate if necessary.
# 5. If there are changes, then
#   ~/svn.icuapps/trunk/src$ cp ../available-collators.txt webdemo/collation/index.html
#   and review diffs again, and commit into the repository.

HOME=/usr/local/google/home/mscherer
ICU_SRC=$HOME/svn.icu/trunk/src/icu4c
PREFIX=$HOME/svn.icu/trunk/inst/icu4c
ICUAPPS_SRC=$HOME/svn.icuapps/trunk/src
CGI_BIN=/var/www/cgi-bin

CXX=clang++
CXXFLAGS="-g --std=c++0x -W -Wall -pedantic -Wpointer-arith -Wwrite-strings -Wno-long-long -DU_USING_ICU_NAMESPACE=0"
CPPFLAGS="-DU_NO_DEFAULT_INCLUDE_UTF_HEADERS=1 -DU_DEBUG=1 -D_REENTRANT -DU_HAVE_ELF_H=1 -DUNISTR_FROM_CHAR_EXPLICIT=explicit -DUNISTR_FROM_STRING_EXPLICIT= -I$ICU_SRC/source/common -I$ICU_SRC/source/i18n -I$ICU_SRC/source/tools/toolutil"
LINKFLAGS="-L$PREFIX/lib -licutu -licui18n -licuuc -licudata -lpthread -ldl -lm"
LDFLAGS=-fsanitize=bounds

# Build & run the tool that writes the contents for the
# ICU collation demo available-collators drop-down list.
# Manually copy the output into the relevant part of index.html.
$CXX $CPPFLAGS $CXXFLAGS $LINKFLAGS $LDFLAGS $ICUAPPS_SRC/webdemo/collation/available.cpp -o $PREFIX/bin/available
LD_LIBRARY_PATH=$PREFIX/lib $PREFIX/bin/available > $ICUAPPS_SRC/../available-collators.txt

# Build the collation demo binary.
$CXX $CPPFLAGS $CXXFLAGS $LINKFLAGS $LDFLAGS $ICUAPPS_SRC/webdemo/collation/sortcgi.cpp -o $PREFIX/bin/sortcgi

# Install the collation demo HTML page & binary.

# cp $PREFIX/bin/sortcgi $CGI_BIN/collation
# chmod a+rx $CGI_BIN/collation/sortcgi

# cp webdemo/collation/index.html /var/www/collation.html
# chmod a+r /var/www/collation.html
