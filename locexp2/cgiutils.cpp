//
//  cgiutils.cpp
//  locexp2
//
//  Copyright 2011 IBM Corporation and Others. All rights reserved.
//

#include "unicode/utypes.h"
#include "ufcgi.h"
#include "cgiutils.h"
#include <string.h>
#include <stdlib.h>

static UChar zeroStr[] = { 0x0000 };

const char *cgi_fieldInQuery(const char *query, const char *field)
{
    const char *q = query;
    size_t len   = strlen(field);
    while (*q && (q = strstr(q, field))) {  /* look for an occurance of the field */
        
        /* fprintf(stderr, "trine %s>%s<\n", field, q); */
        if(((q==query) || (q[-1]=='&')) && /* beginning, or field boundary */
           (q[len]=='=')) {   /* end with = sign */
            return q+len+1; 
        }
        q++;
    }
    return NULL;
}

const char *cgi_fieldInCookie(const char *query, const char *field)
{
    const char *q = query;
    size_t len   = strlen(field);
    while (*q && (q = strstr(q, field))) {  /* look for an occurance of the field */
        
        /* fprintf(stderr, "trine %s>%s<\n", field, q); */
        if(((q==query) || (q[-1]==' ' && q[-2]==';')) && /* beginning, or field boundary */
           (q[len]=='=')) {   /* end with = sign */
            return q+len+1; 
        }
        q++;
        if(*q == ' ') {
            q++;
        }
    }
    return NULL;
}

const char *cgi_copyField(const char *val)
{
    const char *end;
    
    if(!val) return NULL; /* not found */
    if(!*val) return val;  /* 0 length field at end of string */
    
    end = strchr(val, '&');
    if(!end) {
        return strdup(val);  /* you get the whole string - */
    } else {
        char *ret;               /* copy up to but not including & */
        /* fprintf(stderr, "%s:%d end:%d val:%d, malloc %d\n", __FILE__, __LINE__, end, val, (end-val+1)); */
        ret = (char*)malloc(end-val+1);
        strncpy(ret,val,end-val);
        ret[end-val]=0;
        return ret;
    }
}

#if 0
const UChar* cgi_queryFieldU(UFCGI* u, const char *field) {
    const char *val = cgi_getQueryField(u, field);
    if(!val) { 
        return NULL; /* field not found */
    } else if(!*val || *val == '&') {
        return zeroStr; /* 0-length */
    } else {
        const char *end;
        UChar *ret;
        end = strchr(val, '&');
        if(!end) { end = val+strlen(val); }
        /*fprintf(stderr, "%s:%d end:%p val:%p, malloc %d\n", __FILE__, __LINE__, end, val, (end-val+1)); */
        ret = (UChar*)malloc((end-val+1)*sizeof(ret[0])); /* worst case is 1:1. Best case is 9 chars per uchar (%aa%bb%cc):1 */
        unescapeAndDecodeQueryField_enc(ret, end-val, val, NULL /* utf-8 */);
        return ret;
    }
}
#endif

const char *cgi_copyCookieField(const char *val)
{
    const char *end;
    
    if(!val) return NULL; /* not found */
    if(!*val) return val;  /* 0 length field at end of string */
    
    end = strchr(val, ';');
    if(!end) {
        return strdup(val);  /* you get the whole string - */
    } else {
        char *ret;               /* copy up to but not including & */
        ret = (char*)malloc(end-val+1);
        strncpy(ret,val,end-val);
        ret[end-val]=0;
        return ret;
    }
}

/* Generic routines */
const char *cgi_getQueryField(UFCGI* u, const char *field)
{
    const char *val = NULL;
    
    if(u->postData) {
        val = cgi_fieldInQuery(u->postData, field);
    }
    
    const char *qs;
    if(!val && (qs=u->queryString)&&*qs) {
        val = cgi_fieldInQuery(qs, field);
    }
    
    /*    if(val) fprintf(stderr, "%s:%d %s=>%s<\n", __FILE__, __LINE__, field, val);  */
    
    return val;
}

const char *cgi_queryField(UFCGI* u, const char *field)
{
    return cgi_copyField(cgi_getQueryField(u,field));
}

UBool cgi_hasQueryField(UFCGI* u, const char *field)
{
    const char *val =  cgi_getQueryField(u,field);
    if((val == NULL) || (*val == '&')) {
        return false; 
    } else {
        return true; 
    }
}

/* Cookie versions */
const char *cgi_cookieField(UFCGI* u, const char *field)
{
    const char *val;
    
    if((u->cookies == NULL) || !u->cookies[0]) {
        return false;
    }
    val =  cgi_fieldInCookie(u->cookies, field);
    
    return cgi_copyField(val);
}

UBool cgi_hasCookieField(UFCGI* u, const char *field)
{
    const char *val;
    if((u->cookies == NULL) || !u->cookies[0]) {
        return false;
    }
    val =  cgi_fieldInCookie(u->cookies, field);
    if((val == NULL) || (*val == ';')) {
        return false; 
    } else {
        return true; 
    }
}

