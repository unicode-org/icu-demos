#! /bin/sh

# Adjust the local-machine variables, and then do this:
# ~/svn.icuapps/trunk/src$ webdemo/collation/build.sh

HOME=/usr/local/google/home/mscherer
ICU_SRC=$HOME/svn.icu/trunk/src/icu4c
PREFIX=$HOME/svn.icu/trunk/inst/icu4c
ICUAPPS_SRC=$HOME/svn.icuapps/trunk/src
CGI_BIN=/var/www/cgi-bin

CXX=clang++
CXXFLAGS="-g --std=c++0x -W -Wall -pedantic -Wpointer-arith -Wwrite-strings -Wno-long-long -DU_USING_ICU_NAMESPACE=0"
CPPFLAGS="-DU_NO_DEFAULT_INCLUDE_UTF_HEADERS=1 -DU_DEBUG=1 -D_REENTRANT -DU_HAVE_ELF_H=1 -DUNISTR_FROM_CHAR_EXPLICIT=explicit -DUNISTR_FROM_STRING_EXPLICIT= -I$ICU_SRC/source/common -I$ICU_SRC/source/i18n -I$ICU_SRC/source/tools/toolutil"
LINKFLAGS="-L$PREFIX/lib -licutu -licui18n -licuuc -licudata -lpthread -ldl -lm"

# Build & run the tool that writes the contents for the
# ICU collation demo available-collators drop-down list.
# Manually copy the output into the relevant part of index.html.
$CXX $CPPFLAGS $CXXFLAGS $LINKFLAGS $ICUAPPS_SRC/webdemo/collation/available.cpp -o $PREFIX/bin/available
LD_LIBRARY_PATH=$PREFIX/lib $PREFIX/bin/available > $ICUAPPS_SRC/../available-collators.txt

# Build the collation demo binary.
$CXX $CPPFLAGS $CXXFLAGS $LINKFLAGS $ICUAPPS_SRC/webdemo/collation/sortcgi.cpp -o $PREFIX/bin/sortcgi

# Install the collation demo HTML page & binary.

# cp $PREFIX/bin/sortcgi $CGI_BIN/collation
# chmod a+rx $CGI_BIN/collation/sortcgi

# cp webdemo/collation/index.html /var/www/collation.html
# chmod a+r /var/www/collation.html
