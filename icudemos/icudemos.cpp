/*
 *  icudemos.c    The cgi application for the ICU string compare
 *                demo web sample.
 *
 ********************************************************************
 * COPYRIGHT:
 * Copyright (c) 2004-2006, International Business Machines Corporation and
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
#include <unicode/udata.h>
#include <unicode/rbnf.h>
#include <unicode/resbund.h>

#include "demo_settings.h"

#include "tmplutil.h"

#include "unicode/usort.h"

#define PROG_NAME "icudemos"

// ----- main

//
//  main()
//


void addDemoItem(USort *list, ResourceBundle &aRB, const char *demo, UErrorCode &inStatus) {
    if(U_FAILURE(inStatus)) { 
        return;
    }
    
    UErrorCode status = U_ZERO_ERROR;
    
    ResourceBundle dB = aRB.get(demo, status);
    ResourceBundle dName = dB.get("name",status);
    UnicodeString name = dName.getString(status);
    if(U_FAILURE(status)) {
        name = UnicodeString(demo, "");
    }
    usort_addLine(list, name.getTerminatedBuffer(), -1, TRUE, (void*)demo);
}

void appendDemoItem(UnicodeString &theDemos, USort *list, int n, ResourceBundle &aRB, UErrorCode &inStatus) {
    if(U_FAILURE(inStatus)) { 
        return;
    }
    
    const char *demo = (const char*)(list->lines[n].userData);

    // rest is the same.
    UErrorCode status = U_ZERO_ERROR;
    
    ResourceBundle dB = aRB.get(demo, status);
    ResourceBundle dName = dB.get("name",status);
    ResourceBundle dDesc = dB.get("desc",status);
    
    UnicodeString name = dName.getString(status);
    UnicodeString desc = dDesc.getString(status);
    
    if(U_FAILURE(status)) {
        name = UnicodeString(demo, "");
        desc = UnicodeString("Error: ", "")+u_errorName(status);
    }
    
    theDemos = theDemos + 
        "<tr><td><a href='"+demo+"'>"+name+"</a></td><td>"+desc+"</td>\n";
        
    if(U_FAILURE(status) && status != U_MISSING_RESOURCE_ERROR) {
        inStatus = status;
    }
}


U_CFUNC char icudemos_dat[];

char locBuf[200];
const char *ourLocale = "zh_TW";


int main(int argc, const char **argv) {
    UErrorCode      status = U_ZERO_ERROR;
    const char     *request_method;
    const char     *script_name;
    const char     *query_string = getenv("QUERY_STRING");
    UnicodeString   outputText;
    char *allocatedContent = 0;
    
    // set up our locale
    const char *acceptLanguage = getenv("HTTP_ACCEPT_LANGUAGE");
    const char *requestedLocale = NULL; 

    udata_setAppData( "icudemos", (const void*) icudemos_dat, &status);

    u_init(&status);
    
    if(query_string && *query_string) {
        char tmp[200];
        strncpy(tmp, query_string, 200);
        char *p;
        char *s;
        p = strstr(tmp, "_=");
        if(p) {
            p+=2;
            s=strchr(p,'&');
            if(s) {
               *s=0;
            }
            if(*p) {
                strcpy(locBuf, p);
                ourLocale = locBuf;
                requestedLocale = locBuf;
            }
        }
    }
    
    if(requestedLocale==NULL && acceptLanguage && *acceptLanguage) {
        UErrorCode acceptStatus = U_ZERO_ERROR;
        char newLocale[200];
        int32_t newLocaleLen = -1;
        UEnumeration *available = NULL;
        UAcceptResult outResult;
        
        available = ures_openAvailableLocales(NULL, &acceptStatus);
        newLocaleLen = uloc_acceptLanguageFromHTTP(newLocale, 200, &outResult,
            acceptLanguage, available, &acceptStatus);
        if(U_SUCCESS(status) /* && isSupportedLocale(newLocale, TRUE)*/) { 
            strcpy(locBuf, newLocale);
            ourLocale = locBuf;
        }
        uenum_close(available);
    }
    

    //
    // Read the two html template files into the UnicodeString
    //   in which we build up the html for the generated page.
    //   The first template file is the overall page structure, as produced
    //   by the IBM template generator.
    //   The second contains the page content specific to this demo app.
    //
    
    insertTemplateFile(outputText, DEMO_COMMON_DIR PROG_NAME "-page-templ.html", NULL);
    insertTemplateFile(outputText, DEMO_COMMON_DIR PROG_NAME "-content-templ.html",
        "%%space content space table here%%");

    // insert the DEMO_* strings
    insertDemoStrings(outputText, status);

    ResourceBundle *aRb = NULL;
    aRb = new ResourceBundle(UnicodeString("icudemos",""), Locale(ourLocale), status);
    
    if(U_FAILURE(status) || !aRb) {
        char tmp[400];
        sprintf(tmp, "Error in setup: %s.\n", u_errorName(status));
        insertTemplateString(outputText, tmp, "%%str-help%%");
        goto done;
    }

    insertTemplateString(outputText, ourLocale, "%%locale%%");
    
    insertTemplateResource(outputText, *aRb, "%%str-title%%", status);
    insertTemplateResource(outputText, *aRb, "%%str-title%%", status);
    insertTemplateResource(outputText, *aRb, "%%str-title%%", status);

    if(U_FAILURE(status) || !aRb) {
        char tmp[400];
        sprintf(tmp, "Error in fetch: %s.\n", u_errorName(status));
        insertTemplateString(outputText, tmp, "%%str-help%%");
        goto done;
    }
    insertTemplateResource(outputText, *aRb, "%%str-help%%", status);
    insertTemplateResource(outputText, *aRb, "%%str-demo%%", status);
    insertTemplateResource(outputText, *aRb, "%%str-desc%%", status);

    {
        Locale theLocale(ourLocale);
        UnicodeString theDemos;
        const char *demoList[] = { "convexp", "idnbrowser", "locexp", "nbrowser", "scompare", "ubrowse", NULL };

        USort *list = usort_open(ourLocale, UCOL_DEFAULT, TRUE, &status);

        if(U_SUCCESS(status)) {
            // add items unsorted
            for(int n=0;demoList[n];n++) {
                addDemoItem(list, *aRb, demoList[n], status);
            }
            usort_sort(list);
            // append the sorted items
            for(int n=0;demoList[n];n++) {
                appendDemoItem(theDemos, list, n, *aRb, status);
            }
            
            // %%howmany%% ->  "four"
            {
                UErrorCode rbnfStatus = U_ZERO_ERROR;
                
                RuleBasedNumberFormat rbnf(URBNF_SPELLOUT, theLocale, rbnfStatus);
                UnicodeString tmp;
                if(U_SUCCESS(rbnfStatus)) {
                    rbnf.format(list->count, tmp);
                    insertTemplateString(outputText, tmp, "%%howmany%%");
                } else {
                    insertTemplateString(outputText, "???", "%%howmany%%");
                }
            }
            
            
            usort_close(list);
        } else {
            theDemos = UnicodeString("Error: ") + u_errorName(status);
        }
        insertTemplateString(outputText, theDemos, "%%demos%%");
    }

