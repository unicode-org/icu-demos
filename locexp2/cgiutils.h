//
//  cgiutils.h
//  locexp2
//
//  Copyright 2011 IBM Corporation and Others. All rights reserved.
//
#ifndef _CGIUTILS
#define _CGIUTILS

#include "ufcgi.h"

#include "unicode/unum.h"

struct UUrlIter{
	UFCGI *u;
	const char *qs; /* query string */
	const char *pd; /* post data */
	char *field;
	char fldBuf[256];
	char *val;
	char valBuf[1024];
};

const char *cgi_fieldInQuery(const char *query, const char *field);
const char *cgi_fieldInCookie( const char *query, const char *field);
const char *cgi_copyField( const char *val);
const char *cgi_copyCookieField( const char *val);
const char *cgi_getQueryField(UFCGI* lx, const char *field);

//const UChar* cgi_queryFieldU(UFCGI* lx, const char *field);


//double cgi_parseDoubleFromString(UFCGI* lx, UNumberFormat* nf, const char *str, double defVal);

#endif
