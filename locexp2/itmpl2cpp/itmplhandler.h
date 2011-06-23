//
//  itmplhandler.h
//  locexp2
//
//  Copyright 2011 IBM Corporation and Others. All rights reserved.
//

#include "webhandler.h"

class ItmplHandler : public WebHandler {
protected:
    ItmplHandler(const char *shortname, UErrorCode &status);
    
    const char *shortname;
    
};

#define VARS_SETUP(u)       \
    const char *PATH_INFO=cgi_getParam(u, "PATH_INFO");   \
const char *QUERY_STRING=cgi_getParam(u, "QUERY_STRING"); \
const char *SCRIPT_NAME=cgi_getParam(u, "SCRIPT_NAME");
