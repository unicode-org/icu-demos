/*
 *  cgiutil.h
 *  icuapps
 *
 *  Created by Steven R. Loomis on 21/09/2007.
 *  Copyright 2007-2008 IBM. All rights reserved.
 *
 */
 
#ifndef CGIUTIL
#define CGIUTIL 1

#include "unicode/utypes.h"
#include "unicode/unum.h"

#include <stdio.h>

struct CGIContext {
  const char *scriptName;      /* Cached results of getenv("SCRIPT_NAME") */
  const char *queryString;     /* QUERY_STRING */
  const char *cookies;         /* HTTP_COOKIE */
  const char *acceptCharset;   /* HTTP_ACCEPT_CHARSET */
  const char *acceptLanguage;  /* HTTP_ACCEPT_LANGUAGE */
  const char *serverName;      /* SERVER_NAME */
  char hostPort[256];          /* host:port  or just host  (if port 80) */
  const char *pathInfo;        /* PATH_INFO */
  uint16_t    port;            /* SERVER_PORT */
  const char *hostName;        /* HTTP_HOST */
  const char *postData;


  char    *headers;     /* append headers here (use appendHeader function, below) */
  int32_t  headerLen;  /* length of header *buffer*  */

};
typedef struct CGIContext CGIContext;

struct UUrlIter{
	CGIContext *lx;
	const char *qs; /* query string */
	const char *pd; /* post data */
	char *field;
	char fldBuf[256];
	char *val;
	char valBuf[1024];
};
typedef struct UUrlIter UUrlIter;

CGIContext *cgi_open(void);
void cgi_close(CGIContext *lx);
void cgi_initCGIVariables(CGIContext* lx);
void cgi_initPOSTFromFILE(CGIContext* lx, FILE *f);
void cgi_closePOSTFromFILE(CGIContext* lx);
const char *cgi_fieldInQuery(CGIContext* lx, const char *query, const char *field);
const char *cgi_fieldInCookie(CGIContext* lx, const char *query, const char *field);
const char *cgi_copyField(CGIContext* lx, const char *val);
const char *cgi_copyCookieField(CGIContext* lx, const char *val);
static const char *cgi_getQueryField(CGIContext* lx, const char *field);
const char *cgi_queryField(CGIContext* lx, const char *field);
UBool cgi_hasQueryField(CGIContext* lx, const char *field);
const char *cgi_cookieField(CGIContext* lx, const char *field);

const UChar* cgi_queryFieldU(CGIContext* lx, const char *field);


double cgi_parseDoubleFromString(CGIContext* lx, UNumberFormat* nf, const char *str, double defVal);






#endif
