/*
 *  scompare.c    The cgi application for the ICU string compare
 *                demo web sample.
 *
 ********************************************************************
 * COPYRIGHT:
 * Copyright (c) 2004, International Business Machines Corporation and
 * others. All Rights Reserved.
 ********************************************************************
 */

//
//  This cgi program is a little different from many.
//  Rather than return a complete web page, it returns only the compare results themselves.
//
//  The html returned from here is loaded into a non-displaying frame in the browser,
//  then JavaScript running on the browser extracts those results and places them
//  into the displayed page.
//
//  A result is returned in this format:
//    <p id="result-name">result string</p>
//
//  The script on the browser side picks up a value with InvisibleFramedDoc.getElementById("result-name")
//    and, from there, can do whatever it wants with it.
//


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unicode/utypes.h>
#include <unicode/uclean.h>
#include <unicode/unistr.h>
#include <unicode/normlzr.h>
#include <unicode/uchar.h>


//
// Function gets one parameter by name out of the string of data POSTed by
//   the html form.
//
//    parameters:
//       content  - char * pointer to the original POST data.
//       name     - char * name of the parameter to fetch.
//    return
//       UnicodeString containing the parameter, with
//             - the html form escaping removed
//             - converted from utf-8
//             - ICU unescape applied
//
UnicodeString  getParam(const char *content, const char *name) {
    UnicodeString returnStr;

    char *namebuf=new char[strlen(name)+10];
    strcpy(namebuf, name);
    strcat(namebuf, "=");
    char *start = strstr(content, namebuf);
    delete[] namebuf;
    if (start==0) {
        return returnStr;
    }
    start+=strlen(name)+1;
    char *end=strchr(start, '&');
    if (end==0) {
        end = start + strlen(start);
    }
    int len = (int)(end-start);
    char *cdata=new char[len+10];

    char *src = start;
    int dsti = 0;
    while(src < end) {
        unsigned char c = *src++;
        if (c=='+') {
            c = ' ';
        } else if (c=='%') {
            if (src+2 > end) {
                break;
            }
            unsigned char h1 = *src++;
            unsigned char h2 = *src++;
            int v = (u_digit(h1, 16)<<4) + u_digit(h2, 16);
            c = (v>=0 && v<=255)? v: '?';
        }
        cdata[dsti++] = c;
    }
    cdata[dsti++] = 0;

    // POST style encoding is now decoded, we have a char * utf-8 string.
    //  Slam it into the UnicodeString.
    returnStr = UnicodeString(cdata, dsti, "utf-8");
    returnStr = returnStr.unescape();
    delete[] cdata;

    return returnStr;
 }

  

//
//  doCompare
//
//      Compares a pair of strings, producing one result string.
//      The result is wrapped in the appropriate html and written to stdout.
//      The result can be "=", "=(I)", "=(N)" or "=(N,I)"
//
//      parameters:
//          content:  the raw, unprocessed POST data submitted from the html form.
//                    The strings to be compared will be extracted from this.
//          name1:    The name of the first string.
//          name2:    The name of the second string
//          rName:    The name with which to label the output string.
//
//      output (to stdout) looks like
//          <p id="rName">result string</p>

void doCompare(const char *content, const char *name1, const char *name2, const char *rName) {
    UErrorCode    status = U_ZERO_ERROR;

    UnicodeString s1 = getParam(content, name1);
    UnicodeString s2 = getParam(content, name2);
    const char *resultString;

    if (s1.compare(s2) == 0) {
        resultString = "=";
    } else if (s1.caseCompare(s2, U_FOLD_CASE_DEFAULT) == 0) {
        resultString = "=(I)";
    } else if (Normalizer::compare(s1, s2, U_FOLD_CASE_DEFAULT, status) == 0) {
        resultString = "=(N)";
    } else if (Normalizer::compare(s1, s2, U_COMPARE_IGNORE_CASE, status) == 0) {
        resultString = "=(N,I)";
    } else {
        resultString = "<>";
    }
    printf("<p id=\"%s\" >%s</p>\n", rName, resultString);
}


//
//  main()
//
int main(int argc, const char **argv) {
    UErrorCode  status = U_ZERO_ERROR;
    const char *request_method;

    u_init(&status); 
    printf("Content-Type: text/html; charset=utf-8\n"
            "\n"
            "<html lang=\"en-US\">\n"
            "<head>\n"
            "<meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\">\n"
            "</head>\n"
            "<body>\n");

    request_method = getenv("REQUEST_METHOD");
    const char *scontentLen = getenv("CONTENT_LENGTH");
    if (request_method==NULL || strcmp(request_method, "POST") != 0 || scontentLen==NULL) {
        printf("Error!");
        exit(-1);
    }
    int contentLen = atoi(scontentLen);
    if (contentLen<=0 || contentLen>=10000) {
        printf("Error - bad content length!");
        exit(-1);
    }

    char *content = new char[contentLen+20];
    fread(content, 1, contentLen, stdin);
    content[contentLen] = 0;
    
    doCompare(content, "s1a", "s1b", "r1");
    doCompare(content, "s2a", "s2b", "r2");
    doCompare(content, "s3a", "s3b", "r3");
    doCompare(content, "s4a", "s4b", "r4");

    printf("\n</body>\n");
    delete[] content;
    u_cleanup();
    return 0;
}


