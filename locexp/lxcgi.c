/**********************************************************************
*   Copyright (C) 2003, International Business Machines
*   Corporation and others.  All Rights Reserved.
***********************************************************************/
#include "locexp.h"
#include <unicode/ustdio.h>


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

  if((tmp=getenv("QUERY_STRING")) == NULL) {
    static char ho_str[1024];
    static char pi_str[1024];
    static char qs_str[1024];
    static char sn_str[1024];
    static char sp_str[1024];
    
    fprintf(stderr, "This program is designed to be run as a CGI-BIN.  QUERY_STRING is undefined.");
    
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
    perror("fread");
    fprintf(stderr, "err reading %d got %d\n", clen,len);
    free(buf);
    return;
  }

  if(len>clen) { 
    len = clen; /* avoid reading more than requsted? */
  }
  
  buf[len]=0;

  fprintf(stderr, "Got: [%d/%d] >%s<\n", len,clen,buf);

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

/* Generic routines */
const char *getQueryField(LXContext* lx, const char *field)
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
    return FALSE; 
  } else {
    return TRUE; 
  }
}
