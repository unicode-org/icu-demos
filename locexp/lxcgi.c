/**********************************************************************
*   Copyright (C) 2003-2015, International Business Machines
*   Corporation and others.  All Rights Reserved.
***********************************************************************/
#include "locexp.h"
#include <unicode/ustdio.h>
#include "unicode/unum.h"
#include <stdarg.h>
#include <stdio.h>

static const char *getenvOrEmpty(const char *env)
{
  const char *s;
  s = getenv(env);
  if(!s) return "";
  return s;
}


/* helpers for CGI-like environments */
void initCGIVariables(LXContext* lx)
{
  const char *tmp;

  if((tmp=getenv("GATEWAY_INTERFACE")) == NULL) {
    static char ho_str[1024];
    static char pi_str[1024];
    static char qs_str[1024];
    static char sn_str[1024];
    static char sp_str[1024];
    
    fprintf(stderr, "This program is designed to be run as a CGI-BIN.  GATEWAY_INTERFACE is undefined.");
    
    strcpy(ho_str, "HTTP_HOST=host.moc");
    strcpy(pi_str, "PATH_INFO=");
    strcpy(qs_str, "QUERY_STRING=");
    strcpy(sn_str, "SCRIPT_NAME=/cgi-bin/locexp");
    strcpy(sp_str, "SERVER_PORT=80");
    
    puts("");
    /* path info */
    printf(pi_str);
    if(fgets(pi_str+strlen(pi_str),1000, stdin)) {
      pi_str[strlen(pi_str)-1] = 0;
    }
    
    printf(qs_str);
    fgets(qs_str+strlen(qs_str),1000, stdin);

    puts(ho_str);
    puts(pi_str);
    puts(qs_str);
    puts(sn_str);
    puts(sp_str);
    
    putenv(ho_str);
    putenv(pi_str);
    putenv(qs_str);
    putenv(sn_str);
    putenv(sp_str);
    
  }

  lx->scriptName      = getenvOrEmpty("SCRIPT_NAME");
  lx->queryString     = getenvOrEmpty("QUERY_STRING");
  lx->pathInfo        = getenvOrEmpty("PATH_INFO");
  lx->acceptCharset   = getenvOrEmpty("HTTP_ACCEPT_CHARSET");
  lx->serverName      = getenvOrEmpty("SERVER_NAME");
  lx->hostName        = getenvOrEmpty("HTTP_HOST");
  lx->cookies         = getenvOrEmpty("HTTP_COOKIE");
  lx->acceptLanguage  = getenvOrEmpty("HTTP_ACCEPT_LANGUAGE");

  /* fix up hostPort */
}

void initPOSTFromFILE(LXContext* lx, FILE *f)
{
  const char* tmp;
  int32_t len;
  int32_t clen;
  char *buf;

  tmp = getenv("REQUEST_METHOD");
  if(!tmp || strcmp(tmp,"POST")) {
    return;
  }
  
  tmp=getenv("CONTENT_LENGTH");
  
  if(!tmp || (clen=atoi(tmp))<=0) {
    return;
  }
  
  if(clen > 60000) {
    return ; /* a little sanity, please! */
  }

  buf = malloc(clen+1);
  len=fread(buf, 1, clen, f);
  if(len<0) {
#if defined(LX_DEBUG)
    perror("fread");
    fprintf(stderr, "err reading %d got %d\n", clen,len);
#endif
    free(buf);
    return;
  }

  if(len>clen) { 
    len = clen; /* avoid reading more than requsted? */
  }
  
  buf[len]=0;
#ifdef SRL_DEBUG
  fprintf(stderr, "Got: [%d/%d] >%s<\n", len,clen,buf);
#endif

  lx->postData = buf;
}

void closeCGIVariables(LXContext* lx)
{
  /* nothing to do ? */
}


void closePOSTFromFILE(LXContext* lx)
{
  /* free(lx->POSTdata); */
}

const char *fieldInQuery(LXContext* lx, const char *query, const char *field)
{
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
}