#if 0
void cgi_appendHeader(UFCGI* u, const char *header, const char *fmt, ...)
{
    va_list ap;
    char buf[1024];
    int32_t len;
    int32_t hdrlen;
    
    va_start(ap, fmt);
    
    strcpy(buf,header);
    strcat(buf,": ");
    
    vsnprintf(buf+strlen(buf), 1024-3-(strlen(buf)), fmt, ap);
    strcat(buf,"\n");
    
    len = strlen(buf);
    if(!u->headers) {
        u->headers = malloc(1024);
        u->headerLen = 1024;
        u->headers[0] = 0;
    }
    hdrlen = strlen(u->headers);
    if((hdrlen+len+5) > u->headerLen) { /* 5 = \r\n\r\n\0 */ 
        u->headerLen = hdrlen + len + 5;
        u->headers = realloc(u->headers,u->headerLen);
    }
    strcat(u->headers,buf);
}
#endif

#if 0
double cgi_parseDoubleFromField(UFCGI* u, UNumberFormat* nf, const char *key, double defVal) {
    return cgi_parseDoubleFromString(u, nf, cgi_queryField(u, key), defVal);
}
#endif

#if 0
double cgi_parseDoubleFromString(UFCGI* u, UNumberFormat* nf, const char *str, double defVal) {
    UChar valueString[1024];
    int32_t parsePos = 0;
    double val = 0.;
    UErrorCode status = U_ZERO_ERROR;
    if(!str || !*str) {
        return defVal;
    }
    /* Localized # */
    unescapeAndDecodeQueryField(valueString, (sizeof(valueString)/sizeof(valueString[0]))-1, str);
    u_replaceChar(valueString, 0x0020, 0x00A0);
    val = unum_parseDouble(nf, valueString, -1, &parsePos, &status);
    if(U_FAILURE(status)) {
        val = defVal;
    }
    return val;
}
#endif


UUrlIter *uurl_open(UFCGI *u)
{
	UUrlIter *n;
	n = (UUrlIter*)malloc(sizeof(UUrlIter));
	memset((void*)n,0,sizeof(UUrlIter));
	if(u->postData) {
		n->pd = u->postData;
	}
	if(u->queryString) {
		n->qs = u->queryString;
	}
	n->u = u;
	return(UUrlIter*)n;
}

static const char *uurl_next_priv(UUrlIter *u, const char **buf)
{
	const char *j;
	UUrlIter* n = (UUrlIter*)u;
	if(buf&&*buf&&**buf) {
		while(**buf == '&') {
			(*buf)++;
		}
		if(!(**buf)) {
			return NULL;
		}
		j=strchr((*buf),'=');
		if(!j) {
			j = (*buf)+strlen(*buf);
		}
		
		if((j-(*buf)) > (((sizeof((n->fldBuf))/sizeof(n->fldBuf[0]))-1) )) {
			n->field = (char*)malloc(j-(*buf)+1);
		} else {
			n->field = n->fldBuf;
		}
		
		strncpy(n->field, (*buf),j-(*buf));
		n->field[j-(*buf)]=0;
		*buf = j;
		if(**buf == '=') {
			(*buf)++;
		}
		/* get value .. */
		j=strchr((*buf),'&');
		if(!j) {
			j = (*buf)+strlen(*buf);
		}
		
		if((j-(*buf)) > (((sizeof((n->valBuf))/sizeof(n->valBuf[0]))-1) )) {
			n->val = (char*)malloc(j-(*buf)+1);
		} else {
			n->val = n->valBuf;
		}
		strncpy(n->val, (*buf),j-(*buf));
		n->val[j-(*buf)]=0;
		*buf = j;
		if(**buf == '&') {
			(*buf)++;
		}
		return n->field;
	}
	return NULL;
}
#if 0
const char *q = query;
int32_t len   = strlen(field);
while (*q && (q = strstr(q, field))) {  /* look for an occurance of the field */
    
    /* fprintf(stderr, "trine %s>%s<\n", field, q); */
    if(((q==query) || (q[-1]=='&')) && /* beginning, or field boundary */
       (q[len]=='=')) {   /* end with = sign */
        return q+len+1; 
    }
    q++;
}
return NULL;
#endif

const char *uurl_next(UUrlIter *u)
{
	UUrlIter* n = (UUrlIter*)u;
	const char *f = NULL;
	
	if(n->val && (n->val != n->valBuf)) {
		free(n->val);
	}
	n->val=NULL;
	if(n->field && (n->field != n->fldBuf)) {
		free(n->field);
	}
	n->field=NULL;
	
	if(n->pd) {
		f =  uurl_next_priv(u, &(n->pd));
	}
	
	if(!f && n->qs) {
		f = uurl_next_priv(u, &(n->qs));
	}
	
	return f;
}

const char *uurl_value(UUrlIter *u)
{
	return ((UUrlIter*)u)->val;
}

void uurl_close(UUrlIter *u)
{
	free((UUrlIter*)u);
}

const char *cgi_url(UFCGI *u  ) {
    return "./???";
}
