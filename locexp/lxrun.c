/**********************************************************************
*   Copyright (C) 1999-2003, International Business Machines
*   Corporation and others.  All Rights Reserved.
***********************************************************************/
#include "locexp.h"
#include <unicode/udata.h>

/************************ fcns *************************/
/** Called functions from the main() module **/
LXContext *theContext;

void initContext ( LXContext *ctx )
{
    /* INIT THE LX */
    memset(ctx, 0, sizeof(*ctx));
    ctx->dispLocale = "en";
    ctx->dispRB = 0;
    ctx->convName = "utf-8";
    ctx->locales = NULL;
    ctx->curLocale = NULL;
    ctx->parLocale = NULL;
    ctx -> numLocales = 0;
    /* END INIT LX */
    ctx->headers = NULL;
    ctx->headerLen = 0;
    ctx->OUT = NULL;
    ctx->fOUT = NULL;
}

void initLX()
{
    /* set the path for FSWF */
    char newPath[500];
    UErrorCode myError = U_ZERO_ERROR;

#ifdef LX_STATIC
    /* try static data first .. then fall back to individual files */
    udata_setAppData( "locexp", (const void*) locexp_dat, &myError);
    if(U_SUCCESS(myError))
    {
        FSWF_setBundlePath("locexp");
        return;
    }
#endif

    strcpy(newPath, u_getDataDirectory());
    strcat(newPath, "locexp");
    FSWF_setBundlePath(newPath);
}

void closeLX(LXContext *theContext)
{
    FSWF_close();
    if(theContext != NULL)
    {
      destroyLocaleTree(theContext->locales);
      theContext->locales = NULL;
    }
}

