/**********************************************************************
*   Copyright (C) 2000-2013, International Business Machines
*   Corporation and others.  All Rights Reserved.
***********************************************************************/

/* Write an object out of the LX virtual path to stdout */

#include "unicode/utypes.h"
#include "unicode/putil.h"
#include "unicode/ures.h"
#include "unicode/ustdio.h"
#include "locexp.h"
#include <stdlib.h>

void writeSubObject( LXContext *lx, UResourceBundle *n )
{
  UErrorCode s2 = U_ZERO_ERROR;
  UResourceBundle *sub = NULL;

  fprintf(lx->fOUT, "%s :", ures_getKey(n));
  if(ures_getSize(n) > 1) 
    fprintf(lx->fOUT, " [%d]", ures_getSize(n));

  switch(ures_getType(n))
    {
    case RES_NONE: fprintf(lx->fOUT, "NONE");break;
    case RES_STRING:
      /*fprintf(lx->fOUT, "STRING"); */
      {
	const UChar *u;
	int32_t len = 0;
	u = ures_getString(n, &len, &s2);
	if(U_FAILURE(s2)) { fprintf(lx->fOUT, " Err %s\n", u_errorName(s2));  return; }
	if(u == NULL)
	{
	  fprintf(lx->fOUT, "NULL\n");
	}
	else
	{
	  uint8_t *out;
          int32_t length = 0;
          int32_t bufsz = 0;
          const UChar *up = u;
          UBool usError = false;

          bufsz = (u_strlen(u)+2)*(9);
	  out = calloc(1,bufsz); /* space for \uXXXX */
          out[0]=0;
          while(*up) {
            U8_APPEND(out, length, bufsz, *up, usError);
            up++;
          }
          out[length]=0;
          
	  /*u_UCharsToChars(u, out, u_strlen(u)+1); */
	  fprintf(lx->fOUT, "\"%s\"", out);
	}
      }
      break;
    case RES_BINARY: fprintf(lx->fOUT, "<a href=\"./%s\">BINARY</a>", ures_getKey(n)); break;
    case RES_TABLE: fprintf(lx->fOUT, "TABLE"); break;
    case RES_INT: fprintf(lx->fOUT, "INT=%d", ures_getInt(n, &s2)); break;
    case RES_ARRAY: fprintf(lx->fOUT, "ARRAY");
      fprintf(lx->fOUT, "<br /><ol>\r\n");
      {
	int i;
	for(i=0;i<ures_getSize(n);i++)
	{
	  s2 = U_ZERO_ERROR;
	  sub = ures_getByIndex(n, i, sub, &s2);
	  if(U_FAILURE(s2)) break;
	  fprintf(lx->fOUT, " <li> ");
	  writeSubObject(lx, sub);
	}
      }
      fprintf(lx->fOUT, "</OL>\r\n");
      break;

    case RES_INT_VECTOR: fprintf(lx->fOUT, "INT VECTOR"); break;
    case RES_RESERVED: fprintf(lx->fOUT, "RESERVED"); break;
    default: fprintf(lx->fOUT, "unknown %d", ures_getType(n)); break;
    }
}

void writeFileObject( LXContext *lx, const char *path )
{
  UResourceBundle *rb;
  UErrorCode status = U_ZERO_ERROR;
  UResourceBundle *n = NULL;
  UErrorCode s2 = U_ZERO_ERROR;
  const char *thePath = NULL;

  if(strcmp(lx->convRequested,"_icudata"))
  {
    thePath = FSWF_bundlePath();
  }
  else
  {
    thePath = NULL;
  }

  rb = ures_open(thePath, lx->dispLocale, &status);  

  if(U_FAILURE(status))
  {
    appendHeader(lx, "Content-type", "text/html;charset=utf-8");
    fprintf(lx->fOUT, "Error: Couldn't open bundle [%s] in path [%s], looking for [%s].<p>\r\n",
           lx->dispLocale,
           (thePath==NULL)?"NULL":thePath,
           path);
    fprintf(lx->fOUT, "Error: %s\n", u_errorName(status));
    fprintf(lx->fOUT, "<hr /><a href=\"" ICU_URL "\">ICU Home</a>\r\n");
    return;
  }

  if(!path || !(*path) || (*path == '/'))
  {

    appendHeader(lx, "Content-type", "text/html;charset=utf-8");

    fprintf(lx->fOUT, "<html><head><title>bundle list for %s</title></head>\r\n",
           lx->dispLocale);
    fprintf(lx->fOUT, "<body>");
    fprintf(lx->fOUT, "<h1>%s : %s</h1>\r\n", FSWF_bundlePath(), lx->dispLocale);
    fprintf(lx->fOUT, "V: %s<p>\n", ures_getVersionNumber(rb));
    fprintf(lx->fOUT, "<ul>");
    while(ures_hasNext(rb) && U_SUCCESS(s2))
      {
        s2 = U_ZERO_ERROR;
        n = ures_getNextResource(rb, n, &s2);
        fprintf(lx->fOUT, "<li> ");
        if(U_FAILURE(s2))
          {
            fprintf(lx->fOUT, " <b>%s</b>\n", u_errorName(s2));
          }
        else
          {
	    writeSubObject( lx, n);
            fprintf(lx->fOUT, "\n");
          }
        fprintf(lx->fOUT, "</li> ");
      }
    fprintf(lx->fOUT, "</ul>");
    fprintf(lx->fOUT, "</body>");
    fprintf(lx->fOUT, "</html>");
    
/*    exit(0); */
  }
  else
  {
    const uint8_t *bin= NULL;
    int32_t len;

    /* have some kind of path*/
    n = ures_getByKey(rb, path, n, &s2);
    if(U_FAILURE(s2))
    {
      appendHeader(lx, "Content-type", "text/html");
      fprintf(lx->fOUT, "Error: Couldn't get [%s] in bundle [%s] in path [%s]<p>\r\n",
             path,
             lx->dispLocale,
           (thePath==NULL)?"NULL":thePath);
      fprintf(lx->fOUT, "Error: %s\n", u_errorName(s2));
      fprintf(lx->fOUT, "<hr /><a href=\"" ICU_URL "\">ICU Home</a>\r\n");
      return;
    }


    bin = ures_getBinary(n, &len, &s2);
    if(U_FAILURE(s2))
    {
      appendHeader(lx, "Content-type", "text/html");
      fprintf(lx->fOUT, "Error: Couldn't get binary [%s] in bundle [%s] in path [%s]<p>\r\n",
             path,
             lx->dispLocale,
             u_getDataDirectory());
      fprintf(lx->fOUT, "Error: %s\n", u_errorName(s2));
      fprintf(lx->fOUT, "<hr /><a href=\"" ICU_URL "\">ICU Home</a>\r\n");
      return ;
    }
    /* whew! */
    
    {
      const char *type;

      if(strstr(path, ".html"))
        type = "text/html";
      else if(strstr(path, ".gif"))
        type = "image/gif";
      else if(strstr(path, ".jpg"))
        type = "image/jpeg";
      else if(strstr(path, ".css"))
        type = "text/css";
      else if(strstr(path, ".js"))
        type = "application/javascript";
      else
        type = "application/octet-stream";

      appendHeader(lx, "Content-type", type);
    }
    
    appendHeader(lx, "X-Content-length", "%d", len);
    fwrite(bin, 1, len, lx->fOUT);
  }

  ures_close(n);
}
