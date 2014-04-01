#! /bin/sh
ICU_SRC=/home/mscherer/svn.icu/trunk/src
PREFIX=/home/mscherer/svn.icu/trunk/inst
ICUAPPS_SRC=/home/mscherer/svn.icuapps/collv2/src
CGI_BIN=/var/www/cgi-bin

CC=/home/mscherer/clang/bin/clang
CXX=/home/mscherer/clang/bin/clang++
CFLAGS="-g -std=c99 -Wall -pedantic -Wshadow -Wpointer-arith -Wmissing-prototypes -Wwrite-strings"
CXXFLAGS="-g --std=c++0x -W -Wall -pedantic -Wpointer-arith -Wwrite-strings -Wno-long-long -DU_USING_ICU_NAMESPACE=0"
CPPFLAGS="-DU_NO_DEFAULT_INCLUDE_UTF_HEADERS=1 -DU_DEBUG=1 -D_REENTRANT -DU_HAVE_ELF_H=1 -DUNISTR_FROM_CHAR_EXPLICIT=explicit -DUNISTR_FROM_STRING_EXPLICIT= -I$ICU_SRC/source/common -I$ICU_SRC/source/i18n -I$ICU_SRC/source/tools/toolutil"
LINKFLAGS="-L$PREFIX/lib -licutu -licui18n -licuuc -licudata -lpthread -ldl -lm"

$CXX $CPPFLAGS $CXXFLAGS $LINKFLAGS $ICUAPPS_SRC/webdemo/collation/available.cpp -o $PREFIX/bin/available
LD_LIBRARY_PATH=$PREFIX/lib $PREFIX/bin/available > $ICUAPPS_SRC/../available-collators.txt

$CXX $CPPFLAGS $CXXFLAGS $LINKFLAGS $ICUAPPS_SRC/webdemo/collation/sortcgi.cpp -o $PREFIX/bin/sortcgi
cp $PREFIX/bin/sortcgi $CGI_BIN/collation
chmod a+rx $CGI_BIN/collation/sortcgi

cp webdemo/collation/index.html /var/www/collation.html
chmod a+r /var/www/collation.html

# ~/svn.icuapps/collv2/src$ webdemo/collation/build.sh
