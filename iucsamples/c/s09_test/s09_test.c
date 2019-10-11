// Copyright (c) 2010-2012 IBM Corporation and Others. All Rights Reserved.

#include <stdio.h>
#include "unicode/uclean.h"
#include "unicode/ustdio.h"

int main() {
    UErrorCode status = U_ZERO_ERROR;
    u_init(&status);
    u_printf_u(u"This is ICU %s! 😼\n", U_ICU_VERSION);
    if (U_SUCCESS(status)) {
        u_printf_u(u"everything is OK 🎉\n");
    } else {
        u_printf_u(u"error 😕 %s initializing.\n", u_errorName(status));
        return 1;
    }
    return 0;
}

/** Emacs Local Variables: **/
/** Emacs compile-command: "icurun s09_test.cpp" **/
/** Emacs End: **/