/* Initialize the environment */
void setupLocaleExplorer(LXContext *lx)
{
    UErrorCode status = U_ZERO_ERROR;
    char *tmp = NULL;

    /* init ...... */
    /*
      uloc_setDefault("sr_NZ_EURO", &status);
    */ 
    /* BASELINE. Don't use a real locale here - will mess up the fallback error codes [for now] */


    uloc_setDefault("en_US_CALIFORNIA", &status);


#ifdef  WIN32
/*  u_setDataDirectory("c:\\o\\icu\\source\\data\\");
 */ /* ONLY IF you need to force the path .... */
#endif


#if 0
   /** Below is useful for debugging. */
    fprintf(stderr, "PID=%d\n", getpid());  
    system("sleep 20");   
#endif

    status = U_ZERO_ERROR; 

    /* Set up some initial values, just in case something goes wrong later. */
  
    lx->convName = MIMECharsetName(lx->convRequested); /* for some sanity */

    /** Setup the callbacks **/
    /* put our special error handler in */

    if(u_fgetConverter(lx->OUT) == NULL)
    {
        fprintf(stdout,"content-type: text/plain\r\n");
        fprintf(stdout,"\r\nFatal: can't open ICU Data (%s)\r\n", u_getDataDirectory());
        fprintf(stdout,"ICU_DATA=%s\n", getenv("ICU_DATA"));
        exit(0);

        lx->couldNotOpenEncoding = lx->convRequested; /* uhoh */
    }
    else
    {
        /* important - clear out the ctx */
        memset(&lx->backslashCtx, sizeof(lx->backslashCtx), 0);
        ucnv_setFromUCallBack((UConverter*)u_fgetConverter(lx->OUT), 
                              UCNV_FROM_U_CALLBACK_BACKSLASH,
                              &lx->backslashCtx,
                              &lx->backslashCtx.subCallback,
                              &lx->backslashCtx.subContext,
                              &status);
    
    
        lx->backslashCtx.html = TRUE;
    
        memset(&lx->decomposeCtx, sizeof(lx->decomposeCtx), 0);
        ucnv_setFromUCallBack((UConverter*)u_fgetConverter(lx->OUT), 
                              UCNV_FROM_U_CALLBACK_DECOMPOSE,
                              &lx->decomposeCtx,
                              &lx->decomposeCtx.subCallback,
                              &lx->decomposeCtx.subContext,
                              &status);
    
        /* To do: install more cb's later. */
        /* following are for various callbacks - mostly disabled for now. -srl */
      
#if 0
        /* Old transliterated callback - transliterates unconvertible runs. 
           Note: Now, we use ustdio based transliteration */
        if(!strcmp(lx->chosenEncoding, "transliterated"))
        {
            memset(&lx->xlitCtx, sizeof(lx->xlitCtx), 0);
            lx->xlitCtx.html = FALSE;
            ucnv_setFromUCallBack((UConverter*)u_fgetConverter(lx->OUT), 
                                  UCNV_FROM_U_CALLBACK_TRANSLITERATED,
                                  &lx->xlitCtx,
                                  &lx->xlitCtx.subCallback,
                                  &lx->xlitCtx.subContext,
                                  &status);
        }

#endif

#if 0
        /* Collect callback - collect unconvertable chars & suggest a better converter */
        else
        
        {
            ucnv_setFromUCallBack((UConverter*)u_fgetConverter(lx->OUT), &UCNV_FROM_U_CALLBACK_COLLECT, &status);
            COLLECT_lastResortCallback =  UCNV_FROM_U_CALLBACK_DECOMPOSE;
        }
    

#ifdef LX_USE_NAMED
        /* overrides */
        ucnv_setFromUCallBack((UConverter*)u_fgetConverter(lx->OUT), &UCNV_FROM_U_CALLBACK_NAMED, &status);
#endif
    
#endif
    } /* end: if have a converter */

    /* setup the time zone.. */
    if (tmp && !strncmp(tmp,"SETTZ=",6))
    {
        const char *start = (tmp+6);
        const char *end;

        lx->newZone[0] = 0;

        end = strchr(tmp, '&');

        if(!end)
        {
            end = (start+strlen(start));
        }
        unescapeAndDecodeQueryField(lx->newZone,256,start);

        if(u_strlen(lx->newZone))
        {
            char junk[200];
            u_austrcpy(junk, lx->newZone);
            appendHeader(lx, "Set-Cookie", "TZ=%s;path=/;", junk);
        }
    }


    if(lx->newZone[0] == 0x0000) /* if still no zone.. */
    {
        const char *cook;
        cook = getenv("HTTP_COOKIE");

        if((cook)&&(cook=strstr(cook,"TZ=")))
        {
            cook += 3;
            u_uastrcpy(lx->newZone,cook);
        }
    }
  
    /* Print the encoding and last HTTP header... */

    appendHeader(lx, "Content-Type", "text/html;charset=%s", lx->convName);

    /*  what will this do?
       {
       char langBuf[100];
       uloc_getLanguage(NULL,langBuf,100,&status);
       printf("Content-Language: %s\r\n", langBuf);
       }*/


    /* parse & sort the list of locales */
    setupLocaleTree(lx);
    /* Open an RB in the default locale */
    lx->dispRB = ures_open(NULL, lx->dispLocale, &status);

    if(!strcmp(lx->convRequested, "transliterated"))
    {
        char id[200];
        UErrorCode transStatus = U_ZERO_ERROR;
        UTransliterator *trans;
        sprintf(id,"Any-%s", /* lx->curLocaleName,*/ lx->dispLocale);
        if(!strcmp(lx->dispLocale, "xol")) {
            sprintf(id, "Latn-Ital");
        }
        /* fprintf(stderr, "LC=[%s]\n", id);  */
        trans = utrans_open(id, UTRANS_FORWARD, NULL, -1, NULL, &transStatus);
        if(U_FAILURE(transStatus))
        {
            fprintf(stderr,"Failed to open - %s\n", u_errorName(transStatus));
            /* blah blah balh*/
        }
        else
        {
            lx_setHTMLFilterOnTransliterator(trans, TRUE);
            u_fflush(lx->OUT);
            trans = u_fsettransliterator(lx->OUT, U_WRITE, trans, &transStatus);
            if(trans != NULL)
            {
                utrans_close(trans);
            }
        }
    }

/*  u_uastrcpy(lx->newZone, "Europe/Malta"); */
    u_uastrcpy(lx->newZone, "PST"); /* for now */


    if(lx->newZone[0] != 0x0000)
    {
        UTimeZone *tz;

        lx->timeZone = lx->newZone;

#if 0
        /* TODO: 2.8:  C tz interface doesn't seem to be working. Investigate */
        tz = utz_open(lx->newZone); /* returns NULL for nonexistent TZ!! */
#else
        tz = NULL;
#endif

        if(tz)
        {
            utz_setDefault(tz);
            utz_close(tz);
        }
    }
    else
    {
        lx->timeZone = NULL;
    }

}

