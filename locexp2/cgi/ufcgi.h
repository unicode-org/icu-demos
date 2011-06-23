//
//  ufcgi.h
//  locexp2
//
//  Created by Steven R. Loomis on 5/16/11.
//  Copyright 2011 IBM Corporation and Others. All rights reserved.
//

#ifndef _UFCGI
#define _UFCGI


#include "unicode/ustdio.h"

/** Visible structure **/
typedef struct {
    UFILE *out;
    void *stuff;    
    void *user;
    
    const char *queryString;
    const char *postData;
    const char *cookies;
} UFCGI;



/** User API **/
U_CAPI int32_t cgi_printf(UFCGI *u, const char *fmt, ...);
U_CAPI void cgi_setContentType(UFCGI *u, const char *type);
U_CAPI const char *cgi_getParam(UFCGI *u, const char *parm);
U_CAPI void cgi_dumpEnv(UFCGI *u);

/** qs/cookie api **/
const char *cgi_queryField(UFCGI* lx, const char *field);
UBool cgi_hasQueryField(UFCGI* lx, const char *field);
const char *cgi_cookieField(UFCGI* lx, const char *field);


/** User defined **/
U_CAPI void process_cgi(UFCGI *u);
U_CAPI void init_cgi();



#endif
