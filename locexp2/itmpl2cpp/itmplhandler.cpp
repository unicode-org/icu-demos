//
//  itmplhandler.cpp
//  locexp2
//
//  Copyright 2011 IBM Corporation and Others. All rights reserved.
//

#include "itmplhandler.h"

ItmplHandler::ItmplHandler(const char *shortn, UErrorCode &status) : shortname(shortn) {
    UnicodeString us = "^/";
    us.append(shortn);
    us.append(".itml");
    setPattern(us,status);
    
}