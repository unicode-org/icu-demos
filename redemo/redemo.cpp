/*
*******************************************************************************
*
*   Copyright (C) 2006, International Business Machines
*   Corporation and others.  All Rights Reserved.
*
*******************************************************************************
*   file name:  redemo.cpp
*   encoding:   US-ASCII
*   tab size:   4 (not used)
*   indentation:4
*
*   created on: 2006Mar01
*   created by: Andy Heninger
*/

//
// redemo.cpp : server side back end application for the ICU regular expressions demo.
//
//         This is invoked from an http_request (AJAX) style request,
//           not from a form HTML submittal.  The incoming text is
//           in a simple plain text format, not URL-encoded like a normal POST.
//           Like a normal POST, though, input arrives via stdin and data
//           is returned to the client browser side via stdout.
//
//         Data Input Format
//           first line 
//               first char    0 or 1    the case insensitive flag
//               2nd   char    0 or 1    the dotAny flag
//               3rd   char    0 or 1    the multi-line flag
//               4th   char    0 or 1    the Unicode Word boundaries flag
//           second line
//               the regular expression.
//           third line to the end
//               the text to match
//
//
//       Data Output Format
//           first line
//              status from compiling the pattern.  "U_ZERO_ERROR" is all went well.
//           second line
//              n      number of capture groups in the pattern
//           fourth - nth lines each have four numbers, separated by a single space. 
//              match#  group#  start-index end-index
//              match#  group#  start-index end-index
//              match#  group#  start-index end-index
//              ...
//              blank line signifies end of match info.
//
//          There is NO HTML code in the output of this program. 
//          All display formatting is handled on the browser side via Javascript.


#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>
#include <unicode/utypes.h>
#include <unicode/ustring.h>
#include <unicode/uregex.h>

void failBadInputData() {
    printf("Error: Server received bad data.");
    exit(0);
}


int main(int argc, char* argv[])
{

    printf("Content-type: text/html charset=UTF-8\n\n");

    //
    //  Read the raw data coming from the client web page.
    // 
    size_t   BUFSIZE = 10000;
    char  *rawBuf    = (char *)malloc(BUFSIZE);
    size_t len       = fread(rawBuf, 1, BUFSIZE-1, stdin);
    rawBuf[len] = 0;

    //
    //  Parse it apart.  NOTE:  NOT in POST format.  see comments above..
    //

    // flag settings
    int32_t   flags = 0;
    if (rawBuf[0] == '1') {
        flags |= UREGEX_CASE_INSENSITIVE;
    }
    if (rawBuf[1] == '1') {
        flags |= UREGEX_DOTALL;
    }
    if (rawBuf[2] == '1') {
        flags |= UREGEX_MULTILINE;
    }
    if (rawBuf[3] == '1') {
        flags |= UREGEX_UWORD;
    }
    
    // the regular expression.
    char *rawExpr = strchr(rawBuf, '\n');
    if (rawExpr == NULL) {
        failBadInputData();
    }
    rawExpr++;

    // the text to be processed.
    char *rawText = strchr(rawExpr, '\n');
    if (rawText == NULL) {
        failBadInputData();
    }
    *rawText = 0;   // overwrite \n at end of the expression with a string terminating nul.
    size_t rawExprLen = rawText - rawExpr;
    rawText++;
    size_t rawTextLen = (rawBuf+len) - rawText;


    //
    // Convert the regular expression from UTF-8 to UChar *
    //
    URegularExpression *ure;
    UErrorCode          status = U_ZERO_ERROR;

    UChar *u16expr = (UChar *)malloc(2 * rawExprLen + 2);
    u16expr        = u_strFromUTF8(u16expr, (int32_t)rawExprLen+1, NULL, rawExpr, -1, &status);        
    UParseError      pe;

    //
    // Compile the expression
    //
    ure = uregex_open( u16expr,   // ptr to re
                    -1,           // length
                    flags,        // match  flags,
                    &pe,
                    &status);

    //
    // First line returned to client side is the status from the compile, as text
    //    "U_ZERO_ERROR" if all went well.
    //
	printf("%s\n", u_errorName(status));
    if (U_FAILURE(status)) {
        return 0;
    }

    int32_t numCaptureGroups = uregex_groupCount(ure, &status);
    printf("%d\n", numCaptureGroups);

    //
    // Convert the text to match against to UChar (UTF-16) format
    //
    UChar   *u16text = (UChar *)malloc(2 * rawTextLen + 2);
    int32_t  u16Len = 0;
    u16text         = u_strFromUTF8(u16text, (int32_t)rawTextLen+1, &u16Len, rawText, -1, &status); 

    //
    //  Find and output all of the matches
    //
    uregex_setText(ure, u16text, u16Len, &status);
    int32_t  matchNumber = 0;
    while (uregex_findNext(ure, &status)) {
        int32_t group;
        for (group=0; group<=numCaptureGroups; group++) {
            printf("%d %d %d %d \n", matchNumber, group, 
                uregex_start(ure, group, &status), uregex_end(ure, group, &status));
        }
        if (U_FAILURE(status)) {
            // shouldn't happend.
            //break;
        }
        matchNumber++;
    }

    //
    //  Output blank line, signifying end of match data.
    //
    printf("\n");

    return 0;

}

