
/**********************************************************************
*   Copyright (C) 1999-2000, International Business Machines
*   Corporation and others.  All Rights Reserved.
***********************************************************************/

/*
  TODO's:

  see ./README

 FIX:
  TOO MANY COMMENTS ;)
*/


/*--------------------------------------------------------------------------
*
* File listrb.c
*
* Modification History:
*
*   Date        Name        Description
*   7/14/1999    srl        Created
*   8/16/1999    srl        Display overhaul - locale status at bottom, etc.
*  10/14/1999    srl        Update for ICU1.3+, prepare for checkin!!
*   8/17/2000    srl        Update for 1.6
*  10/09/2000    srl        Put .gifs and .htmls, etc into resource bundles
****************************************************************************
*/


/* ==Note==

   this is a very UI intensive application. 
   It uses a whole pile of globals to maintain state and pass data 
   back and forth. As a result, it has a lot of 'convenience functions'
   that make sense in very limited contexts.

   If this thing was pulled into a multithreaded environment 
   they could probably be put in a paramblock of some sort. I was mostly
   just trying to keep the arg count and complexity down. For example, 
   lx->OUT the ubiquitous output file.

                         -- Steven R. Loomis

*/

#include <stdlib.h>
#include <stdio.h>
#include "stddef.h"
#include "string.h"
#include "unicode/ustdio.h"
#include "unicode/ucnv.h"
#include "unicode/ustring.h"
#include "unicode/udat.h"
#include "unicode/uloc.h"
#include "unicode/ures.h"
#include "unicode/ucol.h"
#include "unicode/ucal.h"
#include "ctype.h"
#include "unicode/lx_utils.h"
#include "unicode/ures_additions.h"
#include "unicode/decompcb.h"
#include "unicode/uchar.h"
#include "unicode/umsg.h"
#include "stdlib.h"
#include "string.h"
/* #include "unicode/fontedcb.h" */


#ifdef WIN32
#include <stdlib.h>
#include <fcntl.h>
#include <io.h>
#endif

#include "locexp.h"

# define uprv_strstr(a,b)  strstr(a,b)

#ifdef WIN32
#define LXHOSTNAME "Win_NT"
#define URLPREFIX ""
#endif

#include "unicode/translitcb.h"

#include "unicode/utimzone.h"

/* #include "unicode/collectcb.h" */
#include "unicode/usort.h"
#include "unicode/ucnv.h"

/********************** Some Konstants **** and structs ***************/

/* Define the following to enable 'demonstrate collation' */

#define kStatusBG "\"#EEEEEE\" " 


#define kXKeyBGColor "\"#AAEEAA\" "



#define kShowStringCutoffSize 200
#define kShow2dArrayRowCutoff 5
#define kShow2dArrayColCutoff 5



/********************* prototypes ***************************/


/* setup the UFILE */

/* Setup the 'locales' structure */
void   setupLocaleTree();

/* some fcns for different parts of the screen */
void doFatal(const char *what, UErrorCode err);
void printStatusTable();

/**
 * Print the path [ ICU LocaleExplorer > default > English > English (Canadian) ...] 
 * @param leaf Which node to start printing at
 * @param current The locale that should be selected
 * @param styled Should bold tags and links be put in?
 */
void printPath(const MySortable *leaf, const MySortable *current, bool_t styled);

/* selection of locales and converter */
void chooseLocale(const char *qs, bool_t toOpen, const char *current, const char *restored);
void chooseConverter(const char *restored);

void chooseConverterMatching(const char *restored, UChar *sample);

void chooseConverterFrom(const char *restored, USort *list);
void listBundles(char *b);

/* fcns for dumping the contents of a particular rb */
void showFlagImage(const char *locale, const char *extraIMGparams);
void showCollationElements(UResourceBundle *rb, const char *locale, const char *qs, const char *whichString);
void showString(UResourceBundle *rb, const char *locale, const char *qs, const char *whichString);
void showLocaleCodes(UResourceBundle *myRB, const char *locale);
void showStringWithDescription(UResourceBundle *rb, const char *locale, const char *qs, const UChar *desc[], const char *whichString, bool_t hidable);
void showArray(UResourceBundle *rb, const char *locale, const char *whichString);
void showArrayWithDescription(UResourceBundle *rb, const char *locale, const UChar *desc[], const char *whichString);
void show2dArrayWithDescription(UResourceBundle *rb, const char *locale, const UChar *desc[], const char *queryString, const char *whichString);
void showTaggedArray(UResourceBundle *rb, const char *locale, const char *queryString, const char *whichString);
void showShortLong(UResourceBundle *rb, const char *locale, const char *keyStem, const UChar *shortName, const UChar *longName, int32_t num);
void showDateTimeElements(UResourceBundle *rb, const char *locale);
void showSort(const char *locale, const char *b);

void showExploreDateTimePatterns(UResourceBundle *rb, const char *locale, const char *b);
void showExploreNumberPatterns  (const char *locale, const char *b);

void showExploreButton(UResourceBundle *rb, const char *locale, const UChar *sampleString, const char *key);
void showExploreButtonSort(UResourceBundle *rb, const char *locale, const char *sampleString, const char *key);
void showExploreLink(UResourceBundle *rb, const char *locale, const UChar *sampleString, const char *key);
void showExploreCloseButton(const char *locale, const char *frag);

bool_t didUserAskForKey(const char *key, const char *queryString);

/*  Pluggable UI.  Put these before and after each item. */
void showKeyAndStartItem(const char *key, const UChar *keyName, const char *locale, bool_t cumulative, UErrorCode showStatus);
void showKeyAndStartItemShort(const char *key, const UChar *keyName, const char *locale, bool_t cumulative, UErrorCode showStatus);
void showKeyAndEndItem(const char *key, const char *locale);
void explainStatus_X( UErrorCode status, const char *tag );

/**
 * (for CGI programs)
 * Build a UFILE based on the user's preference for locale and encoding.
 * Try to figure out what a good encoding to use is.
 * 
 * @param chosenEncoding (on return) the encoding that was chosen
 * @param didSetLocale   (on return) TRUE if a locale was chosen
 * @return the new UFILE. Doesn't set any callbacks
 */
UFILE *setLocaleAndEncodingAndOpenUFILE(char *chosenEncoding, bool_t *didSetLocale, bool_t *didSetEncoding, const char **fileObject);



/* write a string in \uXXXX format */
void writeEscaped(const UChar *s);
char *createEscapedSortList(const UChar *source);

/* is this a locale we should advertise as supported? */
bool_t isSupportedLocale(const char *locale, bool_t includeChildren); /* for LX interface */
bool_t isExperimentalLocale(const char *local); /* for real data */


void printHelpTag(const char *helpTag, const UChar *str);
/* ex: printHelpImg("coffee", L"[coffee help symbol]", L"coffee-help.gif", L"BORDER=3"); */
void printHelpImg(const char *helpTag, const UChar *alt, const UChar *img, const UChar *options);

void exploreFetchNextPattern(UChar *dstPattern, const char* qs);
void exploreShowPatternForm(UChar *dstPattern, const char *locale, const char *key, const char* qs, double value, UNumberFormat *valueFmt);

/********************** globals ********************/
LXContext *lx = NULL; /* we aren't very creative at the moment. 
                       move this to a local!*/


/************************ fcns *************************/


/* main function. we write the outside html page here as well. ----------------------------*/

void initContext ( LXContext *ctx )
{
  /* INIT THE LX */
  memset(ctx, 0, sizeof(*ctx));
  strcpy(ctx->cLocale, "en");
  ctx->defaultRB = 0;
  ctx->ourCharsetName = "iso-8859-1";
  ctx->locales = NULL;
  ctx->curLocale = NULL;
  ctx->parLocale = NULL;
  ctx -> numLocales = 0;
  /* END INIT LX */
  ctx->OUT = NULL;
  ctx->fOUT = NULL;
}

void initLX()
{
  /* set the path for FSWF */
  {
    char newPath[500];
    strcpy(newPath, u_getDataDirectory());

    strcat(newPath, "locexp");

    FSWF_setBundlePath(newPath);
  }

}

void closeLX()
{
  FSWF_close();
  destroyLocaleTree(lx->locales);
}

