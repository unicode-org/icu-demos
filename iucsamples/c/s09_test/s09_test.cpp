// Copyright (c) 2010-2012 IBM Corporation and Others. All Rights Reserved.

#include <stdio.h>
#include "unicode/uclean.h"

int main() {
    UErrorCode status = U_ZERO_ERROR;
    u_init(&status);
    printf("This is ICU %s!\n", U_ICU_VERSION);
    if (U_SUCCESS(status)) {
        printf("everything is OK\n");
    } else {
        printf("error %s initializing.\n", u_errorName(status));
        return 1;
    }
    return 0;
}
