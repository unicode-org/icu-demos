/*
*******************************************************************************
*
*   Copyright (C) 2006, International Business Machines
*   Corporation and others.  All Rights Reserved.
*
*******************************************************************************
*   file name:  redemo.cpp
*
*   created on: 2006 07 12
*   created by: Andy Heninger
*/



#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>
#include <unicode/utypes.h>
#include <unicode/ustring.h>
#include <unicode/ubrk.h>
#include <unicode/utext.h>

#include <sys/time.h>
#include <sys/resource.h>
#include <unistd.h>
#include <signal.h>

/*
 *  Global Variable, set by timer signal handler to stop the
 *      break iterator loop.
 *      NOTE:  this idiom can fail on SMP systems with processors
 *             with weak memory coherency  (Power 4 and newer).
 */
UBool     gRun;

void handleALRMSignal(int sig) {
    gRun = FALSE;
}

void checkICUFailure(UErrorCode status) {
    if (U_FAILURE(status)) {
        printf("ICU Error: %s\n", u_errorName(status));
        exit -1;
    }
}


int main(int argc, char* argv[])
{
    FILE            *f;
    char             u8buf[30000];
    size_t           u8len;
    UChar           *u16buf;
    int32_t          u16len;
    UErrorCode       status = U_ZERO_ERROR;
    char            *param;        /* the type of break iterator to test   */
                                   /*  sent, line, etc.                    */
    UBreakIterator   *bi;
    UBreakIteratorType bType;
    
    int32_t          breakCount;
    int32_t          pos;           /* Position of a break boundary        */
    
    printf("Hello, World\n\n");

    //
    //  Read the text data file.
    //
    f = fopen("unicode-data.txt", "rb");
    if (f==0) {
        fprintf(stderr, "Cannot open data file \"unicode-data.txt\"\n");
        return -1;
    }
    u8len = fread(u8buf, 1, sizeof(u8buf), f);
    if (u8len < 1) {
        fprintf(stderr, "Error reading data file.\n");
        return -1;
    }
    if (u8len == sizeof(u8buf)) {
        fprintf(stderr, "warning, data file truncated.\n");
    }
    fclose(f);
    
    //
    //  Convert the UTF-8 text data to UTF-16
    //
    u_strFromUTF8(NULL, 0, &u16len, u8buf, u8len, &status);
    u16buf = (UChar *)malloc(u16len * 2 + 2);
    status = U_ZERO_ERROR;
    u_strFromUTF8(u16buf, u16len+1, &u16len, u8buf, u8len, &status);
    checkICUFailure(status);
        
    //
    //  Pull the break iterator type from the command line argument.
    //  Default is line break
    //
    param = "line";
    if (argc >= 2) {
        param = argv[1];
    }
    
    if (strcmp(param, "line")==0) {
        bType = UBRK_LINE;
    } else if (strcmp(param, "char") == 0) {
        bType = UBRK_CHARACTER;
    } else if (strcmp(param, "word") == 0) {
        bType = UBRK_WORD;
    } else if (strcmp(param, "sent") == 0) {
        bType = UBRK_SENTENCE;
    } else {
        fprintf(stderr, "Unrecognized break iterator type: \"%s\"\n", param);
        exit(-1);
    }
    
    //
    //  Create the break iterator
    //
    //UText  *ut = utext_openUTF8(NULL, u8buf, u8len, &status);
    bi = ubrk_open(bType, "en", u16buf, u16len, &status);
    // ubrk_setUText( bi, ut, &status);
    checkICUFailure(status);
    
    
    
    //
    //  Iterate once to warm up the code.
    //
    for (pos=ubrk_first(bi); pos!=UBRK_DONE; pos=ubrk_next(bi)) {
        breakCount++;
    }
        
    
    //
    //  Do a timed one second test, counting the number of
    //    boundaries found.
    //    One second is not very long, but in practice it produces
    //    very consistent results.
    //
    breakCount = 0;
    gRun = TRUE;
    signal(SIGALRM, handleALRMSignal);
    ualarm(1000000, 0);
    
    for (;;) {
        // for (pos=ubrk_first(bi); pos!=UBRK_DONE; pos=ubrk_next(bi)) {
        for (pos=ubrk_last(bi); pos!=UBRK_DONE; pos=ubrk_previous(bi)) {
             breakCount++;
             if (gRun == FALSE) {
                 goto breakOut;
             }
        }
    }
breakOut:
        
    
    printf("%d breaks per second\n", breakCount);
    
        
    return 0;

}

