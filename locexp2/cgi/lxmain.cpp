//
//  lxmain.cpp
//  locexp2
//
//  Created by Steven R. Loomis on 6/20/11.
//  Copyright 2011 IBM Corporation and Others. All rights reserved.
//

#include "lxmain.h"
#include "webhandler.h"

static int r = 0;

void init_cgi()
{
}


class StringWebHandler : public WebHandler {
public: 
    StringWebHandler(const UnicodeString &pattern, const UnicodeString &str, UErrorCode &status) 
    : WebHandler(pattern,status), str(str) {
    }
protected:
    virtual void handleRequest(const UnicodeString &url, UFCGI *u, const RegexMatcher *match) {
        cgi_printf(u, "%S", str.getTerminatedBuffer());
    }
private:
    UnicodeString str;
};

class DumpEnvHandler : public WebHandler {
public: 
    DumpEnvHandler(const UnicodeString &pattern, UErrorCode &status) 
    : WebHandler(pattern,status) {
    }
protected:
    virtual void handleRequest(const UnicodeString &url, UFCGI *u, const RegexMatcher *match) {
        
        
        cgi_printf(u, "<b>Env:</b> <pre>");
        
        cgi_dumpEnv(u);
        
        cgi_printf(u, "</pre>");
    }
};

extern CompoundWebHandler *init_tmpl(UErrorCode &status);

static CompoundWebHandler *cwh = NULL;

void process_cgi(UFCGI *u)
{
    UErrorCode status = U_ZERO_ERROR;
    
    if(cwh == NULL) {
        cwh = init_tmpl(status);
    }
    
//    
//    
////    cgi_setContentType(u, "text/plain");
//    
//    CompoundWebHandler cwh;
//    
//    
//    StringWebHandler   myDefault("^.*", "This is the default page. <a href='./a'>a</a> | <a href='./b'>b</a> ",status);
//    StringWebHandler   myA("^/a", "This is page A.",status);
//    StringWebHandler   myB("^/b", "This is page B.",status);
//    DumpEnvHandler   myE("^/env", status);
////    
//    
//    if(U_SUCCESS(status)) {
//        cwh.addHandler(&myA);
//        cwh.addHandler(&myB);
//        cwh.addHandler(&myE);
//        cwh.setDefaultHandler(&myDefault);
//    }
////    
    
    if(!cwh->WebHandler::doRequest(u)) {
        cgi_printf(u, "<pre>Error: CWH failed: %s</pre>", 
                   cgi_getParam(u, "PATH_INFO"));
        return;
        
        /*
         cgi_printf(u, "<pre>Error: no handler installed: %S</pre>", 
         UnicodeString(url).getTerminatedBuffer());
         return false;
         */
    }
    
    cgi_printf(u, "<hr> Request #%d - PATH_INFO=%s .. status=%s .. \n",
               ++r, cgi_getParam(u, "PATH_INFO"), u_errorName(status));
}