/* defining _GNU_SOURCE will result in a little more efficiency - 
   use memory instead of a tmp file */

#ifdef _GNU_SOURCE
#define LX_OPEN_MEMSTREAM
#else
#define LX_TMPFILE
#endif

/* Initialize and then run */
void runLocaleExplorer(LXContext *lx)
{

  FILE *tmpf;
  FILE *realout;
#if defined(LX_TMPFILE)
  long int len;
/* #elif ..etc.. */
#else
  char *buf;
  size_t len;
#endif 

  lx->convRequested = "utf-8";
  lx->convName = "utf-8";
  
  setLocaleAndEncoding(lx);
  
  realout = lx->fOUT;
#ifdef LX_TMPFILE
  tmpf = tmpfile();
  lx->fOUT = tmpf;
#else
  len = 16384;
  buf = malloc(len);
  lx->fOUT = open_memstream(&buf, &len);
#endif
  
  lx->OUT = openUFILE(lx);
  
  if(!lx->OUT) {
    doFatal(lx, "u_finit trying to open file", 0);
    return;
  }
  
  if(lx->fileObj) { /* file output */
    writeFileObject(lx, lx->fileObj);
  } else {
    setupLocaleExplorer(lx); /* and output initial headers */
  }

    
  if(!lx->fileObj) {
    displayLocaleExplorer(lx);
  }
  
    fflush(lx->fOUT);
#ifdef LX_TMPFILE
    len = ftell(tmpf); /* get length from pos in file */
#endif
    appendHeader(lx, "Content-length", "%d", len);

    strcat(lx->headers, "\n");

    fwrite(lx->headers,1,strlen(lx->headers),realout); /* write out in one chunk */
#if defined(LX_TMPFILE)
#define OBUFSIZ 16384
    {
      char obuf[OBUFSIZ];
      int32_t rlen;
      int32_t toread;
#ifdef LX_BUF_DEBUG
      fprintf(stderr, "wrote %ld to tmp file\n", len);
      fflush(stderr);
#endif
      fflush(realout);
      rewind(tmpf);
#ifdef LX_BUF_DEBUG
      fprintf(stderr, "rewound tmp file\n");
      fflush(stderr);
#endif
      while(!ferror(tmpf)&&!feof(tmpf)&&len) {
        toread = (OBUFSIZ<len?OBUFSIZ:len);

#ifdef LX_BUF_DEBUG
        fprintf(stderr,"[read: %ld\n", toread);
        fflush(stderr);
#endif
        rlen = fread(obuf,1,toread,tmpf);
#ifdef LX_BUF_DEBUG
        fprintf(stderr,"[%d/%d]\n", rlen, toread);
#endif
        if(rlen>0) {
          fwrite(obuf,1,rlen,realout);
          len -= rlen;
        }
      }
      fflush(realout);
      fclose(tmpf);
    }
#else
    /* mem stream */
      fwrite(buf,1,len,realout); /* write out in one chunk ( omit if HEAD ! ) */
      free(buf);
      fflush(realout);
#endif
}    

UResourceBundle *getCollationBundle(LXContext *lx, UErrorCode *status) 
{
    if(U_FAILURE(*status)) {
        return NULL;
    }

    if(lx->curRB) {
        return(lx->curRB); 
    }

    if(!*(lx->curLocaleName)) {
        /* illegal arg */
        return NULL;
    }
    lx->curRB = ures_open(U_ICUDATA_NAME U_TREE_SEPARATOR_STRING "coll", lx->curLocaleName, status);

    return lx->curRB;
}

UResourceBundle *getCurrentBundle(LXContext *lx, UErrorCode *status) 
{
    if(U_FAILURE(*status)) {
        return NULL;
    }

    if(lx->curRB) {
        return(lx->curRB); 
    }

    if(!*(lx->curLocaleName)) {
        /* illegal arg */
        return NULL;
    }
    lx->curRB = ures_open(NULL, lx->curLocaleName, status);

    return lx->curRB;
}