#if 0
    puts(htmlHeader);
    if (!printHTMLFragment(NULL, NULL, DEMO_COMMON_DIR "convexp-header.html")) {
        puts(defaultHeader);
    }
    puts(endHeaderBeginBody);
    if (printHTMLFragment(NULL, NULL, DEMO_COMMON_MASTHEAD)) {
        puts(DEMO_BEGIN_LEFT_NAV);
        printHTMLFragment(NULL, NULL, DEMO_COMMON_LEFTNAV);
        puts(DEMO_END_LEFT_NAV);
        puts(DEMO_BEGIN_CONTENT);
    }
    puts(breadCrumbMainHeader);
    
#endif    
 

#if 0
    // The IBM page template does not include the css necessary for tables.
    //   Insert the necessary css include now.
    int32_t where = outputText.indexOf(
        "<link rel=\"stylesheet\" type=\"text/css\" media=\"print\" href=\"//www.ibm.com/common");
    if (where>0) {
        outputText.insert(where,
            "<link rel=\"stylesheet\" type=\"text/css\" media=\"screen,print\" "
            "href=\"//www.ibm.com/common/v14/table.css\" />\n");
    } else {
        outputText.append("<!-- Error in icudemos.cpp while inserting table.css -->\n");
    }
#endif

#if 0
    UnicodeString originalScriptName("/software/globalization/icu/demo/compare");
    script_name=getenv("SCRIPT_NAME"); 
    where = outputText.indexOf(originalScriptName);
    if (where>0) {
        outputText.findAndReplace(originalScriptName, UnicodeString(script_name));
    } else {
        outputText.append("<!-- Error in icudemos.cpp while inserting $SCRIPT_NAME -->\n");
    }
#endif

#if 0
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
#endif 
done:
    //
    //  Here is the actual string compare, including stuffing the results
    //    into the various %%fields%% in the html template.
    //
//    doCompare(postData, outputText);
    
    //
    //  Write the completed html page to stdout.
    //
    writeUnicodeStringCGI(outputText);

    delete[] allocatedContent;
    
    u_cleanup();

    return 0;

}


