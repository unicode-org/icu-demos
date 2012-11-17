//
//  lxutils.h
//  locexp2
//
//  Copyright 2011 IBM Corporation and Others. All rights reserved.
//

#ifndef _LXUTILS
#define _LXUTILS

#include "unicode/locid.h"
#include "unicode/uloc.h"
#include "ufcgi.h"

#include <vector>
#include <string>
using namespace std;

class LocaleTree {
private:
    LocaleTree(){} // notused
    LocaleTree(const char *dl, UErrorCode &status);
public:
    ~LocaleTree();
    static LocaleTree *getInstance(const char *dl, UErrorCode& status);
    
    /**
     * Write out the tree.
     * @param u the UFCGI to write to
     * @param newUrl if non-null, this will be appended with the BCP 47 localeid.  So, pass this '/foo/bar?baz='  to set baz to the new id.
     */
    void writeTree(UFCGI *u, const char *newUrl, UErrorCode& status);
private:
    vector<string> allids;
    const char *dl;
};


extern UnicodeString getDisplayNameFor(const char *bcp47, const Locale& dl_l);

extern const char *dupBcp47(const char *ch);

static const char *dupBcp47(const Locale& l) {
    return dupBcp47(l.getName());
}

Locale &setFromBcp47(Locale&l, const char *bcp47);


#endif
