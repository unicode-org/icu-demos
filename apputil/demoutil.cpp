/*
*******************************************************************************
*
*   Copyright (C) 2005-2005, International Business Machines
*   Corporation and others.  All Rights Reserved.
*
*******************************************************************************
*   file name:  demoutil.cpp
*   encoding:   US-ASCII
*   tab size:   4 (not used)
*   indentation:4
*
*   created on: 2003Mar25
*   created by: George Rhoten
*/

#include "demoutil.h"
#include "unicode/ustring.h"
#include "unicode/unistr.h"

#include <stdio.h>
#include <stdlib.h>

U_CAPI int printHTMLFragment(UFILE *fileOut, UResourceBundle *displayBundle, char *templateFileName) {
    size_t size = 0;
    size_t savedPos;
    char *buffer;

    if (displayBundle) {
        int32_t len = 0;
        UErrorCode status = U_ZERO_ERROR;
        const UChar *displayResString = ures_getStringByKey(displayBundle, templateFileName, &len, &status);
        if (U_SUCCESS(status)) {
            if (fileOut) {
                u_fprintf(fileOut, "%S", displayResString);
            }
            else {
                int32_t utf8Size = len * U8_MAX_LENGTH + 1;
                buffer = (char *)malloc(utf8Size);
                u_strToUTF8(buffer, utf8Size, &utf8Size, displayResString, len, &status);
                printf("%s", buffer);
                free(buffer);
            }
            return 1;
        }
        else if (fileOut) {
            u_fprintf(fileOut, "<!-- ERROR: %s resource cannot be opened. Resorting to file instead. -->\n", templateFileName);
        }
        else {
            printf("<!-- ERROR: %s resource cannot be opened. Resorting to file instead. -->\n", templateFileName);
        }
    }

    FILE *templateFile = fopen(templateFileName, "rb");
    
    if (templateFile == NULL) {
        printf("<!-- ERROR: %s cannot be opened -->\n", templateFileName);
        return 0;
    }   
    
    /* Go to the end, find the size, and go back to the beginning. */
    savedPos = ftell(templateFile);
    fseek(templateFile, 0, SEEK_END);
    size = ftell(templateFile);
    fseek(templateFile, savedPos, SEEK_SET);
    
    /* Read in the whole file and print it out */
    buffer = (char *)malloc(size+1);
    fread(buffer, size, 1, templateFile);
    buffer[size] = 0;    // NULL terminate for printing.
    if (fileOut) {
        UnicodeString unistrBuf(buffer, "UTF-8");
        u_fprintf(fileOut, "%S", unistrBuf.getTerminatedBuffer());
    }
    else {
        printf("%s", buffer);
    }
    
    free(buffer);
    fclose(templateFile);
    return 1;
}

