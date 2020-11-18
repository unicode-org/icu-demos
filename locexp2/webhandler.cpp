//
//  webhandler.cpp
//  locexp2
//
//  Created by Steven R. Loomis on 6/22/11.
//  Copyright 2011 IBM Corporation and Others. All rights reserved.
//

#include "webhandler.h"
#include "unicode/localpointer.h"

static const UnicodeString SLASH_STAR("^/*","");


WebHandler::WebHandler() : urlPattern(NULL)
{
}

void WebHandler::setPattern(const UnicodeString& pattern, UErrorCode &status) {
    UParseError perr;
    urlPattern.adoptInstead(RegexPattern::compile(pattern,perr,status));
}


WebHandler::WebHandler(RegexPattern *adopt) : urlPattern(adopt) {
    
}

WebHandler::WebHandler(const UnicodeString &pattern, UErrorCode &status) 
{
    UParseError perr;
    urlPattern.adoptInstead(RegexPattern::compile(pattern,perr,status));
}



WebHandler::~WebHandler()
{
}

UBool WebHandler::handles(const UnicodeString &url) {
    UErrorCode status = U_ZERO_ERROR;
    
    if(urlPattern.isNull()) { return false; }
    
    LocalPointer<RegexMatcher> match(urlPattern->matcher(url, status));
    
    if(U_SUCCESS(status) && match!=NULL && match->matches(status)) {
        return true;
    } else {
        return false;
    }
}

UBool WebHandler::doRequest(const UnicodeString &url, UFCGI *u) {
    if(urlPattern.isNull()) { return false; }

    
    UErrorCode status = U_ZERO_ERROR;
    RegexMatcher *m = urlPattern->matcher(url, status);
    LocalPointer<RegexMatcher> match(m);
    
    if(U_SUCCESS(status) && match!=NULL && match->matches(status)) {
        handleRequest(url,u,match.getAlias());
        return true;
    }
    return false;
}

UBool    WebHandler::doRequest(UFCGI *u) {
    const char *PATH_INFO = cgi_getParam(u, "PATH_INFO");
    UnicodeString url(PATH_INFO, "");
    return doRequest(url, u);
}

CompoundWebHandler::CompoundWebHandler() 
: WebHandler()
{
}

CompoundWebHandler::~CompoundWebHandler() 
{
}

UBool CompoundWebHandler::handles(const UnicodeString &url) {
    for(list<WebHandler*>::iterator iter = handlers.begin();iter!=handlers.end();iter++)  {
        if((*iter)->handles(url)) {
            return true;
        }
    }
    return(defaultHandler && defaultHandler->handles(url));
}

UBool CompoundWebHandler::doRequest(const UnicodeString &url, UFCGI *u) {
    list<WebHandler*>::iterator stopat = handlers.end();
    for(list<WebHandler*>::iterator iter = handlers.begin()
            ;iter!=stopat
            ;iter++)  {
        WebHandler* next = *iter;
        if(next && next->doRequest(url,u)) {
            return true;
        }
    }
    if (defaultHandler!=NULL) {
        defaultHandler->handleRequest(url, u, NULL);
        return true;
    } else {
        return false;
    }
}

void CompoundWebHandler::handleRequest(const UnicodeString&, UFCGI *, const RegexMatcher *) {
    // Not Used.
}


void CompoundWebHandler::addHandler(WebHandler *alias) {
    handlers.push_back(alias);
}

void CompoundWebHandler::setDefaultHandler(WebHandler *alias) {
    defaultHandler=alias;
}

