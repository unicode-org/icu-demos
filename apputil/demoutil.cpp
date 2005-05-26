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
#include <stdio.h>
#include <stdlib.h>

U_CAPI int printHTMLFragment(char * package, char * locale, char *templateFileName) {
    size_t size = 0;
    size_t savedPos;
    char *buffer;
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
    printf("%s", buffer);
    
    free(buffer);
    fclose(templateFile);
    return 1;
}

