dnl aclocal.m4 for ICU apps
dnl Copyright (c) 2002, International Business Machines Corporation and
dnl others. All Rights Reserved.

dnl @TOP@

dnl CHECK_ICU_CONFIG
AC_DEFUN(CHECK_ICU_CONFIG, [
 dnl look for the icu-config script.
 AC_PATH_PROGS(ICU_CONFIG, icu-config)
 AC_SUBST(ICU_CONFIG)

 if test -n "$ac_cv_path_ICU_CONFIG"; then
    AC_MSG_CHECKING([ICU installation])
    if ${ICU_CONFIG} --exists; then
        echo ok
    else
        AC_MSG_ERROR([ICU is not installed properly.])
    fi
    AC_MSG_CHECKING([ICU version])
    ${ICU_CONFIG} --version
 else
    AC_MSG_ERROR([Cannot find icu-config, please check the PATH])
 fi
])
     
