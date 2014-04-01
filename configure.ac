dnl -*-m4-*-
dnl configure.in for ICU Apps
dnl Steven R. Loomis
dnl Copyright (C) 1999-2014, International Business Machines Corporation and
dnl   others. All Rights Reserved.

dnl Process this file with autoconf to produce a configure script

AC_PREREQ(2.69)
AC_CONFIG_MACRO_DIR([m4])
AC_INIT(icuapps, 0.0, http://icu-project.org/bugs, [ICU Apps], [http://icu-project.org])
dnl Check a file in our files, just to make sure we have a sane setup..
AC_CONFIG_SRCDIR(usort/lib/unicode/usort.h)

dnl Will set the variable 'ICU_CONFIG'
CHECK_ICU_CONFIG


AC_CHECK_PROG(AUTOCONF, autoconf, autoconf, true)

# ICU4C configure.in is working too hard. See below.
AC_CHECK_HEADERS([dlfcn.h readline/readline.h fcgiapp.h])
AC_SEARCH_LIBS(readline, [readline edit]) 
AC_SEARCH_LIBS(FCGX_GetParam, [fcgi])
AC_CHECK_FUNCS([dlopen readline])

AC_PROG_INSTALL

# make sure install is relative to srcdir - if a script
if test "$srcdir" = "."; then
  # If srcdir isn't just ., then (srcdir) is already prepended.
  if test "${ac_install_sh}" = "${INSTALL}"; then
   INSTALL="\\\$(top_srcdir)/${ac_install_sh}"
  fi
fi

AC_CONFIG_HEADER(apputil/demo_config.h)


AC_OUTPUT([icuapps.mk \
Makefile \
XMLConverter/Makefile \
apputil/Makefile \
convexp/Makefile \
datacustom/Makefile \
datacustom/gendatadep/Makefile \
dumpdat/Makefile \
icudemos/Makefile \
icusegments/Makefile \
idnbrowser/Makefile \
locexp/Makefile \
locexp/resource/Makefile \
locexp/tools/Makefile \
locexp/util/Makefile \
locexp2/Makefile \
nbrowser/Makefile \
obsolete/Makefile \
rbbiperf/Makefile \
redemo/Makefile \
regressionTest/CollRegressionTest/c/Makefile \
regressionTest/StrsrchRegressionTest/c/Makefile \
rtfcvrt/Makefile \
scompare/Makefile \
test/Makefile \
test/obstest/Makefile \
test/testdata/Makefile \
ubrowse/Makefile \
ucd/Makefile \
usort/Makefile \
usort/client/Makefile \
usort/lib/Makefile \
webdemo/collation/Makefile \
xlitomatic/Makefile])

dnl

