/*
*******************************************************************************
*                                                                             *
* COPYRIGHT:                                                                  *
*   (C) Copyright International Business Machines Corporation, 1999           *
*   Licensed Material - Program-Property of IBM - All Rights Reserved.        *
*   US Government Users Restricted Rights - Use, duplication, or disclosure   *
*   restricted by GSA ADP Schedule Contract with IBM Corp.                    *
*                                                                             *
*******************************************************************************
*   file name:  udatatest.c
*   encoding:   US-ASCII
*   tab size:   8 (not used)
*   indentation:4
*
*   created on: 1999oct25
*   created by: Markus W. Scherer
*/

#include <stdio.h>
#include <stdlib.h>
#include "unicode/utypes.h"
#include "unicode/udata.h"
#include "unames.h"

static char
name[80];

static void
printName(uint32_t code) {
    UErrorCode errorCode=U_ZERO_ERROR;
    UTextOffset length=u_charName(code, U_UNICODE_CHAR_NAME, name, sizeof(name), &errorCode);
    if(U_FAILURE(errorCode)) {
        printf("U-%08x: %s\n", code, u_errorName(errorCode));
    } else if(length>0) {
        printf("U-%08x: %s\n", code, name);
    }
}

extern int
main(int argc, char *argv[]) {
    UErrorCode errorCode;
    uint32_t code;

    /* something to step into for debugging */
    errorCode=U_ZERO_ERROR;
    udata_open("/test/path/base", "", "one", &errorCode);
    errorCode=U_ZERO_ERROR;
    udata_open("/test/path/", "dit", "two", &errorCode);
    errorCode=U_ZERO_ERROR;
    udata_open("base", NULL, "three", &errorCode);
    errorCode=U_ZERO_ERROR;
    udata_open("", "dit", "four", &errorCode);
    errorCode=U_ZERO_ERROR;
    udata_open(NULL, "dit", "five", &errorCode);

    printName(0x284);
    printName(0x61);
    printName(0x3401);
    printName(0x5678);
    printName(0xac00);
    printName(0xd7a3);
    printName(0xaffe);

    for(code=0; code<0x3400/*code<=0x10ffff*/; code+=23) {
        printName(code);
    }

    for(; code<0xe900/*code<=0x10ffff*/; code+=497) {
        printName(code);
    }

    for(; code<=0xffff/*code<=0x10ffff*/; code+=23) {
        printName(code);
    }

    return 0;
}