const char *fieldInCookie(LXContext* lx, const char *query, const char *field)
{
  const char *q = query;
  int32_t len   = strlen(field);
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

const char *copyField(LXContext* lx, const char *val)
{
  const char *end;

  if(!val) return NULL; /* not found */
  if(!*val) return val;  /* 0 length field at end of string */
  
  end = strchr(val, '&');
  if(!end) {
    return strdup(val);  /* you get the whole string - */
  } else {
    char *ret;               /* copy up to but not including & */
    ret = malloc(end-val+1);
    strncpy(ret,val,end-val);
    ret[end-val]=0;
    return ret;
  }
}

const char *copyCookieField(LXContext* lx, const char *val)
{
  const char *end;

  if(!val) return NULL; /* not found */
  if(!*val) return val;  /* 0 length field at end of string */
  
  end = strchr(val, ';');
  if(!end) {
    return strdup(val);  /* you get the whole string - */
  } else {
    char *ret;               /* copy up to but not including & */
    ret = malloc(end-val+1);
    strncpy(ret,val,end-val);
    ret[end-val]=0;
    return ret;
  }
}

/* Generic routines */
static const char *getQueryField(LXContext* lx, const char *field)
{
  const char *val = NULL;

  if(lx->postData) {
    val = fieldInQuery(lx, lx->postData, field);
  }

  if(!val && lx->queryString) {
    val = fieldInQuery(lx, lx->queryString, field);
  }

  /*   if(val) fprintf(stderr, "%s=>%s<\n", field, val); */

  return val;
}

const char *queryField(LXContext* lx, const char *field)
{
  return copyField(lx, getQueryField(lx,field));
}

UBool hasQueryField(LXContext* lx, const char *field)
{
  const char *val =  getQueryField(lx,field);
  if((val == NULL) || (*val == '&')) {
    return false; 
  } else {
    return true; 
  }
}

/* Cookie versions */
const char *cookieField(LXContext* lx, const char *field)
{
  const char *val;

  if((lx->cookies == NULL) || !lx->cookies[0]) {
    return false;
  }
  val =  fieldInCookie(lx,lx->cookies, field);

  return copyField(lx, val);
}

UBool hasCookieField(LXContext* lx, const char *field)
{
  const char *val;
  if((lx->cookies == NULL) || !lx->cookies[0]) {
    return false;
  }
  val =  fieldInCookie(lx,lx->cookies, field);
  if((val == NULL) || (*val == ';')) {
    return false; 
  } else {
    return true; 
  }
}

void appendHeader(LXContext* lx, const char *header, const char *fmt, ...)
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
  if(!lx->headers) {
    lx->headers = malloc(1024);
    lx->headerLen = 1024;
    lx->headers[0] = 0;
  }
  hdrlen = strlen(lx->headers);
  if((hdrlen+len+5) > lx->headerLen) { /* 5 = \r\n\r\n\0 */ 
    lx->headerLen = hdrlen + len + 5;
    lx->headers = realloc(lx->headers,lx->headerLen);
  }
  strcat(lx->headers,buf);
}

double parseDoubleFromField(LXContext* lx, UNumberFormat* nf, const char *key, double defVal) {
  return parseDoubleFromString(lx, nf, queryField(lx, key), defVal);
}

double parseDoubleFromString(LXContext* lx, UNumberFormat* nf, const char *str, double defVal) {
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

typedef struct {
	LXContext *lx;
	const char *qs; /* query string */
	const char *pd; /* post data */
	char *field;
	char fldBuf[256];
	char *val;
	char valBuf[1024];
} UURLITER;

UUrlIter *uurl_open(LXContext *lx)
{
	UURLITER *n;
	n = malloc(sizeof(UURLITER));
	memset((void*)n,0,sizeof(UURLITER));
	if(lx->postData) {
		n->pd = lx->postData;
	}
	if(lx->queryString) {
		n->qs = lx->queryString;
	}
	n->lx = lx;
	return(UUrlIter*)n;
}

static const char *uurl_next_priv(UUrlIter *u, const char **buf)
{
	const char *j;
	UURLITER* n = (UURLITER*)u;
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
			n->field = malloc(j-(*buf)+1);
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
			n->val = malloc(j-(*buf)+1);
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
	UURLITER* n = (UURLITER*)u;
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
	return ((UURLITER*)u)->val;
}

void uurl_close(UUrlIter *u)
{
	free((UURLITER*)u);
}

const char *cgi_url(LXContext *lx  ) {
  return lx->scriptName;
}