void runLocaleExplorer(LXContext *myContext)
{
  UErrorCode status;
  char *tmp;
  UChar subTitle[1024];
  int32_t n,i;
  const char  *fileObj = NULL;
  char portStr[100];
  
  lx = myContext; /* FIXME when we are multithreaded */

#ifdef WIN32
  if( setmode( fileno ( stdout ), O_BINARY ) == -1 ) {
          perror ( "Cannot set stdout to binary mode" );
          exit(-1);
  }
#endif

  /* set up the port string */
  {
    const char *port;
    port = getenv("SERVER_PORT");

    if(port && strcmp(port,"80"))
    {
      portStr[0] = ':';
      strncpy(portStr+1,port,7);
      portStr[7]=0;
    }
  else
    {
      portStr[0] = 0;
    }
  }

  /* init ...... */
/*
  uloc_setDefault("sr_NZ_EURO", &status);
*/ /* BASELINE. Don't use a real locale here - will mess up the fallback error codes [for now] */


  uloc_setDefault("en_US_CALIFORNIA", &status);


#ifdef  WIN32
/*  u_setDataDirectory("c:\\o\\icu\\source\\data\\");
*/ /* ONLY IF you need to force the path .... */
#endif


  /** Below is useful for debugging. */
/*    fprintf(stderr, "PID=%d\n", getpid());  */
/*     system("sleep 20");   */

  status = U_ZERO_ERROR; 


  /* ------- END INIT ----------*/

  if((tmp=getenv("QUERY_STRING")) == NULL)
  {
        fprintf(stderr, "This program is designed to be run as a CGI-BIN.  QUERY_STRING is undefined.");
        exit(1);
  }


  /* Set up some initial values, just in case something goes wrong later. */
  strcpy(lx->chosenEncoding, "LATIN_1");
  lx->ourCharsetName = "iso-8859-1";




  lx->OUT = setLocaleAndEncodingAndOpenUFILE(lx->chosenEncoding, &lx->setLocale, &lx->setEncoding, &fileObj);

  if(fileObj != NULL)
  {
    writeFileObject( lx, fileObj );
    return;
  }


  if(!lx->OUT)
    doFatal("u_finit trying to open file", 0);
  
  lx->ourCharsetName = MIMECharsetName(lx->chosenEncoding); /* for some sanity */

  /** Setup the callbacks **/
  /* put our special error handler in */

  if(u_fgetConverter(lx->OUT) == NULL)
  {
    fprintf(stdout,"content-type: text/plain\r\n");
    fprintf(stdout,"\r\nFatal: can't open ICU Data (%s)\r\n", u_getDataDirectory());
    fprintf(stdout,"ICU_DATA=%s\n", getenv("ICU_DATA"));
    exit(0);

    lx->couldNotOpenEncoding = lx->chosenEncoding; /* uhoh */
  }
  else
  {
    ucnv_setFromUCallBack((UConverter*)u_fgetConverter(lx->OUT), 
                          UCNV_FROM_U_CALLBACK_BACKSLASH,
                          &lx->backslashCtx,
                          &lx->backslashCtx.subCallback,
                          &lx->backslashCtx.subContext,
                          &status);
    
    
    lx->backslashCtx.html = TRUE;
    
    ucnv_setFromUCallBack((UConverter*)u_fgetConverter(lx->OUT), 
                          UCNV_FROM_U_CALLBACK_DECOMPOSE,
                          &lx->decomposeCtx,
                          &lx->decomposeCtx.subCallback,
                          &lx->decomposeCtx.subContext,
                          &status);
    
    /* To do: install more cb's later. */
    
#if 0
    COLLECT_alnum = TRUE; /* only char about alphanumerics. */
    COLLECT_setSize(16);
    
    if(!strcmp(lx->chosenEncoding, "fonted"))
    {
      ucnv_setFromUCallBack((UConverter*)u_fgetConverter(lx->OUT), &UCNV_FROM_U_CALLBACK_FONTED, &status);
      FONTED_lastResortCallback   =  UCNV_FROM_U_CALLBACK_COLLECT;
      COLLECT_lastResortCallback =  UCNV_FROM_U_CALLBACK_DECOMPOSE;
    }
    else
#endif
      if(!strcmp(lx->chosenEncoding, "transliterated"))
        {
          lx->xlitCtx.html = FALSE;
          lx->xlitCtx.utf8 = NULL;
          ucnv_setFromUCallBack((UConverter*)u_fgetConverter(lx->OUT), 
                                UCNV_FROM_U_CALLBACK_TRANSLITERATED,
                                &lx->xlitCtx,
                                &lx->xlitCtx.subCallback,
                                &lx->xlitCtx.subContext,
                                &status);
        }
#if 0
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
  
    /* parse & sort the list of locales */
  setupLocaleTree();
  /* Open an RB in the default locale */
  lx->defaultRB = ures_open(NULL, lx->cLocale, &status);


  /* setup the time zone.. */
  if (!strncmp(tmp,"SETTZ=",6))
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
          fprintf(lx->fOUT,"Set-Cookie: TZ=%s;path=/;\r\n", junk);
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
  
/*  u_uastrcpy(lx->newZone, "Europe/Malta"); */
  u_uastrcpy(lx->newZone, "PST"); /* for now */


  if(lx->newZone[0] != 0x0000)
  {
      UTimeZone *tz;

      lx->timeZone = lx->newZone;

#ifndef LX_NO_USE_UTIMZONE
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

  /* Print the encoding and last HTTP header... */

  fprintf(lx->fOUT, "Content-Type: text/html;charset=%s\r\n\r\n", lx->ourCharsetName);
  fflush(lx->fOUT);

  /* 
     kore wa nandesuka?
     xi trid?

     
     {
    char langBuf[100];
    uloc_getLanguage(NULL,langBuf,100,&status);
  printf("Content-Language: %s\r\n", langBuf);
  }*/


  fflush(lx->fOUT); /* and that, as they say, is that.  All UFILE from here.. */


  /* -------------- */

  u_fprintf(lx->OUT,"<HTML>");

  u_fprintf(lx->OUT, "\r\n<!-- \r\n\r\n   Hello, HTML explorer :)  Don't know how readable this HTML will be!\r\n  If you have any questions, comments, [gasp] bugs, or\r\n [hopehope] improvements, please drop some knowledge to:\r\n    icu4c@us.ibm.com THX! \r\n                 ~srl \r\n\r\n-->");

  u_fprintf(lx->OUT, "<HEAD BGCOLOR=\"#DFDDDD\"><TITLE>");

  printPath(lx->curLocale, lx->curLocale, FALSE);

  if(strstr(getenv("QUERY_STRING"), "EXPLORE"))
  {
    lx->inDemo = TRUE;
    u_fprintf(lx->OUT, " &gt; %U", FSWF("exploreTitle", "Explore"));
  }
  else
  {
    lx->inDemo = FALSE;
  }

  u_fprintf(lx->OUT, "</TITLE>\r\n");

/*   if(!getenv("PATH_INFO") || !(getenv("PATH_INFO")[0])) */
  u_fprintf(lx->OUT, "<BASE HREF=\"http://%s%s%s/%s/%s/\">\r\n", getenv("SERVER_NAME"), portStr, getenv("SCRIPT_NAME"), lx->cLocale, lx->chosenEncoding); /* Ensure that all relative paths have the cgi name followed by a slash.  */
  
  u_fprintf(lx->OUT, "%U", 
	    FSWF ( /* NOEXTRACT */ "htmlHEAD",
		   "</HEAD>\r\n<BODY BGCOLOR=\"#FFFFFF\" > \r\n")
	    );

  /* now see what we're gonna do */
  tmp = getenv ( "QUERY_STRING" );

/*
  u_fprintf(lx->OUT, "COMP: %s %s<BR>\r\n", __DATE__,  __TIME__);
*/
  
  if(strstr(tmp,"EXPLORE"))
    {
      printHelpImg("display", 
		   FSWF("display_ALT", "Display Problems?"),
		   FSWF("display_GIF", "displayproblems.gif"),
		   FSWF("display_OPTIONS", "ALIGN=RIGHT"));
		  
      u_fprintf(lx->OUT, "<FONT SIZE=+1>");
      printPath(lx->curLocale, lx->curLocale, FALSE);
      u_fprintf(lx->OUT, "</FONT><P>");

    }
  else
    {
      if(tmp && tmp[0]  && !lx->curLocale && (tmp[0] == '_'))
	{
	  UChar dispName[1024];
	  UErrorCode stat = U_ZERO_ERROR;
	  dispName[0] = 0;
	  uloc_getDisplayName(lx->curLocaleName, NULL, dispName, 1024, &stat);
	  
	  u_fprintf(lx->OUT, "<UL><B>%U  [%U]</B></UL>\r\n",
		    FSWF("warningInheritedLocale", "Note: You're viewing a non existent locale. The ICU will support this with inherited information. But the Locale Explorer is not designed to understand such locales. Inheritance information may be wrong!"), dispName);
	}
      
      if(isExperimentalLocale(lx->curLocaleName) && tmp && tmp[0] )
	{
	  u_fprintf(lx->OUT, "<UL><B>%U</B></UL>\r\n",
		    FSWF("warningExperimentalLocale", "Note: You're viewing an experimental locale. This locale is not part of the official ICU installation! <FONT COLOR=red>Please do not file bugs against this locale.</FONT>") );
	}
      
      u_fprintf(lx->OUT, "<TABLE WIDTH=100%%><TR><TD ALIGN=LEFT VALIGN=TOP>");

      u_fprintf(lx->OUT, "<FONT SIZE=+1>");
      printPath(lx->curLocale, lx->curLocale, TRUE);
      u_fprintf(lx->OUT, "</FONT><P>");
      
      u_fprintf(lx->OUT, "</TD><TD ROWSPAN=2 ALIGN=RIGHT VALIGN=TOP WIDTH=1>");
      
      printHelpImg("display", 
		   FSWF("display_ALT", "Display Problems?"),
		   FSWF("display_GIF", "displayproblems.gif"),
		   FSWF("display_OPTIONS", "ALIGN=RIGHT"));
		  
      u_fprintf(lx->OUT, "\r\n</TD></TR><TR><TD>");

      if(lx->curLocale && lx->curLocale->nSubLocs)
	{
	  bool_t hadExperimentalLocales = FALSE;

	  u_fprintf(lx->OUT, "%U<BR><UL>", FSWF("sublocales", "Sublocales:"));

	  mySort(lx->curLocale, &status, FALSE); /* Sort sub locales */

	  for(n=0;n<lx->curLocale->nSubLocs;n++)
	    {
	      bool_t wasExperimental = FALSE;

	      if(n != 0)
		u_fprintf(lx->OUT, ", ");
	      u_fprintf(lx->OUT, "<A HREF=\"?_=%s\">", 
			lx->curLocale->subLocs[n].str);
		
	      if(isExperimentalLocale(lx->curLocale->subLocs[n].str))
		{
		  u_fprintf(lx->OUT, "<I><FONT COLOR=\"#9999FF\">");
		  hadExperimentalLocales = TRUE;
		  wasExperimental = TRUE;
		}
	      u_fprintf(lx->OUT, "%U",lx->curLocale->subLocs[n].ustr);
	      if(wasExperimental)
		{
		  u_fprintf(lx->OUT, "</FONT></I>");
		}
              u_fprintf(lx->OUT, "</A>");
	    }
	  
	  if(hadExperimentalLocales)
	    u_fprintf(lx->OUT, "<BR>%U", FSWF("locale_experimental", "Locales in <I>Italics</I> are experimental and not officially supported."));
	  
	  u_fprintf(lx->OUT, "</UL>");
	}
      u_fprintf(lx->OUT, "</TD></TR></TABLE>\r\n");


    }

  if ( tmp == NULL )
    tmp = ""; /* for sanity */

  if( ( (!*tmp)  /* && !lx->setLocale && !(lx->setEncoding)*/) 
      || strstr(tmp, "PANICDEFAULT")) /* They're coming in cold. Give them the spiel.. */
  {
    u_fprintf(lx->OUT, "</H2>"); /* close the 'title text */

	u_fprintf(lx->OUT, "<UL>%U<P>\r\n",
	  FSWF("introSpiel", "This demo illustrates the International Components for Unicode localization data.  The data covers over 45 different languages, further divided into 150+ geographic locales.  For each language, data such as days of the week, months, and their abbreviations are defined.  <P>ICU is an open-source project."));
        
        u_fprintf(lx->OUT, "%U<P>\r\n",
                  FSWF/**/(/**/"specialMessage_2000Oct30",/**/
                       "<I>Note: Locale Explorer should be much faster, but.. there's an ongoing problem where (at least) Microsoft Internet Explorer users will be faced with a blank page or an error page.. if this occurs, please simply hit Reload and all should be corrected.</I>"));

	u_fprintf(lx->OUT, "</UL>");
  }

  /* Logic here: */
  if( /* !lx->setLocale || */  !strncmp(tmp,"locale", 6))     /* ?locale  or not set: pick locale */
    {
      char *restored;

      restored = strchr(tmp, '&');
      if(restored)
	{
	  restored ++;
	}

      if(!restored)
	restored = "converter"; /* what to go on to */

      if(lx->setLocale)
	u_fprintf(lx->OUT, "<H4>%U</H4>\r\n", FSWF("changeLocale", "Change the Locale used for Labels"));
      else
	u_fprintf(lx->OUT, "<H4>%U</H4>\r\n", FSWF("chooseLocale", "Choose Your Locale."));

      u_fprintf(lx->OUT, "<TABLE WIDTH=\"70%\"><TR>");
      u_fprintf(lx->OUT, "<TD COLSPAN=2 ALIGN=RIGHT>");
      printHelpTag("chooseLocale", NULL);
      u_fprintf(lx->OUT, "</TD></TR></TABLE>\r\n");
      chooseLocale(tmp, FALSE, (char*)lx->cLocale, restored);
    }
  else if (!strncmp(tmp,"converter", 9))  /* ?converter */
    {
      char *restored;

      restored = strchr(tmp, '&');
      if(restored)
	  restored ++;

      /*
      if(lx->setEncoding)
	u_fprintf(lx->OUT, ": %U</H2>\r\n", FSWF("changeEncoding", "Change Your Encoding"));
      else
	u_fprintf(lx->OUT, ": %U</H2>\r\n", FSWF("chooseEncoding", "Choose Your Encoding"));
      */
      u_fprintf(lx->OUT, "<HR>");

      if(tmp[9] == '=')
	{
	  /* choose from encodings that match a string */
	  char *sample;
	  char *end;
	  UChar usample[256];
	  
	  sample = tmp + 10;
	  end    = strchr(sample, '&');
	  
	  if(end == NULL)
	    {
	      end = sample + strlen(sample);
	    }
	  
	  unescapeAndDecodeQueryField(usample, 256, sample);

	  *end = 0;
	  u_fprintf(lx->OUT, "%U: %s<P>\r\n", FSWF("converter_matchesTo", "Looking for matches to these chars: "), sample);
	  chooseConverterMatching(restored, usample);
	}
      else
	{
	  /* choose from all the converters */
	  chooseConverter(restored);
	}
    }
  else if (!strncmp(tmp,"SETTZ=",6))
  {
      /* lx->newZone is initted early, need it for cookies :) */
      if(u_strlen(lx->newZone))
      {
          UErrorCode status = U_ZERO_ERROR;
          u_fprintf(lx->OUT, "Got zone=%U<P>\n", lx->newZone);
          u_fprintf(lx->OUT, "Time there =%U\n", date(lx->newZone,UDAT_FULL,&status));
      }
      
      u_fprintf(lx->OUT, "%U: <FORM><INPUT NAME=\"SETTZ\" VALUE=\"%U\"><INPUT TYPE=SUBMIT></FORM>\r\n", 
                FSWF("zone_set", "Set timezone to:"),
                lx->newZone);
      u_fprintf(lx->OUT, "<UL><I>%U</I></UL>\r\n", 
                FSWF("zone_warn","Note: only works if you have cookies turned on."));

      {
          const char *cook;
          cook = getenv("HTTP_COOKIE");
          if(cook)
          {
              u_fprintf(lx->OUT, "<U>%s</U>\r\n", cook);
          }
      }
      
  }
  else
    {
      listBundles(tmp);
    }
  
  printStatusTable();
#if 0
  if(COLLECT_getChars()[0] != 0x0000)
    {
      UConverterFromUCallback oldCallback;
      UErrorCode status2 = U_ZERO_ERROR;
      
      oldCallback = ucnv_getFromUCallBack(((UConverter*)u_fgetConverter(lx->OUT)));

      u_fprintf(lx->OUT, "<TABLE WIDTH=100%% BORDER=1><TR><TD>%U<BR>", FSWF("encoding_Missing", "The following characters could not be displayed properly by the current encoding:"));
      
      ucnv_setFromUCallBack((UConverter*)u_fgetConverter(lx->OUT), &UCNV_FROM_U_CALLBACK_ESCAPE, &status2);
      
      u_fprintf(lx->OUT, "%U", COLLECT_getChars());
      
      ucnv_setFromUCallBack((UConverter*)u_fgetConverter(lx->OUT), oldCallback, &status2);

      u_fprintf(lx->OUT, "<BR><A HREF=\"?converter=");
      writeEscaped(COLLECT_getChars());
      
      if(strncmp(getenv("QUERY_STRING"), "converter",9))
	u_fprintf(lx->OUT,"&%s", getenv("QUERY_STRING"));
      u_fprintf(lx->OUT, "\">");
      u_fprintf(lx->OUT, "%U</A>\r\n",
		FSWF("encoding_PickABetter", "Click here to search for a better encoding"));

      u_fprintf(lx->OUT, "</TD></TR></TABLE>\r\n");
    }
#endif
  
  u_fprintf(lx->OUT, "%U", FSWF( /* NOEXTRACT */ "htmlTAIL", "<!-- No HTML footer -->"));
    
  /* a last resort. will switch to English if they get lost.. */
  /* DO NOT localize the following */
  /* What this does:  
         - brings them to the 'choose your locale' pane in English, then
	 - brings them to the 'choose your encoding' pane in their locale, then
	 - lists the locales to browse
  */


  if(!strcmp(lx->cLocale,"klingon"))
    u_fprintf(lx->OUT, "<P>Thank you for using the ICU LocaleExplorer, from %s compiled %s %s<P>\r\n", LXHOSTNAME, __DATE__, __TIME__);

  u_fprintf(lx->OUT, "</BODY></HTML>\r\n");

  fflush(lx->fOUT);

  u_fclose(lx->OUT);

  fflush(stderr);

  if(lx->defaultRB)
    ures_close(lx->defaultRB);


  return;
}

const UChar *defaultLanguageDisplayName()
{
  UErrorCode status = U_ZERO_ERROR;
  static UChar displayName[1024] = { 0x0000 };

  if(displayName[0] == 0x0000)
    {
      uloc_getDisplayLanguage(lx->cLocale, lx->cLocale ,displayName, 1024, &status);
    }
  
  return displayName;
}

/* snag the locale, followed optionally by the encoding, from the path_info -----------------
*/



void setupLocaleTree()
{
  const char *qs, *amp;
  char       *loc = lx->curLocaleName;

  /* setup base locale */
  lx->locales = createLocaleTree(lx->cLocale, &lx->numLocales);

  qs = getenv("QUERY_STRING");
  if(   qs &&
     (*(qs++) == '_') &&
     (*(qs++) == '='))
    {
      amp = strchr(qs,'&');
      if(!amp)
	amp = qs+strlen(qs);
      
      if((amp-qs)>100) /* safety */
	{
	  strncpy(loc,qs,100);
	  loc[100] = 0;
	}
      else
	{
	  strncpy(loc,qs,amp-qs);
	  loc[amp-qs] = 0;
	}
      
      /* setup cursors.. */
      lx->curLocale = findLocale(lx->locales, loc);

      if(lx->curLocale)
	lx->parLocale = lx->curLocale->parent;
    }
}
  
/* do a fatal error. may not have a content type yet. --------------------------------------- */
/* TODO: this doesn't actually work yet. Should it be localized ? probably. */
void doFatal(const char *what, UErrorCode err)
{
  fprintf(lx->fOUT, "Content-type:text/html\r\n\r\n");
  fprintf(lx->fOUT, "<TITLE>ICU LocaleExplorer: Error</TITLE>\r\n");
  fprintf(lx->fOUT, "<H1>ICU LocaleExplorer: Error</H1>\r\n");
  fprintf(lx->fOUT, "<UL>An error of type %d occured while trying to %s.</UL><HR><P>\r\n",err,what);
  fprintf(stderr, "listrb: err %d trying to %s\n",err,what);
  fprintf(lx->fOUT, "You can try <A HREF=\"%s\">starting over</A>, or complain to srl.<P>\r\n",
	 getenv("SCRIPT_NAME"));
  fflush(lx->fOUT);
  exit(0);
}

/* convert from unicode format with the pipe (|) character as the separator,
   to fully escaped ( '%5CuXXXX' ) format. */
char *createEscapedSortList(const UChar *source)
{
  int32_t i, inlen;
  int32_t len;
  int32_t maxlen;
  char *out, *p;

  inlen = u_strlen(source);

  len = 0;
  maxlen = (inlen * 6)+1;
  
  /* sputid imp. */
  out = malloc(maxlen);
  p = out;
  
  for(i=0;i<inlen;i++)
  {
    if(source[i] == '|')
      {
        strcpy(p, "\\u");
        p += 2;
        sprintf(p, "%04X", 0x000D);
        p += 4;
      }
    else if((source[i] == '\\') && (source[i+1] == 'u'))
      {
        int togo = 6; /* copy the next 6 */
        for(;(i<inlen) && (togo--);i++)
        {
          *p++ = source[i];
        }
        i--; 
      }
    else
      {
        strcpy(p, "\\u");
        p += 2;
        sprintf(p, "%04X", source[i]);
        p += 4;
      }
  }
  *p = 0; /* null */
  return out;
}
void writeEscaped(const UChar *s)
{
  UErrorCode status = U_ZERO_ERROR;

  lx->backslashCtx.html = FALSE;

  if(u_strchr(s, 0x00A0))
    {
      while(*s)
	{
	  if(*s == 0x00A0)
	    u_fprintf(lx->OUT, " ");
	  else
	    u_fprintf(lx->OUT, "%K", *s);
	  
	  s++;
	}
    }
  else
    u_fprintf(lx->OUT, "%U", s); 

  /* should 'get/restore' here. */
  lx->backslashCtx.html = TRUE;
}

void writeEscapedQuery(const UChar *s)
{
  UErrorCode status = U_ZERO_ERROR;

  lx->backslashCtx.html = FALSE;

  if(u_strchr(s, 0x00A0))
    {
      while(*s)
	{
	  if(*s == 0x00A0)
	    u_fprintf(lx->OUT, " ");
	  else
	    u_fprintf(lx->OUT, "%K", *s);
	  
	  s++;
	}
    }
  else
    u_fprintf(lx->OUT, "%U", s); 

  lx->backslashCtx.html = TRUE;
}


/* print that little box in the TR corner ----------------------------------------------------- */

void printStatusTable()
{
    UChar myChars[1024];
    UErrorCode status;
    
    status = U_ZERO_ERROR;
    
    u_fprintf(lx->OUT, "<P><TABLE BORDER=0 CELLSPACING=0 WIDTH=100%%>");
    u_fprintf(lx->OUT, "<TR><TD HEIGHT=5 BGCOLOR=\"#0F080F\" COLSPAN=3><IMG SRC=\"../_/c.gif\" ALT=\"---\" WIDTH=0 HEIGHT=0></TD></TR>\r\n");
    u_fprintf(lx->OUT, "  <TR>\r\n   <TD COLSPAN=3 WIDTH=0 VALIGN=TOP BGCOLOR=" kXKeyBGColor "><A NAME=%s><B>", "YourSettings");
    
    /* PrintHelpTag */
    u_fprintf(lx->OUT, "%U",   FSWF("statusTableHeader", "Your settings:"));

    if(lx->inDemo == FALSE)
    {
      u_fprintf(lx->OUT, "%U",   FSWF("statusTableHeaderChange", "(click to change)"));
    }

    /* /PrintHelpTag */
    u_fprintf(lx->OUT, "</TD>\r\n"
                   "  </TR>\r\n"
                   "  <TR>\r\n"
                   "   <TD>");
    u_fprintf(lx->OUT, "<B>%U</B></TD>\r\n", FSWF("myConverter", "Encoding:"));
  u_fprintf(lx->OUT, "   <TD>");
              /* now encoding */

  if(lx->inDemo == FALSE)
  {
    u_fprintf(lx->OUT, "<A HREF=\"?converter");
    if(strncmp(getenv("QUERY_STRING"), "converter",9))
      u_fprintf(lx->OUT,"&%s", getenv("QUERY_STRING"));
    u_fprintf(lx->OUT, "\">");
  }

  u_fprintf(lx->OUT, "<FONT SIZE=+1>%s</FONT>", lx->ourCharsetName);
  
  if(lx->inDemo == FALSE)
  {
    u_fprintf(lx->OUT, "</A>\r\n");
  }
  
  u_fprintf(lx->OUT, "</TD>\r\n");

  u_fprintf(lx->OUT, "<TD ALIGN=RIGHT ROWSPAN=3>\r\n"); /* ====== begin right hand thingy ======== */

  u_fprintf(lx->OUT, "<A HREF=\"http://oss.software.ibm.com/icu/\"><I>%U</I> %U</A><BR>",
	    FSWF("poweredby", "Powered by"),
	    FSWF( /* NODEFAULT */ "poweredby_vers", "ICU " U_ICU_VERSION) );

#ifdef LX_SET_TZ
  u_fprintf(lx->OUT, "<A HREF=\"?SETTZ=\">");
#endif
  u_fprintf(lx->OUT, "%U", date( NULL,UDAT_FULL,&status));
#ifdef LX_SET_TZ
  u_fprintf(lx->OUT, "</A>");
#endif
  u_fprintf(lx->OUT, "<BR>\r\n");

  if(lx->inDemo == FALSE)
  {
    u_fprintf(lx->OUT, "<A HREF=\"%s/en/iso-8859-1/?PANICDEFAULT\"><IMG SRC=\"../_/incorrect.gif\" ALT=\"Click here if text displays incorrectly\"></A>", getenv("SCRIPT_NAME"));
  }

  u_fprintf(lx->OUT, "</TD></TR>\r\n"); /* end little right hand thingy */

  /* === Begin line 4 - display locale == */
  u_fprintf(lx->OUT, "<TR><TD><B>%U</B></TD>\r\n", FSWF("myLocale", "Label Locale:"));

  u_fprintf(lx->OUT, "<TD>");
  
  if(lx->inDemo == FALSE)
  {
    u_fprintf(lx->OUT, "<A HREF=\"?locale");
    if(strncmp(getenv("QUERY_STRING"), "locale",6))
      u_fprintf(lx->OUT,"&%s", getenv("QUERY_STRING"));
    u_fprintf(lx->OUT, "\">");
  }
  uloc_getDisplayName(lx->cLocale, lx->cLocale, myChars, 1024, &status);
  u_fprintf(lx->OUT, "%U", myChars);
  if(lx->inDemo == FALSE)
  {
    u_fprintf(lx->OUT, "</A>");
  }
  u_fprintf(lx->OUT, "</TD></TR>\r\n");

  u_fprintf(lx->OUT, "<TR><TD><B>%U</B></TD>\r\n",
            FSWF("encoding_translit_setting", "Transliteration:"));

#if 0  /* Transliteration isn't working at present. */
  u_fprintf(lx->OUT, "<TD><I>%U</I></TD>",
            FSWF("off", "off"));
#else
  if(lx->inDemo == FALSE)
  {
    if(!strcmp(lx->chosenEncoding, "transliterated"))
      {
        const char *qs;
        qs = getenv("QUERY_STRING");
        if(qs == NULL)
          qs = "";
        
        u_fprintf(lx->OUT, "<TD><B>*%U*</B> / <A HREF=\"%s/%s/?%s\">%U</A></TD>",
                  FSWF("on","on"),
                  getenv("SCRIPT_NAME"),
                  lx->cLocale,
                  qs,
                  FSWF("off","off"));
      }
    else
      {
        const char *qs;
      qs = getenv("QUERY_STRING");
      if(qs == NULL)
        qs = "";
      
      u_fprintf(lx->OUT, "<TD><A HREF=\"%s/%s/transliterated/?%s\">%U</A> / <B>*%U*</B></TD>",
                getenv("SCRIPT_NAME"),
                lx->cLocale,
                qs,
                FSWF("on","on"),
                FSWF("off","off"));
      }
  }
  else
  { /* indemo */
    u_fprintf(lx->OUT, "<TD><B>%U</B></TD>", 
              (!strcmp(lx->chosenEncoding, "transliterated"))?
                FSWF("on","on") :
                FSWF("off","off"));
  }
#endif  
  
  u_fprintf(lx->OUT, "</TR>\r\n");

  if(!FSWF_getBundle())
    {
      /* No reason to use FSWF, this error means we have nothing to fetch strings from! */
      u_fprintf(lx->OUT, "<TR><TD COLSPAN=3><B><I>Note: Could not open our private resource bundle %s </I></B><P> - [%s]</TD></TR>\r\n",
		FSWF_bundlePath(), u_errorName(FSWF_bundleError()));
    }

  if(!isSupportedLocale(lx->cLocale, TRUE)) 
    {
      u_fprintf(lx->OUT, "<TD COLSPAN=3 ><FONT COLOR=\"#FF0000\">");
      u_fprintf_u(lx->OUT, FSWF("locale_unsupported", "This display locale, <U>%s</U>, is unsupported. [i think]"), lx->cLocale);
      u_fprintf(lx->OUT, "</FONT></TD>");
    }


    u_fprintf(lx->OUT, "<TR><TD HEIGHT=5 BGCOLOR=\"#AFA8AF\" COLSPAN=3><IMG SRC=\"../_/c.gif\" ALT=\"---\" WIDTH=0 HEIGHT=0></TD></TR>\r\n");

  u_fprintf(lx->OUT, "</TABLE>\r\n");

  u_fprintf(lx->OUT, "<CENTER>\r\n");

  printHelpTag("", FSWF("help", "Help"));
  
  u_fprintf(lx->OUT, " &nbsp; ");

  printHelpTag("transliteration", FSWF("transliterate_help", "Transliteration Help"));

  u_fprintf(lx->OUT, " &nbsp; ");

  u_fprintf(lx->OUT, "<A TARGET=\"_new\" HREF=\"http://oss.software.ibm.com/developerworks/opensource/icu/bugs\">%U</A>",
            FSWF("poweredby_filebug", "File a bug"));
  
  u_fprintf(lx->OUT, "</CENTER><P>\r\n");

  if(lx->couldNotOpenEncoding)
  {
    /* Localize this when it actually works! */
    u_fprintf(lx->OUT,"<TR><TD COLSPAN=2><FONT COLOR=\"#FF0000\">Warning, couldn't open the encoding '%s', using a default.</FONT></TD></TR>\r\n", lx->couldNotOpenEncoding); 
  }
}

void printPath(const MySortable *leaf, const MySortable *current, bool_t styled)
{

  if(!leaf) /* top level */
    {
      if(styled) 
	u_fprintf(lx->OUT, "<A HREF=\"?\">"); /* Reset */
      
      u_fprintf(lx->OUT, "%U",
		FSWF("title", "ICU LocaleExplorer"));
      
      if(styled) 
	u_fprintf(lx->OUT, "</A>");

      return;
    }

  
  /* reverse order recursively */
  printPath(leaf->parent,current,styled);


  u_fprintf(lx->OUT, " &gt; ");
  


  if(styled)
    {
      if(leaf == current)
	u_fprintf(lx->OUT, "<B>");

      u_fprintf(lx->OUT, "<A HREF=\"?_=%s\">", leaf->str);
    }

  
  
  u_fprintf(lx->OUT, "%U", leaf->ustr);



  if(styled)
    {
      u_fprintf(lx->OUT, "</A>");

      if(leaf == current)
	u_fprintf(lx->OUT, "</B>");

    }
}


static void printLocaleLink(bool_t toOpen, MySortable *l, const char *current, const char *restored, bool_t *hadUnsupportedLocales)
{
  bool_t supported;

  if ( toOpen == FALSE)
    {
      /* locales for LOCEXP TEXT */
      supported = isSupportedLocale(l->str, TRUE);
    }
  else
    {
      /* locales for VIEWING */
      supported = !isExperimentalLocale(l->str);
    }


  u_fprintf(lx->OUT, "<A HREF=\"");
  
  if(toOpen == TRUE)
    {
      u_fprintf(lx->OUT, "%s/%s/",
		getenv("SCRIPT_NAME"),
		(char*)lx->cLocale);
      if(lx->setEncoding)
	u_fprintf(lx->OUT,"%s/", lx->chosenEncoding);	  
	  u_fprintf(lx->OUT,"?_=%s&", l->str);
    }
  else
    {
      u_fprintf(lx->OUT, "%s/%s/",
		getenv("SCRIPT_NAME"),
		l->str);
      if(lx->setEncoding)
	u_fprintf(lx->OUT,"%s/", lx->chosenEncoding);
      
      if(restored)
	u_fprintf(lx->OUT, "?%s", restored);
    }
  
  
  u_fprintf(lx->OUT,"\">");

  if(!supported)
    {
      u_fprintf(lx->OUT, "<I><FONT COLOR=\"#9999FF\">");
      *hadUnsupportedLocales = TRUE;
    }

  if(toOpen == TRUE) 
  {
    u_fprintf(lx->OUT, "%U", l->ustr);
  }
  else
  {
    UErrorCode status = U_ZERO_ERROR;
    UChar displayName[1024] = { 0x0000 };
    
    uloc_getDisplayVariant(l->str,l->str ,displayName, 1024, &status);
    if(u_strlen(displayName)==0 || U_FAILURE(status)) {
      status = U_ZERO_ERROR;
      uloc_getDisplayCountry(l->str,l->str ,displayName, 1024, &status);
      if(u_strlen(displayName)==0 || U_FAILURE(status)) {
        status = U_ZERO_ERROR;
        uloc_getDisplayLanguage(l->str,l->str ,displayName, 1024, &status);
      }
    }
    
    u_fprintf(lx->OUT, "%U", displayName);
  }

  if(!supported)
    u_fprintf(lx->OUT, "</FONT></I>");
  
  u_fprintf(lx->OUT,"</A>");

  
}

static void printLocaleAndSubs(bool_t toOpen, MySortable *l, const char *current, const char *restored, bool_t *hadUnsupportedLocales)
{
  int32_t n;


  printLocaleLink(toOpen,l,current,restored, hadUnsupportedLocales);
  
  if(l->nSubLocs)
    {
	u_fprintf(lx->OUT, "&nbsp;[<FONT SIZE=-1>&nbsp;");

      for(n=0;n<(l->nSubLocs);n++)
	{
          if(n > 0)
            {
              u_fprintf(lx->OUT, ", ");
            }
	  printLocaleAndSubs(toOpen, &(l->subLocs[n]), current, restored, hadUnsupportedLocales);
	}
      
	u_fprintf(lx->OUT, "&nbsp;</FONT SIZE=-1>]");
    }
}


/* chooselocale --------------------------------------------------------------------------- */
void chooseLocale(const char *qs, bool_t toOpen, const char *current, const char *restored)
{
  UBool  hit = FALSE;
  UBool showAll = FALSE;
  int32_t n, j;
  UErrorCode status = U_ZERO_ERROR;
  bool_t  hadUnsupportedLocales = FALSE;

  if((toOpen == FALSE) && qs && strstr(qs, "SHOWall"))
  {
    showAll = TRUE;
  }

  u_fprintf(lx->OUT, "<TABLE BORDER=2 CELLPADDING=2 CELLSPACING=2>\r\n");

  u_fprintf(lx->OUT, "<TR><TD><B>%U</B></TD><TD><B>%U</B></TD></TR>\r\n",
	    FSWF("localeList_Locale", "Languages"),
	    FSWF("localeList_Sublocale", "Countries"));

  u_fprintf(lx->OUT, "<TR><TD COLSPAN=2><A HREF=\"?_=%s\">%U</A></TD></TR>\r\n",
	    lx->locales->str, lx->locales->ustr); /* default */ 

  mySort(lx->locales, &status, TRUE); /* need the whole thing sorted */

  for(n=0;n<lx->locales->nSubLocs;n++)
    {
      /* This will hide display locales that don't exist. */
      if((toOpen == FALSE) && !isSupportedLocale(lx->locales->subLocs[n].str, TRUE) && showAll == FALSE)
	continue;

      u_fprintf(lx->OUT, "<TR>\r\n");


      hit = !strcmp(lx->locales->subLocs[n].str,current);

      if(hit)
	{
	  u_fprintf(lx->OUT,"<TD VALIGN=TOP BGCOLOR=\"#FFDDDD\">");
	}
      else
	u_fprintf(lx->OUT,"<TD VALIGN=TOP>");

      if(hit)  
	u_fprintf(lx->OUT, "<b>");
      
      
      printLocaleLink(toOpen, &(lx->locales->subLocs[n]), current, restored, &hadUnsupportedLocales);

      if(hit)
	u_fprintf(lx->OUT, "</b>");
      
      u_fprintf(lx->OUT, "</FONT>");

      u_fprintf(lx->OUT, "</TD>");
      
      if(lx->locales->subLocs[n].nSubLocs)
	{
	  u_fprintf(lx->OUT, "<TD>");
	  
	  for(j=0;j< (lx->locales->subLocs[n].nSubLocs); j++)
	    {
#ifndef LX_SHOW_ALL_DISPLAY_LOCALES
	      /* This will hide display locales that don't exist. */
	      if((toOpen == FALSE) && !isSupportedLocale(lx->locales->subLocs[n].subLocs[j].str, TRUE))
		continue;
#endif

	      if(j>0)
		u_fprintf(lx->OUT, ", ");
	      
	      printLocaleAndSubs(toOpen, &(lx->locales->subLocs[n].subLocs[j]), current, restored, &hadUnsupportedLocales);
	      
	    }

	  u_fprintf(lx->OUT, "</TD>");
	}

      u_fprintf(lx->OUT, "</TR>\r\n");


    }

  u_fprintf(lx->OUT, "</TABLE>\r\n");

  if(hadUnsupportedLocales)
    u_fputs(FSWF("locale_experimental", "Locales in <I>Italics</I> are experimental and not officially supported."), lx->OUT);

  if(showAll == FALSE && toOpen == FALSE)
    {
      u_fprintf(lx->OUT, "<A HREF=\"?locale&SHOWall\">%U</A>\r\n<BR>",
                FSWF("showAll", "Show All"));
    }
}


/* chooseconverter ----------------------------------------------------------------------------- */

/* Choose from among all converters */
void chooseConverter(const char *restored)
{
  int32_t  ncnvs, naliases;
  int32_t i;
  int32_t j;

  USort *mysort;

  UErrorCode status = U_ZERO_ERROR;

  ncnvs = ucnv_countAvailable();

  mysort = usort_open(NULL, UCOL_DEFAULT_STRENGTH, TRUE, &status);
  
  if(U_FAILURE(status))
    {
      u_fprintf(lx->OUT, "%U<HR>\r\n", FSWF("convERR", "AN error occured trying to sort the converters.\n"));
      explainStatus_X( status, NULL);
      return;
    }

  for(i=0;i<ncnvs;i++)
    {
      const char *name;
      const char *alias;
      char        dispName[200];
      const char *number;
      UErrorCode  err = U_ZERO_ERROR;

      name = ucnv_getAvailableName(i);

      alias = ucnv_getStandardName(name, "MIME", &err);
      if( (alias == NULL) || U_FAILURE(err))
      {
        err = U_ZERO_ERROR;
        alias = ucnv_getStandardName(name, "MIME", &err);
        if( (alias == NULL) || U_FAILURE(err))
          {
            continue;

            err = U_ZERO_ERROR;
            alias = name;
          }
      }
      
      if(number = strstr(name, "ibm-"))
	number+= 4;      /* ibm-[949] */
      else
	number = name; /* '[fullnameofconverter]' */

#if 0
      
      for(j=0;U_SUCCESS(err);j++)
	{
	  alias = ucnv_getAlias(name, j, &err);

	  if(!alias)
	    break;
#endif
	  if(!strstr(alias, "ibm-") || !strstr(alias,name))
	    sprintf(dispName, "%s [%s]", alias, number);
	  else
	    strcpy(dispName, alias);

	  if(!strcmp(alias,"fonted"))
	    usort_addLine(mysort, uastrdup(dispName), -1, FALSE, (void*)"fonted");
	  else
	    usort_addLine(mysort, uastrdup(dispName), -1, FALSE, (void*)MIMECharsetName(name));
#if 0
	}

#endif
    }
  

  naliases = (mysort->count);
  u_fprintf_u(lx->OUT, FSWF("convsAvail","%d converters available, %d aliases total."), ncnvs, naliases);


  usort_sort(mysort);
  
  chooseConverterFrom(restored, mysort);

  usort_close(mysort);


}


/* Choose a converter which can properly convert the sample string. */
void chooseConverterMatching(const char *restored, UChar *sample)
{
  int32_t  ncnvs, naliases, nmatch = 0;
  int32_t i;
  int32_t j;
  int32_t sampleLen;

  USort *mysort;
  
  int8_t junkChars[1024];

  UErrorCode status = U_ZERO_ERROR;

  sampleLen = u_strlen(sample);

  /* A little bit of sanity. c'mon, FFFD is just the subchar.. */
  for(i=0;i<sampleLen;i++)
    {
      if( (sample[i] == 0xFFFD) ||  /* subchar */
          (sample[i] == 0x221E) ||  /* infinity */
          (sample[i] == 0x2030)     /* permille */
       ) 
	sample[i] = 0x0020;
    }
  
  ncnvs = ucnv_countAvailable();

  mysort = usort_open(NULL, UCOL_DEFAULT_STRENGTH, TRUE, &status);
  
  if(U_FAILURE(status))
    {
      u_fprintf(lx->OUT, "%U<HR>\r\n", FSWF("convERR", "AN error occured trying to sort the converters.\n"));
      return;
    }

  u_fprintf(lx->OUT, "%U<BR>\r\n",
	    FSWF("converter_searching", "Searching for converters which match .."));


  for(i=0;i<ncnvs;i++)
    {
      const char *name;
      const char *alias;
      char        dispName[200];
      const char *number;
      UErrorCode  err = U_ZERO_ERROR;

      name = ucnv_getAvailableName(i);

      if(testConverter(name, sample, sampleLen, junkChars, 1023) == FALSE)
	continue; /* Too bad.. */

      nmatch++;
      
      if(number = strstr(name, "ibm-"))
	number+= 4;      /* ibm-[949] */
      else
	number = name; /* '[fullnameofconverter]' */

      for(j=0;U_SUCCESS(err);j++)
	{
	  alias = ucnv_getAlias(name, j, &err);

	  if(!alias)
	    break;
	  
	  if(!strstr(alias, "ibm-") || !strstr(alias,name))
	    sprintf(dispName, "%s [%s]", alias, number);
	  else
	    strcpy(dispName, alias);

	  if(!strcmp(alias,"fonted"))
	    usort_addLine(mysort, uastrdup(dispName), -1, FALSE, (void*)"fonted");
	  else
	    usort_addLine(mysort, uastrdup(dispName), -1, FALSE, (void*)name);
	}

    }

  naliases = (mysort->count);
  u_fprintf_u(lx->OUT, FSWF("convsMatch","%d converters match (out of %d), %d aliases total."), nmatch,ncnvs, naliases);
  
  usort_sort(mysort);
  
  chooseConverterFrom(restored, mysort);

  usort_close(mysort);
}

/* Show a list of converters based on the USort passed in */
void chooseConverterFrom(const char *restored, USort *list)
{
  int32_t naliases, ncnvs;
  int32_t  i, j;
  int32_t  COLS = 4; /* number of columns */
  int32_t rows;
  const char *cnvMime, *defaultMime;

  UErrorCode status = U_ZERO_ERROR;

  defaultMime = MIMECharsetName(lx->chosenEncoding);
  
  ncnvs = ucnv_countAvailable();

  naliases = list->count;
  
 if(!restored)
	restored = "";

  u_fprintf(lx->OUT,"<A HREF=\"?%s\"><H2>%U%s%U</H2></A>\r\n",
	     restored,
	     FSWF("encodingOK0", "Click here if the encoding '"),
	     lx->chosenEncoding,
	     FSWF("encodingOK1", "' is acceptable, or choose one from below."));

  u_fprintf(lx->OUT,"<I>%U</I>\r\n", 
            FSWF("encoding_mime","Note (ICU 1.6 release): This list has been pared down to only show MIME aliases that browsers would understand. I'll add a 'show all' button later.<!--If you translate this, remember it'll go away soon -->"));
  rows = (naliases / COLS) + 1;

  u_fprintf(lx->OUT, "<P><TABLE cellpadding=3 cellspacing=2 >\r\n"
	         "<TR>\r\n");

  for(i=0;i<(rows * COLS);i++)
    {
      int32_t theCell;
      bool_t hit;
      const char *cnv = NULL; 
      

      u_fprintf(lx->OUT, "<!-- %d -->", i);

      theCell=(rows * (i%COLS)) + (i/COLS); 
      if(theCell >= naliases)
	{
	  u_fprintf(lx->OUT,"<td><!-- Overflow %d --></td>", theCell);
	  if(((i+1)%COLS) == 0)
	    u_fprintf(lx->OUT,"</TR>\n<TR>");
	  continue;
	}

      cnv = (const char*)list->lines[theCell].userData;

      if(cnv < (const char *)0x100)
	return;

      if(!cnv)
	continue;

      cnvMime = MIMECharsetName(cnv);

      hit = !strcmp(cnvMime, defaultMime);

      if(hit)
	u_fprintf(lx->OUT,"<TD BGCOLOR=\"#FFDDDD\">");
      else
	u_fprintf(lx->OUT,"<TD>");

      u_fprintf(lx->OUT, "<FONT SIZE=-1>");

      if(hit)  
	u_fprintf(lx->OUT, "<b>");
      
      u_fprintf(lx->OUT, "<A HREF=\"");


      u_fprintf(lx->OUT, "%s/%s/",
		getenv("SCRIPT_NAME"),
		lx->cLocale);
      u_fprintf(lx->OUT,"%s/", cnv);
      if(restored)
	u_fprintf(lx->OUT, "?%s", restored); 
      
      u_fprintf(lx->OUT,"\">");
      u_fprintf(lx->OUT, "%U", list->lines[theCell].chars);
      /*       theCnvale.getDisplayName(o.GetLocale(),tmp); */
      u_fprintf(lx->OUT,"</A>\n");
      
      if(hit)
	u_fprintf(lx->OUT, "</b>");
      
      u_fprintf(lx->OUT, "</FONT>");
      u_fprintf(lx->OUT, "</FONT>");
      u_fprintf(lx->OUT, "</TD>");
      if(((i+1)%COLS) == 0)
	u_fprintf(lx->OUT, "</TR>\n<TR>");
    }
  u_fprintf(lx->OUT,"</TABLE>\r\n");
 

  { /* Todo: localize */
    const char *ts = "??";
    UErrorCode status;
    char tmp[100];
    UConverter *u = u_fgetConverter(lx->OUT);
    const char *tmp2;
    

    status = U_ZERO_ERROR;

    u_fprintf(lx->OUT,"<HR>");
    u_fprintf(lx->OUT,"<H3>Information about <B><TT>%s</TT></B></H3>\r\n",
	      lx->chosenEncoding);
    u_fprintf(lx->OUT,"<UL>");
    
    u_fprintf(lx->OUT,"  <LI>ID = %d, platform=%s, name=%s\n",
	      ucnv_getCCSID(u,&status),
	      (ucnv_getPlatform(u,&status) == UCNV_IBM) ? "IBM" : "other",
              ucnv_getName(u, &status) );
    
    /* TODO: iterate over std names */
    status = U_ZERO_ERROR;
    tmp2 = ucnv_getStandardName(lx->chosenEncoding, "MIME", &status);
    if(tmp2 && U_SUCCESS(status)) {
      u_fprintf(lx->OUT, "  <LI>MIME: %s\n", tmp2);
    }

    status = U_ZERO_ERROR;
    tmp2 = ucnv_getStandardName(lx->chosenEncoding, "IANA", &status);
    if(tmp2 && U_SUCCESS(status)) {
      u_fprintf(lx->OUT, "  <LI>IANA: %s\n", tmp2);
    }
    status = U_ZERO_ERROR;
	      
    u_fprintf(lx->OUT,"  <LI>min/max chars: %d to %d\n",
	      ucnv_getMinCharSize(u),
	      ucnv_getMaxCharSize(u));

    u_fprintf(lx->OUT,"  <LI>Type=");
    switch(ucnv_getType(u))
      {
      case UCNV_UNSUPPORTED_CONVERTER:  ts = "Unsupported"; break;
      case UCNV_SBCS: ts = "Single Byte Character Set"; break;
      case UCNV_DBCS: ts = "Double Byte Character Set"; break;
      case UCNV_MBCS: ts = "Multiple Byte Character Set (variable)"; break;
      case UCNV_LATIN_1: ts = "Latin-1"; break;
      case UCNV_UTF8: ts = "UTF-8 (8 bit unicode)"; break;
      case UCNV_UTF7: ts = "UTF-7 (7 bit unicode transformation format)"; break;
      case UCNV_UTF16_BigEndian: ts = "UTF-16 Big Endian"; break;
      case UCNV_UTF16_LittleEndian: ts = "UTF-16 Little Endian"; break;
      case UCNV_EBCDIC_STATEFUL: ts = "EBCDIC Stateful"; break;
      case UCNV_ISO_2022: ts = "iso-2022 meta-converter"; break;
      case UCNV_LMBCS_1: ts="UCNV_LMBCS_1"; break;
      case UCNV_LMBCS_2: ts="UCNV_LMBCS_2"; break; 
      case UCNV_LMBCS_3: ts="UCNV_LMBCS_3"; break;		
      case UCNV_LMBCS_4: ts="UCNV_LMBCS_4"; break;
      case UCNV_LMBCS_5: ts="UCNV_LMBCS_5"; break;
      case UCNV_LMBCS_6: ts="UCNV_LMBCS_6"; break;
      case UCNV_LMBCS_8: ts="UCNV_LMBCS_8"; break;
      case UCNV_LMBCS_11: ts="UCNV_LMBCS_11"; break;
      case UCNV_LMBCS_16: ts="UCNV_LMBCS_16"; break;
      case UCNV_LMBCS_17: ts="UCNV_LMBCS_17"; break;
      case UCNV_LMBCS_18: ts="UCNV_LMBCS_18"; break;
      case UCNV_LMBCS_19: ts="UCNV_LMBCS_19"; break;

      case UCNV_HZ: ts = "HZ Encoding"; break;
      case UCNV_SCSU: ts = "Standard Compression Scheme for Unicode"; break; /* ? */
      case UCNV_US_ASCII: ts = "7-bit ASCII"; break; /* ? */

      default: ts = tmp; sprintf(tmp, "Unknown type %d", ucnv_getType(u));
      }
    u_fprintf(lx->OUT, "%s\n", ts);

#if defined(LX_UBROWSE_PATH)
    u_fprintf(lx->OUT, "<A TARGET=unibrowse HREF=\"%U/%s/\">%U</A>\r\n", getenv("SERVER_NAME"), getenv("SCRIPT_NAME"), 
	      FSWF( /* NOEXTRACT */ "ubrowse_path", LX_UBROWSE_PATH),
	      defaultMime,
	      FSWF("ubrowse", "Browse Unicode in this codepage"));
#endif
    
    u_fprintf(lx->OUT, "<LI>Aliases:<OL>\r\n");
    {
      int i;
      UErrorCode status = U_ZERO_ERROR;
      const char *name;
      const char *alias;

      name = ucnv_getName(u, &status);

      for(i=0;U_SUCCESS(status);i++)
	{
	  alias = ucnv_getAlias(name, i, &status);

	  if(!alias)
	    break;

	  u_fprintf(lx->OUT, "  <LI>%s\r\n", alias);
	}
    }
    u_fprintf(lx->OUT, "</OL>\r\n");


    u_fprintf(lx->OUT, "</UL>\r\n");
  }
  
	      
}

  /*  Main function for dumping the contents of a particular locale ---------------------------- */

void listBundles(char *b)
{
  char *tmp, *locale = NULL;
  UChar myChars[1024];
  UErrorCode status = U_ZERO_ERROR;
  UResourceBundle *myRB = NULL;
  bool_t doShowSort = FALSE;
  const char *qs;

  qs = b;

  if(*b == '_')
    {
      b++;
      
      if(*b == '=')
	{
	  
	  b++;
	  
	  tmp =strchr(b,'&');
	  if(tmp)
	    { 
	      *tmp = 0;
	    }
	  
	  locale = b;

	  if(tmp)
	    {
	      b = tmp;
	      b++;
	    }
	}
    }


  if(! locale )
    {
      chooseLocale(qs, TRUE, b, "");

      u_fprintf(lx->OUT, "<H3>%U</H3>\r\n<UL><LI>",
                FSWF("demos", "Demos"));

      showExploreLink(NULL, "g7",
                        FSWF("EXPLORE_CollationElements_g7sampleString","bad\\u000DBad\\u000DBat\\u000Dbat\\u000Db\\u00E4d\\u000DBzt\\u000DB\\u00E4d\\u000Db\\u00E4t\\u000DB\\u00E4t"),
                        "CollationElements");

      u_fprintf(lx->OUT, "%U</A>\r\n",
                FSWF("explore_G7", "Try Multi-lingual Sorting"));


#ifdef LX_HAVE_XLITOMATIC
      u_fprintf(lx->OUT, "<LI><A HREF=\"/II/xlitomatic/%s/%s/\">%U</A>\r\n",
                lx->cLocale, lx->chosenEncoding,
                FSWF("explore_xlitomatic", "Translit-o-matic"));
#endif
      u_fprintf(lx->OUT, "<P></UL>\r\n");

      return; /* BREAK out */
    }

  u_fprintf(lx->OUT, "<TABLE BORDER=0 CELLSPACING=2>\r\n");

  u_fprintf(lx->OUT, "<TR><TD COLSPAN=2>");

  u_fprintf(lx->OUT, "<TD ALIGN=left>");
  
  myRB = ures_open(NULL, locale, &status);


  if(U_FAILURE(status))
    {
      u_fprintf(lx->OUT,"</TR></TABLE><B>An error occured [%d] opening that resource bundle [%s]. Perhaps it doesn't exist? </B><P><HR>\r\n",status, locale);
      return;
    }

#if SRL_HRCAK
  /* check for aliasing */
  {
    UErrorCode qxc = U_ZERO_ERROR;
    int32_t aliasLen = 0;
    const UChar *alias;
    alias   = ures_getStringByKey(myRB, "%%ALIAS", &aliasLen, &qxc);

    if(aliasLen > 0) {
      u_fprintf(lx->OUT, "(%U <A HREF=\"_=%U\">%U</A>)<P>",
                  FSWF("aliasFor", "Really an alias for "),
                alias,
                alias);
    }
    else
      {
        const char *c;
        c = ures_getName(myRB);
        u_fprintf(lx->OUT, "[%s]\n", c?c:"NULL");
        c = ures_getLocale(myRB, &qxc);
        u_fprintf(lx->OUT, "[%s]\n", c?c:"NULL" );
      }

#if 0
  if(status != U_ZERO_ERROR /* && (!lx->parLocale) */ ) 
  {
    /* Don't have a parent locale, AND it's a fallback or somesuch.  Set the parent .. */
    lx->specialParLocale.nSubLocs = 0;
    lx->specialParLocale.subLocsSize = 0;
    lx->specialParLocale.str = ures_getLocale(myRB, &status);
    lx->specialParLocale.ustr = uastrdup(lx->specialParLocale.str);
    
    lx->parLocale = &lx->specialParLocale;
  }
#endif
  }
#endif  /* srl_HRCEK */

  explainStatus_X(status,"top");

  /*  showFlagImage(locale, " "); */
  /*   u_fprintf(lx->OUT, "</TD></TR><TR><TD COLSPAN=2>"); */

  /* analyze what kind of locale we've got.  Should this be a little smarter? */

#if 0
  u_fprintf(lx->OUT, "%U", FSWF("localeDataWhat", "This page shows the localization data for the locale listed at left. "));

  if(strlen(locale)==2) /* just the language */
    {
      u_fprintf(lx->OUT, " %U",FSWF("localeDataLanguage","No country is specified, so the data is generic to this language."));
    }
  else if(!strcmp(locale,"root"))
    {
      u_fprintf(lx->OUT, " %U", FSWF("localeDataDefault", "This is the default localization data, which will be used if no other installed locale is specified."));
    }
  else if(locale[2] == '_')
    {
      if(strlen(locale) == 5)
	{
	   u_fprintf(lx->OUT, " %U", FSWF("localeDataLangCountry", "This Locale contains data for this language, as well as conventions for this particular country."));
	 }
       else
	 {
	  u_fprintf(lx->OUT, " %U", FSWF("localeDataLangCountryVariant", "This Locale contains data for this language, as well as conventions for a variant within this particular country."));
	}
    }

  if(strstr(locale, "_EURO"))
    {
      u_fprintf(lx->OUT, " %U", FSWF("localeDataEuro", "This Locale contains currency formatting information for use with the Euro currency."));
    }
#endif
  u_fprintf(lx->OUT, "</TD></TR>\r\n");

  u_fprintf(lx->OUT, "</TABLE>");
  
  status = U_ZERO_ERROR;

  /* Show the explore.. things first. ======================*/
  if(strstr(b,"EXPLORE_DateTimePatterns"))
    {
      showExploreDateTimePatterns(myRB, locale, b);
    }

  else if (strstr(b, "EXPLORE_NumberPatterns"))
    {
      showExploreNumberPatterns(locale, b);
    }
  else if (strstr(b, "EXPLORE_Calendar"))
    {
      showExploreCalendar(lx, b);
    }
  else if (strstr(b, "EXPLORE_CollationElements"))
    {
      showKeyAndStartItem("EXPLORE_CollationElements", 
			  FSWF("EXPLORE_CollationElements", "Collation (sorting) Example"),
			  locale,
                          FALSE,
			  U_ZERO_ERROR);

#if 0
      u_fprintf(lx->OUT, "<A HREF=\"?_=%s#usort\"><IMG border=0 width=16 height=16 SRC=\"../_/c.gif\" ALT=\"\"> %U</A>\r\n",
		locale,
		FSWF("usortHide", "Hide this example"));
#endif
      
      u_fprintf(lx->OUT, "%U<P>", FSWF("usortWhat","This example demonstrates sorting (collation) in this locale."));
      showSort(locale, b);
      
      u_fprintf(lx->OUT, "</TD>");

      u_fprintf(lx->OUT, "<TD VALIGN=TOP ALIGN=RIGHT>");
      printHelpTag("EXPLORE_CollationElements", NULL);
      u_fprintf(lx->OUT, "</TD>");

#if 0
      if(!strstr(usort))
	{
	  u_fprintf(lx->OUT, "<A HREF=\"?_=%s&usort&_#usort\"><IMG border=0 width=16 height=16 SRC=\"../_/closed.gif\" ALT=\"\"> %U</A>\r\n",
		    locale,
		    FSWF("usortShow", "Show this example"));
	  
	  
	  u_fprintf(lx->OUT, "<BR>%U</TD>",
		    FSWF("usortWhat","This example demonstrates sorting (collation) in this locale."));
	}
#endif

      showKeyAndEndItem("EXPLORE_CollationElements", locale);
    }
  else /* ================================= Normal ShowXXXXX calls ===== */
    {
      showLocaleCodes(myRB, locale);
      
      /* Now, a list of the things to show. */
      /*   showString(myRB, "Souffle"); */
      /* Let's organize these. */
      /* DATE/TIME */
      showShortLong(myRB, locale, "Day", 
		    FSWF("DayAbbreviations", "Short Names"),
		    FSWF("DayNames", "Long Names"),7);
      showShortLong(myRB, locale, "Month",
		    FSWF("MonthAbbreviations", "Short Names"),
		    FSWF("MonthNames", "Long Names"), 12);

      {
	const UChar *ampmDesc[3];
	ampmDesc[0] = FSWF("AmPmMarkers0", "am");
	ampmDesc[1] = FSWF("AmPmMarkers1", "pm");
	ampmDesc[2] = 0;
    
	showArrayWithDescription(myRB, locale, ampmDesc, "AmPmMarkers");
      }


      {
	const UChar *currDesc[4];
	currDesc[0] = FSWF("CurrencyElements0", "Currency symbol");
	currDesc[1] = FSWF("CurrencyElements1", "Int'l Currency symbol");
	currDesc[2] = FSWF("CurrencyElements2", "Currency separator");
	currDesc[3] = 0;
    
	showArrayWithDescription(myRB, locale, currDesc, "CurrencyElements");
      }


      {
	const UChar *dtpDesc[10]; /* flms */
	dtpDesc[0] = FSWF("DateTimePatterns0", "Full Time");
	dtpDesc[1] = FSWF("DateTimePatterns1", "Long Time");
	dtpDesc[2] = FSWF("DateTimePatterns2", "Medium Time");
	dtpDesc[3] = FSWF("DateTimePatterns3", "Short Time");
	dtpDesc[4] = FSWF("DateTimePatterns4", "Full Date");
	dtpDesc[5] = FSWF("DateTimePatterns5", "Long Date");
	dtpDesc[6] = FSWF("DateTimePatterns6", "Medium Date");
	dtpDesc[7] = FSWF("DateTimePatterns7", "Short Date");
	dtpDesc[8] = FSWF("DateTimePatterns8", "Date-Time pattern.<BR>{0} = time, {1} = date");
	dtpDesc[9] = 0;

	showArrayWithDescription(myRB, locale, dtpDesc, "DateTimePatterns");
      }

      showArray(myRB, locale, "Eras");


      { /*from dcfmtsym */
	const UChar *numDesc[12];
	numDesc[0] = FSWF("NumberElements0", "Decimal Separator");
	numDesc[1] = FSWF("NumberElements1", "Grouping Separator");
	numDesc[2] = FSWF("NumberElements2", "Pattern Separator");
	numDesc[3] = FSWF("NumberElements3", "Percent");
	numDesc[4] = FSWF("NumberElements4", "ZeroDigit");
	numDesc[5] = FSWF("NumberElements5", "Digit");
	numDesc[6] = FSWF("NumberElements6", "Minus Sign");
	numDesc[7] = FSWF("NumberElements7", "Exponential");
	numDesc[8] = FSWF("NumberElements8", "PerMill [/1000]");
	numDesc[9] = FSWF("NumberElements9", "Infinity");
	numDesc[10] = FSWF("NumberElements10", "Not a number");
	numDesc[11] = 0;
	showArrayWithDescription(myRB, locale, numDesc, "NumberElements");
      }


      { /*from dcfmtsym */
	const UChar *numDesc[5];
	numDesc[0] = FSWF("NumberPatterns0", "Decimal Pattern");
	numDesc[1] = FSWF("NumberPatterns1", "Currency Pattern");
	numDesc[2] = FSWF("NumberPatterns2", "Percent Pattern");
	numDesc[3] = FSWF("NumberPatterns3", "Scientific Pattern");
	numDesc[4] = 0;

	showArrayWithDescription(myRB, locale, numDesc, "NumberPatterns");
      }

      showDateTimeElements(myRB, locale);

      { 
	const UChar *zsDesc[8];
	zsDesc[0] = FSWF("zoneStrings0", "Canonical");
	zsDesc[1] = FSWF("zoneStrings1", "Normal Name");
	zsDesc[2] = FSWF("zoneStrings2", "Normal Abbrev");
	zsDesc[3] = FSWF("zoneStrings3", "Summer/DST Name");
	zsDesc[4] = FSWF("zoneStrings4", "Summer/DST Abbrev");
	zsDesc[5] = FSWF("zoneStrings5", "Example City");
#ifndef LX_NO_USE_UTIMZONE
	zsDesc[6] = FSWF("zoneStrings6", "Raw Offset");
#else
	zsDesc[6] = 0;
#endif
	zsDesc[7] = 0;

	show2dArrayWithDescription(myRB, locale, zsDesc, b, "zoneStrings");
      }
      /*
	'NOT USED IN ICU'
	show2dArray(myRB, locale, b, "LocaleNamePatterns");
      */


      /* locale pattern chars */
      {
	const UChar *charDescs[19];

	charDescs[0] = FSWF("localPatternChars0", "Era");
	charDescs[1] = FSWF("localPatternChars1", "Year");
	charDescs[2] = FSWF("localPatternChars2", "Month");
	charDescs[3] = FSWF("localPatternChars3", "Date");
	charDescs[4] = FSWF("localPatternChars4", "Hour Of Day 1");
	charDescs[5] = FSWF("localPatternChars5", "Hour Of Day 0"); 
	charDescs[6] = FSWF("localPatternChars6", "Minute");
	charDescs[7] = FSWF("localPatternChars7", "Second");
	charDescs[8] = FSWF("localPatternChars8", "Millisecond");
	charDescs[9] = FSWF("localPatternChars9", "Day Of Week");
	charDescs[10] = FSWF("localPatternChars10", "Day Of Year");
	charDescs[11] = FSWF("localPatternChars11", "Day Of Week In Month");
	charDescs[12] = FSWF("localPatternChars12", "Week Of Year");
	charDescs[13] = FSWF("localPatternChars13", "Week Of Month");
	charDescs[14] = FSWF("localPatternChars14", "Am/Pm");
	charDescs[15] = FSWF("localPatternChars15", "Hour 1");
	charDescs[16] = FSWF("localPatternChars16", "Hour 0");
	charDescs[17] = FSWF("localPatternChars17", "Timezone");
	charDescs[18] = 0;
    
	showStringWithDescription(myRB, locale, b, charDescs, "localPatternChars", TRUE);
      }

      /* COLLATION */
      showCollationElements(myRB, locale, b, "CollationElements");

      /* LOCALE */
      showString(myRB, locale, b, "LocaleID");

      /*
	'NOT USED IN THE ICU'

	showString(myRB, locale, b, "LocaleString");
      */

      showTaggedArray(myRB, locale, b, "Languages");
      showTaggedArray(myRB, locale, b, "Countries"); 

  /* GENERAL */
      showString(myRB, locale, b, "Version");
  
      /* Todo:
	 "%%B"    /    "%%NY"     - NO variants. ???

	 "%%EURO"   -- ?
     
      */     

    }
  /* have to have this at the end */
  /*  u_fprintf(lx->OUT, kItemTrailer); */


  /*   u_fprintf(lx->OUT, "</TABLE>\r\n"); */
  
  ures_close(myRB);

}


/* Show a resource that's a collation rule list -----------------------------------------------------*/
/**
 * Show a string.  Make it progressive disclosure if it exceeds some length !
 * @param rb the resourcebundle to pull junk out of 
 * @param locale the name of the locale (for URL generation)
 * @param queryString the querystring of the request.
 * @param key the key we're listing
 */
#define UCA_LEN 110000
void showCollationElements( UResourceBundle *rb, const char *locale, const char *queryString, const char *key )
{
  
  UErrorCode status = U_ZERO_ERROR;
  const UChar *s  = 0;
  UChar temp[UCA_LEN]={'\0'};
      UChar *scopy = 0;
        UChar *comps = 0;
  bool_t bigString = FALSE; /* is it big? */
  bool_t userRequested = FALSE; /* Did the user request this string? */
  const char *tmp1, *tmp2;
  int32_t len = 0, len2, i;
  UConverterFromUCallback oldCallback;
      UCollator *coll = NULL;


  s = ures_getStringByKey(rb, key, &len, &status);

  if(!s || !s[0] || (status == U_MISSING_RESOURCE_ERROR))
    {

      /* Special case this to fetch what REALLY happens in this case ! */
      status = U_ZERO_ERROR;

      coll = ucol_open(locale, &status);


      if(U_SUCCESS(status))
	{
/*
            UChar tmp[1400];
            int nn;
*/
          if(strcmp(locale,"root")==0){
            len = ucol_getRulesEx(coll,UCOL_FULL_RULES, temp,UCA_LEN);
            s=temp;
          }
          else{
              s = ucol_getRules(coll,&len);
          }        

#if 0
          memcpy(tmp,s,len*sizeof(UChar));
          tmp[1399] = 0;
          u_fprintf(lx->OUT, "<TR><TD>roolz=%d -  %p<P><U>", len, s);
          for(nn=0;nn<1400;nn++)
          {
              if(u_isprint(s[nn]))
              {
                  u_fprintf(lx->OUT, "%K", s[nn]);
              }
              else
                  u_fprintf(lx->OUT, "\\u%04X", (int)s[nn]);
          }
          u_fprintf(lx->OUT, "</U></TD></TR>\n");
#endif
	}


      /* fix the status */
      if(!strcmp(locale, "root"))
	status = U_ZERO_ERROR;
      else
	status = U_USING_DEFAULT_ERROR;
    }
  else
    len = u_strlen(s);

  len2 = len;

  scopy = malloc(len * sizeof(UChar));
  memcpy(scopy,s,len*sizeof(UChar));
  for(i=0;i<len;i++)
  {
      if(scopy[i] == 0x0000)
      {
          scopy[i] = 0x0020; /* normalizer seems to croak on nulls */
      }
  }
  s = scopy;

  if(U_SUCCESS(status) && ( len > kShowStringCutoffSize ) )
    {
      bigString = TRUE;
      userRequested = didUserAskForKey(key, queryString);
    }

  showKeyAndStartItemShort(key, NULL, locale, FALSE, status);

  u_fprintf(lx->OUT, "&nbsp;</TD></TR><TR><TD></TD><TD>");
  
#if 1
  /* Ripped off from ArrayWithDescription. COPY BACK */
  {
      const UChar *sampleString, *sampleString2;
      char *sampleChars;
      UResourceBundle *sampleRB;
      UErrorCode sampleStatus = U_ZERO_ERROR;
      int32_t len;

      /* samplestring will vary with label locale! */
      sampleString =  FSWF(/*NOEXTRACT*/"EXPLORE_CollationElements_sampleString","bad|Bad|Bat|bat|b\\u00E4d|B\\u00E4d|b\\u00E4t|B\\u00E4t|c\\u00f4t\\u00e9|cot\\u00e9|c\\u00f4te|cote");

      sampleRB = ures_open(FSWF_bundlePath(), locale, &sampleStatus);
      if(U_SUCCESS(sampleStatus))
      {
          sampleString2 = ures_getStringByKey(sampleRB, "EXPLORE_CollationElements_sampleString", &len, &sampleStatus);
          ures_close(sampleRB);
      }

      if(U_FAILURE(sampleStatus))
      {
          sampleString2 = sampleString; /* fallback */
      }

      sampleChars = createEscapedSortList(sampleString2);
      showExploreButtonSort(rb,locale, sampleChars, "CollationElements");
      free(sampleChars);

  }
#endif

  u_fprintf(lx->OUT, "</TD>"); /* Now, we're done with the ShowKey.. cell */



  u_fprintf(lx->OUT, "</TR><TR><TD COLSPAN=2>");


  if(U_SUCCESS(status))
    {

      if(bigString && !userRequested) /* it's hidden. */
	{
	  /* WIERD!! outputting '&#' through UTF8 seems to be -> '?' or something 
	     [duh, HTML numbered escape sequence] */
	  u_fprintf(lx->OUT, "<A HREF=\"?_=%s&SHOW%s&_#%s\"><IMG BORDER=0 WIDTH=16 HEIGHT=16 SRC=\"../_/closed.gif\" ALT=\"\">%U</A>\r\n<P>", locale, key,key, FSWF("bigStringClickToShow","(Omitted due to size. Click here to show.)"));
	  u_fprintf(lx->OUT, "</TD><TD></TD>");
	}
      else
	{
	  if(bigString)
	    {
	      u_fprintf(lx->OUT, "<A HREF=\"?_=%s#%s\"><IMG border=0 width=16 height=16 SRC=\"../_/opened.gif\" ALT=\"\"> %U</A><P>\r\n",
			locale,
			key,
			FSWF("bigStringHide", "Hide"));
	    }
	  
	  if(U_SUCCESS(status))
	    {

	      comps = malloc(sizeof(UChar) * (len*3));
              
              {
                  for(i=0;i<(len*3);i++)
                  {
                      comps[i] = 0x0610;
                  }
              }

	      len = u_normalize(s,
			  len,
			  UCOL_DECOMP_COMPAT_COMP_CAN,
				/*UCOL_DECOMP_CAN_COMP_COMPAT, */
			  0,
			  comps,
			  len*3,
			  &status);

              
/*              u_fprintf(lx->OUT, "xlit: %d to %d<P>\n",
                        len2,len); */
              if(U_FAILURE(status))
              {
                  u_fprintf(lx->OUT, "xlit failed -} %s<P>\n",
                            u_errorName(status));
                  comps = (UChar*)s;
                  len = len2;
              }

	      /* DO NOT collect chars from the normalization rules.
		 Sorry, but they contain decomposed chars which mess up the codepage selection.. */
/*
  oldCallback = ucnv_getFromUCallBack((UConverter*)u_fgetConverter(lx->OUT));
  if(oldCallback != UCNV_FROM_U_CALLBACK_TRANSLITERATED)
  {
  ucnv_setFromUCallBack((UConverter*)u_fgetConverter(lx->OUT), COLLECT_lastResortCallback, &status);
  }
*/
	      
	      if(*comps == 0)
		{
		  u_fprintf(lx->OUT, "<I>%U</I>", FSWF("empty", "(Empty)"));
		}
	      else while(len--)
		{
		  
		  if(*comps == '&')
		    u_fprintf(lx->OUT, "<P>");
		  else if(*comps == '<')
		    u_fprintf(lx->OUT, "<BR>&nbsp;");
		  
		  if((*comps == 0x000A) || u_isprint(*comps))
		    u_fprintf(lx->OUT, "%K", *comps);
		  else
		    u_fprintf(lx->OUT, "<B>\\u%04X</B>", *comps); /* to emulate the callback */
		  
		  comps++;
		};

/* **	      ucnv_setFromUCallBack((UConverter*)u_fgetConverter(lx->OUT), oldCallback, &status); */

	    }
	  else
	    explainStatus_X(status, key);
	}
    }
  u_fprintf(lx->OUT, "</TD>");
  
  free(scopy);
  if(coll) ucol_close(coll);
  
  showKeyAndEndItem(key, locale);
}


void showLocaleCodes( UResourceBundle *rb, const char *locale)
{
  
  UErrorCode status = U_ZERO_ERROR;
  bool_t bigString = FALSE; /* is it big? */
  bool_t userRequested = FALSE; /* Did the user request this string? */
  const char *tmp1, *tmp2;
  char tempchar[1000];
  int32_t len;

  UErrorCode countStatus = U_ZERO_ERROR,langStatus = U_ZERO_ERROR;
  const UChar *count3 = 0, *lang3 = 0;

  count3 = ures_getStringByKey(rb, "ShortCountry", &len, &countStatus);
  lang3 = ures_getStringByKey(rb, "ShortLanguage", &len, &langStatus);


  showKeyAndStartItem("LocaleCodes", FSWF("LocaleCodes", "Locale Codes"), locale, FALSE, status);

  u_fprintf(lx->OUT, "<TABLE BORDER=1><TR><TD></TD><TD><B>%U</B></TD><TD><B>%U</B></TD><TD><B>%U</B></TD></TR>\r\n",
	    FSWF("LocaleCodes_Language", "Language"),
	    FSWF("LocaleCodes_Country", "Country"),
	    FSWF("LocaleCodes_Variant", "Variant"));
  u_fprintf(lx->OUT, "<TR><TD>%U</TD><TD>",
	    FSWF("LocaleCodes_2", "2"));
  
  status = U_ZERO_ERROR;
  uloc_getLanguage(locale, tempchar, 1000, &status);
  if(U_SUCCESS(status))
    u_fprintf(lx->OUT, tempchar);
  else
    explainStatus_X(status, "LocaleCodes");
  
  u_fprintf(lx->OUT, "</TD><TD>");
  
  status = U_ZERO_ERROR;
  uloc_getCountry(locale, tempchar, 1000, &status);
  if(U_SUCCESS(status))
    u_fprintf(lx->OUT, tempchar);
  else
    explainStatus_X(status, "LocaleCodes");
  
  u_fprintf(lx->OUT, "</TD><TD>");
  
  status = U_ZERO_ERROR;
  uloc_getVariant(locale, tempchar, 1000, &status);
  if(U_SUCCESS(status))
    u_fprintf(lx->OUT, tempchar);
  else
    explainStatus_X(status, "LocaleCodes");

  u_fprintf(lx->OUT, "</TD></TR>\r\n");

  /* 3 letter line */

  u_fprintf(lx->OUT, "<TR><TD>%U</TD>",
	    FSWF("LocaleCodes_3", "3"));

  u_fprintf(lx->OUT, "<TD>");

  if(U_SUCCESS(langStatus))
    {
      u_fprintf(lx->OUT, "%U", lang3);
      if(langStatus != U_ZERO_ERROR)
	{
	  u_fprintf(lx->OUT, "<BR>");
	}
    }

  explainStatus_X(langStatus, "LocaleCodes");

  u_fprintf(lx->OUT, "</TD><TD>");

  if(U_SUCCESS(countStatus))
    {
      u_fprintf(lx->OUT, "%U", count3);
      if(countStatus != U_ZERO_ERROR)
	{
	  u_fprintf(lx->OUT, "<BR>");
	}
    }

  explainStatus_X(countStatus, "LocaleCodes");

  u_fprintf(lx->OUT, "</TD><TD></TD></TR>\r\n");
  
  u_fprintf(lx->OUT, "</TABLE>\r\n");


  u_fprintf(lx->OUT, "</TD>");
  showKeyAndEndItem("LocaleCodes", locale);
}


/* Show a resource that's a simple string -----------------------------------------------------*/
/**
 * Show a string.  Make it progressive disclosure if it exceeds some length !
 * @param rb the resourcebundle to pull junk out of 
 * @param locale the name of the locale (for URL generation)
 * @param queryString the querystring of the request.
 * @param key the key we're listing
 */

void showString( UResourceBundle *rb, const char *locale, const char *queryString, const char *key )
{
  
  UErrorCode status = U_ZERO_ERROR;
  const UChar *s  = 0;
  bool_t bigString = FALSE; /* is it big? */
  bool_t userRequested = FALSE; /* Did the user request this string? */
  const char *tmp1, *tmp2;
  int32_t len;

  s = ures_getStringByKey(rb, key, &len, &status);

  if(U_SUCCESS(status) && ( u_strlen(s) > kShowStringCutoffSize ) )
    {
      bigString = TRUE;
      userRequested = didUserAskForKey(key, queryString);
    }

  showKeyAndStartItem(key, NULL, locale, FALSE, status);


  if(U_SUCCESS(status))
    {

      if(bigString && !userRequested) /* it's hidden. */
	{
	  /* WIERD!! outputting '&#' through UTF8 seems to be -> '?' or something 
	     [duh, HTML numbered escape sequence] */
	  u_fprintf(lx->OUT, "<A HREF=\"?_=%s&SHOW%s&_#%s\"><IMG BORDER=0 WIDTH=16 HEIGHT=16 SRC=\"../_/closed.gif\" ALT=\"\">%U</A>\r\n<P>", locale, key,key, FSWF("bigStringClickToShow","(Omitted due to size. Click here to show.)"));
	}
      else
	{
	  if(bigString)
	    {
	      u_fprintf(lx->OUT, "<A HREF=\"?_=%s#%s\"><IMG border=0 width=16 height=16 SRC=\"../_/opened.gif\" ALT=\"\"> %U</A><P>\r\n",
			locale,
			key,
			FSWF("bigStringHide", "Hide"));
	    }
	  
	  if(U_SUCCESS(status))
	    {
	      if(*s == 0)
		u_fprintf(lx->OUT, "<I>%U</I>", FSWF("empty", "(Empty)"));
	      else
		u_fprintf(lx->OUT, "%U ", s);
	    }
	}
    }
  u_fprintf(lx->OUT, "</TD>");
  showKeyAndEndItem(key, locale);
}

/* Show a resource that's a simple string, but explain each character.-----------------------------------------------------*/
/**
 * Show a string.  Make it progressive disclosure if it exceeds some length !
 * @param rb the resourcebundle to pull junk out of 
 * @param locale the name of the locale (for URL generation)
 * @param queryString the querystring of the request.
 * @param desc array (0 at last item) of char desc
 * @param key the key we're listing
 */

void showStringWithDescription(UResourceBundle *rb, const char *locale, const char *qs, const UChar *desc[], const char *key, bool_t hidable)
{
  
  UErrorCode status = U_ZERO_ERROR;
  const UChar *s  = 0;
  bool_t bigString = FALSE; /* is it big? */
  bool_t userRequested = FALSE; /* Did the user request this string? */
  const char *tmp1, *tmp2;
  int32_t i;
  int32_t len;

  s = ures_getStringByKey(rb, key, &len, &status);

  /* we'll assume it's always big, for now. */
  bigString = TRUE;
  userRequested = didUserAskForKey(key, qs);

  showKeyAndStartItem(key, NULL, locale, FALSE, status);

  /** DON'T show the string as a string. */
  /* 
     if(U_SUCCESS(status) && s)
     u_fprintf(lx->OUT, "%U<BR>\r\n", s);
  */
  if(!hidable)
    {
      userRequested = TRUE;
      bigString = FALSE;
    }
  

  if(bigString && !userRequested) /* it's hidden. */
    {
      /* WIERD!! outputting '&#' through UTF8 seems to be -> '?' or something */
	u_fprintf(lx->OUT, "<A HREF=\"?_=%s&SHOW%s&_#%s\"><IMG BORDER=0 WIDTH=16 HEIGHT=16 SRC=\"../_/closed.gif\" ALT=\"\">%U</A>\r\n<P>", locale, key,key, FSWF("stringClickToShow","(Click here to show.)"));
	u_fprintf(lx->OUT, "<P>");
    }
  else
    {
      if(bigString)
	{
	  u_fprintf(lx->OUT, "<A HREF=\"?_=%s#%s\"><IMG border=0 width=16 height=16 SRC=\"../_/opened.gif\" ALT=\"\"> %U</A><P>\r\n",
		    locale,
		    key,
		    FSWF("bigStringHide", "Hide"));
	}
  
      if(U_SUCCESS(status))
	{
	  u_fprintf(lx->OUT, "<TABLE BORDER=1 WIDTH=100%>");
	  u_fprintf(lx->OUT, "<TR><TD><B>%U</B></TD><TD><B>%U</B></TD><TD><B>%U</B></TD></TR>\r\n",
		    FSWF("charNum", "#"),
		    FSWF("char", "Char"),
		    FSWF("charMeaning", "Meaning"));
	  
	  
	  for(i=0;desc[i];i++)
	    {
	      if(!s[i])
		break;
	      
	      u_fprintf(lx->OUT, "<TR><TD WIDTH=5>%d</TD><TD>%K</TD><TD>%U</TD></TR>\r\n",
		       i,
			s[i],
		       desc[i]);
	    }
	  u_fprintf(lx->OUT, "</TABLE>\r\n");
	}
    }
  u_fprintf(lx->OUT, "</TD>");
  showKeyAndEndItem(key, locale);
}
  
/* Show a resource that's an array. Useful for types we haven't written viewers for yet --------*/

void showArray( UResourceBundle *rb, const char *locale, const char *key )
{
  UErrorCode status = U_ZERO_ERROR;
  UErrorCode firstStatus = U_ZERO_ERROR;
  UResourceBundle  *array = NULL, *item = NULL;
  int32_t len;
  const UChar *s  = 0;
  int i;

  array = ures_getByKey(rb, key, array, &firstStatus);
  item = ures_getByIndex(array, 0, item, &firstStatus);
//  msg   = ures_getString(item, &rsrclen, &firstStatus); /* dont care about this str */

  showKeyAndStartItem(key, NULL, locale, FALSE, firstStatus);

  u_fprintf(lx->OUT, "<OL>\r\n");

  for(i=0;;i++)
    {
      status = U_ZERO_ERROR;
      if(U_FAILURE(firstStatus)) {
        status = firstStatus; /* ** todo: clean up err handling! */
      }

      item = ures_getByIndex(array, i, item, &status);
      s  = ures_getString(item, &len, &status);

      if(!s)
	break;

      if(U_SUCCESS(status))
	u_fprintf(lx->OUT, "<LI> %U\r\n", s);
      else
	{
	  u_fprintf(lx->OUT, "<LI>");
	  explainStatus_X(status, key);
	  u_fprintf(lx->OUT, "\r\n");
	  break;
	}

    }
  u_fprintf(lx->OUT, "</OL></TD>");
  showKeyAndEndItem(key, locale);

  ures_close(item);
  ures_close(array);
}


/* Show a resource that's an array, wiht an explanation ------------------------------- */

void showArrayWithDescription( UResourceBundle *rb, const char *locale, const UChar *desc[], const char *key )
{
  UErrorCode status = U_ZERO_ERROR;
  UErrorCode firstStatus;
  UChar *s  = 0, *toShow =0;
  UChar nothing[] = {(UChar)0x00B3, (UChar)0x0000};
  UResourceBundle  *array = NULL, *item = NULL;
  int32_t len;

  enum { kNoExample = 0, kDateTimeExample, kNumberExample } exampleType;
  int32_t i;
  UDate now;  /* example date */
  double d = 1234.567;   /* example number */
  UDateFormat   *exampleDF = 0;
  UNumberFormat *exampleNF = 0;
  UErrorCode exampleStatus = U_ZERO_ERROR;
  UChar tempChars[1024];
  UChar tempDate[1024]; /* for Date-Time */
  UChar tempTime[1024]; /* for Date-Time */

  /* figure out what example to use */
  if(!strcmp(key,"DateTimePatterns"))
    exampleType = kDateTimeExample;
  else if(!strcmp(key, "NumberPatterns"))
    exampleType = kNumberExample;
  else
    exampleType = kNoExample;

  /* store the date now..just in case formatting takes multiple seconds! */
  now = ucal_getNow();

  firstStatus = U_ZERO_ERROR;
  array = ures_getByKey(rb, key, array, &firstStatus);
  item = ures_getByIndex(array, 0, item, &firstStatus);
  s = ures_getString(item, &len, &firstStatus);
//  s = ures_getArrayItem(rb, key, 0, &firstStatus);
  showKeyAndStartItemShort(key, NULL, locale, FALSE, firstStatus);

  if(exampleType != kNoExample)
    {
      toShow = nothing+1;

      exampleStatus = U_ZERO_ERROR;

      switch(exampleType)
	{

	case kDateTimeExample:
	  exampleStatus = U_ZERO_ERROR;
	  exampleDF = udat_openPattern(s,-1,locale,&exampleStatus);
	  if(U_SUCCESS(exampleStatus))
	    {
	      len = udat_toPattern(exampleDF, TRUE, tempChars, 1024,&exampleStatus);
	      if(U_SUCCESS(exampleStatus))
		{
		  toShow = tempChars;
		}
	    }
	  break;
	  
	case kNumberExample:

      toShow = nothing;

	  exampleNF = unum_openPattern(s,-1,locale,&exampleStatus);
	  if(U_SUCCESS(exampleStatus))
	    {
	      len = unum_toPattern(exampleNF, TRUE, tempChars, 1024, &exampleStatus);
	      if(U_SUCCESS(exampleStatus))
		{
		  toShow = tempChars;
		}
	      unum_close(exampleNF);
	    }
	  break;
	}
      exampleStatus = U_ZERO_ERROR;
      showExploreButton(rb, locale, toShow, key);
    }
  else
    {
        u_fprintf(lx->OUT, "&nbsp;");
    }


#ifdef LX_USE_CURRENCY
  /* Currency Converter link */
  if(!strcmp(key, "CurrencyElements"))
    {
      UErrorCode curStatus = U_ZERO_ERROR;
      UChar *curStr = NULL, *homeStr = NULL;

      /* index [1] is the int'l currency symbol */
      item = ures_getByIndex(array, 1, item, &curStatus);
      curStr  = ures_getString(item, &len, &curStatus);
      if(lx->defaultRB)
      {
        item = ures_getByKey(lx->defaultRB, key, item, &curStatus);
        item = ures_getByIndex(item, 1, item, &curStatus);
        curStr  = ures_getString(item, &len, &curStatus);

//	homeStr = ures_getArrayItem(lx->defaultRB, key, 1, &curStatus);
      }
      else
	homeStr = (const UChar[]){0x0000};
      
      /* OANDA doesn't quite follow the same conventions for currency.  

	 TODO:

 	  RUR->RUB
	  ...
      */

      
      u_fprintf(lx->OUT, "<FORM TARGET=\"_currency\" METHOD=\"POST\" ACTION=\"http:www.oanda.com/converter/travel\" ENCTYPE=\"x-www-form-encoded\"><INPUT TYPE=\"hidden\" NAME=\"result\" VALUE=\"1\"><INPUT TYPE=\"hidden\" NAME=\"lang\" VALUE=\"%s\"><INPUT TYPE=\"hidden\" NAME=\"date_fmt\" VALUE=\"us\"><INPUT NAME=\"exch\" TYPE=HIDDEN VALUE=\"%U\"><INPUT TYPE=HIDDEN NAME=\"expr\" VALUE=\"%U\">",
		"en", /* lx->cLocale */
		curStr,
		homeStr
		);

      u_fprintf(lx->OUT, "<INPUT TYPE=IMAGE WIDTH=48 HEIGHT=20 BORDER=0 SRC=\"../_/explore.gif\"  ALIGN=RIGHT   ");
      u_fprintf(lx->OUT, " VALUE=\"%U\"></FORM>",
	    FSWF("exploreTitle", "Explore"));
      u_fprintf(lx->OUT, "</FORM>");
    }
#else
#endif
  u_fprintf(lx->OUT, "</TD>"); /* Now, we're done with the ShowKey.. cell */


  u_fprintf(lx->OUT, "</TR><TR><TD COLSPAN=2><TABLE BORDER=2 WIDTH=\"100%\" HEIGHT=\"100%\">\r\n");

  for(i=0;desc[i];i++)
    {
      
      u_fprintf(lx->OUT, "<TR><TD WIDTH=5>%d</TD><TD>%U</TD><TD>",
		i, desc[i]);

      status = U_ZERO_ERROR;
      exampleStatus = U_ZERO_ERROR;

      item = ures_getByIndex(array, i, item, &status);
      s =    ures_getString(item, &len, &status);

      if(i==0)
	firstStatus = status;

      
      if(U_SUCCESS(status) && s)
	{
	  toShow = s;

	  switch(exampleType)
	    {
	    case kDateTimeExample: /* localize pattern.. */
	      if(i < 8)
		{
		  len = 0;

		  exampleDF = udat_openPattern(s,-1,locale,&exampleStatus);
		  if(U_SUCCESS(exampleStatus))
		    {
		      len = udat_toPattern(exampleDF, TRUE, tempChars, 1024,&exampleStatus);

		      if(U_SUCCESS(exampleStatus))
			{
			  toShow = tempChars;
			}
		    }	   
		}
	      break;

	    case kNumberExample:
	      if(i == 3) /* scientific */
		d = 1234567890;

	      exampleNF = unum_openPattern(s,-1,locale,&exampleStatus);
	      if(U_SUCCESS(exampleStatus))
		{
		  len = unum_toPattern(exampleNF, TRUE, tempChars, 1024, &exampleStatus);
		  if(U_SUCCESS(exampleStatus))
		    {
		      toShow = tempChars;
		    }
		}
	      break;

	      
	    default:
                ;
	    }
	  
	  u_fprintf(lx->OUT, "%U\r\n", toShow);
	}
      else
	{
	  s = 0;
	  explainStatus_X(status, key);
	  u_fprintf(lx->OUT, "\r\n");
	  break;
	}
      u_fprintf(lx->OUT, "</TD>");
      
      if(s) /* only if pattern exists */
      switch(exampleType)
	{
	case kDateTimeExample:
	  if(i < 8)
	  {
	    u_fprintf(lx->OUT, "<TD>");

	    if(U_SUCCESS(exampleStatus))
	      {
		exampleStatus = U_ZERO_ERROR; /* clear fallback info from exampleDF */
		udat_format(exampleDF, now, tempChars, 1024, NULL, &exampleStatus);
		udat_close(exampleDF);
		
		if(U_SUCCESS(exampleStatus))
		  u_fprintf(lx->OUT, "%U", tempChars);

	      }
	    explainStatus_X(exampleStatus, key);
	    u_fprintf(lx->OUT, "</TD>\r\n");

	    if(i == 3) /* short time */
	      u_strcpy(tempTime, tempChars);
	    else if(i == 7) /* short date */
	      u_strcpy(tempDate, tempChars);
	  }
	  else
	  {
	    u_fprintf(lx->OUT, "<TD>");
	    exampleStatus = U_ZERO_ERROR;
	    if(s)
	      if(u_formatMessage(locale, s, -1, tempChars,1024,&exampleStatus, 
				 tempTime,
				 tempDate))
		u_fprintf(lx->OUT,"%U", tempChars);
	    u_fprintf(lx->OUT, "</TD>\r\n");
	  }
	  break;

	case kNumberExample:
	  {
	    u_fprintf(lx->OUT, "<TD>");

	    if(U_SUCCESS(exampleStatus))
	      {
		exampleStatus = U_ZERO_ERROR; /* clear fallback info from exampleDF */

 		if(i == 3) /* scientific */
		  d = 1234567890;
		unum_formatDouble(exampleNF, d, tempChars, 1024, NULL, &exampleStatus);
		
		if(U_SUCCESS(exampleStatus))
		  u_fprintf(lx->OUT, "%U", tempChars);

		
		u_fprintf(lx->OUT, "</TD><TD>");

 		if(i == 3) /* scientific */
		  d = 0.00000000000000000005;

		unum_formatDouble(exampleNF, -d, tempChars, 1024, NULL, &exampleStatus);
		
		if(U_SUCCESS(exampleStatus))
		  u_fprintf(lx->OUT, "%U", tempChars);

		unum_close(exampleNF);

	      }
	    explainStatus_X(exampleStatus, key);
	    u_fprintf(lx->OUT, "</TD>\r\n");
	  }
	  break;

	case kNoExample:
	default:
	  break;
	}

      u_fprintf(lx->OUT, "</TR>\r\n");

    }
  

  u_fprintf(lx->OUT, "</TABLE></TD>");

  showKeyAndEndItem(key, locale);
  ures_close(item);
  ures_close(array);
}


/* show the DateTimeElements string *------------------------------------------------------*/

void showDateTimeElements( UResourceBundle *rb, const char *locale)
{
  UErrorCode status = U_ZERO_ERROR;
  UErrorCode firstStatus;
  const UChar *s  = 0;
  int32_t    len;
  UResourceBundle *array = NULL, *item = NULL;

  const char *key = "DateTimeElements";
  int i;
  /*
    0: first day of the week 
    1: minimaldaysinfirstweek 
  */

  status = U_ZERO_ERROR;

  array = ures_getByKey(rb, key, array, &status);
  item  = ures_getByIndex(array, 0, item, &status);
  s     = ures_getString(item, &len, &status);

  showKeyAndStartItem(key, FSWF("DateTimeElements","Date and Time Options"), locale, FALSE, status);

  /* First day of the week ================= */
  u_fprintf(lx->OUT, "%U ", FSWF("DateTimeElements0", "First day of the week: "));
  

  if(U_SUCCESS(status))
    {
      UChar myWkday[100];
      int32_t  firstDay;

      firstDay = (((s[0] & 0x000F)+6)%7); /* REVISIT: parse */
      
      u_fprintf(lx->OUT, " %U \r\n", s);
      /* here's something fun: try to fetch that day from the user's current locale */
      status = U_ZERO_ERROR;
      
      if(lx->defaultRB && U_SUCCESS(status))
	{
          /* don't use 'array' here because it's the DTE resource */
          item = ures_getByKey(lx->defaultRB, "DayNames", item, &status);
          item = ures_getByIndex(item, firstDay, item, &status);
          s    = ures_getString(item, &len, &status);
            
	  if(s && U_SUCCESS(status))
	    {
	      u_fprintf(lx->OUT, " = %U \r\n", s);
	    }
	  status = U_ZERO_ERROR;

          item = ures_getByKey(rb, "DayNames", item, &status);
          item = ures_getByIndex(item, firstDay, item, &status);
          s    = ures_getString(item, &len, &status);

	  if(s && U_SUCCESS(status))
	    {
	      u_fprintf(lx->OUT, " = %U \r\n", s);
	    }


	}
      status = U_ZERO_ERROR;
    }
  else
    {
      explainStatus_X(status, key);
      u_fprintf(lx->OUT, "\r\n");
    }


  u_fprintf(lx->OUT, "<BR>\r\n");

  /* minimal days in week ================= */
  u_fprintf(lx->OUT, "%U", FSWF("DateTimeElements1", "Minimal Days in First Week: "));
  
  status = U_ZERO_ERROR;

  item = ures_getByIndex(array, 1, item, &status);
  s = ures_getString(item, &len, &status);

  firstStatus = status;
  
  if(U_SUCCESS(status))
    u_fprintf(lx->OUT, " %U \r\n", s);
  else
    {
      explainStatus_X(status, key);
      u_fprintf(lx->OUT, "\r\n");
    }

  u_fprintf(lx->OUT, "</TD>");

  showKeyAndEndItem(key, locale);
  ures_close(array);
  ures_close(item);
}

/* Show a resource that has a short (*Abbreviations) and long (*Names) version ---------------- */
/* modified showArray */
void showShortLong( UResourceBundle *rb, const char *locale, const char *keyStem, const UChar *shortName, const UChar *longName, int32_t num )
{
  UErrorCode status = U_ZERO_ERROR;
  UErrorCode shortStatus = U_ZERO_ERROR, longStatus = U_ZERO_ERROR;
  char       shortKey[100], longKey[100];
  UResourceBundle  *shortArray = NULL, *longArray = NULL, *item = NULL;
  int32_t len;
  const UChar *s  = 0;
  int i;

  showKeyAndStartItem(keyStem, NULL, locale, FALSE, U_ZERO_ERROR); /* No status possible  because we have two items */

  sprintf(shortKey, "%sNames", keyStem);
  sprintf(longKey,  "%sAbbreviations", keyStem);

  /* pre load the status of these things */
  shortArray = ures_getByKey(rb, shortKey, shortArray, &shortStatus);
  longArray  = ures_getByKey(rb, longKey, longArray, &longStatus);
  item       = ures_getByIndex(shortArray, 0, item, &shortStatus);
  item       = ures_getByIndex(longArray, 0, item, &longStatus);

  u_fprintf(lx->OUT, "<TABLE BORDER=1 WIDTH=100%% HEIGHT=100%%><TR><TD><B>#</B></TD><TD><B>%U</B> ", shortName);
  explainStatus_X(shortStatus, keyStem);
  u_fprintf(lx->OUT, "</TD><TD><B>%U</B> ", longName);
  explainStatus_X(longStatus, keyStem);
  u_fprintf(lx->OUT, "</TD></TR>\r\n");

 
  for(i=0;i<num;i++)
    {
      char *key;

      u_fprintf(lx->OUT, " <TR><TD>%d</TD><TD>", i);

      /* get the normal name */
      status = U_ZERO_ERROR;
      key = longKey;
      item = ures_getByIndex(longArray, i, item, &status);
      s    = ures_getString(item, &len, &status);

      if(i==0)
	longStatus = status;
  
      if(U_SUCCESS(status))
	u_fprintf(lx->OUT, " %U ", s);
      else
	explainStatus_X(status, keyStem); /* if there was an error */

      u_fprintf(lx->OUT, "</TD><TD>");

      /* get the short name */
      status = U_ZERO_ERROR;
      key = shortKey;
      item = ures_getByIndex(shortArray, i, item, &status);
      s    = ures_getString(item, &len, &status);

      if(i==0)
	shortStatus = status;
  
      if(U_SUCCESS(status))
	u_fprintf(lx->OUT, " %U ", s);
      else
	explainStatus_X(status, keyStem); /* if there was an error */

      u_fprintf(lx->OUT, "</TD></TR>");
    }

  u_fprintf(lx->OUT, "</TABLE>");
  u_fprintf(lx->OUT, "</TD>");

  showKeyAndEndItem(keyStem, locale);
  ures_close(item);
  ures_close(shortArray);
  ures_close(longArray);

}

/* Show a 2d array  -------------------------------------------------------------------*/

void show2dArrayWithDescription( UResourceBundle *rb, const char *locale, const UChar *desc[], const char *queryString, const char *key )
{
  UErrorCode status = U_ZERO_ERROR;
  UErrorCode firstStatus;
  const UChar *s  = 0;
  int32_t h,v;
  int32_t rows,cols;
  bool_t bigString = FALSE; /* is it big? */
  bool_t userRequested = FALSE; /* Did the user request this string? */
  const char *tmp1, *tmp2;
  bool_t isTZ = FALSE; /* do special TZ processing */
  int32_t len;

  UResourceBundle *array = ures_getByKey(rb, key, NULL, &status);
  UResourceBundle *row   = ures_getByIndex(array, 0, NULL, &status);
  UResourceBundle *item = NULL;

  rows = ures_getSize(array);
  cols = ures_getSize(row);

#ifndef LX_NO_USE_UTIMZONE
  isTZ = !strcmp(key, "zoneStrings");
  if(isTZ)
    cols = 7;
#endif

  if(U_SUCCESS(status) && ((rows > kShow2dArrayRowCutoff) || (cols > kShow2dArrayColCutoff)) )
    {
      bigString = TRUE;
      userRequested = didUserAskForKey(key, queryString);
    }

  showKeyAndStartItem(key, NULL, locale, TRUE, status);


  if(bigString && !userRequested) /* it's hidden. */
    {
      /* WIERD!! outputting '&#' through UTF8 seems to be -> '?' or something */
	u_fprintf(lx->OUT, "<A HREF=\"?_=%s&SHOW%s&_#%s\"><IMG BORDER=0 WIDTH=16 HEIGHT=16 SRC=\"../_/closed.gif\" ALT=\"\">%U</A>\r\n<P>", locale, key,key, FSWF("bigStringClickToShow","(Omitted due to size. Click here to show.)"));
    }
  else
    {
      if(bigString)
	{
	  u_fprintf(lx->OUT, "<A HREF=\"?_=%s#%s\"><IMG border=0 width=16 height=16 SRC=\"../_/opened.gif\" ALT=\"\"> %U</A><P>\r\n",
		    locale,
		    key,
		    FSWF("bigStringHide", "Hide"));
	}

      firstStatus = status;  /* save this for the next column.. */

      if(U_SUCCESS(status))
      {	


	  u_fprintf(lx->OUT,"<TABLE BORDER=1>\r\n");
	  
	  /* print the top row */
	  u_fprintf(lx->OUT,"<TR><TD></TD>");
	  for(h=0;h<cols;h++)
	  {
	      if(!desc[h])
		break;

              u_fprintf(lx->OUT, "<TD><B>");
              if(h == 0)
              {
                  u_fprintf(lx->OUT, "<A TARGET=lx_tz HREF=\"http://oss.software.ibm.com/cvs/icu/~checkout~/icu/docs/tz.htm?content-type=text/html\">");
              }
	      u_fprintf(lx->OUT,"%U", desc[h]);
              if(h == 0)
              {
                  u_fprintf(lx->OUT, "</A>");
              }
              u_fprintf(lx->OUT, "</B></TD>\r\n");
          }
	  u_fprintf(lx->OUT,"</TR>\r\n");
	  
	  for(v=0;v<rows;v++)
	  {
	      const UChar *zn = NULL;
	      
              row   = ures_getByIndex(array, v, row, &status);

              if(U_FAILURE(status)) {
                u_fprintf(lx->OUT, "<TR><TD><B>ERR: ");
                explainStatus(lx, status, NULL);
                status = U_ZERO_ERROR;
                continue;
              }

	      u_fprintf(lx->OUT,"<TR><TD><B>%d</B></TD>", v);
	      for(h=0;h<cols;h++)
	      {
		  status = U_ZERO_ERROR;

		  
#ifndef LX_NO_USE_UTIMZONE
		  if(isTZ && (h == 6))
		    {
		      UTimeZone *zone = utz_open(zn);

		      s = NULL;

		      if(zone == NULL)
			s = FSWF("zoneStrings_open_failed", "<I>Unknown</I>");
		      else
			{
			  s = utz_hackyGetDisplayName(zone);
			  utz_close(zone); /* s will be NULL, so nothing will get printed below. */
			}
		    }
		  else
#endif
		    {
                      item   = ures_getByIndex(row, h, item, &status);
		      s = ures_getString(item, &len, &status);
		    }

		  if(isTZ && (h == 0)) /* save off zone for later use */
		    zn = s;
		  
		  /*      if((h == 0) && (v==0))
			  firstStatus = status; */ /* Don't need to track firstStatus, countArrayItems should do that for us. */
		  
		  if(U_SUCCESS(status) && s)
		    u_fprintf(lx->OUT, "<TD>%U</TD>\r\n", s);
		  else
		    {
		      u_fprintf(lx->OUT, "<TD BGCOLOR=" kStatusBG " VALIGN=TOP>");
		      explainStatus_X(status, key);
		      u_fprintf(lx->OUT, "</TD>\r\n");
		      break;
		    }
		}
	      u_fprintf(lx->OUT, "</TR>\r\n");
	    }
	  u_fprintf(lx->OUT, "</TABLE>\r\n<BR>");
	}
    }

  ures_close(item);
  ures_close(row);
  ures_close(array);
  showKeyAndEndItem(key, locale);
}

/* Show a Tagged Array  -------------------------------------------------------------------*/

void showTaggedArray( UResourceBundle *rb, const char *locale, const char *queryString, const char *key )
{
  UErrorCode status = U_ZERO_ERROR;
  UErrorCode firstStatus;
  const UChar *s  = 0;
  UChar displayName[1024];
  int32_t v;
  int32_t rows;
  bool_t bigString = FALSE; /* is it big? */
  bool_t userRequested = FALSE; /* Did the user request this string? */
  const char *tmp1, *tmp2;
  int32_t len;
  UResourceBundle *item = NULL;

  rows = ures_countArrayItems(rb, key, &status);

  if(U_SUCCESS(status) && ((rows > kShow2dArrayRowCutoff)))
    {
      bigString = TRUE;
      userRequested = didUserAskForKey(key, queryString);
    }

  showKeyAndStartItem(key, NULL, locale, TRUE, status);

  if(bigString && !userRequested) /* it's hidden. */
    {
      /* WIERD!! outputting '&#' through UTF8 seems to be -> '?' or something */
	u_fprintf(lx->OUT, "<A HREF=\"?_=%s&SHOW%s&_#%s\"><IMG BORDER=0 WIDTH=16 HEIGHT=16 SRC=\"../_/closed.gif\" ALT=\"\">%U</A>\r\n<P>", locale, key,key, FSWF("bigStringClickToShow","(Omitted due to size. Click here to show.)"));
    }
  else
    {
      if(bigString)
	{
	  u_fprintf(lx->OUT, "<A HREF=\"?_=%s#%s\"><IMG border=0 width=16 height=16 SRC=\"../_/opened.gif\" ALT=\"\"> %U</A><P>\r\n",
		    locale,
		    key,
		    FSWF("bigStringHide", "Hide"));
	}

      firstStatus = status;  /* save this for the next column.. */

      if(U_SUCCESS(status))
	{	
      UResourceBundle *tagged =  ures_getByKey(rb, key, NULL, &status);
      UResourceBundle *defaultTagged = NULL;
      UResourceBundle *taggedItem = NULL;
      if(lx->defaultRB)
        defaultTagged =  ures_getByKey(lx->defaultRB, key, NULL, &status);

	  

	  u_fprintf(lx->OUT,"<TABLE BORDER=1>\r\n");
	  
	  /* print the top row */
	  u_fprintf(lx->OUT,"<TR><TD><B>%U</B></TD><TD><I>%U</I></TD><TD><B>%U</B></TD></TR>",
		    FSWF("taggedarrayTag", "Tag"),
		    defaultLanguageDisplayName(),
		    lx->curLocale ? lx->curLocale->ustr : FSWF("none","None"));
	  
	  for(v=0;v<rows;v++)
	    {
	      const char *tag;

	      status = U_ZERO_ERROR;
          taggedItem = ures_getByIndex(tagged, v, NULL, &status);
          tag = ures_getKey(taggedItem);
	      
	      /*tag = ures_getTaggedArrayTag(rb, key, v, &status);*/
	      if(!tag)
		break;
	      
	      u_fprintf(lx->OUT,"<TR> ");

	      if(U_SUCCESS(status))
		{
		  u_fprintf(lx->OUT, "<TD><TT>%s</TT></TD> ", tag);
		  
		  if(lx->defaultRB)
		    {
                      item = ures_getByKey(defaultTagged, tag, item, &status);
                      s = ures_getString(item, &len, &status);

		      if(s)
			u_fprintf(lx->OUT, "<TD><I>%U</I></TD>", s);
		      else
			u_fprintf(lx->OUT, "<TD></TD>");
		    }
		      else
			u_fprintf(lx->OUT, "<TD></TD>");
		  
		  status = U_ZERO_ERROR;

		  s = ures_getString(taggedItem, &len, &status);

		  if(s)
		    u_fprintf(lx->OUT, "<TD>%U</TD>", s);
		  else
		    {
		      u_fprintf(lx->OUT, "<TD BGCOLOR=" kStatusBG " VALIGN=TOP>");
		      explainStatus_X(status, key);
		      u_fprintf(lx->OUT, "</TD>\r\n");
		    }
		}
	      u_fprintf(lx->OUT, "</TR>\r\n");
	    }
	  u_fprintf(lx->OUT, "</TABLE>\r\n<BR>");
          ures_close(taggedItem); /* todo: mem. management? */
	}
    }

  u_fprintf(lx->OUT, "</TD>");
  showKeyAndEndItem(key, locale);
}


/* Explain what the status code means --------------------------------------------------------- */

void explainStatus_X( UErrorCode status, const char *tag )
{
  explainStatus(lx, status, tag);
}

void explainStatus( LXContext *lx, UErrorCode status, const char *tag )
{

  if(tag == 0)
    tag = "_top_";

  if(status != U_ZERO_ERROR)
      u_fprintf(lx->OUT, " <B><FONT SIZE=-1>");

  switch(status)
    {
    case U_MISSING_RESOURCE_ERROR:
      printHelpTag("U_MISSING_RESOURCE_ERROR",
		   FSWF("U_MISSING_RESOURCE_ERROR", "(missing resource)"));
      break;

    case U_USING_FALLBACK_ERROR:
      if(lx->parLocale && lx->parLocale->str)
	{
	  u_fprintf(lx->OUT, "<A HREF=\"?_=%s#%s\">", lx->parLocale->str, tag);
	  u_fprintf_u(lx->OUT, FSWF("inherited_from", "(inherited from %U)"), lx->parLocale->ustr); 
	}
      else
	{
	  u_fprintf(lx->OUT, "<A HREF=\"?_=root#%s\">", tag);
	  u_fprintf_u(lx->OUT, FSWF("inherited", "(inherited)"));
	}

      u_fprintf(lx->OUT, "</A>");
      break;

    case U_USING_DEFAULT_ERROR:
	u_fprintf(lx->OUT, "<A HREF=\"?_=root#%s\">", tag);
	  u_fprintf_u(lx->OUT, FSWF("inherited_from", "(inherited from %U)"), lx->locales->ustr); 
	  u_fprintf(lx->OUT, "</A>");
      break;

    default:
      if(status != U_ZERO_ERROR)
	{
	  u_fprintf(lx->OUT, "(%U %d - %s)", FSWF("UNKNOWN_ERROR", "unknown error"), (int) status,
                    u_errorName(status));
	  fprintf(stderr,"LRB: caught Unknown err- %d\n", status); 
	}
    }

  if(status != U_ZERO_ERROR)
      u_fprintf(lx->OUT, "</FONT></B>");
}


bool_t didUserAskForKey(const char *key, const char *queryString)
{
  const char *tmp1, *tmp2;
  
  tmp1 = strstr(queryString, "SHOW");
  
  /* look to see if they asked for it */
  if(tmp1)
    {
      tmp1 += 4;
      
      tmp2 = strchr(tmp1,'&'); /*look for end of that field */
      if(!tmp2)
	tmp2 = tmp1 + strlen(tmp1); /* no end in sight */
      
      
      if(!strncmp(tmp1, key, (tmp2-tmp1)))
	{
	  return TRUE;
	}
    }
  return FALSE;
}

/* Convenience function.  print <A HREF="..."> for a link to the correct Help page.  if str=null it defaults to Help*/

void printHelpTag(const char *helpTag, const UChar *str)
{
  if(str == NULL)
    {
      /* str = FSWF("help", "Help"); */
      
      printHelpImg(helpTag, FSWF("help", "Help"), 
		   FSWF("helpgif", "help.gif"),
		   FSWF("helpgif_opt", "BORDER=0"));
      return;

    }

  u_fprintf(lx->OUT, "<A TARGET=\"icu_lx_help\" HREF=\"../_/help.html#%s\" TARGET=\"help\">%U</A>",
	    helpTag,str);
}

void printHelpImg(const char *helpTag, const UChar *alt, const UChar *src, const UChar *options)
{
  u_fprintf(lx->OUT, "<A HREF=\"../_/help.html#%s\" TARGET=\"icu_lx_help\"><IMG %U SRC=\"../_/%U\" ALT=\"%U\"></A>",
	    helpTag, 
	    options, src, alt);
}

void showExploreCloseButton(const char *locale, const char *frag)
{
    /* What do we do here? */
    u_fprintf(lx->OUT, "<!-- no CLOSE BUTTON here. -->\r\n");
}

void showExploreButton(UResourceBundle *rb, const char *locale, const UChar *sampleString, const char *key)
{
  UChar nullString[] = { 0x0000 };
  
  if(!sampleString)
    sampleString = nullString;

  u_fprintf(lx->OUT, "\r\n<FORM TARGET=\"_new\" NAME=EXPLORE_%s ACTION=\"#EXPLORE_%s\">"
	    "<INPUT TYPE=HIDDEN NAME=_ VALUE=\"%s\">"
	    "<INPUT TYPE=HIDDEN NAME=\"EXPLORE_%s\" VALUE=\"",
	    key, key,locale,key);
  writeEscaped(sampleString);
  u_fprintf(lx->OUT, "\">");
  
  u_fprintf(lx->OUT, "<INPUT TYPE=IMAGE VALIGN=TOP WIDTH=48 HEIGHT=20 BORDER=0 SRC=\"../_/explore.gif\"  ALIGN=RIGHT   ");
  u_fprintf(lx->OUT, " VALUE=\"%U\"></FORM>",
	    FSWF("exploreTitle", "Explore"));
}

void showExploreButtonSort(UResourceBundle *rb, const char *locale, const char *sampleString, const char *key)
{
  if(!sampleString)
    sampleString = "";

  u_fprintf(lx->OUT, "\r\n<FORM TARGET=\"_new\" NAME=EXPLORE_%s ACTION=\"#EXPLORE_%s\">"
	    "<INPUT TYPE=HIDDEN NAME=_ VALUE=\"%s\">"
	    "<INPUT TYPE=HIDDEN NAME=\"EXPLORE_%s\" VALUE=\"",
	    key, key,locale,key);
  u_fprintf(lx->OUT, "%s", sampleString);
  u_fprintf(lx->OUT, "\">");
  
  u_fprintf(lx->OUT, "<INPUT TYPE=IMAGE VALIGN=TOP WIDTH=48 HEIGHT=20 BORDER=0 SRC=\"../_/explore.gif\"  ALIGN=RIGHT   ");
  u_fprintf(lx->OUT, " VALUE=\"%U\"></FORM>",
	    FSWF("exploreTitle", "Explore"));
}

void showExploreLink(UResourceBundle *rb, const char *locale, const UChar *sampleString, const char *key)
{
  UChar nullString[] = { 0x0000 };
  
  if(!sampleString)
    sampleString = nullString;

  u_fprintf(lx->OUT, "<A TARGET=\"lx_explore_%s_%s\" HREF=\"?_=%s&EXPLORE_%s=",
	    locale,key,locale,key);
  writeEscaped(sampleString);
  u_fprintf(lx->OUT, "&\">");
}

void exploreFetchNextPattern(UChar *dstPattern, const char *qs)
{

  /* make QS point to the first char of the field data */
  qs = strchr(qs, '=');
  qs++;

  unescapeAndDecodeQueryField(dstPattern, 1000, qs);
  u_replaceChar(dstPattern, 0x0020, 0x00A0);
}

/**
 */

void exploreShowPatternForm(UChar *dstPattern, const char *locale, const char *key, const char* qs, double value, UNumberFormat *valueFmt)
{
  UChar *p;
  int32_t len;
  UErrorCode status = U_ZERO_ERROR;
  UChar tmp[1024];

  /**********  Now we've got the pattern from the user. Now for the form.. ***/
  u_fprintf(lx->OUT, "<FORM METHOD=GET ACTION=\"#EXPLORE_%s\">\r\n",
	    key);
  u_fprintf(lx->OUT, "<INPUT NAME=_ TYPE=HIDDEN VALUE=%s>", locale);

  if(valueFmt)
    {
      
      u_fprintf(lx->OUT, "<INPUT NAME=NP_DBL TYPE=HIDDEN VALUE=\"");
      tmp[0] = 0;
      unum_formatDouble(valueFmt, value, tmp, 1000, 0, &status);
      u_fprintf(lx->OUT, "%U\">", tmp);
    }
  u_fprintf(lx->OUT, "<TEXTAREA ROWS=2 COLS=60 NAME=\"EXPLORE_%s\">",
	    key);


  lx->backslashCtx.html = FALSE;

  u_fprintf(lx->OUT, "%U", dstPattern); 

  lx->backslashCtx.html = TRUE;
  
  u_fprintf(lx->OUT, "</TEXTAREA><P>\r\n<INPUT TYPE=SUBMIT VALUE=Format><INPUT TYPE=RESET VALUE=Reset></FORM>\r\n");

}


/* Demonstrate sorting ------------------------------------------------------------------------- */
/* locale to view, b is the rest of the querystring */

/* To demonstrate sorting, we are going to call the usort external program.
   This is a little simpler than maintaining two sets of code for doing sorting. 
   the real solution would be to modularize the code in usort.. 
*/

#define SORTSIZE 8192

void showSort(const char *locale, const char *b)
{
  char   inputChars[SORTSIZE];
  char   tmpChars[SORTSIZE];
  char *text;
  char *p;
  int32_t length;
  UChar  strChars[SORTSIZE];
  bool_t  doingG7 = FALSE;
  int    i;
#define G7COUNT 8
  const char   G7s[G7COUNT][10] = { "de_DE", "en_GB", "en_US", "fr_CA", "fr_FR", "it_IT", "ja_JP", "sv_SE" };
  UErrorCode status = U_ZERO_ERROR;

  strChars[0] = 0;

  if(strstr(locale,"g7") != NULL)
  {
      doingG7 = TRUE;
  }

  text = strstr(b, "EXPLORE_CollationElements");

  if(text)
    {
      text += 26;

      unescapeAndDecodeQueryField_enc(strChars, 1000, text, lx->chosenEncoding );
      
      p = strchr(text, '&');
      if(p) /* there is a terminating ampersand */
	length = p - text;
      else
	length = strlen(text);

      if(length > (SORTSIZE-1))
	length = SORTSIZE-1; /* safety ! */

      strncpy(inputChars, text, length); /* make a copy for future use */
      inputChars[length] = 0;
      /*      doDecodeQueryField(text, inputChars, length); ** length limited */
    }
  else
    {
      inputChars[0] = 0; 
    }

  u_fprintf(lx->OUT, "%U<P>\r\n", FSWF("EXPLORE_CollationElements_Prompt", "Type in some lines of text to be sorted."));

  u_fprintf(lx->OUT, "<TABLE BORDER=1 CELLSPACING=1 CELLPADDING=1 WIDTH=100% HEIGHT=100%><TR><TD WIDTH=\"20%\"><B>%U</B></TD>\r\n",
            FSWF("usortSource", "Source"));


  if(inputChars[0])
  {
      if(doingG7 == FALSE)
      {
          u_fprintf(lx->OUT, "<TD WIDTH=\"20%\"><B>%U</B></TD><TD WIDTH=\"20%\"><B>%U</B></TD><TD WIDTH=\"20%\"><B>%U</B></TD><TD WIDTH=\"20%\"><B>%U</B></TD>",
                    FSWF("usortOriginal", "Original"),
                    FSWF("usortTertiary", "Primary-Tertiary"),
                    FSWF("usortSecondary", "Primary & Secondary"),
                    FSWF("usortPrimary", "Primary only"));
      }
      else
      {
          for(i=0;i<G7COUNT;i++)
          {
              UChar junk[1000];
              uloc_getDisplayName(G7s[i], lx->cLocale, junk, 1000, &status);
              u_fprintf(lx->OUT, "<TD WIDTH=\"10%\">%U</TD>",junk);
          }
                        
      }
  }
      
  u_fprintf(lx->OUT, "</TR>\r\n");
  
  u_fprintf(lx->OUT, "<TR><TD WIDTH=\"20%\">");
  
  /* the source box */
  u_fprintf(lx->OUT, "<FORM ACTION=\"#EXPLORE_CollationElements\" METHOD=GET>\r\n");
  u_fprintf(lx->OUT, "<INPUT TYPE=HIDDEN NAME=\"_\" VALUE=\"%s\">\r\n", locale);
  u_fprintf(lx->OUT, "<TEXTAREA ROWS=10 COLUMNS=20 COLS=20 NAME=\"EXPLORE_CollationElements\">");
  
  writeEscaped(strChars); 
  /* if(*inputChars)
     u_fprintf(lx->OUT, "%s", inputChars);  */
  
  u_fprintf(lx->OUT, "</TEXTAREA><BR>\r\n");
  
  u_fprintf(lx->OUT, "<INPUT TYPE=SUBMIT VALUE=\"%U\"></FORM><P>\r\n",
            FSWF("EXPLORE_CollationElements_Sort", "Sort"));

  /* END source box */
  u_fprintf(lx->OUT, "</TD>\r\n");

  if(inputChars[0] != 0)
      {
          FILE *sortpipe;
          UCollationStrength sortTypes[] = { UCOL_IDENTICAL /* not used */, UCOL_TERTIARY, UCOL_SECONDARY, UCOL_PRIMARY };
          int n;

          UChar in[SORTSIZE];
          int32_t inLen, outLen;
          UErrorCode status2 = U_ZERO_ERROR;
      
          /* have some text to sort */
          unescapeAndDecodeQueryField_enc(in, SORTSIZE, inputChars, lx->chosenEncoding);
          u_replaceChar(in, 0x000D, 0x000A); /* CRLF */
      
          if(doingG7 == FALSE)
          {
              /* Loop through each sort method */
              for(n=0;n<4;n++) /* not 4 */
              {
                  USort *aSort;
                  UErrorCode sortErr = U_ZERO_ERROR;
                  UChar *first, *next;
                  int32_t i, count=0;
                  
                  aSort = usort_open(locale, sortTypes[n], TRUE, &sortErr);
                  
                  /* add lines from buffer */
                  
                  /* For now, we pass TRUE to clone the text. Wasteful! But, 
                     it avoids having to modify the in text AND keep track of the
                     ptrs. Now if a usort could be cloned and resorted before
                     output.. */
                  first = in;
                  next = first;
                  while(*next)
                  {
                      if(*next == 0x000A)
                      {
                          if(first != next)
                          {
                              usort_addLine(aSort, first, next-first, TRUE, (void*)++count);
                          }
                          first = next+1;
                      }
                      next++;
                  }
                  
                  if(first != next) /* get the LAST line */
                  {
                      usort_addLine(aSort, first, next-first, TRUE, (void*)++count);
                  }      
                  
                  if(n != 0)
                      usort_sort(aSort); /* first item is 'original' */
                  
                  
                  u_fprintf(lx->OUT, " <TD VALIGN=TOP>");
                  
                  for(i=0;i<aSort->count;i++)
                  {
                    UBool doUnderline = TRUE;

                    if( ((i+1)<aSort->count) &&
                        (aSort->lines[i].keySize == aSort->lines[i+1].keySize) &&
                        !memcmp(aSort->lines[i].key,
                                aSort->lines[i+1].key,
                                aSort->lines[i].keySize))
                      {
                        doUnderline = FALSE;
                      }

                    u_fprintf(lx->OUT, "%s%02d.%U%s<BR>\n",
                              (doUnderline?"<U>":""),
                                (int32_t)aSort->lines[i].userData, aSort->lines[i].chars,
                              (doUnderline?"</U>":"")
                              );
                  }
                  
                  u_fprintf(lx->OUT, "</TD>");	  
                  
                  usort_close(aSort);
              }
          }
          else /* g7 ============================================ */
          {
              for(n=0;n<G7COUNT;n++)
              {
                  USort *aSort;
                  UErrorCode sortErr = U_ZERO_ERROR;
                  UChar *first, *next;
                  int32_t i, count=0;
                  
                  aSort = usort_open(G7s[n], UCOL_TERTIARY, TRUE, &sortErr);
                  
                  /* add lines from buffer */
                  
                  /* For now, we pass TRUE to clone the text. Wasteful! But, 
                     it avoids having to modify the in text AND keep track of the
                     ptrs. Now if a usort could be cloned and resorted before
                     output.. */
                  first = in;
                  next = first;
                  while(*next)
                  {
                      if(*next == 0x000A)
                      {
                          if(first != next)
                          {
                              usort_addLine(aSort, first, next-first, TRUE, (void*)++count);
                          }
                          first = next+1;
                      }
                      next++;
                  }
                  
                  if(first != next) /* get the LAST line */
                  {
                      usort_addLine(aSort, first, next-first, TRUE, (void*)++count);
                  }      
                  
                  usort_sort(aSort);
                  
                  u_fprintf(lx->OUT, " <TD VALIGN=TOP>");
                  
                  for(i=0;i<aSort->count;i++)
                  {
                    UBool doUnderline = TRUE;
                    
                    if( ((i+1)<aSort->count) &&
                        (aSort->lines[i].keySize == aSort->lines[i+1].keySize) &&
                        !memcmp(aSort->lines[i].key,
                                aSort->lines[i+1].key,
                                aSort->lines[i].keySize))
                      {
                        doUnderline = FALSE;
                      }

                    u_fprintf(lx->OUT, "%s%02d.%U%s<BR>\n",
                              doUnderline?"<U>":"",
                              (int32_t)aSort->lines[i].userData, aSort->lines[i].chars,
                              doUnderline?"</U>":""
                              );
                  }
                  
                  u_fprintf(lx->OUT, "</TD>");	  
                  
                  usort_close(aSort);
              }
          } /* end G7 demo */
      }
      u_fprintf(lx->OUT, "</TR></TABLE><P>");

      if(doingG7 == FALSE)
          u_fprintf(lx->OUT, "<P><P>%U", FSWF("EXPLORE_CollationElements_strength", "You see four different columns as output. The first is the original text for comparison. The lines are numbered to show their original position. The remaining columns show sorting by different strengths (available as a parameter to the collation function). Groups of lines that sort precisely the same are separated by an underline. Since collation treats these lines as identical, lines in the same group could appear in any order (depending on the precise sorting algorithm used). "));

  u_fprintf(lx->OUT, "<P>\r\n");
  showExploreCloseButton(locale, "CollationElements");

}


/******************************************************************************
 *  Explorer for dates
 */
    
void showExploreDateTimePatterns(UResourceBundle *myRB, const char *locale, const char *b)
{
  UChar pattern[1024];
  UChar tempChars[1024];
  UChar defChars[1024];
  UChar valueString[1024];
  UDateFormat  *df = NULL, *df_default = NULL;
  UErrorCode   status = U_ZERO_ERROR, defStatus = U_ZERO_ERROR, locStatus = U_ZERO_ERROR;
  UDate now;  /* example date */
  UNumberFormat *nf = NULL; /* for formatting the number */
  char *tmp;
  int32_t parsePos = 0;

  nf = unum_openPattern(FSWF("EXPLORE_DateTimePatterns_dateAsNumber", "#"), -1, NULL, &status);
  status = U_ZERO_ERROR; /* ? */
  
  df_default = udat_open(UDAT_SHORT, UDAT_SHORT, NULL, NULL, -1,  &status);
  status = U_ZERO_ERROR; /* ? */

  now = ucal_getNow();
  
  showKeyAndStartItem("EXPLORE_DateTimePatterns",
		      FSWF("EXPLORE_DateTimePatterns", "Explore &gt; Date/Time"),
		      locale, FALSE, U_ZERO_ERROR);

  u_fprintf(lx->OUT, "%U<P>", FSWF("formatExample_DateTimePatterns_What","This example demonstrates the formatting of date and time patterns in this locale."));
  
  /* fetch the current pattern */
  exploreFetchNextPattern(pattern, strstr(b,"EXPLORE_DateTimePatterns"));

  df = udat_open(0,0,locale, NULL, -1, &status);
  udat_applyPattern(df, TRUE, pattern, -1);

  status = U_ZERO_ERROR;
  
  if (tmp = strstr(b,"NP_DBL")) /* Double: UDate format input ============= */
    {
      /* Localized # */
      tmp += 7;

      unescapeAndDecodeQueryField(valueString, 1000, tmp);
      u_replaceChar(valueString, 0x0020, 0x00A0);

      status = U_ZERO_ERROR;
      now = unum_parseDouble(nf, valueString, -1, &parsePos, &status);
    }
  else if(tmp = strstr(b, "NP_DEF")) /* Default: 'display' format input ============== */
    {
      int32_t q;
      UChar patn[1024];

      /* Localized # */
      tmp += 7;

      unescapeAndDecodeQueryField(valueString, 1000, tmp);
      /*      u_replaceChar(valueString, 0x0020, 0x00A0); */ /* NOt for the default pattern */

#if 0
      u_fprintf(lx->OUT, "VS_=((");
      for(q=0;valueString[q];q++)
	{
	  u_fprintf(lx->OUT, "%c:%04x ", (char)valueString[q], valueString[q]);
	}
      u_fprintf(lx->OUT, "))<BR>");

      udat_toPattern(df_default, FALSE, patn, 1024, &status);

      u_fprintf(lx->OUT, "PAT=((");
      for(q=0;patn[q];q++)
	{
	  u_fprintf(lx->OUT, "%c:%04x ", (char)patn[q], patn[q]);
	}
      u_fprintf(lx->OUT, "))<BR>");
#endif

      status = U_ZERO_ERROR;
      
      now = udat_parse(df_default, valueString, -1, &parsePos, &status);
    }
  else if(tmp = strstr(b, "NP_LOC")) /* Localized: pattern format input ============== */
    {

      int32_t q;
      UChar patn[1024];

      /* Localized # */
      tmp += 7;

      unescapeAndDecodeQueryField(valueString, 1000, tmp);
      u_replaceChar(valueString, 0x0020, 0x00A0); 

      /*      valueString[u_strlen(valueString)-1] = 0; */
	      /* extra space ? */


#if 0
      u_fprintf(lx->OUT, "VS_=((");
      for(q=0;valueString[q];q++)
	{
	  u_fprintf(lx->OUT, "%c:%04x ", (char)valueString[q], valueString[q]);
	}
      u_fprintf(lx->OUT, "))<BR>");

      udat_toPattern(df, FALSE, patn, 1024, &status);

      u_fprintf(lx->OUT, "PAT=((");
      for(q=0;patn[q];q++)
	{
	  u_fprintf(lx->OUT, "%c:%04x ", (char)patn[q], patn[q]);
	}
      u_fprintf(lx->OUT, "))<BR>");
#endif


      status = U_ZERO_ERROR;
      now = udat_parse(df, valueString, -1, &parsePos, &status);
    }

  /* Common handler for input errs */

  if(U_FAILURE(status) || (now == 0))
    {
      status = U_ZERO_ERROR;
      u_fprintf(lx->OUT, "%U %d<P>\r\n", FSWF("formatExample_errorParse", "Could not parse this, replaced with a default value. Formatted This many chars:"), parsePos);
      now = ucal_getNow();
    }
  status = U_ZERO_ERROR;
  /* ======================== End loading input date ================================= */

  if(U_FAILURE(status))
    {
      u_fprintf(lx->OUT, "%U: [%d] <P>", FSWF("formatExample_errorOpen", "Couldn't open the formatter"), (int) status);
      explainStatus_X(status, "EXPLORE_DateTimePatterns");
      exploreShowPatternForm(pattern, locale, "DateTimePatterns", strstr(b,"EXPLORE_DateTimePatterns"), now, nf);
    }
  else
    {
      
      /* now display the form */
      exploreShowPatternForm(pattern, locale, "DateTimePatterns", strstr(b,"EXPLORE_DateTimePatterns"), now, nf);
      
    }
  
  status = U_ZERO_ERROR;
  udat_format(df,now,tempChars, 1024, 0, &locStatus);
  udat_format(df_default,now,defChars, 1024, 0, &defStatus);
  
  if(U_FAILURE(status))
    u_fprintf(lx->OUT, "%U<P>", FSWF("formatExample_DateTimePatterns_errorFormat", "Couldn't format the date."));
  
  explainStatus_X(status,"EXPLORE_DateTimePatterns");



  /* =======================  Now, collect the new date values ====================== */

  /* Now, display the results in <default> and in their locale */
  u_fprintf(lx->OUT, "<TABLE BORDER=1><TR><TD>\r\n");


  /* ============ 'default' side of the table  */

  if(U_FAILURE(defStatus))
    {
      u_fprintf(lx->OUT, "%U<P>", FSWF("formatExample_errorFormatDefault", "Unable to format number using default version of the pattern"));
      explainStatus_X(status, "EXPLORE_DateTimePatterns");
    }
  else
    {
      UConverterFromUCallback oldCallback;
      
      u_fprintf(lx->OUT, "<B><I>%U</I></B><BR>\r\n", defaultLanguageDisplayName());
#if 0
      /* Just the pattern */
      u_fprintf(lx->OUT, "%U", defChars);
#else
      u_fprintf(lx->OUT, "<FORM METHOD=GET ACTION=\"#EXPLORE_DateTimePatterns\">\r\n");
      u_fprintf(lx->OUT, "<INPUT NAME=_ TYPE=HIDDEN VALUE=%s>\r\n", locale);
      u_fprintf(lx->OUT, "<INPUT TYPE=HIDDEN NAME=EXPLORE_DateTimePatterns VALUE=\"");
      writeEscaped(pattern);
      u_fprintf(lx->OUT, "\">\r\n");

      u_fprintf(lx->OUT, "<TEXTAREA NAME=NP_DEF ROWS=1 COLS=30>");

      lx->backslashCtx.html = FALSE;
      u_fprintf(lx->OUT, "%U", defChars); 
      lx->backslashCtx.html = TRUE;
      
      status = U_ZERO_ERROR;
      
      u_fprintf(lx->OUT, "</TEXTAREA><BR><INPUT TYPE=SUBMIT VALUE=\"%U\"></FORM>", FSWF("EXPLORE_change", "Change"));
#endif
    }
  
  u_fprintf(lx->OUT, "</TD><TD WIDTH=1 BGCOLOR=\"#EEEEEE\"><IMG src=\"../_/c.gif\" ALT=\"---\" WIDTH=0 HEIGHT=0></TD><TD>");

  /* ============ 'localized' side ================================= */

  if(U_FAILURE(locStatus))
    {
      u_fprintf(lx->OUT, "%U<P>", FSWF("formatExample_DateTimePatterns_errorFormat", "Couldn't format the date."));
      explainStatus_X(status, "EXPLORE_DateTimePatterns");
    }
  else
    {
      /*  === local side */
      u_fprintf(lx->OUT, "\r\n\r\n<!--  LOCALIZED SIDE -->\r\n<B>%U</B><BR>\r\n",lx->curLocale?lx->curLocale->ustr:FSWF("NoLocale","MISSING LOCALE NAME") );
#if 0
      u_fprintf(lx->OUT, "%U", tempChars);
#else
      u_fprintf(lx->OUT, "<FORM METHOD=GET ACTION=\"#EXPLORE_DateTimePatterns\">\r\n");
      u_fprintf(lx->OUT, "<INPUT NAME=_ TYPE=HIDDEN VALUE=%s>\r\n", locale);
      u_fprintf(lx->OUT, "<INPUT TYPE=HIDDEN NAME=EXPLORE_DateTimePatterns VALUE=\"");
      writeEscaped(pattern);
      u_fprintf(lx->OUT, "\">\r\n");
      
      u_fprintf(lx->OUT, "<TEXTAREA NAME=NP_LOC ROWS=1 COLS=30>");
      writeEscaped(tempChars);
      u_fprintf(lx->OUT, "</TEXTAREA><BR><INPUT TYPE=SUBMIT VALUE=\"%U\"></FORM>", FSWF("EXPLORE_change", "Change"));
#endif

    }
  /*  ============== End of the default/localized split =============== */

  u_fprintf(lx->OUT, "</TD></TR>");
  u_fprintf(lx->OUT, "</TABLE>");
  

  /* =============== All done ========================== */
  
  if(df)
    udat_close(df);

  if(df_default)
    udat_close(df_default);

  if(nf)
    unum_close(nf);

  u_fprintf(lx->OUT, "<P><P>");

  showExploreCloseButton(locale, "DateTimePatterns");

  u_fprintf(lx->OUT, "</TD><TD ALIGN=LEFT VALIGN=TOP>");
  printHelpTag("EXPLORE_DateTimePatterns", NULL);
  u_fprintf(lx->OUT, "</TD>\r\n");

  showKeyAndEndItem("EXPLORE_DateTimePatterns", locale);
  

  /* ========= Show LPC's for reference ================= */

  /* ..... */
      /* locale pattern chars */
      {
	const UChar *charDescs[19];

	charDescs[0] = FSWF("localPatternChars0", "Era");
	charDescs[1] = FSWF("localPatternChars1", "Year");
	charDescs[2] = FSWF("localPatternChars2", "Month");
	charDescs[3] = FSWF("localPatternChars3", "Date");
	charDescs[4] = FSWF("localPatternChars4", "Hour Of Day 1");
	charDescs[5] = FSWF("localPatternChars5", "Hour Of Day 0"); 
	charDescs[6] = FSWF("localPatternChars6", "Minute");
	charDescs[7] = FSWF("localPatternChars7", "Second");
	charDescs[8] = FSWF("localPatternChars8", "Millisecond");
	charDescs[9] = FSWF("localPatternChars9", "Day Of Week");
	charDescs[10] = FSWF("localPatternChars10", "Day Of Year");
	charDescs[11] = FSWF("localPatternChars11", "Day Of Week In Month");
	charDescs[12] = FSWF("localPatternChars12", "Week Of Year");
	charDescs[13] = FSWF("localPatternChars13", "Week Of Month");
	charDescs[14] = FSWF("localPatternChars14", "Am/Pm");
	charDescs[15] = FSWF("localPatternChars15", "Hour 1");
	charDescs[16] = FSWF("localPatternChars16", "Hour 0");
	charDescs[17] = FSWF("localPatternChars17", "Timezone");
	charDescs[18] = 0;
	
	showStringWithDescription(myRB, locale, "SHOWlocalPatternChars", charDescs, "localPatternChars", FALSE);
      }
}

/*****************************************************************************
 *
 * Explorer for #'s
 */

void showExploreNumberPatterns(const char *locale, const char *b)
{
  UChar pattern[1024];
  UChar tempChars[1024];
  UNumberFormat  *nf = NULL; /* numfmt in the current locale */
  UNumberFormat  *nf_default = NULL; /* numfmt in the default locale */
  UErrorCode   status = U_ZERO_ERROR;
  int32_t i;
  double   value;
  char encodedPattern[1024];
  UChar valueString[1024];
  
  const UChar *defaultValueErr = 0,
              *localValueErr   = 0;
  
  const char *tmp, *tmpLimit;
  
  showKeyAndStartItem("EXPLORE_NumberPatterns", FSWF("EXPLORE_NumberPatterns", "Explore &gt; Numbers"), locale, FALSE, U_ZERO_ERROR);

  u_fprintf(lx->OUT, "%U<P>", FSWF("formatExample_NumberPatterns_What","This example demonstrates formatting of numbers in this locale."));

  exploreFetchNextPattern(pattern, strstr(b,"EXPLORE_NumberPatterns")); 

  nf = unum_open(UNUM_DEFAULT,locale,  &status);
  
  if(U_FAILURE(status))
    {
      u_fprintf(lx->OUT, "</TD></TR></TABLE></TD></TR></TABLE><P><HR>%U: ", FSWF("formatExample_errorOpen", "Couldn't open the formatter"));
      explainStatus_X(status, "EXPLORE_NumberPattern");
      return; /* ? */
    }
  
  unum_applyPattern(nf, TRUE, pattern, -1);
  
  unum_toPattern(nf, FALSE, tempChars, 1024, &status);

  if(U_FAILURE(status))
    {
      u_fprintf(lx->OUT, "</TD></TR></TABLE></TD></TR></TABLE><P><HR>  %U<P>", FSWF("formatExample_errorToPattern", "Couldn't convert the pattern [toPattern]"));
      explainStatus_X(status, "EXPLORE_NumberPattern");
      return;
    }

  nf_default = unum_open(UNUM_DEFAULT, NULL, &status);
  
  if(U_FAILURE(status))
    {
      u_fprintf(lx->OUT, "</TD></TR></TABLE></TD></TR></TABLE><P><HR>%U<P>", FSWF("formatExample_errorOpenDefault", "Couldn't open the default number fmt"));
      explainStatus_X(status, "EXPLORE_NumberPattern");
      return;
    }
  
  /* Load the default with a simplistic pattern .. */
  unum_applyPattern(nf_default, FALSE, FSWF("EXPLORE_NumberPatterns_defaultPattern", "#,###.###############"), -1);
      
  /* Allright. we've got 'nf' which is our custom pattern in the target 
     locale, and we've got 'nf_default' which is a pattern that we hope is
     reasonable for displaying a number in the *default* locale

     Confused yet?
  */

  value = 12345.6789; /* for now */

  /* Now, see if the user is trying to change the value. */
  if((tmp = strstr(b,"NP_LOC")))
    {
      /* Localized # */
      tmp += 7;

      unescapeAndDecodeQueryField(valueString, 1000, tmp);
      u_replaceChar(valueString, 0x0020, 0x00A0);
      

      status = U_ZERO_ERROR;
      value = unum_parseDouble(nf, valueString, -1, 0, &status);
      
      if(U_FAILURE(status))
	{
	  status = U_ZERO_ERROR;
	  localValueErr = FSWF("formatExample_errorParse_num", "Could not parse this, replaced with a default value.");
	}
    }
  else if ((tmp = strstr(b,"NP_DEF")) || (tmp = strstr(b,"NP_DBL")))
    {
      /* Localized # */
      tmp += 7;

      unescapeAndDecodeQueryField(valueString, 1000, tmp);
      u_replaceChar(valueString, 0x0020, 0x00A0);


      status = U_ZERO_ERROR;
      value = unum_parseDouble(nf_default, valueString, -1, 0, &status);
      
      if(U_FAILURE(status))
	{
	  status = U_ZERO_ERROR;
	  defaultValueErr = FSWF("formatExample_errorParse3", "Could not parse this, replaced with a default value.");
	}
    }

  /** TODO: replace with:
      
      case NP_LOC:
      value = unum_parseDouble(nf, str, ... )
      break;
      
      case NP_DEF:
      value = unum_parseDouble(defaultNF, str, ... );
	     break;
  **/

  /* NOW we are ready ! */

  /* display the FORM, and fetch the current pattern */
  exploreShowPatternForm(pattern, locale, "NumberPatterns", strstr(b,"EXPLORE_NumberPatterns"), value, nf_default); 


  /* Now, display the results in <default> and in their locale */
  u_fprintf(lx->OUT, "<TABLE BORDER=1><TR><TD>\r\n");


  /* ============ 'default' side of the table ==========  */

  unum_formatDouble(nf_default,value,tempChars, 1024, 0, &status);

  if(U_FAILURE(status))
    {
      u_fprintf(lx->OUT, "%U<P>", FSWF("formatExample_errorFormatDefault", "Unable to format number using default version of the pattern"));
      explainStatus_X(status, "EXPLORE_NumberPattern");
    }
  else
    {
      UConverterFromUCallback oldCallback;
      
      u_fprintf(lx->OUT, "<B><I>%U</I></B><BR>\r\n", defaultLanguageDisplayName());
      u_fprintf(lx->OUT, "<FORM METHOD=GET ACTION=\"#EXPLORE_NumberPatterns\">\r\n");
      u_fprintf(lx->OUT, "<INPUT NAME=_ TYPE=HIDDEN VALUE=%s>\r\n", locale);
      u_fprintf(lx->OUT, "<INPUT TYPE=HIDDEN NAME=EXPLORE_NumberPatterns VALUE=\"");
      writeEscaped(pattern);
      u_fprintf(lx->OUT, "\">\r\n");

      u_fprintf(lx->OUT, "<TEXTAREA NAME=NP_DEF ROWS=1 COLS=20>");
      lx->backslashCtx.html = FALSE;
      u_fprintf(lx->OUT, "%U", tempChars); 
      lx->backslashCtx.html = TRUE;
      
      status = U_ZERO_ERROR;
      
      u_fprintf(lx->OUT, "</TEXTAREA><INPUT TYPE=SUBMIT VALUE=\"%U\"></FORM>", FSWF("EXPLORE_change", "Change"));
      
    }
  
  u_fprintf(lx->OUT, "</TD><TD WIDTH=1 BGCOLOR=\"#EEEEEE\"><IMG SRC=\"../_/c.gif\" ALT=\"---\" WIDTH=0 HEIGHT=0></TD><TD>");

  /* ============ 'localized' side ================================= */

  unum_formatDouble(nf,value,tempChars, 1024, 0, &status);

  if(U_FAILURE(status))
    {
      u_fprintf(lx->OUT, "%U<P>", FSWF("formatExample_errorFormat_number", "Couldn't format the number."));
      explainStatus_X(status, "EXPLORE_NumberPattern");
    }
  else
    {
      /*  === local side */
      u_fprintf(lx->OUT, "\r\n\r\n<!--  LOCALIZED SIDE -->\r\n<B>%U</B><BR>\r\n",lx->curLocale?lx->curLocale->ustr:FSWF("NoLocale","MISSING LOCALE NAME") );
      u_fprintf(lx->OUT, "<FORM METHOD=GET ACTION=\"#EXPLORE_NumberPatterns\">\r\n");
      u_fprintf(lx->OUT, "<INPUT NAME=_ TYPE=HIDDEN VALUE=%s>\r\n", locale);
      u_fprintf(lx->OUT, "<INPUT TYPE=HIDDEN NAME=EXPLORE_NumberPatterns VALUE=\"");
      writeEscaped(pattern);
      u_fprintf(lx->OUT, "\">\r\n");
      
      u_fprintf(lx->OUT, "<TEXTAREA NAME=NP_LOC ROWS=1 COLS=20>");
      writeEscaped(tempChars);
      u_fprintf(lx->OUT, "</TEXTAREA><INPUT TYPE=SUBMIT VALUE=\"%U\"></FORM>", FSWF("EXPLORE_change", "Change"));

      if(localValueErr)
	u_fprintf(lx->OUT, "<P>%U", localValueErr);
    }
  /*  ============== End of the default/localized split =============== */

  u_fprintf(lx->OUT, "</TD></TR>");
  u_fprintf(lx->OUT, "</TABLE>");

  if(nf)
    unum_close(nf);

  if(nf_default)
    unum_close(nf_default);
  
  showExploreCloseButton(locale, "NumberPatterns");
  u_fprintf(lx->OUT, "</TD><TD ALIGN=LEFT VALIGN=TOP>");
  printHelpTag("EXPLORE_NumberPatterns", NULL);
  u_fprintf(lx->OUT, "</TD>\r\n");

  
  showKeyAndEndItem("EXPLORE_NumberPatterns", locale);
}


bool_t isSupportedLocale(const char *locale, bool_t includeChildren)
{
  UResourceBundle *newRB;
  UErrorCode       status = U_ZERO_ERROR;
  bool_t           supp   = TRUE;

  newRB = ures_open(FSWF_bundlePath(), locale, &status);

  if(U_FAILURE(status))
    supp = FALSE;
  else
    {
      if(status == U_USING_DEFAULT_ERROR)
	supp = FALSE;
      else if( (!includeChildren) && (status == U_USING_FALLBACK_ERROR))
	supp = FALSE;
      else
	{
          int32_t len;

	  ures_getStringByKey(newRB, "helpPrefix", &len, &status);

	  if(status == U_USING_DEFAULT_ERROR)
	    supp = FALSE;
	  else if( (!includeChildren) && (status == U_USING_FALLBACK_ERROR))
	    supp = FALSE;
	}

      ures_close(newRB);
    }

  return supp;
}

bool_t isExperimentalLocale(const char *locale)
{
  UResourceBundle *newRB;
  UErrorCode       status = U_ZERO_ERROR;
  bool_t           supp   = FALSE;
  int32_t len;

  newRB = ures_open(NULL, locale, &status);
  if(U_FAILURE(status))
    supp = TRUE;
  else
    {
      const UChar *s = ures_getStringByKey(newRB, "Version", &len, &status);
      
      if(*s == 0x0078) /* If it starts with an 'x'.. */
	supp = TRUE;

      ures_close(newRB);
    }

  return supp;
}

void showKeyAndStartItemShort(const char *key, const UChar *keyName, const char *locale, bool_t cumulative, UErrorCode showStatus)
{
      u_fprintf(lx->OUT, "<P><TABLE BORDER=0 CELLSPACING=0 WIDTH=100%%>");
      u_fprintf(lx->OUT, "<TR><TD HEIGHT=5 BGCOLOR=\"#AFA8AF\" COLSPAN=2><IMG SRC=\"../_/c.gif\" ALT=\"---\" WIDTH=0 HEIGHT=0></TD></TR>\r\n");
      u_fprintf(lx->OUT, "<TR><TD COLSPAN=1 WIDTH=0 VALIGN=TOP BGCOLOR=" kXKeyBGColor "><A NAME=%s><B>", key);

      if(keyName == NULL)
	keyName = FSWF( key, key );

      printHelpTag(key, keyName);

      u_fprintf(lx->OUT,"</B></A>", keyName);

      if(cumulative == TRUE )
      {
          u_fprintf(lx->OUT, " (%U)", FSWF("cumulative_notshown", "cumulative data from parent not shown"));
      }

      u_fprintf(lx->OUT," </TD><TD BGCOLOR=" kXKeyBGColor "   VALIGN=TOP ALIGN=RIGHT>");
      explainStatus_X(showStatus, key);
}

void showKeyAndStartItem(const char *key, const UChar *keyName, const char *locale, bool_t cumulative, UErrorCode showStatus)
{
  showKeyAndStartItemShort(key,keyName,locale, cumulative, showStatus);
  u_fprintf(lx->OUT,"&nbsp;</TD><TR><TD COLSPAN=2>\r\n");
}

void showKeyAndEndItem(const char *key, const char *locale)
{
  u_fprintf(lx->OUT, "</TR></TABLE>\r\n");
}


/* ----------------- lx->setEncoding */
UFILE *setLocaleAndEncodingAndOpenUFILE(char *chosenEncoding, bool_t *didSetLocale, bool_t *didsetEncoding, const char **fileObject)
{
  char *pi;
  char *tmp;
  const char *locale = NULL;
  const char *encoding = NULL;
  UErrorCode status = U_ZERO_ERROR;
  char *acceptLanguage;
  char newLocale[100];
  UFILE *f;
  UChar x[2];



  locale = (const char *)lx->cLocale;
  encoding = lx->chosenEncoding; 

  pi = getenv("PATH_INFO");
  if( (pi) && (*pi && '/') )
    {
      pi++; /* eat first slash */
      tmp = strchr(pi, '/');
      
      if(tmp)
	*tmp = 0; /* tie off at the slash */

      status = U_ZERO_ERROR;
      locale = pi;

      if ( *locale != 0) /* don't want 0-length locales */
	{
          strcpy(lx->cLocale, locale);
	  *didSetLocale = TRUE;
	}

      if(tmp) /* have encoding */
	{
  
	  tmp++; /* skip '/' */
  
	  pi = tmp;
  
	  if(*pi) /* don't want 0 length encodings */
	    {
	      encoding = pi;

              tmp = strchr(tmp, '/');
              if(tmp)
              {
                pi = tmp + 1;
                *tmp = 0;
              }

	      *didsetEncoding = TRUE; 
	    }
	}
    }

  /* Now, did we get a file object? */
  if(*didsetEncoding && pi && !strcmp(encoding, "_"))
  {
    *didsetEncoding = FALSE;
    *fileObject = pi;
    return NULL;
  }

  if(!(*didSetLocale) && (acceptLanguage=getenv("HTTP_ACCEPT_LANGUAGE")) && acceptLanguage[0] )
    {

      /* OK, if they haven't set a locale, maybe their web browser has. */
	if(!(tmp=strchr(acceptLanguage,','))) /* multiple item separator */
      if(!(tmp=strchr(acceptLanguage,'='))) /* strength separator */
	  tmp = acceptLanguage + strlen(acceptLanguage);

      strncpy(newLocale, acceptLanguage, my_min(100,tmp-acceptLanguage));
      newLocale[my_min(100,tmp-acceptLanguage)] = 0;

      /* Note we don't do the PROPER thing here, which is to sort the possible languages by weight. Oh well. */
      
      status = U_ZERO_ERROR;

      /* half hearted attempt at canonicalizing the locale string. */
      newLocale[0] = tolower(newLocale[0]);
      newLocale[1] = tolower(newLocale[1]);
      if(newLocale[2] == '-')
	newLocale[2] = '_';
      if(newLocale[5] == '-')
	newLocale[5] = '_';

      newLocale[3] = toupper(newLocale[3]);
      newLocale[4] = toupper(newLocale[4]);

      if(isSupportedLocale(newLocale, TRUE)) /* DO NOT pick an unsupported locale from the browser's settings! */
        strcpy(lx->cLocale, newLocale);

      status = U_ZERO_ERROR;

      /* that might at least get something.. It's better than defaulting to en_US */
    }
  
  if(!(*didsetEncoding))
    {
      const char *accept;
      const char *agent;
            char *newEncoding;

      accept = getenv("HTTP_ACCEPT_CHARSET");

      if(accept && strstr(accept, "utf-8"))
	{
	  encoding = "utf-8"; /* use UTF8 if they have it ! */
	}
      else if( (agent = (const char *)getenv("HTTP_USER_AGENT")) &&
	   (strstr(agent, "MSIE 4") || strstr(agent, "MSIE 5")) &&
	   (strstr(agent, "Windows NT")))
	{
	  encoding = "utf-8"; /* MSIE can handle utf8 but doesn't request it. */
	}
#if 0	 
      else /* OK, see if we can find a valid codepage */
	{
	  /** OLD code to choose a preferred encoding for a locale. */

	  const UChar *defaultCodepage;
	  int32_t i = 0;
	  UErrorCode status;

	  FSWF( /*NOEXTRACT*/ "",""); /* just to init it.. */
	  if(gRB)
	    {
	      while( (defaultCodepage = ures_getArrayItem(gRB, "DefaultEncoding", i++, &status)) &&
		     U_SUCCESS(status) )
		{
		  newEncoding = malloc(u_strlen(defaultCodepage) + 1);
		  u_austrcpy((char*)newEncoding, defaultCodepage);

		  if(accept)
		    {
		      if(strstr(accept,newEncoding) || strchr(accept,'*'))
			encoding = newEncoding;
		      break;
		    }
		  else
		    free(newEncoding);
		}
	    }
	  else
	    {
	      /* default wasn't accepted. */
	    }

	}
#endif

    }


  if(encoding)
    {
      strcpy(lx->chosenEncoding, encoding);
    }

  /* Map transliterated/fonted : */
  if((0==strcmp(encoding, "transliterated")) ||
     (0==strcmp(encoding, "fonted"))) 
  {
    encoding = "usascii";
  }

  /* now, open the file */
  f = u_finit(lx->fOUT, locale, encoding);

  if(!f)
    {
      lx->couldNotOpenEncoding = encoding;
      f = u_finit(lx->fOUT, locale, "LATIN_1"); /* this fallback should only happen if the encoding itself is bad */
      if(!f)
      {
          fprintf(stderr, "Could not finit the file.\n");
          fflush(stderr);
    	return f; /* :( */
      }
    }



  /* we know that ufile won't muck withthe locale.
     But we are curious what encoding it chose, and we will propagate it. */
  if(encoding == NULL)
    {
      encoding = u_fgetcodepage(f);
      strcpy(lx->chosenEncoding, encoding);
    }


  FSWF_setLocale(lx->cLocale);

  return f;
}




