/*
 *  tmplutil.cpp
 *  icu4capps
 *
 *  Created by Steven R. Loomis on 28/07/2006.
 *  Copyright 2006 IBM. All rights reserved.
 *
 */

#include "tmplutil.h"

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
UnicodeString *getParam(const char *pdata, const char *name, UnicodeString *returnStr) {
    if (returnStr == NULL) {
        return NULL;
    }

    char *namebuf=new char[strlen(name)+10];
    strcpy(namebuf, name);
    strcat(namebuf, "=");
    const char *start = strstr(pdata, namebuf);
    delete[] namebuf;
    if (start==0) {
        returnStr->setToBogus();
        return returnStr;
    }
    start+=strlen(name)+1;
    const char *end=strchr(start, '&');
    if (end==0) {
        end = start + strlen(start);
    }
    int len = (int)(end-start);
    char *cdata=new char[len+10];

    const char *src = start;
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
    *returnStr = UnicodeString(cdata, dsti, "utf-8");
    delete[] cdata;

    return returnStr;
 }


//
//  storeField
//      Store a string value into a %%field%% in the generated html.
//      Html escape any '&' or '<' chars in the value.
//
U_CAPI void storeField(const char *fieldName, const UnicodeString &val, UnicodeString &content) {
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
U_CAPI void toHex(UnicodeString &dest, const UnicodeString &src) {
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
//  insertTemplateFile   Insert the contents of one of the html template
//                       files into a UnicodeString in which we are building up
//                       the html for the page.
//
U_CAPI void insertTemplateFile(UnicodeString &dest,        // Target UnicodeString
                        const char *templateFileName,     //  File name of template.
                        const char *insertPointMarker)    //  A string dest that marks the
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

// insert a string
U_CAPI void insertTemplateString(UnicodeString &dest,        // Target UnicodeString
                        const UnicodeString& str,     //  string to replace.
                        const char *insertPointMarker)    //  A string dest that marks the
                                                    //    desired insertion point
{
    if (insertPointMarker != NULL && *insertPointMarker != 0) {
        UnicodeString ipm(insertPointMarker);
        int insertStart = dest.indexOf(UnicodeString(ipm));
        if (insertStart > 0) {
            dest.replace(insertStart, ipm.length(), str);
        }
    } else {
        dest.append(str);
    }
}

U_CAPI void insertTemplateResource(UnicodeString &dest, ResourceBundle& res, const char *str, UErrorCode &status) {
    char key2[512];
    const char *p, *q;
    int32_t len;
    if(U_FAILURE(status)) return;
    for(p=str;*p=='%';p++)
        ;
    q=strchr(p,'%');
    if(!q) {
        len = strlen(p);
    } else {
        len = q-p;
    }
    strncpy(key2,p,len);
    key2[len]=0;
    
    ResourceBundle sub = res.get(key2,status);
    if(U_SUCCESS(status)) {
        insertTemplateString(dest,sub.getString(status), str);
    }
}


U_CAPI void writeUnicodeStringCGI(const UnicodeString& outputText) {
    UFILE *u_stdout = u_finit(stdout, "", "utf-8");
    u_fprintf(u_stdout, "Content-Type: text/html; charset=utf-8\n\n");
    const UChar *htmlBuf = outputText.getBuffer();
    u_file_write(htmlBuf, outputText.length(), u_stdout);
    // Debugging aid - put the raw post data back into the generated html,
    //                 as an html comment.
    //u_fprintf(u_stdout, "<!--%s-->\n", postData);
    u_fclose(u_stdout);
}

/**
 * substitute the DEMO_* macros into the string
 */
U_CAPI void insertDemoStrings(UnicodeString& outputText, UErrorCode& status) {
    insertTemplateFile(outputText, DEMO_COMMON_MASTHEAD, "%%DEMO_COMMON_MASTHEAD%%");
    insertTemplateString(outputText, DEMO_BEGIN_LEFT_NAV, "%%DEMO_BEGIN_LEFT_NAV%%");
    insertTemplateFile(outputText, DEMO_COMMON_LEFTNAV, "%%DEMO_COMMON_LEFTNAV%%");
    insertTemplateString(outputText, DEMO_END_LEFT_NAV, "%%DEMO_END_LEFT_NAV%%");
    insertTemplateString(outputText, DEMO_BEGIN_CONTENT, "%%DEMO_BEGIN_CONTENT%%");
    insertTemplateString(outputText, DEMO_END_CONTENT, "%%DEMO_END_CONTENT%%");

    insertTemplateFile(outputText, DEMO_COMMON_FOOTER, "%%DEMO_COMMON_FOOTER%%");
}
