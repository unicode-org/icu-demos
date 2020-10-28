/*
 *  icudemos.c    The cgi application for the ICU string compare
 *                demo web sample.
 *
 ********************************************************************
 * COPYRIGHT:
 * Copyright (c) 2004-2015, International Business Machines Corporation and
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
#include "demoutil.h"

#include "unicode/usort.h"

#define PROG_NAME "icudemos"

// ----- main

//
//  main()
//

#ifndef LENGTHOF
#define LENGTHOF(array) (int32_t)(sizeof(array)/sizeof((array)[0]))
#endif

#include "demolist.h"

void addDemoItem(USort *list, ResourceBundle &aRB, const char *demo, UErrorCode &inStatus) {
    if(U_FAILURE(inStatus)) { 
        return;
    }

    if(!strcmp(demo,PROG_NAME)) {
      return; // don't list ourself.
    }
    
    UErrorCode status = U_ZERO_ERROR;
    
    ResourceBundle dB = aRB.get(demo, status);

    ResourceBundle dName = dB.get("name",status);
    UnicodeString name = dName.getString(status);
    if(U_FAILURE(status)) {
        name = UnicodeString(demo, "");
    }
    usort_addLine(list, name.getTerminatedBuffer(), -1, true, (void*)demo);
}

void appendDemoItem(UnicodeString &theDemos, USort *list, int n, ResourceBundle &aRB, UErrorCode &inStatus) {
    if(U_FAILURE(inStatus)) { 
        return;
    }
    
    const char *demo = (const char*)(list->lines[n].userData);
    const char *demoUrl = demo;

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
        "<tr><td colspan=3><b><a href='"+demoUrl+"'>"+name+"</a></b></td></tr><td></td><td style='border-bottom: 1px solid gray' colspan=2>"+desc+"</td></tr>\n";
        
    if(U_FAILURE(status) && status != U_MISSING_RESOURCE_ERROR) {
        inStatus = status;
    }
}


U_CFUNC char icudemos_dat[];

char locBuf[200];
const char *ourLocale = "en_US_POSIX"; // C



void getDefaultLocale(UErrorCode &status) {
    // set up our locale
    const char *acceptLanguage = getenv("HTTP_ACCEPT_LANGUAGE");
    const char *requestedLocale = NULL; 
    const char     *query_string = getenv("QUERY_STRING");
    
    if(U_FAILURE(status)) {
        return;
    }

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
        if(U_SUCCESS(status) /* && isSupportedLocale(newLocale, true)*/) { 
            strcpy(locBuf, newLocale);
            ourLocale = locBuf;
        }
        uenum_close(available);
    }
    
}

void icuDemos(UnicodeString &outputText, UErrorCode &status) {

    if(U_FAILURE(status)) {     
        return;
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
        return;
    }

    insertTemplateString(outputText, ourLocale, "%%locale%%");
    
    insertTemplateResource(outputText, *aRb, "%%str-title%%", status);
    insertTemplateResource(outputText, *aRb, "%%str-title%%", status);
    insertTemplateResource(outputText, *aRb, "%%str-title%%", status);

    if(U_FAILURE(status) || !aRb) {
        char tmp[400];
        sprintf(tmp, "Error in fetch: %s.\n", u_errorName(status));
        insertTemplateString(outputText, tmp, "%%str-help%%");
        return;
    }
    insertTemplateResource(outputText, *aRb, "%%str-help%%", status);
    insertTemplateResource(outputText, *aRb, "%%str-demo%%", status);
    insertTemplateResource(outputText, *aRb, "%%str-desc%%", status);

    {
        Locale theLocale(ourLocale);
        UnicodeString theDemos;
        

        USort *list = usort_open(ourLocale, UCOL_DEFAULT, true, &status);

        if(U_SUCCESS(status)) {
            // add items unsorted

          for(int n=0;n<LENGTHOF(demoList);n++) {
                addDemoItem(list, *aRb, demoList[n], status);
            }
            // TODO: hack: add this one manually.
            addDemoItem(list, *aRb, "collation.html", status);
            usort_sort(list);
            // append the sorted items
            for(int n=0;n<list->count;n++) {
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
}


#define CSS_FILE "/icu.css"
#define CSS_DEMO_FILE "/main.css"
#define CSS_TYPE "text/css"
#define GIF_TYPE "image/gif"
#define HTML_TYPE "text/html"

const char *  files[] =

{ 
    CSS_FILE, CSS_TYPE ,
	"/icu_v2.css", CSS_TYPE ,
    CSS_DEMO_FILE, CSS_TYPE ,
    "/1x1.gif", GIF_TYPE ,
    "/lines.gif", GIF_TYPE ,
    "/lines-gradient.gif", GIF_TYPE ,
    NULL, NULL
};

int main(int /* argc */, const char ** /* argv */) {
    UErrorCode      status = U_ZERO_ERROR;
    UnicodeString   outputText;
    char *allocatedContent = 0;
    char *pi = 0;

    
    if(serveStaticFile(files, NULL)) return 0;
    pi = getenv("PATH_INFO");
    if(pi&&*pi) {
        printf("Status: 404 Not Found\nContent-type: text/plain\n\nError: no file at this location.\n");
        return 0;
    }

    udata_setAppData( "icudemos", (const void*) icudemos_dat, &status);

    u_init(&status);

    if(U_FAILURE(status)) {
        char tmp[400];
        sprintf(tmp, "Error in u_init: %s.\n", u_errorName(status));
        outputText = UnicodeString(tmp);
    }
    
    getDefaultLocale(status);

    // insert the text to be output
    icuDemos(outputText, status);

    
    //
    //  Write the completed html page to stdout.
    //
    writeUnicodeStringCGI(outputText);

    delete[] allocatedContent;
    
    u_cleanup();

    return 0;

}


