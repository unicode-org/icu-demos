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
//    <p id="s1a>input string 1 after unescaping it</p>
//    <p id="s1b>input string 2 after unescaping it</p>
//    <p id="r1">--YY</p>
//
//   and continuing with [s2a, s2b, r2] [s3a, s3b, r3] ...
//   for as many input strings as are found in the incoming POST data.
//
//   Within the rn strings, 
//            r[0] : 'Y' if input strings are binary equal, otherwise '-'
//            r[1] : 'Y' if input strings are case-insesitive equal.
//            r[2] : 'Y' if input strings are canonically-equivalent equal
//            r[3] : 'Y' if input strings are case / canon equiv equal.

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
//       If the parameter does not exist in the POST data, return
//          a BOGUS string.
//
UnicodeString  getParam(const char *content, const char *name) {
    UnicodeString returnStr;

    char *namebuf=new char[strlen(name)+10];
    strcpy(namebuf, name);
    strcat(namebuf, "=");
    char *start = strstr(content, namebuf);
    delete[] namebuf;
    if (start==0) {
        returnStr.setToBogus();
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
//
//      return true if name1 and name2 exist as parameters in the POST data
//             false if eithe is missing.

bool doCompare(const char *content, const char *name1, const char *name2, const char *rName) {
    UErrorCode    status = U_ZERO_ERROR;

    UnicodeString s1 = getParam(content, name1);
    UnicodeString s2 = getParam(content, name2);
    if (s1.isBogus() || s2.isBogus()) {
        return false;
    }
  
    // html escape all '<' and '&' in the two strings,
    //   in preparation for putting them into the output html.
    UnicodeString s1Copy(s1);
    UnicodeString s2Copy(s2);
    s1Copy.findAndReplace("&", "&amp;");
    s1Copy.findAndReplace("<", "&lt;");
    s2Copy.findAndReplace("&", "&amp;");
    s2Copy.findAndReplace("<", "&lt;");

    // Convert the two input strings to utf-8 and spit them back into
    //  the output so that the client can see them after the unescape().
    int len = s1Copy.length() > s2Copy.length()? s1Copy.length() : s2Copy.length();
    char *buf = new char[len*5];
    s1Copy.extract(0, s1Copy.length(), buf, "utf-8");
    printf("<p id=\"%s\" >%s</p>\n", name1, buf);
    s2Copy.extract(0, s2Copy.length(), buf, "utf-8");
    printf("<p id=\"%s\" >%s</p>\n", name2, buf);
    delete buf;
    buf = 0;

    //
    // Do the actual comparisons
    //
    char resultString[5];
    strcpy(resultString, "----");
    if (s1.compare(s2) == 0) {
        resultString[0] = 'Y';
    }
    if (s1.caseCompare(s2, U_FOLD_CASE_DEFAULT) == 0) {
        resultString[1] = 'Y';
    }
    if (Normalizer::compare(s1, s2, U_FOLD_CASE_DEFAULT, status) == 0) {
        resultString[2] = 'Y';
    }
    if (Normalizer::compare(s1, s2, U_COMPARE_IGNORE_CASE, status) == 0) {
        resultString[3] = 'Y';
    }
    printf("<p id=\"%s\" >%s</p>\n", rName, resultString);
    return true;
}


//
//  main()
//
int main(int argc, const char **argv) {
    UErrorCode  status = U_ZERO_ERROR;
    const char *request_method;
    int         n;

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
    
    for (n=1; ; n++) {
        char sAName[20];
        char sBName[20];
        char rName[20];
        sprintf(sAName, "s%da", n);
        sprintf(sBName, "s%db", n);
        sprintf(rName,  "r%d", n);

        if (doCompare(content, sAName, sBName, rName) == false) {
            break;
        }
    }

    printf("\n</body>\n");
    delete[] content;
    u_cleanup();
    return 0;
}


