//
//  lxutils.cpp
//  locexp2
//
//  Copyright 2011 IBM Corporation and Others. All rights reserved.
//

#include "lxutils.h"

#include <string.h>


LocaleTree::LocaleTree(const char *dl, UErrorCode &status) : dl(dl) {
    for(int i=0;i<uloc_countAvailable();i++) {
        const char *l = uloc_getAvailable(i);
        allids.push_back(string(l));
    }
}

LocaleTree::~LocaleTree() {
    
}

void LocaleTree::writeTree(UFCGI *u, const char *newUrl, UErrorCode &status) {
    UChar str[1024];
    char bcp47[400];
    for(vector<string>::iterator iter = allids.begin();
        iter!=allids.end();
        iter++) {
        
        string thestr = *iter;
        const char *cstr = thestr.c_str();
        
        bcp47[0]=0;
        uloc_toLanguageTag(cstr, bcp47, 400, FALSE, &status);
        if(newUrl!=NULL) {
            cgi_printf(u, "<a href='%s%s'>", newUrl,bcp47);
        }
        cgi_printf(u, "%s", bcp47);
        
        {
            UErrorCode status2 = U_ZERO_ERROR;
            uloc_getDisplayName(cstr,dl,str,1024,&status2);
            if(U_SUCCESS(status2)) {
                cgi_printf(u, " - %S", str);
            }
        }
        
        if(newUrl!=NULL) {
            cgi_printf(u, "</a>");
        }
        cgi_printf(u,"<br/>");
    }
}


LocaleTree* LocaleTree::getInstance(const char *dl, UErrorCode &status) {
    return new LocaleTree(dl,status);
}


extern const char *dupBcp47(const char *ch) {
    char bcp47[400];
    UErrorCode status2 = U_ZERO_ERROR;
    uloc_toLanguageTag(ch, bcp47, 400, FALSE, &status2);
    return strdup(bcp47);
}

Locale &setFromBcp47(Locale&l, const char *bcp47) {
    char icu[400];
    UErrorCode status=U_ZERO_ERROR;
    icu[0]=0;
    uloc_forLanguageTag(bcp47, icu, 400,NULL, &status);
    if(icu[0]==0) {
        strcpy(icu,bcp47);
    }
    return (l=Locale(icu));
}

extern UnicodeString getDisplayNameFor(const char *bcp47, const Locale& dl_l) {
    Locale icu;
    UnicodeString us;
    setFromBcp47(icu, bcp47).getDisplayName(dl_l,us);
    return us; // + UnicodeString("/")+UnicodeString(icu)+UnicodeString("/")+bcp47;
}
