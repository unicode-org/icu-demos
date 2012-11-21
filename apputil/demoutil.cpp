/*
*******************************************************************************
*   Copyright (C) 2005-2012, International Business Machines
*
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
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

U_CAPI int printHTMLFragment(UFILE *fileOut, UResourceBundle *displayBundle, const char *templateFileName) {
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

#define MBUFSIZ 4096
// output the icu.css
U_CAPI int writeOutFile(const char *fn, const char *type) {
    FILE *f;
    char buf[MBUFSIZ];
    char file[1024];
    sprintf(file, "./data%s", fn);
    f = fopen(file, "r");
    if(!f) {
      printf("Status: 404\n\nCould not retreive the file indicated\n");
      return 1;
    }
    printf("Content-type: %s\n", type);
#if 0
    // TODO
    fseek ( seek_end, 0)
      ftell(..)
      frewind(..)
      printf("Content-Length: %d\n", ...);
#endif
    printf("\n");
    fflush(stdout);
    fflush(stderr);
    
    while(!feof(f)) {
        int n;
        n = fread(buf,1,MBUFSIZ,f);
        if(n>0) {
            fwrite(buf,1,n,stdout);
        }
    }
    fclose(f);
    fflush(stdout);
    
    return 0;
}


U_CAPI 
const char *serveStaticFile(const char *files[], const char *pathInfo) {
  if(pathInfo==NULL) pathInfo=getenv("PATH_INFO");
  if(pathInfo&&*pathInfo) {
        int n;
        for(n=0;files[n];n+=2) {
            if(!strcmp(pathInfo, files[n])) {
              (void)writeOutFile(files[n], files[n+1]);
              return files[n];
            }
        }
    }
  
}
