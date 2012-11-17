// Copyright 2011 IBM Corporation and Others. All rights reserved. 

#include "lxuser.h"

LXUser::LXUser(UFCGI *u) :u(u),lt(NULL) {
    ul = cgi_queryField(u, "_");
    dl = cgi_queryField(u, "_d");
    
    if(ul==NULL || *ul==0) {
        ul_l=Locale::getDefault();
        ul=dupBcp47(ul_l);
    } else {
        setFromBcp47(ul_l,ul);
    }
    if(dl==NULL || *dl==0) {
        dl_l=Locale::getDefault();
        dl=dupBcp47(dl_l);
    } else {
        setFromBcp47(dl_l,dl);
    }
}
