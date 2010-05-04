/*
*******************************************************************************
* Copyright (C) 2004-2010, International Business Machines Corporation and    *
* others. All Rights Reserved.                                                *
**/

#include "unicode/utypes.h"
#include "unicode/putil.h"
#include "unicode/udata.h"
#include "unicode/ustdio.h"
#include "unicode/ures.h"
#include "icons.h"
#include "demo_settings.h"
#include "unicode/lx_utils.h"

#include <string.h>

#define BUNDLE_PATH "ubrowseres"
#define ICON_BUNDLE "icons"

U_CFUNC char ubrowseres_dat[];

int icons_init() {
  UErrorCode status = U_ZERO_ERROR;
  udata_setAppData(BUNDLE_PATH, (const void*) ubrowseres_dat, &status);
  if(U_FAILURE(status)) {
    return 1;
  }
  FSWF_setBundlePath(BUNDLE_PATH);
  return 0;
}
  

void icons_write ( /* UFILE *OUT, */ const char *path )
{
  UResourceBundle *rb;
  UErrorCode status = U_ZERO_ERROR;
  UResourceBundle *n = NULL;
  UErrorCode s2 = U_ZERO_ERROR;
  const char *thePath = NULL;

  thePath = BUNDLE_PATH;

  rb = ures_open(thePath,ICON_BUNDLE, &status);   /* direct */

  if(U_FAILURE(status))
  {
    fprintf(stdout, "Content-type: text/html;charset=utf-8\n\n");
    fprintf(stdout, "Error: Couldn't open bundle [%s] in path [%s], looking for [%s].<P>\r\n",
	    "icons",
           (thePath==NULL)?"NULL":thePath,
           path);
    fprintf(stdout, "Error: %s\n", u_errorName(status));
    fprintf(stdout, "<hr><A HREF=\"" ICU_URL "\">ICU Home</A>\r\n");
    return;
  }

  if(!path || !(*path) || (*path == '/'))
  {
#if 1
    fprintf(stdout, "Content-type: text/html;charset=utf-8\n\n<i>an error has occured.</i>\n");
#else
    fprintf(stdout, "Content-type", "text/html;charset=utf-8\n\n");

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
#endif
  }
  else
  {
    const uint8_t *bin= NULL;
    int32_t len;

    /* have some kind of path*/
    n = ures_getByKey(rb, path, n, &s2);
    if(U_FAILURE(s2))
    {
      fprintf(stdout, "Content-type: %s\n\n", "text/html");
      fprintf(stdout, "Error: Couldn't get [%s] in bundle [%s] in path [%s]<P>\r\n",
             path,
             "icons",
           (thePath==NULL)?"NULL":thePath);
      fprintf(stdout, "Error: %s\n", u_errorName(s2));
      fprintf(stdout, "<hr><A HREF=\"" ICU_URL "\">ICU Home</A>\r\n");
      return;
    }


    bin = ures_getBinary(n, &len, &s2);
    if(U_FAILURE(s2))
    {
      fprintf(stdout, "Content-type: %s\n\n", "text/html");
      fprintf(stdout, "Error: Couldn't get binary [%s] in bundle [%s] in path [%s]<P>\r\n",
             path,
             "icons",
             u_getDataDirectory());
      fprintf(stdout, "Error: %s\n", u_errorName(s2));
      fprintf(stdout, "<hr><A HREF=\"" ICU_URL "\">ICU Home</A>\r\n");
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
      else
        type = "application/octet-stream";

      fprintf(stdout, "Content-type: %s\n", type);
    }
    
    fprintf(stdout, "Content-length: %d\n\n", len);
    fflush(stdout);
    fwrite(bin, 1, len, stdout);
  }

  ures_close(n);
}
