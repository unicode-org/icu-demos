dnl aclocal.m4 for ICU apps
dnl Copyright (c) 2002-2010, International Business Machines Corporation and
dnl others. All Rights Reserved.

dnl @TOP@

dnl CHECK_ICU_CONFIG
AC_DEFUN(CHECK_ICU_CONFIG, [
dnl look for the icu-config script.
dnl example shown for apps that might want --without-icu as an option.
dnl sets ICU_CONFIG and ICU_VERSION

icu_path=
icu_path_extra=${prefix}/bin
ICU_VERSION=unknown

AC_ARG_WITH(icu,
	[  --with-icu, --with-icu=yes, or --with-icu={path} specify the installed ICU to compile against [default=yes] - ICU autodetected from PATH],
	[case "${withval}" in
		no) AC_MSG_ERROR([Error: These are ICU apps... --with-icu=no  / --without-icu don't make sense.])  ;;
                yes)  ;;
		*) icu_path=${withval}; icu_path_extra=${icu_path}/bin ;;
		esac], 
        [icu_path=])

 AC_PATH_PROGS(ICU_CONFIG, icu-config, :, ${icu_path_extra})
 AC_SUBST(ICU_CONFIG)

 if test -n "$ac_cv_path_ICU_CONFIG"; then
    AC_MSG_CHECKING([ICU installation]))
    if ${ICU_CONFIG} --exists; then
        AC_MSG_RESULT([ok])
    else
        AC_MSG_ERROR([ICU is not installed properly.])
    fi
    AC_MSG_CHECKING([ICU version])
    ICU_VERSION=`${ICU_CONFIG} --version`
    AC_MSG_RESULT([${ICU_VERSION}])
    AC_SUBST(ICU_VERSION)
 else
    AC_MSG_ERROR([Cannot find icu-config, please check the PATH or use --with-icu=path/to/icu])
 fi
])
     
