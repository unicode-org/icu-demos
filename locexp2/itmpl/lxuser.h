// Copyright 2011 IBM Corporation and Others. All rights reserved. 

#ifndef _LXUSER
#define _LXUSER

#include "ufcgi.h"
#include "unicode/utypes.h"
#include "unicode/locid.h"
#include "lxutils.h"
#include <string>


struct LXUser {
    UFCGI *u;
// loc junk
    const char *ul; // user loc
    Locale ul_l; // user loc
    const char *dl; // display loc
    Locale dl_l; // display loc, locid
// ui junk
    UnicodeString pageTitle;

// api junk
    LXUser(UFCGI* u);
    
    
    LocaleTree *getTree(UErrorCode &status) {
        if(lt==NULL) {
            lt=LocaleTree::getInstance(dl_l.getName(),status);
        }
        return lt;
    }
    
    
    UnicodeString getDisplayName() {
        return getDisplayName(ul_l);
    }
    
    UnicodeString getDisplayName(const Locale& l) {
        return getDisplayName(l.getName());
    }
    
    UnicodeString getDisplayName(const char *bcp47) {
        return getDisplayNameFor(bcp47, dl_l);
    }
private:
    LocaleTree *lt;
    LXUser(){}
};

#define LXUSER  LXUser* lxu = (LXUser*)u->user;
#define LXBEGIN LXUser lxu_stack(u);  u->user=(void*)&lxu_stack;
#define LXEND   /* just passes out of scope */

#endif