/* Copyright (c) 2000 IBM, Inc. all rights reserved */

/* Write an object out of the LX virtual path to stdout */

#include "unicode/utypes.h"
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
	  char *out;
	  out = calloc(1,(u_strlen(u)+2)*(6)); /* space for \uXXXX */
	  u_UCharsToChars(u, out, u_strlen(u)+1);
	  fprintf(lx->fOUT, "\"%s\"", out);
	}
      }
      break;
    case RES_BINARY: fprintf(lx->fOUT, "<A HREF=\"./%s\">BINARY</A>", ures_getKey(n)); break;
    case RES_TABLE: fprintf(lx->fOUT, "TABLE"); break;
    case RES_INT: fprintf(lx->fOUT, "INT=%d", ures_getInt(n, &s2)); break;
    case RES_ARRAY: fprintf(lx->fOUT, "ARRAY");
      fprintf(lx->fOUT, "<BR><OL>\r\n");
      {
	int i;
	for(i=0;i<ures_getSize(n);i++)
	{
	  s2 = U_ZERO_ERROR;
	  sub = ures_getByIndex(n, i, sub, &s2);
	  if(U_FAILURE(s2)) break;
	  fprintf(lx->fOUT, " <LI> ");
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

  if(strcmp(lx->chosenEncoding,"icudata"))
    rb = ures_open(FSWF_bundlePath(), lx->cLocale, &status);  
  else
    rb = ures_open(NULL, lx->cLocale, &status);  

  if(U_FAILURE(status))
  {
    printf("Content-type: text/html\r\n\r\n");
    printf("Error: Couldn't open bundle [%s] in path [%s], looking for [%s].<P>\r\n",
           lx->cLocale,
           u_getDataDirectory(),
           FSWF_bundlePath(),
           path);
    printf("Error: %s\n", u_errorName(status));
    printf("<hr><A HREF=\"http://oss.software.ibm.com/icu\">ICU Home</A>\r\n");
    return;
  }

  if(!path || !(*path) || (*path == '/'))
  {

    fprintf(lx->fOUT, "Content-type: text/html\r\n\r\n");
    fprintf(lx->fOUT, "<html><head><title>bundle list for %s</title></head>\r\n",
           lx->cLocale);
    fprintf(lx->fOUT, "<body>");
    fprintf(lx->fOUT, "<h1>%s : %s</h1>\r\n", FSWF_bundlePath(), lx->cLocale);
    fprintf(lx->fOUT, "V: %s<p>\n", ures_getVersionNumber(rb));
    fprintf(lx->fOUT, "<ul>");
    while(ures_hasNext(rb) && U_SUCCESS(s2))
      {
        s2 = U_ZERO_ERROR;
        n = ures_getNextResource(rb, n, &s2);
        fprintf(lx->fOUT, "<li> ");
        if(U_FAILURE(s2))
          {
            fprintf(lx->fOUT, " <B>%s</B>\n", u_errorName(s2));
          }
        else
          {
	    writeSubObject( lx, n);
            fprintf(lx->fOUT, "\n");
          }
      }
    fprintf(lx->fOUT, "</UL>");
    fprintf(lx->fOUT, "</BODY>");
    fprintf(lx->fOUT, "</HTML>");
    
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
      fprintf(lx->fOUT, "Content-type: text/html\r\n\r\n");
      fprintf(lx->fOUT, "Error: Couldn't get [%s] in bundle [%s] in path [%s]<P>\r\n",
             path,
             lx->cLocale,
             u_getDataDirectory());
      fprintf(lx->fOUT, "Error: %s\n", u_errorName(s2));
      fprintf(lx->fOUT, "<hr><A HREF=\"http://oss.software.ibm.com/icu\">ICU Home</A>\r\n");
      return;
    }


    bin = ures_getBinary(n, &len, &s2);
    if(U_FAILURE(s2))
    {
      fprintf(lx->fOUT, "Content-type: text/html\r\n\r\n");
      fprintf(lx->fOUT, "Error: Couldn't get binary [%s] in bundle [%s] in path [%s]<P>\r\n",
             path,
             lx->cLocale,
             u_getDataDirectory());
      fprintf(lx->fOUT, "Error: %s\n", u_errorName(s2));
      fprintf(lx->fOUT, "<hr><A HREF=\"http://oss.software.ibm.com/icu\">ICU Home</A>\r\n");
      return ;
    }
    /* whew! */
    
    if(strstr(path, ".html"))
      fprintf(lx->fOUT, "Content-type: text/html\r\n");
    else if(strstr(path, ".gif"))
      fprintf(lx->fOUT, "Content-type: image/gif\r\n");
    else if(strstr(path, ".jpg"))
      fprintf(lx->fOUT, "Content-type: image/jpeg\r\n");
    else
      fprintf(lx->fOUT, "Content-type: application/octet-stream\r\n");

    fprintf(lx->fOUT, "Content-length: %d\r\n", len);
    fprintf(lx->fOUT, "\r\n");
    fflush(lx->fOUT);
    fwrite(bin, 1, len, lx->fOUT);
    fprintf(lx->fOUT, "\r\n");
    fflush(lx->fOUT);
  }

  ures_close(n);
}
