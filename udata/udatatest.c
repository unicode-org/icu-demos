#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "utypes.h"
#include "udata.h"
#include "unames.h"

static char
name[80];

static void
printName(uint32_t code) {
    UErrorCode errorCode=U_ZERO_ERROR;
    UTextOffset length=u_charName(code, U_UNICODE_CHAR_NAME, name, sizeof(name), &errorCode);
    if(length>0) {
        printf("U-%08x: %s\n", code, name);
    }
    if(U_FAILURE(errorCode))
      {
	printf("U-%08x: %s\n", code, errorName(errorCode));
      }
}

extern int
main(int argc, char *argv[]) {
    uint32_t code;

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
