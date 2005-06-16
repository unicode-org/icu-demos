/*
 *  scompare.c    The cgi application for the ICU string compare
 *                demo web sample.
 *
 ********************************************************************
 * COPYRIGHT:
 * Copyright (c) 2004-2005, International Business Machines Corporation and
 * others. All Rights Reserved.
 ********************************************************************
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unicode/utypes.h>
#include <unicode/uclean.h>
#include <unicode/unistr.h>
#include <unicode/normlzr.h>
#include <unicode/uchar.h>
#include <unicode/ustdio.h>

#include "demo_settings.h"

//
// Function gets one parameter by name out of the string of data POSTed by
//   the html form.
//
//    parameters:
//       pdata    - char * pointer to the original POST data.
//       name     - char * name of the parameter to fetch.
//    return
//       UnicodeString containing the parameter, with
//             - the html form escaping removed
//             - converted from utf-8
//       If the parameter does not exist in the POST data, return
//          a BOGUS string.
//
UnicodeString  getParam(const char *pdata, const char *name) {
    UnicodeString returnStr;

    char *namebuf=new char[strlen(name)+10];
    strcpy(namebuf, name);
    strcat(namebuf, "=");
    char *start = strstr(pdata, namebuf);
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
    cdata[dsti] = 0;

    // POST style encoding is now decoded, we have a char * utf-8 string.
    //  Slam it into the UnicodeString.
    returnStr = UnicodeString(cdata, dsti, "utf-8");
    delete[] cdata;

    return returnStr;
 }


//
//  storeField
//      Store a string value into a %%field%% in the generated html.
//      Html escape any '&' or '<' chars in the value.
//
void storeField(const char *fieldName, const UnicodeString &val, UnicodeString &content) {
    UnicodeString vCopy(val);           
    vCopy.findAndReplace("&", "&amp;"); 
    vCopy.findAndReplace("<", "&lt;");
    vCopy.findAndReplace(">", "&gt;");
    content.findAndReplace(fieldName, vCopy);
}


//
//  StringToHex    Create a Hex representation of a UnicodeString.
//                  Example:  "ABC" goes to "\u0041 \u0042 \u0043"
//
void toHex(UnicodeString &dest, const UnicodeString &src) {
    int i;
    int hexLen = 0;

    dest.truncate(0);
    UChar *bp = dest.getBuffer(src.length()*7 + 3);       // length per char is "\uxxxx " = 7 uchars.
    for (i=0; i<src.length(); i=src.moveIndex32(i, 1)) {   //   (Supplementaries take less: 2 src chars -> 11 in dest)
        UChar32  c = src.char32At(i);
        if (c <= 0xffff) {
            hexLen += u_sprintf(bp+hexLen, "\\u%04x ", c);
        } else {
            hexLen += u_sprintf(bp+hexLen, "\\U%08x ", c);
        }
    }
    dest.releaseBuffer(hexLen);
}

//
//  doCompare
//
//      Compares a pair of strings, producing a result string.
//      The results are  wrapped in the appropriate html and written to stdout.
//
//      parameters:
//          postData: the raw, unprocessed POST data submitted from the html form.
//                    The strings to be compared will be extracted from this.
//          content:  The html output page as put together from the template files,
//                    ready for the result fields to be filled.
//
//      output 
//          The html content (the content string paramter) will have the %%field name%%
//            fields replaced with the actual contents.
//

void doCompare(const char *postData, UnicodeString &content) {
    UErrorCode    status = U_ZERO_ERROR;

    // Fetch the two strings to be compared from the post data.
    //
    UnicodeString s1 = getParam(postData, "s1a");
    UnicodeString s2 = getParam(postData, "s1b");
    if (s1.isBogus() || s2.isBogus()) {
        content.append("<!-- ERROR could not find string(s) in POST data -->\n");
        return;
    }
  
    // Put the two strings back into the text input field in the html page,
    //   so that what the user typed will still be there when the page reloads.
    //
    storeField("%%s1a%%", s1, content);
    storeField("%%s1b%%", s2, content);

    // Unescape the two input strings and put those too into the output html.
    s2 = s2.unescape();
    s2 = s2.unescape();
    storeField("%%str1%%", s1, content);
    storeField("%%str2%%", s2, content);

    // Put the hex-formatted input strings into the output html.
    //
    UnicodeString hexS;
    toHex(hexS, s1);
    content.findAndReplace("%%hex1%%", hexS);
    toHex(hexS, s2);
    content.findAndReplace("%%hex2%%", hexS);

    //
    // Do the actual comparisons
    //
    UnicodeString binary("-");
    UnicodeString caseless("-");
    UnicodeString equiv("-");
    UnicodeString caseless_equiv("-");

    if (s1.compare(s2) == 0) {
        binary = (UChar)'Y';
    }
    if (s1.caseCompare(s2, U_FOLD_CASE_DEFAULT) == 0) {
        caseless = (UChar)'Y';
    }
    if (Normalizer::compare(s1, s2, U_FOLD_CASE_DEFAULT, status) == 0) {
        equiv = (UChar)'Y';
    }
    if (Normalizer::compare(s1, s2, U_COMPARE_IGNORE_CASE, status) == 0) {
        caseless_equiv = (UChar)'Y';
    }
    content.findAndReplace("%%binary%%", binary);
    content.findAndReplace("%%case%%", caseless);
    content.findAndReplace("%%equiv%%", equiv);
    content.findAndReplace("%%eq_ca%%", caseless_equiv);

    return;
}


//
//  insertTemplateFile   Insert the contents of one of the html template
//                       files into a UnicodeString in which we are building up
//                       the html for the page.
//
void insertTemplateFile(UnicodeString &dest,        // Target UnicodeString
                        char *templateFileName,     //  File name of template.
                        char *insertPointMarker)    //  A string dest that marks the
                                                    //    desired insertion point
{
    UFILE          *templFile;
    int             fileSize;
    const int       MAX_FILE_SIZE = 50000;
    UChar          *buf;

    templFile = u_fopen(templateFileName, "r", "", "utf-8");
    if (templFile == NULL) {
        dest.append("<!-- ERROR: could not open template file -->\n");
        return;
    }
    buf = (UChar *)malloc(MAX_FILE_SIZE * sizeof(UChar));
    if (buf == NULL) {
        return;
    }

    fileSize = u_file_read(buf, MAX_FILE_SIZE, templFile);
    if (fileSize >= MAX_FILE_SIZE || fileSize <= 0) {
        dest.append("<!-- ERROR: could not read template file -->\n");
        return;
    }

    if (insertPointMarker != NULL && *insertPointMarker != 0) {
        UnicodeString ipm(insertPointMarker);
        int insertStart = dest.indexOf(UnicodeString(ipm));
        if (insertStart > 0) {
            dest.replace(insertStart, ipm.length(), buf, fileSize);
        }
    } else {
        dest.append(buf, fileSize);
    }

    delete buf;
}

//
//  main()
//
int main(int argc, const char **argv) {
    UErrorCode      status = U_ZERO_ERROR;
    const char     *request_method;
    const char     *script_name;
    UnicodeString   outputText;

    u_init(&status); 

    //
    // Read the two html template files into the UnicodeString
    //   in which we build up the html for the generated page.
    //   The first template file is the overall page structure, as produced
    //   by the IBM template generator.
    //   The second contains the page content specific to this demo app.
    //
    insertTemplateFile(outputText, DEMO_COMMON_DIR "scompare-page-templ.html", NULL);
    insertTemplateFile(outputText, DEMO_COMMON_DIR "scompare-content-templ.html",
        "%%space content space table here%%");

 
    // The IBM page template does not include the css necessary for tables.
    //   Insert the necessary css include now.
    int32_t where = outputText.indexOf(
        "<link rel=\"stylesheet\" type=\"text/css\" media=\"print\" href=\"//www.ibm.com/common");
    if (where>0) {
        outputText.insert(where,
            "<link rel=\"stylesheet\" type=\"text/css\" media=\"screen,print\" "
            "href=\"//www.ibm.com/common/v14/table.css\" />\n");
    } else {
        outputText.append("<!-- Error in scompare.cpp while inserting table.css -->\n");
    }


    UnicodeString originalScriptName("/software/globalization/icu/demo/compare");
    script_name=getenv("SCRIPT_NAME"); 
    where = outputText.indexOf(originalScriptName);
    if (where>0) {
        outputText.findAndReplace(originalScriptName, UnicodeString(script_name));
    } else {
        outputText.append("<!-- Error in scompare.cpp while inserting $SCRIPT_NAME -->\n");
    }


    //
    //  Fetch the POST data sent from the user's browser.
    //  If there is none, this program was probably invoked directly, rather than
    //    by a form submit.  In this case, provide some default data to preload the
    //    displayed results.
    //
    //   Note:  the easiest way to change the default data is to run the demo,
    //          enter the desired new defaults into the form, submit, then
    //          in the browser, display the source of the resulting page.
    //          In an html <!-- comment --> at the bottom will be the post data
    //          for that submit, which can be pasted in as the new initialization string
    //          for the postData variable, below.
    //
    request_method = getenv("REQUEST_METHOD");
    const char *sPostDataLen = getenv("CONTENT_LENGTH");
    const char *postData = "s1a=%CE%B1%CE%BB%CF%86%CE%B1&s1b=%CE%B1%CE%BB%CF%86%CE%B1";
    size_t postDataLen = strlen(postData);  // Default values
    char *allocatedContent = 0;
    if (request_method!=NULL && (strcmp(request_method, "POST")==0) && sPostDataLen!=NULL) {
        // Real user data from form submit.  Read it in.
        size_t t = atoi(sPostDataLen);
        if (t<=0 || t>=10000) {
            outputText.append("<!-- POST data length was bad -->\n");
        } else {
            postDataLen = t;
            allocatedContent = new char[postDataLen+20];
            fread(allocatedContent, 1, postDataLen, stdin);
            allocatedContent[postDataLen] = 0;
            postData = allocatedContent;   
        }
    }
    

    //
    //  Here is the actual string compare, including stuffing the results
    //    into the various %%fields%% in the html template.
    //
    doCompare(postData, outputText);
    
    //
    //  Write the completed html page to stdout.
    //
    UFILE *u_stdout = u_finit(stdout, "", "utf-8");
    u_fprintf(u_stdout, "Content-Type: text/html; charset=utf-8\n\n");
    const UChar *htmlBuf = outputText.getBuffer();
    u_file_write(htmlBuf, outputText.length(), u_stdout);
    // Debugging aid - put the raw post data back into the generated html,
    //                 as an html comment.
    //u_fprintf(u_stdout, "<!--%s-->\n", postData);
    u_fclose(u_stdout);

    delete[] allocatedContent;
    u_cleanup();

    return 0;

}


