//
//  webhandler.h
//  locexp2
//
//  Created by Steven R. Loomis on 6/22/11.
//  Copyright 2011 IBM Corporation and Others. All rights reserved.
//

#ifndef _WEBHANDLER
#define _WEBHANDLER

#include "unicode/regex.h"
#include "ufcgi.h"
#include <list>
using namespace std;

class WebHandler : public UMemory {

public:
    virtual ~WebHandler();
    virtual UBool   handles(const UnicodeString &url);
    
    virtual UBool    doRequest(const UnicodeString &url, UFCGI *u);
    
    virtual UBool    doRequest(UFCGI *u);
    
protected:
    WebHandler();
    WebHandler(RegexPattern *adopt);
    WebHandler(const UnicodeString &pattern, UErrorCode &status); 

    void setPattern(const UnicodeString &pattern, UErrorCode &status);
    
    /**
     * @param url the original url
     * @param u  the CGI context
     * @param match the matcher returned from your pattern
     */
public:
    virtual void handleRequest(const UnicodeString &url, UFCGI *u, const RegexMatcher *match) = 0;
private:
    
    LocalPointer<RegexPattern> urlPattern;
    
};

class CompoundWebHandler : public WebHandler {
public:
    virtual ~CompoundWebHandler();
    virtual UBool   handles(const UnicodeString &url);
    virtual UBool   doRequest(const UnicodeString &url, UFCGI *u);
    CompoundWebHandler();
    
    /**
     * Add a handler. WebHandler remains owned by the caller.
     */
    void addHandler(WebHandler *alias);
    /**
     * Add the default handler. It will be called last if nothing else matches, possibly with a NULL match.
     * WebHandler remains owned by the caller.
     */
    void setDefaultHandler(WebHandler *alias);
protected:
    virtual void handleRequest(const UnicodeString &url, UFCGI *u, const RegexMatcher *match); // not used. 
    
private:
    WebHandler *defaultHandler;
    list<WebHandler *> handlers;
};

#endif