UResourceBundle *getDisplayBundle(LXContext *lx, UErrorCode *status) 
{
  if(U_FAILURE(*status)) {
    return NULL;
  }

  if(lx->dispRB) {
    return(lx->dispRB); 
  }

  if(!*(lx->dispLocale)) { /* illegal arg */ return NULL; }
  lx->dispRB = ures_open(NULL, lx->dispLocale, status);
  return lx->dispRB;
}

void setLocaleAndEncoding(LXContext *lx)
{
    char *pi;
    char *tmp;

    /* First, parse out /locale/conv/  from pathInfo */
    pi = strdup(lx->pathInfo); /* TODO: string pool */ 

    if( (pi) && (*pi && '/') ) {
      pi++; /* eat first slash */
      tmp = strchr(pi, '/');
      
      if(tmp)
        *tmp = 0; /* tie off at the slash */
      
      if ( *pi != 0) { /* don't want 0-length locales */
        lx->dispLocale = pi;
        lx->dispLocaleSet = TRUE;
      } else {
        lx->dispLocale = "";
      }
      
      if(tmp) { /* have encoding */
        tmp++; /* skip '/' */
        pi = tmp;
        if(*pi) { /* don't want 0 length encodings */
          lx->convRequested = pi;

          tmp = strchr(tmp, '/');
          if(tmp) {
            pi = tmp + 1;
            *tmp = 0;
          
            if(lx->convRequested[0] == '_') {
              lx->fileObj = pi;
            }
          }

          lx->convSet = TRUE; 
	    }
      }
    }

    if(!lx->convRequested) {
      lx->convRequested="";
    }
    if(!lx->dispLocale) {
      lx->dispLocale = "";
    }

    if(!lx->dispLocaleSet && lx->acceptLanguage && *lx->acceptLanguage) {
      char *newLocale;
      /* OK, if they haven't set a locale, maybe their web browser has. */
      if(!(tmp=strchr(lx->acceptLanguage,','))) /* multiple item separator */
        if(!(tmp=strchr(lx->acceptLanguage,'='))) { /* strength separator */
          const char *lang;

          lang = lx->acceptLanguage + strlen(lx->acceptLanguage);
          newLocale = strdup(lx->acceptLanguage);
          newLocale[my_min(100,lang-lx->acceptLanguage)]=0;
          
          /* Note we don't do the PROPER thing here, which is to sort the possible languages by weight.  */
          
          /* half hearted attempt at canonicalizing the locale string. */
          newLocale[0] = tolower(newLocale[0]);
          newLocale[1] = tolower(newLocale[1]);
          if(newLocale[2] == '-')
            newLocale[2] = '_';
          if(newLocale[5] == '-')
            newLocale[5] = '_';
          
          newLocale[3] = toupper(newLocale[3]);
          newLocale[4] = toupper(newLocale[4]);
          
          if(isSupportedLocale(newLocale, TRUE)) { /* DO NOT pick an unsupported locale from the browser's settings! */
            lx->dispLocale =  newLocale;
            lx->dispLocaleSet = TRUE;
          } else {
            free(newLocale);
          }
        }
      /* that might at least get something.. It's better than defaulting to en_US */
    }
  
    if(!lx->convSet) {
      const char *accept;
      const char *agent;
      
      accept = lx->acceptCharset;

      if(accept && strstr(accept, "utf-8")) {
        lx->convRequested = "utf-8"; /* use UTF8 if they have it ! */
      } else if( (agent = (const char *)getenv("HTTP_USER_AGENT")) &&
                 (strstr(agent, "MSIE 4") || strstr(agent, "MSIE 5")) &&
                 (strstr(agent, "Windows NT")))  {
        lx->convRequested = "utf-8"; /* MSIE can handle utf8 but doesn't request it. */
      }
    }


    if(lx->convRequested) {
      lx->convUsed = lx->convRequested;
    }

    /* Map transliterated/fonted : */
    if(0==strcmp(lx->convRequested, "fonted"))
    {
      lx->convUsed = "usascii";
    }
    if(0==strcmp(lx->convUsed, "transliterated"))
    {
      lx->convUsed = "utf-8";
    }

 }

