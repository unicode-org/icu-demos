/*
  TODO's:

  - javascript on the explore button

  -- lots of updates
    - initSortable needs to init only a part of the name. 

    - localize '>', ',' (list sep)

    - improve the 'default' pattern in the DTexplorer

  TOO MANY COMMENTS ;)
*/


/*
*******************************************************************************
*                                                                             *
* COPYRIGHT:                                                                  *
*   (C) Copyright International Business Machines Corporation, 1998, 1999     *
*   Licensed Material - Program-Property of IBM - All Rights Reserved.        *
*   US Government Users Restricted Rights - Use, duplication, or disclosure   *
*   restricted by GSA ADP Schedule Contract with IBM Corp.                    *
*                                                                             *
*******************************************************************************
*
* File listrb.c
*
* Modification History:
*
*   Date        Name        Description
*   7/14/1999    srl        Created
*   8/16/1999    srl        Display overhaul - locale status at bottom, etc.
*  10/14/1999    srl        Update for ICU1.3+, prepare for checkin!!
*******************************************************************************
*/


/* ==Note==

   this is a very UI intensive application. 
   It uses a whole pile of globals to maintain state and pass data 
   back and forth. As a result, it has a lot of 'convenience functions'
   that make sense in very limited contexts.

   If this thing was pulled into a multithreaded environment 
   they could probably be put in a paramblock of some sort. I was mostly
   just trying to keep the arg count and complexity down. For example, 
   OUT the ubiquitous output file.

                         -- Steven R. Loomis

*/
   

#include "stdio.h"
#include "stddef.h"
#include "string.h"
#include "ustdio.h"
#include "ucnv.h"
#include "ustring.h"
#include "udat.h"
#include "uloc.h"
#include "ures.h"
#include "ucol.h"
#include "ucal.h"
#include "ctype.h"
#include "lx_utils.h"
#include "ures_additions.h"
#include "decompcb.h"

/********************** Some Konstants **** and structs ***************/

#ifndef URLPREFIX
# define kStaticURLPrefix "/developerworks/opensource/icu/project"
#else
# define kStaticURLPrefix  URLPREFIX
#endif

/* #define kStaticURLPrefix ""  */


/* Define the following to enable 'demonstrate collation' */
#define kUSort "/home/srl/icu/source/samples/usort/usort"

#define kStatusBG "\"#EEEEEE\" " 


#define kShowStringCutoffSize 200
#define kShow2dArrayRowCutoff 5
#define kShow2dArrayColCutoff 5


/********************* prototypes ***************************/

int main(const char *argv[], int argc);

/* setup the UFILE */
UFILE *setLocaleAndEncodingAndOpenUFILE();

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
void chooseLocale(bool_t toOpen, const char *current, const char *restored);
void chooseConverter(const char *restored);
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
void explainStatus(UErrorCode status, const char *tag); /* utility fcn to explain an errorcode. [tag = #tag ] */
void showSort(const char *locale, const char *b);

void showExploreDateTimePatterns(UResourceBundle *rb, const char *locale, const char *b);
void showExploreNumberPatterns  (const char *locale, const char *b);

void showExploreButton(UResourceBundle *rb, const char *locale, const UChar *sampleString, const char *key);
void showExploreCloseButton(const char *locale, const char *frag);

bool_t didUserAskForKey(const char *key, const char *queryString);

/*  Pluggable UI.  Put these before and after each item. */
void showKeyAndStartItem(const char *key, const UChar *keyName, const char *locale, UErrorCode showStatus);
void showKeyAndStartItemShort(const char *key, const UChar *keyName, const char *locale, UErrorCode showStatus);
void showKeyAndEndItem(const char *key, const char *locale);


/* fetch string with fallback */
const UChar *FSWF(const char *key, const char *fallback);

/* write a string in \uXXXX format */
void writeEscaped(const UChar *s);

/* is this a locale we should advertise as supported? */
bool_t isSupportedLocale(const char *locale, bool_t includeChildren);


void printHelpTag(const char *helpTag, const UChar *str);
/* ex: printHelpImg("coffee", L"[coffee help symbol]", L"coffee-help.gif", L"BORDER=3"); */
void printHelpImg(const char *helpTag, const UChar *alt, const UChar *img, const UChar *options);

void exploreFetchNextPattern(UChar *dstPattern, const char* qs);
void exploreShowPatternForm(UChar *dstPattern, const char *locale, const char *key, const char* qs, double value, UNumberFormat *valueFmt);

/********************** globals ********************/
UFILE *OUT = NULL;                     /* used everywhere */
const char *couldNotOpenEncoding;      /* contains error string if nonnull */
const char *ourCharsetName = "iso-8859-1"; /* HTML friendly name of the current charset */

bool_t setEncoding = FALSE;            /* what is our state? What's setup? */
bool_t setLocale = FALSE;

UResourceBundle *gRB  = 0;             /* the RB to get our localized strs from (for this app) */
UResourceBundle *defaultRB = 0;        /* RB in the selected locale */

UErrorCode rbErr = U_ZERO_ERROR;         /* errorcode if above is null */

/* ====== locale navigation ===== */
MySortable      *locales   = NULL;     /* tree of locales */
MySortable      *curLocale = NULL;     /* Current locale */
char             curLocaleName[128];
MySortable      *parLocale = NULL;     /* Parent locale of current */

int32_t          numLocales = 0;
/************************ fcns *************************/

/*UChar x[2] = { 0x0030, 0x0000 };*/ /* test string */

const char *myBundlePath()
{
  static char gMyBundlePath[500] = "";

  if(gMyBundlePath[0] == 0)
    {
      strcpy(gMyBundlePath, icu_getDefaultDataDirectory());
      strcat(gMyBundlePath, "locexp/");
    }
  
  return gMyBundlePath;
}

/*** fetch string with fallback -------------------------------------------------*/
const UChar *FSWF(const char *key, const char *fallback)
{
  UErrorCode status = U_ZERO_ERROR;
  const UChar *str = 0;
  static UChar   gFSWFTempChars[1024];

  if(strlen(fallback) >  1020)
    ((char*)fallback)[1020] = 0; /* const violation. Tough. */

  if(gRB == 0)
    {
      gRB = ures_open(  myBundlePath(), NULL, &status);
      if(U_FAILURE(status))
	{
	  gRB = 0;
	  rbErr = status;
	}
    }

  status = U_ZERO_ERROR;

  if(gRB != 0)
    str = ures_get( gRB, key, &status);

  if(str == 0) /* failed to get a str */
    {
      /* fallback: use our temp buffer [NON MT safe] and do a strcpy.. */
      u_uastrcpy(gFSWFTempChars, fallback);
      str = malloc((u_strlen(gFSWFTempChars)+1) * sizeof(UChar)); /* LEAK but who cares, it's an error case */
      u_strcpy((UChar*)str,gFSWFTempChars);
    }

  return str;
}

/* main function. we write the outside html page here as well. ----------------------------*/

int main(const char *argv[], int argc)
{
  UErrorCode status;
  char *tmp;
  UChar subTitle[1024];
  int32_t n;

  /*  fprintf(stderr, "PID=%d\n", getpid()); */
  /*   system("sleep 20");  */
  
  status = U_ZERO_ERROR;

  ourCharsetName = MIMECharsetName(ucnv_getDefaultName()); /* for some sanity */

  OUT = setLocaleAndEncodingAndOpenUFILE();
  if(!OUT)
    doFatal("u_finit trying to open file", 0);
  
  setupLocaleTree();

  printf("content-type: text/html;charset=%s\r\n\r\n\r\n\n", ourCharsetName);
  fflush(stdout);

  /* 
     kore wa nandesuka?
     xi trid?

     
     {
    char langBuf[100];
    uloc_getLanguage(NULL,langBuf,100,&status);
  printf("Content-Language: %s\r\n", langBuf);
  }*/

  printf("\n" );


  fflush(stdout); /* and that, as they say, is that.  All UFILE from here.. */

  u_fprintf(OUT,"<HTML>");

  u_fprintf(OUT, "\r\n<!-- \r\n\r\n   Hello, HTML explorer :)  Don't know how readable this HTML will be!\r\n  If you have any questions, comments, [gasp] bugs, or\r\n [hopehope] improvements, please drop some knowledge to:\r\n    icu4c@us.ibm.com THX! \r\n                 ~srl \r\n\r\n-->");

  u_fprintf(OUT, "<HEAD BGCOLOR=\"#DFDDDD\"><TITLE>");

  printPath(curLocale, curLocale, FALSE);
  if(strstr(getenv("QUERY_STRING"), "EXPLORE"))
    u_fprintf(OUT, " &gt; %U", FSWF("exploreTitle", "Explore"));

  u_fprintf(OUT, "</TITLE>\r\n");
  
  if(!getenv("PATH_INFO") || !getenv("PATH_INFO")[0])
    u_fprintf(OUT, "<BASE HREF=\"http://%s%s/\">\r\n", getenv("SERVER_NAME"), getenv("SCRIPT_NAME")); /* Ensure that all relative paths have the cgi name followed by a slash. */
  
  
  u_fprintf(OUT, "%U", 
	    FSWF ( /* NOEXTRACT */ "htmlHEAD",
		   "</HEAD>\r\n<BODY BGCOLOR=\"#FFFFFF\" > \r\n")
	    );
  /* now see what we're gonna do */
  tmp = getenv ( "QUERY_STRING" );
  
  if(strstr(tmp,"EXPLORE"))
    {
      printHelpImg("display", 
		   FSWF("display_ALT", "Display Problem?"),
		   FSWF("display_GIF", "displayproblems.gif"),
		   FSWF("display_OPTIONS", "ALIGN=RIGHT"));
		  
      u_fprintf(OUT, "<FONT SIZE=+1>");
      printPath(curLocale, curLocale, FALSE);
      u_fprintf(OUT, "</FONT><P>");


    }
  else
    {
      if(tmp && tmp[0]  && !curLocale && (tmp[0] == '_'))
	{
	  UChar dispName[1024];
	  UErrorCode stat = U_ZERO_ERROR;
	  dispName[0] = 0;
	  uloc_getDisplayName(curLocaleName, NULL, dispName, 1024, &stat);
	  
	  u_fprintf(OUT, "<UL><B>%U  [%U]</B></UL>\r\n",
		    FSWF("warningInheritedLocale", "Note: You're viewing a non existent locale. The ICU will support this with inherited information. But the Locale Explorer is not designed to understand such locales. Inheritance information may be wrong!"), dispName);
	}


      u_fprintf(OUT, "<TABLE WIDTH=100%%><TR><TD ALIGN=LEFT VALIGN=TOP>");

      u_fprintf(OUT, "<FONT SIZE=+1>");
      printPath(curLocale, curLocale, TRUE);
      u_fprintf(OUT, "</FONT><P>");
      
      u_fprintf(OUT, "</TD><TD ROWSPAN=2 ALIGN=RIGHT VALIGN=TOP WIDTH=1>");
      
      printHelpImg("display", 
		   FSWF("display_ALT", "Display Problem?"),
		   FSWF("display_GIF", "displayproblems.gif"),
		   FSWF("display_OPTIONS", "ALIGN=RIGHT"));
		  
      u_fprintf(OUT, "\r\n</TD></TR><TR><TD>");

      if(curLocale && curLocale->nSubLocs)
	{
	  u_fprintf(OUT, "%U<BR><UL>", FSWF("sublocales", "Sublocales:"));

	  mySort(curLocale, &status, FALSE); /* Sort sub locales */

	  for(n=0;n<curLocale->nSubLocs;n++)
	    {
	      if(n != 0)
		u_fprintf(OUT, ", ");
	      u_fprintf(OUT, "<A HREF=\"?_=%s\">%U</A>",
			curLocale->subLocs[n].str,
			curLocale->subLocs[n].ustr);
	    }
	  u_fprintf(OUT, "</UL>");
	}
      u_fprintf(OUT, "</TD></TR></TABLE>\r\n");


    }


  if ( tmp == NULL )
    tmp = ""; /* for sanity */

  if( ((!*tmp) && !setLocale && !setEncoding) || strstr(tmp, "PANICDEFAULT")) /* They're coming in cold. Give them the spiel.. */
  {
    u_fprintf(OUT, "</H2>"); /* close the 'title text */

	u_fprintf(OUT, "<UL>%U<P>\r\n",
	  FSWF("introSpiel", "This demo illustrates the IBM International Classes for Unicode localization data.  The classes cover 45 different languages, further divided into 154 geographic locales.  For each language, data such as days of the week, months, and their abbreviations are defined.  <P> The IBM Classes for Unicode are open-source. See the <A HREF=../project>ICU Project Page</A>  for further information."));

	u_fprintf(OUT, "</UL>");
  }

  /* Logic here: */
  if( /* !setLocale || */  !strncmp(tmp,"locale", 6))     /* ?locale  or not set: pick locale */
    {
      char *restored;

      restored = strchr(tmp, '&');
      if(restored)
	{
	  restored ++;
	}

      if(!restored)
	restored = "converter"; /* what to go on to */

      if(setLocale)
	u_fprintf(OUT, ": %U</H2>\r\n", FSWF("changeLocale", ": Change Your Locale"));
      else
	u_fprintf(OUT, ": %U</H2>\r\n", FSWF("chooseLocale", ": Choose Your Locale."));


      u_fprintf(OUT, "<TABLE WIDTH=\"70%\"><TR>");
      

      u_fprintf(OUT, "<TD COLSPAN=2 ALIGN=RIGHT>");

      printHelpTag("chooseLocale", NULL);

      u_fprintf(OUT, "</TD></TR></TABLE>\r\n");

      chooseLocale(FALSE, (char*)uloc_getDefault(), restored);
    }
  else if (!strncmp(tmp,"converter", 9))  /* ?converter */
    {
      char *restored;

      restored = strchr(tmp, '&');
      if(restored)
	  restored ++;

      /*
      if(setEncoding)
	u_fprintf(OUT, ": %U</H2>\r\n", FSWF("changeEncoding", "Change Your Encoding"));
      else
	u_fprintf(OUT, ": %U</H2>\r\n", FSWF("chooseEncoding", "Choose Your Encoding"));
      */
      u_fprintf(OUT, "<HR>");
      chooseConverter(restored);
    }
  else
    {
      listBundles(tmp);
    }
  
  u_fprintf(OUT, "<P><BR><P><P><BR><A NAME=\"mySettings\"></A><P><P><P><P><HR>");
  printStatusTable();
  u_fprintf(OUT, "<I>%U</I> 1.3.1+<BR>", FSWF("poweredby", "Powered by ICU"));
  u_fprintf(OUT, "%U", date(NULL,UDAT_FULL,&status));
  
  if(!gRB)
    {
      /* No reason to use FSWF, this error means we have nothing to fetch strings from! */
      u_fprintf(OUT, "<HR><B><I>Note: Could not open our private resource bundle %s, err %d </I></B><P>\r\n",
		myBundlePath(), rbErr);
    }

  
  u_fprintf(OUT, "%U", FSWF( /* NOEXTRACT */ "htmlTAIL", "<!-- No HTML footer -->"));
    
  /* a last resort. will switch to English if they get lost.. */
  /* DO NOT localize the following */
  /* What this does:  
         - brings them to the 'choose your locale' pane in English, then
	 - brings them to the 'choose your encoding' pane in their locale, then
	 - lists the locales to browse
  */

#ifndef NO_LOCALE_CHANGE
  u_fprintf(OUT, "<A HREF=\"%s/en/iso-8859-1/?PANICDEFAULT\"><IMG SRC=\"%s/localeexplorer/incorrect.gif\" ALT=\"Click here if text displays incorrectly\"></A>", getenv("SCRIPT_NAME"), kStaticURLPrefix);
#endif

  if(!strcmp(uloc_getDefault(),"klingon"))
    u_fprintf(OUT, "<P>Thank you for using the ICU LocaleExplorer, from %s compiled %s %s<P>\r\n", LXHOSTNAME, __DATE__, __TIME__);

  u_fprintf(OUT, "</BODY></HTML>\r\n");

  u_fclose(OUT);

  if(gRB)
    ures_close(gRB);

  if(defaultRB)
    ures_close(defaultRB);

}

const UChar *defaultLanguageDisplayName()
{
  UErrorCode status = U_ZERO_ERROR;
  static UChar displayName[1024] = { 0x0000 };

  if(displayName[0] == 0x0000)
    {
      uloc_getDisplayLanguage(uloc_getDefault(), uloc_getDefault() ,displayName, 1024, &status);
    }
  
  return displayName;
}

/* snag the locale, followed optionally by the encoding, from the path_info -----------------
This is the only fcn which should be calling ucnv_setDefaultName
*/

#ifdef HACKY_DEV_TIEIN
  extern int srl_mode;
#endif

UFILE *setLocaleAndEncodingAndOpenUFILE()
{
  char *pi;
  char *tmp;
  const char *locale = NULL;
  const char *encoding = NULL;
  UErrorCode status = U_ZERO_ERROR;
  char *acceptLanguage;
  char newLocale[100];
  UFILE *f;

  uloc_setDefault("en", &status); /* BASELINE */

  locale = (const char *)uloc_getDefault();
  encoding = ucnv_getDefaultName(); 

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
	  uloc_setDefault(locale, &status);
	  if(U_FAILURE(status))
	    {
	      doFatal("uloc_setDefault", status);
	      /* doesn't return */
	    }
	  setLocale = TRUE;
	}

      if(tmp) /* have encoding */
	{
  
	  tmp++; /* skip '/' */
  
	  pi = tmp;

	  tmp = strchr(tmp, '/');
	  if(tmp)
	    *tmp = 0;
  
	  if(*pi) /* don't want 0 length encodings */
	    {
	      encoding = pi;
	      setEncoding = TRUE; 



	      ourCharsetName = MIMECharsetName(encoding);
	    }
	}
    }

  if(!setLocale && (acceptLanguage=getenv("HTTP_ACCEPT_LANGUAGE")) && acceptLanguage[0] )
    {

      /* OK, if they haven't set a locale, maybe their web browser has. */
	if(!(tmp=strchr(acceptLanguage,','))) /* multiple item separator */
      if(!(tmp=strchr(acceptLanguage,'='))) /* strength separator */
	  tmp = acceptLanguage + strlen(acceptLanguage);

      strncpy(newLocale, acceptLanguage, my_min(100,tmp-acceptLanguage));
      newLocale[my_min(100,tmp-acceptLanguage)] = 0;
      /*      fprintf(stderr," NL=[%s], al=[%s]\r\n", newLocale, acceptLanguage);
	      fflush(stderr);*/

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
	uloc_setDefault(newLocale, &status);
      status = U_ZERO_ERROR;

      /* that might at least get something.. It's better than defaulting to en_US */
    }
  
  if(!setEncoding)
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
      else /* OK, see if we can find a valid codepage */
	{
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
		      fprintf(stderr,"Looking for %s in %s..\n", newEncoding, accept);
		      if(strstr(accept,newEncoding) || strchr(accept,'*'))
			encoding = newEncoding;
		      break;
		    }
		  else
		    free(newEncoding);
		}

	      fprintf(stderr,"Got %d on try %d for defaultencoding [bnd%s]\n", status, i-1, uloc_getDefault() );
	    }
	  else
	    {
	      /* default wasn't accepted. */
	    }

	}

    }

  if(encoding)
    {
      ourCharsetName = MIMECharsetName(encoding);

#ifdef HACKY_DEV_TIEIN

      if(!strcmp(encoding, "x-devanagari"))
	{
	  ourCharsetName = encoding;
	  srl_mode = 3; /* turn on dev processing */
	  encoding = "iso-8859-1";
	}
#endif

      ucnv_setDefaultName(encoding);
    }

  /* Open an RB in the default locale */
  defaultRB = ures_open(NULL, NULL, &status);

  /* now, open the file */
  f = u_finit(stdout, locale, encoding);

  if(!f)
    {
      couldNotOpenEncoding = encoding;
      f = u_finit(stdout, locale, "LATIN_1"); /* this fallback should only happen if the encoding itself is bad */
      if(!f)
	return f; /* :( */
    }


  /* we know that ufile won't muck withthe locale.
     But we are curious what encoding it chose, and we will propagate it. */
  if(encoding == NULL)
    {
      encoding = u_fgetcodepage(f);
      ucnv_setDefaultName(encoding);
    }

  /* put our special error handler in */
  /*  ucnv_setFromUCallBack((UConverter*)u_fgetConverter(f), &UCNV_FROM_U_CALLBACK_BACKSLASH_ESCAPE_HTML, &status); */

  ucnv_setFromUCallBack((UConverter*)u_fgetConverter(f), &UCNV_FROM_U_CALLBACK_DECOMPOSE, &status);

  /* Change what DECOMPOSE calls as it's last resort */
  DECOMPOSE_lastResortCallback = UCNV_FROM_U_CALLBACK_BACKSLASH_ESCAPE_HTML;

  return f;
}

void setupLocaleTree()
{
  const char *qs, *amp;
  char       *loc = curLocaleName;

  /* setup base locale */
  locales = createLocaleTree(NULL, &numLocales);

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
      curLocale = findLocale(locales, loc);

      if(curLocale)
	parLocale = curLocale->parent;
    }
}
  
/* do a fatal error. may not have a content type yet. --------------------------------------- */
/* TODO: this doesn't actually work yet. Should it be localized ? probably. */
void doFatal(const char *what, UErrorCode err)
{
  fprintf(stdout, "Content-type:text/html\r\n\r\n");
  fprintf(stdout, "<TITLE>ICU LocaleExplorer: Error</TITLE>\r\n");
  fprintf(stdout, "<H1>ICU LocaleExplorer: Error</H1>\r\n");
  fprintf(stdout, "<UL>An error of type %d occured while trying to %s.</UL><HR><P>\r\n",err,what);
  fprintf(stderr, "listrb: err %d trying to %s\n",err,what);
  fprintf(stdout, "You can try <A HREF=\"%s\">starting over</A>, or complain to srl.<P>\r\n",
	 getenv("SCRIPT_NAME"));
  fflush(stdout);
  exit(0);
}

void writeEscaped(const UChar *s)
{
  UErrorCode status = U_ZERO_ERROR;

  ucnv_setFromUCallBack((UConverter*)u_fgetConverter(OUT), &UCNV_FROM_U_CALLBACK_BACKSLASH_ESCAPE, &status); 

  if(u_strchr(s, 0x00A0))
    {
      while(*s)
	{
	  if(*s == 0x00A0)
	    u_fprintf(OUT, " ");
	  else
	    u_fprintf(OUT, "%K", *s);
	  
	  s++;
	}
    }
  else
    u_fprintf(OUT, "%U", s); 
  
  /* should 'get/restore' here. */
  /*  ucnv_setFromUCallBack((UConverter*)u_fgetConverter(OUT), &SubstituteWithValueHTML, &status); */
  ucnv_setFromUCallBack((UConverter*)u_fgetConverter(OUT), &UCNV_FROM_U_CALLBACK_DECOMPOSE, &status);
}

void writeEscapedQuery(const UChar *s)
{
  UErrorCode status = U_ZERO_ERROR;

  ucnv_setFromUCallBack((UConverter*)u_fgetConverter(OUT), &UCNV_FROM_U_CALLBACK_BACKSLASH_ESCAPE, &status); 

  if(u_strchr(s, 0x00A0))
    {
      while(*s)
	{
	  if(*s == 0x00A0)
	    u_fprintf(OUT, " ");
	  else
	    u_fprintf(OUT, "%K", *s);
	  
	  s++;
	}
    }
  else
    u_fprintf(OUT, "%U", s); 
  
  /* should 'get/restore' here. */
  /*  ucnv_setFromUCallBack((UConverter*)u_fgetConverter(OUT), &SubstituteWithValueHTML, &status); */
  ucnv_setFromUCallBack((UConverter*)u_fgetConverter(OUT), &UCNV_FROM_U_CALLBACK_DECOMPOSE, &status);
}


/* print that little box in the TR corner ----------------------------------------------------- */

void printStatusTable()
{
  UChar myChars[1024];
  UErrorCode status;

  status = U_ZERO_ERROR;

  u_fprintf(OUT, "<TABLE BORDER=0 BGCOLOR=\"#DDDDDD\" ALIGN=RIGHT><TR>"); 
  u_fprintf(OUT, "<TD ROWSPAN=2>");

  u_fprintf(OUT, "%U",
	   FSWF("statusTableHeader", "Your settings:<BR>(click to change)"));
  u_fprintf(OUT, "</TD>");
  
#ifndef NO_LOCALE_CHANGE
  u_fprintf(OUT, "<TD ALIGN=RIGHT><B>");

  u_fprintf(OUT, "%U</B></TD><TD>", FSWF("myLocale", "Locale:"));

  u_fprintf(OUT, "<A HREF=\"?locale");
  if(strncmp(getenv("QUERY_STRING"), "locale",6))
    u_fprintf(OUT,"&%s", getenv("QUERY_STRING"));
  u_fprintf(OUT, "\">");

  uloc_getDisplayName(NULL, NULL, myChars, 1024, &status);
  u_fprintf(OUT, "%U", myChars);

  u_fprintf(OUT, "</A>\r\n");

  
  u_fprintf(OUT, "</TD>");
#else
  if(!isSupportedLocale(uloc_getDefault(), TRUE))
    {
      u_fprintf(OUT, "<TD COLSPAN=2 ALIGN=RIGHT><FONT COLOR=\"#FF0000\">");
      u_fprintf_u(OUT, FSWF("locale_unsupported", "This display locale, <U>%s</U>, is unsupported."), uloc_getDefault());
      u_fprintf(OUT, "</FONT></TD>");
    }
#endif



  u_fprintf(OUT, "</TR>");

  /**  Converter **/  
  u_fprintf(OUT, "<TR><TD ALIGN=RIGHT><B>");

  u_fprintf(OUT, "%U</B></TD><TD>", FSWF("myConverter", "Encoding:"));
  
  u_fprintf(OUT, "<A HREF=\"?converter");
  if(strncmp(getenv("QUERY_STRING"), "converter",9))
    u_fprintf(OUT,"&%s", getenv("QUERY_STRING"));
  u_fprintf(OUT, "\">");

  u_fprintf(OUT, "<FONT SIZE=+1>%s</FONT>", ourCharsetName);
  u_fprintf(OUT, "</A>\r\n");
  
  u_fprintf(OUT, "</TD></TR>");
#if 0
  if(couldNotOpenEncoding)
  {
    /* Localize this when it actually works! */
    u_fprintf(OUT,"<TR><TD COLSPAN=2><FONT COLOR=\"#FF0000\">Warning, couldn't open the encoding '%s', using a default.</FONT></TD></TR>\r\n", couldNotOpenEncoding); 
  }
#endif
  u_fprintf(OUT, "</TABLE>\r\n");
}

void printPath(const MySortable *leaf, const MySortable *current, bool_t styled)
{

  if(!leaf) /* top level */
    {
      if(styled) 
	u_fprintf(OUT, "<A HREF=\"?\">"); /* Reset */
      
      u_fprintf(OUT, "%U",
		FSWF("title", "ICU LocaleExplorer"));
      
      if(styled) 
	u_fprintf(OUT, "</A>");

      return;
    }

  
  /* reverse order recursively */
  printPath(leaf->parent,current,styled);


  u_fprintf(OUT, " &gt; ");
  


  if(styled)
    {
      if(leaf == current)
	u_fprintf(OUT, "<B>");

      u_fprintf(OUT, "<A HREF=\"?_=%s\">", leaf->str);
    }

  
  
  u_fprintf(OUT, "%U", leaf->ustr);



  if(styled)
    {
      u_fprintf(OUT, "</A>");

      if(leaf == current)
	u_fprintf(OUT, "</B>");

    }
}


static void printLocaleLink(bool_t toOpen, MySortable *l, const char *current, const char *restored)
{
  u_fprintf(OUT, "<A HREF=\"");
  
  if(toOpen == TRUE)
    {
      u_fprintf(OUT, "%s/%s/",
		getenv("SCRIPT_NAME"),
		(char*)uloc_getDefault());
      if(setEncoding)
	u_fprintf(OUT,"%s/", ucnv_getDefaultName());	  
	  u_fprintf(OUT,"?_=%s&", l->str);
    }
  else
    {
      u_fprintf(OUT, "%s/%s/",
		getenv("SCRIPT_NAME"),
		l->str);
      if(setEncoding)
	u_fprintf(OUT,"%s/", ucnv_getDefaultName());
      
      if(restored)
	u_fprintf(OUT, "?%s", restored);
    }
  
  
  u_fprintf(OUT,"\">");
  u_fprintf(OUT, " %U", l->ustr);
  
  u_fprintf(OUT,"</A>");
  
}

static void printLocaleAndSubs(bool_t toOpen, MySortable *l, const char *current, const char *restored)
{
  int32_t n;

  printLocaleLink(toOpen,l,current,restored);
  
  if(l->nSubLocs)
    {
	u_fprintf(OUT, "&nbsp;[<FONT SIZE=-1>&nbsp;");

      for(n=0;n<(l->nSubLocs);n++)
	{
	  printLocaleAndSubs(toOpen, &(l->subLocs[n]), current, restored);
	}
      
	u_fprintf(OUT, "&nbsp;</FONT SIZE=-1>]");
    }
}


/* chooselocale --------------------------------------------------------------------------- */
void chooseLocale(bool_t toOpen, const char *current, const char *restored)
{
  bool_t  hit = FALSE;
  int32_t n, j;
  UErrorCode status = U_ZERO_ERROR;

  u_fprintf(OUT, "<TABLE BORDER=2 CELLPADDING=2 CELLSPACING=2>\r\n");

  u_fprintf(OUT, "<TR><TD><B>%U</B></TD><TD><B>%U</B></TD></TR>\r\n",
	    FSWF("localeList_Locale", "Languages"),
	    FSWF("localeList_Sublocale", "Countries"));

  u_fprintf(OUT, "<TR><TD COLSPAN=2><A HREF=\"?_=%s\">%U</A></TD></TR>\r\n",
	    locales->str, locales->ustr); /* default */ 

  mySort(locales, &status, TRUE); /* need the whole thing sorted */

  for(n=0;n<locales->nSubLocs;n++)
    {
      u_fprintf(OUT, "<TR>\r\n");


      hit = !strcmp(locales->subLocs[n].str,current);

      if(hit)
	{
	  u_fprintf(OUT,"<TD VALIGN=TOP BGCOLOR=\"#FFDDDD\">");
	}
      else
	u_fprintf(OUT,"<TD VALIGN=TOP>");

      if(hit)  
	u_fprintf(OUT, "<b>");
      
      
      printLocaleLink(toOpen, &(locales->subLocs[n]), current, restored);

      if(hit)
	u_fprintf(OUT, "</b>");
      
      u_fprintf(OUT, "</FONT>");

      u_fprintf(OUT, "</TD>");
      
      if(locales->subLocs[n].nSubLocs)
	{
	  u_fprintf(OUT, "<TD>");
	  
	  for(j=0;j< (locales->subLocs[n].nSubLocs); j++)
	    {
	      if(j>0)
		u_fprintf(OUT, ", ");
	      
	      printLocaleAndSubs(toOpen, &(locales->subLocs[n].subLocs[j]), current, restored);
	      
	    }

	  u_fprintf(OUT, "</TD>");
	}

      u_fprintf(OUT, "</TR>\r\n");


    }

  u_fprintf(OUT, "</TABLE>\r\n");
}


/* chooseconverter ----------------------------------------------------------------------------- */
void chooseConverter(const char *restored)
{
  int32_t  ncnvs;
  int32_t  i;
  int32_t  COLS = 8; /* number of columns */
  int32_t rows;
  const char *cnvMime, *defaultMime;

  defaultMime = MIMECharsetName(ucnv_getDefaultName());

  
 if(!restored)
	restored = "";

  u_fprintf(OUT,"<A HREF=\"?%s\"><H2>%U%s%U</H2></A>\r\n",
	     restored,
	     FSWF("encodingOK0", "Click here if the encoding '"),
	     ucnv_getDefaultName(),
	     FSWF("encodingOK1", "' is acceptable, or choose one from below."));


  ncnvs = ucnv_countAvailable();

  u_fprintf_u(OUT, FSWF("convsAvail","%d converters available."), ncnvs);

  rows = (ncnvs / COLS) + 1;

  u_fprintf(OUT, "<P><TABLE cellpadding=3 cellspacing=2 >\r\n"
	         "<TR>\r\n");

  for(i=0;i<(rows * COLS);i++)
    {
      int32_t theCell;
      bool_t hit;
      const char *cnv = NULL; 
      

      u_fprintf(OUT, "<!-- %d -->", i);

      theCell=(rows * (i%COLS)) + (i/COLS); 
      if(theCell >= ncnvs)
	{
	  u_fprintf(OUT,"<td></td>");
	  if(((i+1)%COLS) == 0)
	    u_fprintf(OUT,"</TR>\n<TR>");
	  continue;
	}

      cnv = (const char *)ucnv_getAvailableName(theCell);

      if(!cnv)
	continue;

      /*      if(!cnv)
	{
	  fprintf(stderr,"**************** END OF CNVS AT %d\n", i); fflush(stderr);
	  return;
	  } */
      cnvMime = MIMECharsetName(cnv);

      hit = !strcmp(cnvMime, defaultMime);

      if(hit)
	u_fprintf(OUT,"<TD BGCOLOR=\"#FFDDDD\">");
      else
	u_fprintf(OUT,"<TD>");

      u_fprintf(OUT, "<FONT SIZE=-1>");

      if(hit)  
	u_fprintf(OUT, "<b>");
      
      u_fprintf(OUT, "<A HREF=\"");


      u_fprintf(OUT, "%s/%s/",
		getenv("SCRIPT_NAME"),
		uloc_getDefault());
      u_fprintf(OUT,"%s/", cnv);
      if(restored)
	u_fprintf(OUT, "?%s", restored); 
      
      u_fprintf(OUT,"\">");
      u_fprintf(OUT, "%s", MIMECharsetName(cnv));
      /*       theCnvale.getDisplayName(o.GetLocale(),tmp); */
      u_fprintf(OUT,"</A>\n");
      
      if(hit)
	u_fprintf(OUT, "</b>");
      
      u_fprintf(OUT, "</FONT>");
      u_fprintf(OUT, "</FONT>");
      u_fprintf(OUT, "</TD>");
      if(((i+1)%COLS) == 0)
	u_fprintf(OUT, "</TR>\n<TR>");
    }
  u_fprintf(OUT,"</TABLE>\r\n");
 

  { /* Todo: localize */
    const char *ts = "??";
    UErrorCode status;
    char tmp[100];
    UConverter *u = u_fgetConverter(OUT);
    

    status = U_ZERO_ERROR;

    u_fprintf(OUT,"<HR>");
    u_fprintf(OUT,"<H3>Information about <B><TT>%s</TT></B></H3>\r\n",
	      ucnv_getName(u, &status));
    u_fprintf(OUT,"<UL>");
    
    u_fprintf(OUT,"  <LI>ID = %d, platform=%s\n",
	      ucnv_getCCSID(u,&status),
	      (ucnv_getPlatform(u,&status) == UCNV_IBM) ? "IBM" : "other");
       
	      
    u_fprintf(OUT,"  <LI>min/max chars: %d to %d\n",
	      ucnv_getMinCharSize(u),
	      ucnv_getMaxCharSize(u));

    u_fprintf(OUT,"  <LI>Type=");
    switch(ucnv_getType(u))
      {
      case UCNV_UNSUPPORTED_CONVERTER:  ts = "Unsupported"; break;
      case UCNV_SBCS: ts = "Single Byte Character Set"; break;
      case UCNV_DBCS: ts = "Double Byte Character Set"; break;
      case UCNV_MBCS: ts = "Multiple Byte Character Set (variable)"; break;
      case UCNV_LATIN_1: ts = "Latin-1"; break;
      case UCNV_UTF8: ts = "UTF-8 (8 bit unicode)"; break;
      case UCNV_UTF16_BigEndian: ts = "UTF-16 Big Endian"; break;
      case UCNV_UTF16_LittleEndian: ts = "UTF-16 Little Endian"; break;
      case UCNV_EBCDIC_STATEFUL: ts = "EBCDIC Stateful"; break;
      case UCNV_ISO_2022: ts = "iso-2022 meta-converter"; break;
      case UCNV_JIS: ts = "JIS (Japan Industrial Society?)"; break;
      case UCNV_EUC: ts = "EUC"; break; /* ? */
      case UCNV_GB: ts = "GB"; break; /* ? */
      default: ts = tmp; sprintf(tmp, "Unknown type %d", ucnv_getType(u));
      }
    u_fprintf(OUT, "%s\n", ts);

    u_fprintf(OUT, "<A TARGET=unibrowse HREF=\"/cgi-bin/unibrowse.sh/%s/\">Browse Unicode in this codepage</A>\r\n", defaultMime);
    
    u_fprintf(OUT, "</UL>\r\n");
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
      chooseLocale(TRUE, b, "");
      return;
    }

  u_fprintf(OUT, "<TABLE BORDER=0 CELLSPACING=2>\r\n");

  u_fprintf(OUT, "<TR><TD COLSPAN=2>");

  u_fprintf(OUT, "<TD ALIGN=left>");
  
  myRB = ures_open(NULL, locale, &status);
  if(U_FAILURE(status))
    {
      u_fprintf(OUT,"</TR></TABLE><B>An error occured [%d] opening that resource bundle [%s]. Perhaps it doesn't exist? </B><P><HR>\r\n",status, locale);
      return;
    }

  explainStatus(status,"top");

  /*  showFlagImage(locale, " "); */
  /*   u_fprintf(OUT, "</TD></TR><TR><TD COLSPAN=2>"); */

  /* analyze what kind of locale we've got.  Should this be a little smarter? */

#if 0
  u_fprintf(OUT, "%U", FSWF("localeDataWhat", "This page shows the localization data for the locale listed at left. "));

  if(strlen(locale)==2) /* just the language */
    {
      u_fprintf(OUT, " %U",FSWF("localeDataLanguage","No country is specified, so the data is generic to this language."));
    }
  else if(!strcmp(locale,"default"))
    {
      u_fprintf(OUT, " %U", FSWF("localeDataDefault", "This is the default localization data, which will be used if no other installed locale is specified."));
    }
  else if(locale[2] == '_')
    {
      if(strlen(locale) == 5)
	{
	   u_fprintf(OUT, " %U", FSWF("localeDataLangCountry", "This Locale contains data for this language, as well as conventions for this particular country."));
	 }
       else
	 {
	  u_fprintf(OUT, " %U", FSWF("localeDataLangCountryVariant", "This Locale contains data for this language, as well as conventions for a variant within this particular country."));
	}
    }

  if(strstr(locale, "_EURO"))
    {
      u_fprintf(OUT, " %U", FSWF("localeDataEuro", "This Locale contains currency formatting information for use with the Euro currency."));
    }
#endif
  u_fprintf(OUT, "</TD></TR>\r\n");

  u_fprintf(OUT, "</TABLE>");
  
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

  else if (strstr(b, "EXPLORE_CollationElements"))
    {
      showKeyAndStartItem("EXPLORE_CollationElements", 
			  FSWF("EXPLORE_CollationElements", "Collation (sorting) Example"),
			  locale,
			  U_ZERO_ERROR);

#if 0
      u_fprintf(OUT, "<A HREF=\"?_=%s#usort\"><IMG border=0 width=16 height=16 SRC=\"%s/localeexplorer/opened.gif\" ALT=\"\"> %U</A>\r\n",
		locale,
		kStaticURLPrefix,
		FSWF("usortHide", "Hide this example"));
#endif
      
      u_fprintf(OUT, "%U<P>", FSWF("usortWhat","This example demonstrates sorting (collation) in this locale."));
      showSort(locale, b);
      
      u_fprintf(OUT, "</TD>");

      u_fprintf(OUT, "<TD VALIGN=TOP ALIGN=RIGHT>");
      printHelpTag("EXPLORE_CollationElements", NULL);
      u_fprintf(OUT, "</TD>");

#if 0
      if(!strstr(usort))
	{
	  u_fprintf(OUT, "<A HREF=\"?_=%s&usort&_#usort\"><IMG border=0 width=16 height=16 SRC=\"%s/localeexplorer/closed.gif\" ALT=\"\"> %U</A>\r\n",
		    locale,
		    kStaticURLPrefix,
		    FSWF("usortShow", "Show this example"));
	  
	  
	  u_fprintf(OUT, "<BR>%U</TD>",
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
	const UChar *zsDesc[6];
	zsDesc[0] = FSWF("zoneStrings0", "Canonical");
	zsDesc[1] = FSWF("zoneStrings1", "Normal Name");
	zsDesc[2] = FSWF("zoneStrings2", "Normal Abbrev");
	zsDesc[3] = FSWF("zoneStrings3", "Summer/DST Name");
	zsDesc[4] = FSWF("zoneStrings4", "Summer/DST Abbrev");
	zsDesc[5] = FSWF("zoneStrings5", "Example City");
	zsDesc[6] = 0;

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
  /*  u_fprintf(OUT, kItemTrailer); */


  /*   u_fprintf(OUT, "</TABLE>\r\n"); */
  
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

void showCollationElements( UResourceBundle *rb, const char *locale, const char *queryString, const char *key )
{
  
  UErrorCode status = U_ZERO_ERROR;
  const UChar *s  = 0;
        UChar *comps = 0;
  bool_t bigString = FALSE; /* is it big? */
  bool_t userRequested = FALSE; /* Did the user request this string? */
  const char *tmp1, *tmp2;
  int32_t len;

  s = ures_get(rb, key, &status);

  if(!s || !s[0] || (status == U_MISSING_RESOURCE_ERROR))
    {
      UCollator *coll;

      /* Special case this to fetch what REALLY happens in this case ! */
      status = U_ZERO_ERROR;

      coll = ucol_open(locale, &status);
      if(U_SUCCESS(status))
	{
	  s = ucol_getRules(coll, &len);

	  ucol_close(coll);
	}

      /* fix the status */
      if(!strcmp(locale, "default"))
	status = U_ZERO_ERROR;
      else
	status = U_USING_DEFAULT_ERROR;
    }
  else
    len = u_strlen(s);

  if(U_SUCCESS(status) && ( len > kShowStringCutoffSize ) )
    {
      bigString = TRUE;
      userRequested = didUserAskForKey(key, queryString);
    }

  showKeyAndStartItemShort(key, NULL, locale, status);

  u_fprintf(OUT, "</TD></TR><TR><TD></TD><TD>");
  
  /* Ripped off from ArrayWithDescription. COPY BACK */
  
  showExploreButton(rb,locale,
		    FSWF("EXPLORE_CollationElements_sampleString","bad\\u000DBad\\u000DBat\\u000Dbat\\u000Db\\u00E4d\\u000DB\\u00E4d\\u000Db\\u00E4t\\u000DB\\u00E4t"),
		    "CollationElements");



  u_fprintf(OUT, "</TD>"); /* Now, we're done with the ShowKey.. cell */



  u_fprintf(OUT, "</TR><TR><TD COLSPAN=2>");


  if(U_SUCCESS(status))
    {

      if(bigString && !userRequested) /* it's hidden. */
	{
	  /* WIERD!! outputting '&#' through UTF8 seems to be -> '?' or something 
	     [duh, HTML numbered escape sequence] */
	  u_fprintf(OUT, "<A HREF=\"?_=%s&SHOW%s&_#%s\"><IMG BORDER=0 WIDTH=16 HEIGHT=16 SRC=\"%s/localeexplorer/closed.gif\" ALT=\"\">%U</A>\r\n<P>", locale, key,key, kStaticURLPrefix, FSWF("bigStringClickToShow","(Omitted due to size. Click here to show.)"));
	  u_fprintf(OUT, "</TD><TD></TD>");
	}
      else
	{
	  if(bigString)
	    {
	      u_fprintf(OUT, "<A HREF=\"?_=%s#%s\"><IMG border=0 width=16 height=16 SRC=\"%s/localeexplorer/opened.gif\" ALT=\"\"> %U</A><P>\r\n",
			locale,
			key,
			kStaticURLPrefix,
			FSWF("bigStringHide", "Hide"));
	    }
	  
	  if(U_SUCCESS(status))
	    {
	      
	      comps = malloc(sizeof(UChar) * (len*2));


	      len = u_normalize(s,
			  len,
			  UCOL_DECOMP_COMPAT_COMP_CAN,
			  0,
			  comps,
			  len*2,
			  &status);


	      if(*comps == 0)
		{
		  u_fprintf(OUT, "<I>%U</I>", FSWF("empty", "(Empty)"));
		}
	      else while(len--)
		{
		  
		  if(*comps == '&')
		    u_fprintf(OUT, "<P>");
		  else if(*comps == '<')
		    u_fprintf(OUT, "<BR>&nbsp;");
		  
		  if((*comps == 0x000A) || u_isprint(*comps))
		    u_fprintf(OUT, "%K", *comps);
		  else
		    u_fprintf(OUT, "<B>\\u%04X</B>", *comps); /* to emulate the callback */
		  
		  comps++;
		};
	    }
	  else
	    explainStatus(status, key);
	}
    }
  u_fprintf(OUT, "</TD>");
  
  showKeyAndEndItem(key, locale);
}


void showLocaleCodes( UResourceBundle *rb, const char *locale)
{
  
  UErrorCode status = U_ZERO_ERROR;
  bool_t bigString = FALSE; /* is it big? */
  bool_t userRequested = FALSE; /* Did the user request this string? */
  const char *tmp1, *tmp2;
  char tempchar[1000];

  UErrorCode countStatus = U_ZERO_ERROR,langStatus = U_ZERO_ERROR;
  const UChar *count3 = 0, *lang3 = 0;

  count3 = ures_get(rb, "ShortCountry", &countStatus);
  lang3 = ures_get(rb, "ShortLanguage", &langStatus);


  showKeyAndStartItem("LocaleCodes", FSWF("LocaleCodes", "Locale Codes"), locale, status);

  u_fprintf(OUT, "<TABLE BORDER=1><TR><TD></TD><TD><B>%U</B></TD><TD><B>%U</B></TD><TD><B>%U</B></TD></TR>\r\n",
	    FSWF("LocaleCodes_Language", "Language"),
	    FSWF("LocaleCodes_Country", "Country"),
	    FSWF("LocaleCodes_Variant", "Variant"));
  u_fprintf(OUT, "<TR><TD>%U</TD><TD>",
	    FSWF("LocaleCodes_2", "2"));
  
  status = U_ZERO_ERROR;
  uloc_getLanguage(locale, tempchar, 1000, &status);
  if(U_SUCCESS(status))
    u_fprintf(OUT, tempchar);
  else
    explainStatus(status, "LocaleCodes");
  
  u_fprintf(OUT, "</TD><TD>");
  
  status = U_ZERO_ERROR;
  uloc_getCountry(locale, tempchar, 1000, &status);
  if(U_SUCCESS(status))
    u_fprintf(OUT, tempchar);
  else
    explainStatus(status, "LocaleCodes");
  
  u_fprintf(OUT, "</TD><TD>");
  
  status = U_ZERO_ERROR;
  uloc_getVariant(locale, tempchar, 1000, &status);
  if(U_SUCCESS(status))
    u_fprintf(OUT, tempchar);
  else
    explainStatus(status, "LocaleCodes");

  u_fprintf(OUT, "</TD></TR>\r\n");

  /* 3 letter line */

  u_fprintf(OUT, "<TR><TD>%U</TD>",
	    FSWF("LocaleCodes_3", "3"));

  u_fprintf(OUT, "<TD>");

  if(U_SUCCESS(langStatus))
    {
      u_fprintf(OUT, "%U", lang3);
      if(langStatus != U_ZERO_ERROR)
	{
	  u_fprintf(OUT, "<BR>");
	}
    }

  explainStatus(langStatus, "LocaleCodes");

  u_fprintf(OUT, "</TD><TD>");

  if(U_SUCCESS(countStatus))
    {
      u_fprintf(OUT, "%U", count3);
      if(countStatus != U_ZERO_ERROR)
	{
	  u_fprintf(OUT, "<BR>");
	}
    }

  explainStatus(countStatus, "LocaleCodes");

  u_fprintf(OUT, "</TD><TD></TD></TR>\r\n");
  
  u_fprintf(OUT, "</TABLE>\r\n");


  u_fprintf(OUT, "</TD>");
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

  s = ures_get(rb, key, &status);

  if(U_SUCCESS(status) && ( u_strlen(s) > kShowStringCutoffSize ) )
    {
      bigString = TRUE;
      userRequested = didUserAskForKey(key, queryString);
    }

  showKeyAndStartItem(key, NULL, locale, status);


  if(U_SUCCESS(status))
    {

      if(bigString && !userRequested) /* it's hidden. */
	{
	  /* WIERD!! outputting '&#' through UTF8 seems to be -> '?' or something 
	     [duh, HTML numbered escape sequence] */
	  u_fprintf(OUT, "<A HREF=\"?_=%s&SHOW%s&_#%s\"><IMG BORDER=0 WIDTH=16 HEIGHT=16 SRC=\"%s/localeexplorer/closed.gif\" ALT=\"\">%U</A>\r\n<P>", locale, key,key, kStaticURLPrefix, FSWF("bigStringClickToShow","(Omitted due to size. Click here to show.)"));
	}
      else
	{
	  if(bigString)
	    {
	      u_fprintf(OUT, "<A HREF=\"?_=%s#%s\"><IMG border=0 width=16 height=16 SRC=\"%s/localeexplorer/opened.gif\" ALT=\"\"> %U</A><P>\r\n",
			locale,
			key,
			kStaticURLPrefix,
			FSWF("bigStringHide", "Hide"));
	    }
	  
	  if(U_SUCCESS(status))
	    {
	      if(*s == 0)
		u_fprintf(OUT, "<I>%U</I>", FSWF("empty", "(Empty)"));
	      else
		u_fprintf(OUT, "%U ", s);
	    }
	}
    }
  u_fprintf(OUT, "</TD>");
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

  s = ures_get(rb, key, &status);

  /* we'll assume it's always big, for now. */
  bigString = TRUE;
  userRequested = didUserAskForKey(key, qs);

  showKeyAndStartItem(key, NULL, locale, status);

  /** DON'T show the string as a string. */
  /* 
     if(U_SUCCESS(status) && s)
     u_fprintf(OUT, "%U<BR>\r\n", s);
  */
  if(!hidable)
    {
      userRequested = TRUE;
      bigString = FALSE;
    }
  

  if(bigString && !userRequested) /* it's hidden. */
    {
      /* WIERD!! outputting '&#' through UTF8 seems to be -> '?' or something */
	u_fprintf(OUT, "<A HREF=\"?_=%s&SHOW%s&_#%s\"><IMG BORDER=0 WIDTH=16 HEIGHT=16 SRC=\"%s/localeexplorer/closed.gif\" ALT=\"\">%U</A>\r\n<P>", locale, key,key, kStaticURLPrefix, FSWF("stringClickToShow","(Click here to show.)"));
	u_fprintf(OUT, "<P>");
    }
  else
    {
      if(bigString)
	{
	  u_fprintf(OUT, "<A HREF=\"?_=%s#%s\"><IMG border=0 width=16 height=16 SRC=\"%s/localeexplorer/opened.gif\" ALT=\"\"> %U</A><P>\r\n",
		    locale,
		    key,
		kStaticURLPrefix,
		    FSWF("bigStringHide", "Hide"));
	}
  
      if(U_SUCCESS(status))
	{
	  u_fprintf(OUT, "<TABLE BORDER=1 WIDTH=100%>");
	  u_fprintf(OUT, "<TR><TD><B>%U</B></TD><TD><B>%U</B></TD><TD><B>%U</B></TD></TR>\r\n",
		    FSWF("charNum", "#"),
		    FSWF("char", "Char"),
		    FSWF("charMeaning", "Meaning"));
	  
	  
	  for(i=0;desc[i];i++)
	    {
	      if(!s[i])
		break;
	      
	      u_fprintf(OUT, "<TR><TD WIDTH=5>%d</TD><TD>%K</TD><TD>%U</TD></TR>\r\n",
		       i,
			s[i],
		       desc[i]);
	    }
	  u_fprintf(OUT, "</TABLE>\r\n");
	}
    }
  u_fprintf(OUT, "</TD>");
  showKeyAndEndItem(key, locale);
}
  
/* Show a resource that's an array. Useful for types we haven't written viewers for yet --------*/

void showArray( UResourceBundle *rb, const char *locale, const char *key )
{
  UErrorCode status = U_ZERO_ERROR;
  UErrorCode firstStatus = U_ZERO_ERROR;
  const UChar *s  = 0;
  int i;

  ures_getArrayItem(rb, key, 0, &firstStatus);

  showKeyAndStartItem(key, NULL, locale, firstStatus);

  u_fprintf(OUT, "<OL>\r\n");

  for(i=0;;i++)
    {
      status = U_ZERO_ERROR;

      s = ures_getArrayItem(rb, key, i, &status);
      if(!s)
	break;

      if(U_SUCCESS(status))
	u_fprintf(OUT, "<LI> %U\r\n", s);
      else
	{
	  u_fprintf(OUT, "<LI>");
	  explainStatus(status, key);
	  u_fprintf(OUT, "\r\n");
	  break;
	}

    }
  u_fprintf(OUT, "</OL></TD>");
  showKeyAndEndItem(key, locale);
}


/* Show a resource that's an array, wiht an explanation ------------------------------- */

void showArrayWithDescription( UResourceBundle *rb, const char *locale, const UChar *desc[], const char *key )
{
  UErrorCode status = U_ZERO_ERROR;
  UErrorCode firstStatus;
  const UChar *s  = 0, *toShow =0;
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
  int32_t len = 0;

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
  s = ures_getArrayItem(rb, key, 0, &firstStatus);
  showKeyAndStartItemShort(key, NULL, locale, firstStatus);

  if(exampleType != kNoExample)
    {
      toShow = (UChar[]){ 0x0000 };
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
	  toShow = (UChar[]){ 0x00B3,0x0000 }; /* # */
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

  u_fprintf(OUT, "</TD>"); /* Now, we're done with the ShowKey.. cell */


  u_fprintf(OUT, "</TR><TR><TD COLSPAN=2><TABLE BORDER=2 WIDTH=\"100%\" HEIGHT=\"100%\">\r\n");

  for(i=0;desc[i];i++)
    {
      
      u_fprintf(OUT, "<TR><TD WIDTH=5>%d</TD><TD>%U</TD><TD>",
		i, desc[i]);

      status = U_ZERO_ERROR;
      exampleStatus = U_ZERO_ERROR;

      s = ures_getArrayItem(rb, key, i, &status);

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
	    }
	  
	  u_fprintf(OUT, "%U\r\n", toShow);
	}
      else
	{
	  s = 0;
	  explainStatus(status, key);
	  u_fprintf(OUT, "\r\n");
	  break;
	}
      u_fprintf(OUT, "</TD>");
      
      if(s) /* only if pattern exists */
      switch(exampleType)
	{
	case kDateTimeExample:
	  if(i < 8)
	  {
	    u_fprintf(OUT, "<TD>");

	    if(U_SUCCESS(exampleStatus))
	      {
		exampleStatus = U_ZERO_ERROR; /* clear fallback info from exampleDF */
		udat_format(exampleDF, now, tempChars, 1024, NULL, &exampleStatus);
		udat_close(exampleDF);
		
		if(U_SUCCESS(exampleStatus))
		  u_fprintf(OUT, "%U", tempChars);

	      }
	    explainStatus(exampleStatus, key);
	    u_fprintf(OUT, "</TD>\r\n");

	    if(i == 3) /* short time */
	      u_strcpy(tempTime, tempChars);
	    else if(i == 7) /* short date */
	      u_strcpy(tempDate, tempChars);
	  }
	  else
	  {
	    u_fprintf(OUT, "<TD>");
	    exampleStatus = U_ZERO_ERROR;
	    if(s)
	      if(u_formatMessage(locale, s, -1, tempChars,1024,&exampleStatus, 
				 tempTime,
				 tempDate))
		u_fprintf(OUT,"%U", tempChars);
	    u_fprintf(OUT, "</TD>\r\n");
	  }
	  break;

	case kNumberExample:
	  {
	    u_fprintf(OUT, "<TD>");

	    if(U_SUCCESS(exampleStatus))
	      {
		exampleStatus = U_ZERO_ERROR; /* clear fallback info from exampleDF */

 		if(i == 3) /* scientific */
		  d = 1234567890;
		unum_formatDouble(exampleNF, d, tempChars, 1024, NULL, &exampleStatus);
		
		if(U_SUCCESS(exampleStatus))
		  u_fprintf(OUT, "%U", tempChars);

		
		u_fprintf(OUT, "</TD><TD>");

 		if(i == 3) /* scientific */
		  d = 0.00000000000000000005;

		unum_formatDouble(exampleNF, -d, tempChars, 1024, NULL, &exampleStatus);
		
		if(U_SUCCESS(exampleStatus))
		  u_fprintf(OUT, "%U", tempChars);

		unum_close(exampleNF);

	      }
	    explainStatus(exampleStatus, key);
	    u_fprintf(OUT, "</TD>\r\n");
	  }
	  break;

	case kNoExample:
	default:
	  break;
	}

      u_fprintf(OUT, "</TR>\r\n");

    }
  

  u_fprintf(OUT, "</TABLE></TD>");

  showKeyAndEndItem(key, locale);
}


/* show the DateTimeElements string *------------------------------------------------------*/

void showDateTimeElements( UResourceBundle *rb, const char *locale)
{
  UErrorCode status = U_ZERO_ERROR;
  UErrorCode firstStatus;
  const UChar *s  = 0;

  const char *key = "DateTimeElements";
  int i;
  /*
    0: first day of the week 
    1: minimaldaysinfirstweek 
  */

  status = U_ZERO_ERROR;

  s = ures_getArrayItem(rb, key, 0, &status);

  showKeyAndStartItem(key, FSWF(key, "Date and Time Options"), locale, status);

  /* First day of the week ================= */
  u_fprintf(OUT, "%U ", FSWF("DateTimeElements0", "First day of the week: "));
  

  if(U_SUCCESS(status))
    {
      UChar myWkday[100];
      int32_t  firstDay;

      firstDay = (((s[0] & 0x000F)+6)%7); /* REVISIT: parse */
      
      u_fprintf(OUT, " %U \r\n", s);
      /* here's something fun: try to fetch that day from the user's current locale */
      status = U_ZERO_ERROR;
      
      if(defaultRB && U_SUCCESS(status))
	{
	  s = ures_getArrayItem(defaultRB, "DayNames", firstDay, &status);
	  if(s && U_SUCCESS(status))
	    {
	      u_fprintf(OUT, " = %U \r\n", s);
	    }
	  status = U_ZERO_ERROR;

	  s = ures_getArrayItem(rb, "DayNames", firstDay , &status);
	  if(s && U_SUCCESS(status))
	    {
	      u_fprintf(OUT, " = %U \r\n", s);
	    }


	}
      status = U_ZERO_ERROR;
    }
  else
    {
      explainStatus(status, key);
      u_fprintf(OUT, "\r\n");
    }


  u_fprintf(OUT, "<BR>\r\n");

  /* minimal days in week ================= */
  u_fprintf(OUT, "%U", FSWF("DateTimeElements1", "Minimal Days in First Week: "));
  
  status = U_ZERO_ERROR;

  s = ures_getArrayItem(rb, key, 1, &status);

  firstStatus = status;
  
  if(U_SUCCESS(status))
    u_fprintf(OUT, " %U \r\n", s);
  else
    {
      explainStatus(status, key);
      u_fprintf(OUT, "\r\n");
    }

  u_fprintf(OUT, "</TD>");

  showKeyAndEndItem(key, locale);
}

/* Show a resource that has a short (*Abbreviations) and long (*Names) version ---------------- */
/* modified showArray */
void showShortLong( UResourceBundle *rb, const char *locale, const char *keyStem, const UChar *shortName, const UChar *longName, int32_t num )
{
  UErrorCode status = U_ZERO_ERROR;
  UErrorCode shortStatus = U_ZERO_ERROR, longStatus = U_ZERO_ERROR;
  char       shortKey[100], longKey[100];
  const UChar *s  = 0;
  int i;

  showKeyAndStartItem(keyStem, NULL, locale, U_ZERO_ERROR); /* No status possible  because we have two items */

  sprintf(shortKey, "%sNames", keyStem);
  sprintf(longKey,  "%sAbbreviations", keyStem);

  /* pre load the status of these things */
  ures_getArrayItem(rb,longKey, 0, &longStatus);
  ures_getArrayItem(rb,shortKey, 0, &shortStatus);

  u_fprintf(OUT, "<TABLE BORDER=1 WIDTH=100%% HEIGHT=100%%><TR><TD><B>#</B></TD><TD><B>%U</B> ", shortName);
  explainStatus(shortStatus, keyStem);
  u_fprintf(OUT, "</TD><TD><B>%U</B> ", longName);
  explainStatus(longStatus, keyStem);
  u_fprintf(OUT, "</TD></TR>\r\n");

 
  for(i=0;i<num;i++)
    {
      char *key;

      u_fprintf(OUT, " <TR><TD>%d</TD><TD>", i);

      /* get the normal name */
      status = U_ZERO_ERROR;
      key = longKey;
      s = ures_getArrayItem(rb, key, i, &status);

      if(i==0)
	longStatus = status;
  
      if(U_SUCCESS(status))
	u_fprintf(OUT, " %U ", s);
      else
	explainStatus(status, keyStem); /* if there was an error */

      u_fprintf(OUT, "</TD><TD>");

      /* get the short name */
      status = U_ZERO_ERROR;
      key = shortKey;
      s = ures_getArrayItem(rb, key, i, &status);

      if(i==0)
	shortStatus = status;
  
      if(U_SUCCESS(status))
	u_fprintf(OUT, " %U ", s);
      else
	explainStatus(status, keyStem); /* if there was an error */

      u_fprintf(OUT, "</TD></TR>");
    }

  u_fprintf(OUT, "</TABLE>");
  u_fprintf(OUT, "</TD>");

  showKeyAndEndItem(keyStem, locale);

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

  ures_count2dArrayItems(rb, key, &rows, &cols, &status);

  if(U_SUCCESS(status) && ((rows > kShow2dArrayRowCutoff) || (cols > kShow2dArrayColCutoff)) )
    {
      bigString = TRUE;
      userRequested = didUserAskForKey(key, queryString);
    }

  showKeyAndStartItem(key, NULL, locale, status);


  if(bigString && !userRequested) /* it's hidden. */
    {
      /* WIERD!! outputting '&#' through UTF8 seems to be -> '?' or something */
	u_fprintf(OUT, "<A HREF=\"?_=%s&SHOW%s&_#%s\"><IMG BORDER=0 WIDTH=16 HEIGHT=16 SRC=\"%s/localeexplorer/closed.gif\" ALT=\"\">%U</A>\r\n<P>", locale, key,key, kStaticURLPrefix, FSWF("bigStringClickToShow","(Omitted due to size. Click here to show.)"));
    }
  else
    {
      if(bigString)
	{
	  u_fprintf(OUT, "<A HREF=\"?_=%s#%s\"><IMG border=0 width=16 height=16 SRC=\"%s/localeexplorer/opened.gif\" ALT=\"\"> %U</A><P>\r\n",
		    locale,
		    key,
		    kStaticURLPrefix,
		    FSWF("bigStringHide", "Hide"));
	}

      firstStatus = status;  /* save this for the next column.. */

      if(U_SUCCESS(status))
	{	


	  u_fprintf(OUT,"<TABLE BORDER=1>\r\n");
	  
	  /* print the top row */
	  u_fprintf(OUT,"<TR><TD></TD>");
	  for(h=0;h<cols;h++)
	    {
	      if(!desc[h])
		break;

	      u_fprintf(OUT,"<TD><B>%U</B></TD>", desc[h]);
	    }
	  u_fprintf(OUT,"</TR>\r\n");
	  
	  for(v=0;v<rows;v++)
	    {
	      u_fprintf(OUT,"<TR><TD><B>%d</B></TD>", v);
	      for(h=0;h<cols;h++)
		{
		  status = U_ZERO_ERROR;
		  
		  s = ures_get2dArrayItem(rb, key, v, h, &status);
		  
		  /*      if((h == 0) && (v==0))
			  firstStatus = status; */ /* Don't need to track firstStatus, countArrayItems should do that for us. */
		  
		  if(U_SUCCESS(status) && s)
		    u_fprintf(OUT, "<TD>%U</TD>\r\n", s);
		  else
		    {
		      u_fprintf(OUT, "<TD BGCOLOR=" kStatusBG " VALIGN=TOP>");
		      explainStatus(status, key);
		      u_fprintf(OUT, "</TD>\r\n");
		      break;
		    }
		}
	      u_fprintf(OUT, "</TR>\r\n");
	    }
	  u_fprintf(OUT, "</TABLE>\r\n<BR>");
	}
    }

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

  rows = ures_countArrayItems(rb, key, &status);

  if(U_SUCCESS(status) && ((rows > kShow2dArrayRowCutoff)))
    {
      bigString = TRUE;
      userRequested = didUserAskForKey(key, queryString);
    }

  showKeyAndStartItem(key, NULL, locale, status);

  if(bigString && !userRequested) /* it's hidden. */
    {
      /* WIERD!! outputting '&#' through UTF8 seems to be -> '?' or something */
	u_fprintf(OUT, "<A HREF=\"?_=%s&SHOW%s&_#%s\"><IMG BORDER=0 WIDTH=16 HEIGHT=16 SRC=\"%s/localeexplorer/closed.gif\" ALT=\"\">%U</A>\r\n<P>", locale, key,key, kStaticURLPrefix, FSWF("bigStringClickToShow","(Omitted due to size. Click here to show.)"));
    }
  else
    {
      if(bigString)
	{
	  u_fprintf(OUT, "<A HREF=\"?_=%s#%s\"><IMG border=0 width=16 height=16 SRC=\"%s/localeexplorer/opened.gif\" ALT=\"\"> %U</A><P>\r\n",
		    locale,
		    key,
		    kStaticURLPrefix,
		    FSWF("bigStringHide", "Hide"));
	}

      firstStatus = status;  /* save this for the next column.. */

      if(U_SUCCESS(status))
	{	
	  

	  u_fprintf(OUT,"<TABLE BORDER=1>\r\n");
	  
	  /* print the top row */
	  u_fprintf(OUT,"<TR><TD><B>%U</B></TD><TD><I>%U</I></TD><TD><B>%U</B></TD></TR>",
		    FSWF("taggedarrayTag", "Tag"),
		    defaultLanguageDisplayName(),
		    curLocale->ustr);
	  
	  for(v=0;v<rows;v++)
	    {
	      const char *tag;

	      status = U_ZERO_ERROR;
	      
	      tag = ures_getTaggedArrayTag(rb, key, v, &status);
	      if(!tag)
		break;
	      
	      u_fprintf(OUT,"<TR> ");

	      if(U_SUCCESS(status))
		{
		  u_fprintf(OUT, "<TD><TT>%s</TT></TD> ", tag);
		  
		  if(defaultRB)
		    {
		      s = ures_getTaggedArrayItem(defaultRB, key, tag, &status);
		      if(s)
			u_fprintf(OUT, "<TD><I>%U</I></TD>", s);
		      else
			u_fprintf(OUT, "<TD></TD>");
		    }
		      else
			u_fprintf(OUT, "<TD></TD>");
		  
		  status = U_ZERO_ERROR;

		  s = ures_getTaggedArrayItem(rb, key, tag, &status);

		  if(s)
		    u_fprintf(OUT, "<TD>%U</TD>", s);
		  else
		    {
		      u_fprintf(OUT, "<TD BGCOLOR=" kStatusBG " VALIGN=TOP>");
		      explainStatus(status, key);
		      u_fprintf(OUT, "</TD>\r\n");
		    }
		}
	      u_fprintf(OUT, "</TR>\r\n");
	    }
	  u_fprintf(OUT, "</TABLE>\r\n<BR>");
	}
    }

  u_fprintf(OUT, "</TD>");
  showKeyAndEndItem(key, locale);
}


/* Explain what the status code means --------------------------------------------------------- */

void explainStatus( UErrorCode status, const char *tag )
{
  if(tag == 0)
    tag = "_top_";

  u_fprintf(OUT, " <B><FONT SIZE=-1>");
  switch(status)
    {
    case U_MISSING_RESOURCE_ERROR:
      printHelpTag("U_MISSING_RESOURCE_ERROR",
		   FSWF("U_MISSING_RESOURCE_ERROR", "(missing resource)"));
      break;

    case U_USING_FALLBACK_ERROR:
      if(parLocale && parLocale->str)
	{
	  u_fprintf(OUT, "<A HREF=\"?_=%s#%s\">", parLocale->str, tag);
	  u_fprintf_u(OUT, FSWF("inherited_from", "(inherited from %U)"), parLocale->ustr); 
	}
      else
	{
	  u_fprintf(OUT, "<A HREF=\"?_=default#%s\">", tag);
	  u_fprintf_u(OUT, FSWF("inherited", "(inherited)"));
	}

      u_fprintf(OUT, "</A>");
      break;

    case U_USING_DEFAULT_ERROR:
	u_fprintf(OUT, "<A HREF=\"?_=default#%s\">", tag);
	  u_fprintf_u(OUT, FSWF("inherited_from", "(inherited from %U)"), locales->ustr); 
	  u_fprintf(OUT, "</A>");
      break;

    default:
      if(status != U_ZERO_ERROR)
	{
	  u_fprintf(OUT, "(%U %d)", FSWF("UNKNOWN_ERROR", "unknown error"), (int) status);
	  fprintf(stderr,"LRB: caught Unknown err- %d\n", status); 
	}
    }
  u_fprintf(OUT, "</FONT></B>");
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

  u_fprintf(OUT, "<SCRIPT LANGUAGE=\"JavaScript\">");
  u_fprintf(OUT, "<!--\r\n  top.document.write('<A HREF=\\\"javascript:openHelpWin(\\'%s/localeexplorer/%Uhelp.html#%s\\', \\'%s\\')\\\" >%U</A>');\r\n",
	    kStaticURLPrefix,
	    FSWF("helpPrefix", "default_"),
	    helpTag, helpTag, str);
  u_fprintf(OUT, "// -->\r\n</SCRIPT>\r\n");
  u_fprintf(OUT, "<NOSCRIPT>\r\n");
  u_fprintf(OUT, "<A HREF=\"%s/localeexplorer/%Uhelp.html#%s\" TARGET=\"help\">%U</A></NOSCRIPT>",
	    kStaticURLPrefix,
	    FSWF("helpPrefix", "default_"),
	    helpTag,str);
}

void printHelpImg(const char *helpTag, const UChar *alt, const UChar *src, const UChar *options)
{
  u_fprintf(OUT, "<SCRIPT LANGUAGE=\"JavaScript\">");
  u_fprintf(OUT, "<!--\r\n  top.document.write('");
  u_fprintf(OUT, "<A HREF=\\\"javascript:openHelpWin(\\'%s/localeexplorer/%Uhelp.html#%s\\', \\'%s\\')\\\" ><IMG %U SRC=\\\"%s/localeexplorer/%U\\\" ALT=\\\"%U\\\"></A>",
	    kStaticURLPrefix,
	    FSWF("helpPrefix", "default_"),
	    helpTag, 
	    helpTag,
	    options, kStaticURLPrefix, src, alt);
  u_fprintf(OUT, "');\r\n// -->\r\n</SCRIPT>\r\n");
  u_fprintf(OUT, "<NOSCRIPT>\r\n");
  u_fprintf(OUT, "<A HREF=\"%s/localeexplorer/%Uhelp.html#%s\" TARGET=\"help\"><IMG %U SRC=\"%s/localeexplorer/%U\" ALT=\"%U\"></A>",
	    kStaticURLPrefix,
	    FSWF("helpPrefix", "default_"),
	    helpTag, 
	    options, kStaticURLPrefix, src, alt);
  u_fprintf(OUT, "</NOSCRIPT>\r\n");
}

void showExploreCloseButton(const char *locale, const char *frag)
{
  u_fprintf(OUT, "<!-- non javascript version first -->\r\n"
                 "<SCRIPT LANGUAGE=\"JavaScript\">");
  u_fprintf(OUT, "<!--\r\n   top.document.write('<B><I>When done, you may close this window</I></B>');\r\n// -->\r\n</SCRIPT>\r\n");
  u_fprintf(OUT, "<NOSCRIPT><FORM ACTION=\"#%s\">"
                 "<INPUT TYPE=HIDDEN NAME=\"_\" VALUE=\"%s\">"
	         "<INPUT TYPE=SUBMIT VALUE=\"%U\" ONCLICK=\"window.close()\">"
                 "</FORM></NOSCRIPT>",
	    frag, locale, FSWF("explore_close", "Done"));
}

void showExploreButton(UResourceBundle *rb, const char *locale, const UChar *sampleString, const char *key)
{
  UChar nullString[] = { 0x0000 };
  
  if(!sampleString)
    sampleString = nullString;
  
  /**   
	todo:
	
	
	<SCRIPT>
	... non-javascript form ...
	
	
	<!-- 
	doc.write("javascript form");
	// -->
	</SCRIPT>
  **/
#if 0
  u_fprintf(OUT, "<SCRIPT LANGUAGE=\"JavaScript\">\r\n"
	    "<!--\r\n"
	    "  top.document.write('");
  
  /* replace with window.open */
  u_fprintf(OUT, "<FORM target=\\\"_new\\\" NAME=EXPLORE_%s ACTION=\"#EXPLORE_%s\">"
	    "<INPUT TYPE=HIDDEN NAME=_ VALUE=\"%s\">"
	    "<INPUT TYPE=HIDDEN NAME=\"EXPLORE_%s\" VALUE=\"",
	    key, key,locale,key);
  writeEscaped(sampleString);
  u_fprintf(OUT, "\">");
  
  u_fprintf(OUT, "<INPUT TYPE=IMAGE WIDTH=48 HEIGHT=20 BORDER=0 SRC=/developerworks/opensource/icu/project/localeexplorer/explore.gif ALIGN=RIGHT   ");
  u_fprintf(OUT, " VALUE=\"%U\"></FORM>",
	    FSWF("explore_Button", "Explore"));

  u_fprintf(OUT, "');\r\n//-->\r\n</SCRIPT>\r\n");

  /* ================ */
  
  u_fprintf(OUT, "<NOSCRIPT>\r\n");
#endif
  u_fprintf(OUT, "<FORM TARGET=\"_new\" NAME=EXPLORE_%s ACTION=\"#EXPLORE_%s\">"
	    "<INPUT TYPE=HIDDEN NAME=_ VALUE=\"%s\">"
	    "<INPUT TYPE=HIDDEN NAME=\"EXPLORE_%s\" VALUE=\"",
	    key, key,locale,key);
  writeEscaped(sampleString);
  u_fprintf(OUT, "\">");
  
  u_fprintf(OUT, "<INPUT TYPE=IMAGE WIDTH=48 HEIGHT=20 BORDER=0 SRC=/developerworks/opensource/icu/project/localeexplorer/explore.gif  ALIGN=RIGHT   ");
  u_fprintf(OUT, " VALUE=\"%U\"></FORM>",
	    FSWF("explore_Button", "Explore"));
#if 0
  u_fprintf(OUT, "</NOSCRIPT>\r\n");
#endif
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
  u_fprintf(OUT, "<FORM METHOD=GET ACTION=\"#EXPLORE_%s\">\r\n",
	    key);
  u_fprintf(OUT, "<INPUT NAME=_ TYPE=HIDDEN VALUE=%s>", locale);

  if(valueFmt)
    {
      
      u_fprintf(OUT, "<INPUT NAME=NP_DBL TYPE=HIDDEN VALUE=\"");
      tmp[0] = 0;
      unum_formatDouble(valueFmt, value, tmp, 1000, 0, &status);
      u_fprintf(OUT, "%U\">", tmp);
    }
  u_fprintf(OUT, "<TEXTAREA ROWS=2 COLS=60 NAME=\"EXPLORE_%s\">",
	    key);

  ucnv_setFromUCallBack((UConverter*)u_fgetConverter(OUT), &UCNV_FROM_U_CALLBACK_BACKSLASH_ESCAPE, &status); 

  u_fprintf(OUT, "%U", dstPattern); 

	    /* should 'get/restore' here. */
  /*  ucnv_setFromUCallBack((UConverter*)u_fgetConverter(OUT), &SubstituteWithValueHTML, &status); */
  ucnv_setFromUCallBack((UConverter*)u_fgetConverter(OUT), &UCNV_FROM_U_CALLBACK_DECOMPOSE, &status);

  
  u_fprintf(OUT, "</TEXTAREA><P>\r\n<INPUT TYPE=SUBMIT VALUE=Format><INPUT TYPE=RESET VALUE=Reset></FORM>\r\n");

}


#ifdef kUSort

/* Demonstrate sorting ------------------------------------------------------------------------- */
/* locale to view, b is the rest of the querystring */

/* To demonstrate sorting, we are going to call the usort external program.
   This is a little simpler than maintaining two sets of code for doing sorting. 
   the real solution would be to modularize the code in usort.. 
*/

void showSort(const char *locale, const char *b)
{
  char   inputChars[1024];
  char   tmpChars[1024];
  char *text;
  char *p;
  int32_t length;
  UChar  strChars[1024];

  /* Actually, we don't need to convert the input text to Unicode. the usort program will
     accept it in the specified charset.. */

  strChars[0] = 0;

  text = strstr(b, "EXPLORE_CollationElements");
  if(text)
    {
      text += 26;

      unescapeAndDecodeQueryField(strChars, 1000, text);
      
      p = strchr(text, '&');
      if(p) /* there is a terminating ampersand */
	length = p - text;
      else
	length = strlen(text);

      if(length > 1023)
	length = 1023; /* safety ! */

      doDecodeQueryField(text, inputChars, length); /* length limited */
    }
  else
    {
      inputChars[0] = 0; 
    }

  u_fprintf(OUT, "%U<P>\r\n", FSWF("EXPLORE_CollationElements_Prompt", "Type in some lines of text to be sorted."));

  u_fprintf(OUT, "<TABLE BORDER=1 CELLSPACING=1 CELLPADDING=1 WIDTH=100% HEIGHT=100%><TR><TD><B>%U</B></TD>\r\n",
	    FSWF("usortSource", "Source"));
  
  if(inputChars[0])
    u_fprintf(OUT, "<TD><B>%U</B></TD><TD><B>%U</B></TD><TD><B>%U</B></TD>",
	      FSWF("usortPrimary", "Primary"),
	      FSWF("usortSecondary", "Secondary"),
	      FSWF("usortTertiary", "Tertiary"));
  
  u_fprintf(OUT, "</TR><TR><TD VALIGN=TOP>");

  u_fprintf(OUT, "<FORM ACTION=\"#EXPLORE_CollationElements\" METHOD=GET>\r\n");
  u_fprintf(OUT, "<INPUT TYPE=HIDDEN NAME=\"_\" VALUE=\"%s\">\r\n", locale);
  u_fprintf(OUT, "<TEXTAREA ROWS=10 COLUMNS=25 COLS=40 NAME=\"EXPLORE_CollationElements\">");

  writeEscaped(strChars);
  /*  if(*inputChars)
      u_fprintf(OUT, "%s", inputChars); */

  u_fprintf(OUT, "</TEXTAREA><BR>\r\n");

  u_fprintf(OUT, "<INPUT TYPE=SUBMIT VALUE=\"%U\"></FORM><P>\r\n",
	    FSWF("EXPLORE_CollationElements_Sort", "Sort"));

  u_fprintf(OUT, "</TD>");

  if(inputChars[0] != 0)
    {
      FILE *sortpipe;
      char sortTypes[4][10] = { "-1", "-2", "-3", "-I" };
      int n;

      /* have some text to sort */

      /* * set up the env vars */
      sprintf(tmpChars, "LANG=%s", locale);
      putenv(tmpChars);

      sprintf(tmpChars, "ICU_ENCODING=%s", ucnv_getDefaultName() );
      putenv(tmpChars);


      /* Loop through each sort method */
      for(n=0;n<3;n++) /* not 4 */
	{
	  u_fprintf(OUT, " <TD><PRE>");

	  sprintf(tmpChars, "%s -T %s %s %s ", kUSort, ourCharsetName, " -D -escape",  sortTypes[n]); /* pass the -escape string if they asked for it */
	  
	  fprintf(stderr, "OPENED: %s\n", tmpChars);

	  sortpipe = popen(tmpChars, "w");

	  if(!sortpipe)
	    {
	      u_fprintf(OUT, "<B>%U</B><BR>%s\r\n", FSWF("EXPLORE_CollationElements_PopenFailed", "Sorry, failed to popen the sort program!"), tmpChars);
	    }
	  else
	    {
	      fflush(stdout);
	      fflush(stderr);

	      fprintf(sortpipe, "%s\n", inputChars);
	      fflush(sortpipe);
	      pclose(sortpipe); /* that oughtta do it.. */

	      fflush(stdout);
	      fflush(stderr);
	    } 
	  u_fprintf(OUT, "</PRE></TD>");	  
	}
    }

  u_fprintf(OUT, "</TR></TABLE><P>");
  u_fprintf(OUT, "<P><P>%U", FSWF("EXPLORE_CollationElements_strength", "There are four different strengths which may be used in collation.<P> Primary means that only primary differences are considered, such as letters. <P>Secondary considers letters, and also the secondary characteristics of accent marks. <P>Tertiary considers these two, and the case of the letter. <P> A strength of Identical means that all Unicode features are considered in determining a match."));

  u_fprintf(OUT, "<P>\r\n");
  showExploreCloseButton(locale, "CollationElements");

}

#endif /* kUSort */

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
		      locale, U_ZERO_ERROR);

  u_fprintf(OUT, "%U<P>", FSWF("formatExample_DateTimePatterns_What","This example demonstrates the formatting of date and time patterns in this locale."));
  
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
      u_fprintf(OUT, "VS_=((");
      for(q=0;valueString[q];q++)
	{
	  u_fprintf(OUT, "%c:%04x ", (char)valueString[q], valueString[q]);
	}
      u_fprintf(OUT, "))<BR>");

      udat_toPattern(df_default, FALSE, patn, 1024, &status);

      u_fprintf(OUT, "PAT=((");
      for(q=0;patn[q];q++)
	{
	  u_fprintf(OUT, "%c:%04x ", (char)patn[q], patn[q]);
	}
      u_fprintf(OUT, "))<BR>");
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
      u_fprintf(OUT, "VS_=((");
      for(q=0;valueString[q];q++)
	{
	  u_fprintf(OUT, "%c:%04x ", (char)valueString[q], valueString[q]);
	}
      u_fprintf(OUT, "))<BR>");

      udat_toPattern(df, FALSE, patn, 1024, &status);

      u_fprintf(OUT, "PAT=((");
      for(q=0;patn[q];q++)
	{
	  u_fprintf(OUT, "%c:%04x ", (char)patn[q], patn[q]);
	}
      u_fprintf(OUT, "))<BR>");
#endif


      status = U_ZERO_ERROR;
      now = udat_parse(df, valueString, -1, &parsePos, &status);
    }

  /* Common handler for input errs */

  if(U_FAILURE(status) || (now == 0))
    {
      status = U_ZERO_ERROR;
      u_fprintf(OUT, "%U %d<P>\r\n", FSWF("formatExample_errorParse", "Could not parse this, replaced with a default value. Formatted This many chars:"), parsePos);
      now = ucal_getNow();
    }
  status = U_ZERO_ERROR;
  /* ======================== End loading input date ================================= */

  if(U_FAILURE(status))
    {
      u_fprintf(OUT, "%U: [%d] <P>", FSWF("formatExample_DateTimePatterns_errorOpen", "Couldn't open the formatter"), (int) status);
      explainStatus(status, "EXPLORE_DateTimePatterns");
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
    u_fprintf(OUT, "%U<P>", FSWF("formatExample_DateTimePatterns_errorFormat", "Couldn't format the date"));
  
  explainStatus(status,"EXPLORE_DateTimePatterns");



  /* =======================  Now, collect the new date values ====================== */

  /* Now, display the results in <default> and in their locale */
  u_fprintf(OUT, "<TABLE BORDER=1><TR><TD>\r\n");


  /* ============ 'default' side of the table  */

  if(U_FAILURE(defStatus))
    {
      u_fprintf(OUT, "%U<P>", FSWF("formatExample_errorFormatDefault", "Unable to format number using default version of the pattern"));
      explainStatus(status, "EXPLORE_DateTimePatterns");
    }
  else
    {
      u_fprintf(OUT, "<B><I>%U</I></B><BR>\r\n", defaultLanguageDisplayName());
#if 0
      /* Just the pattern */
      u_fprintf(OUT, "%U", defChars);
#else
      u_fprintf(OUT, "<FORM METHOD=GET ACTION=\"#EXPLORE_DateTimePatterns\">\r\n");
      u_fprintf(OUT, "<INPUT NAME=_ TYPE=HIDDEN VALUE=%s>\r\n", locale);
      u_fprintf(OUT, "<INPUT TYPE=HIDDEN NAME=EXPLORE_DateTimePatterns VALUE=\"");
      writeEscaped(pattern);
      u_fprintf(OUT, "\">\r\n");

      u_fprintf(OUT, "<TEXTAREA NAME=NP_DEF ROWS=1 COLS=30>");
      ucnv_setFromUCallBack((UConverter*)u_fgetConverter(OUT), &UCNV_FROM_U_CALLBACK_BACKSLASH_ESCAPE, &status); 
      u_fprintf(OUT, "%U", defChars); 
      
      /* should 'get/restore' here. */
      /*  ucnv_setFromUCallBack((UConverter*)u_fgetConverter(OUT), &SubstituteWithValueHTML, &status); */
      ucnv_setFromUCallBack((UConverter*)u_fgetConverter(OUT), &UCNV_FROM_U_CALLBACK_DECOMPOSE, &status);

      status = U_ZERO_ERROR;
      
      u_fprintf(OUT, "</TEXTAREA><BR><INPUT TYPE=SUBMIT VALUE=\"%U\"></FORM>", FSWF("EXPLORE_change", "Change"));
#endif
    }
  
  u_fprintf(OUT, "</TD><TD WIDTH=1 BGCOLOR=\"#EEEEEE\"><IMG SRC=/developerworks/opensource/icu/project/c.gif ALT=\"---\" WIDTH=0 HEIGHT=0></TD><TD>");

  /* ============ 'localized' side ================================= */

  if(U_FAILURE(locStatus))
    {
      u_fprintf(OUT, "%U<P>", FSWF("formatExample_errorFormatDate2", "Couldn't format the date normally"));
      explainStatus(status, "EXPLORE_DateTimePatterns");
    }
  else
    {
      /*  === local side */
      u_fprintf(OUT, "\r\n\r\n<!--  LOCALIZED SIDE -->\r\n<B>%U</B><BR>\r\n",curLocale?curLocale->ustr:FSWF("NoLocale","MISSING LOCALE NAME") );
#if 0
      u_fprintf(OUT, "%U", tempChars);
#else
      u_fprintf(OUT, "<FORM METHOD=GET ACTION=\"#EXPLORE_DateTimePatterns\">\r\n");
      u_fprintf(OUT, "<INPUT NAME=_ TYPE=HIDDEN VALUE=%s>\r\n", locale);
      u_fprintf(OUT, "<INPUT TYPE=HIDDEN NAME=EXPLORE_DateTimePatterns VALUE=\"");
      writeEscaped(pattern);
      u_fprintf(OUT, "\">\r\n");
      
      u_fprintf(OUT, "<TEXTAREA NAME=NP_LOC ROWS=1 COLS=30>");
      writeEscaped(tempChars);
      u_fprintf(OUT, "</TEXTAREA><BR><INPUT TYPE=SUBMIT VALUE=\"%U\"></FORM>", FSWF("EXPLORE_change", "Change"));
#endif

    }
  /*  ============== End of the default/localized split =============== */

  u_fprintf(OUT, "</TD></TR>");
  u_fprintf(OUT, "</TABLE>");
  

  /* =============== All done ========================== */
  
  if(df)
    udat_close(df);

  if(df_default)
    udat_close(df_default);

  if(nf)
    unum_close(nf);

  u_fprintf(OUT, "<P><P>");

  showExploreCloseButton(locale, "DateTimePatterns");

  u_fprintf(OUT, "</TD><TD ALIGN=LEFT VALIGN=TOP>");
  printHelpTag("EXPLORE_DateTimePatterns", NULL);
  u_fprintf(OUT, "</TD>\r\n");

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
  
  showKeyAndStartItem("EXPLORE_NumberPatterns", FSWF("EXPLORE_NumberPatterns", "Explore &gt; Numbers"), locale, U_ZERO_ERROR);

  u_fprintf(OUT, "%U<P>", FSWF("formatExample_NumberPatterns_What","This example demonstrates formatting of numbers in this locale."));

  exploreFetchNextPattern(pattern, strstr(b,"EXPLORE_NumberPatterns")); 

  nf = unum_open(UNUM_DEFAULT,locale,  &status);
  
  if(U_FAILURE(status))
    {
      u_fprintf(OUT, "</TD></TR></TABLE></TD></TR></TABLE><P><HR>%U: ", FSWF("formatExample_errorOpen", "Couldn't open the formatter"));
      explainStatus(status, "EXPLORE_NumberPattern");
      return; /* ? */
    }
  
  unum_applyPattern(nf, TRUE, pattern, -1);
  
  unum_toPattern(nf, FALSE, tempChars, 1024, &status);

  if(U_FAILURE(status))
    {
      u_fprintf(OUT, "</TD></TR></TABLE></TD></TR></TABLE><P><HR>  %U<P>", FSWF("formatExample_errorToPattern", "Couldn't convert the pattern [toPattern]"));
      explainStatus(status, "EXPLORE_NumberPattern");
      return;
    }

  nf_default = unum_open(UNUM_DEFAULT, NULL, &status);
  
  if(U_FAILURE(status))
    {
      u_fprintf(OUT, "</TD></TR></TABLE></TD></TR></TABLE><P><HR>%U<P>", FSWF("formatExample_errorOpenDefault", "Couldn't open the default number fmt"));
      explainStatus(status, "EXPLORE_NumberPattern");
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
  u_fprintf(OUT, "<TABLE BORDER=1><TR><TD>\r\n");


  /* ============ 'default' side of the table ==========  */

  unum_formatDouble(nf_default,value,tempChars, 1024, 0, &status);

  if(U_FAILURE(status))
    {
      u_fprintf(OUT, "%U<P>", FSWF("formatExample_errorFormatDefault", "Unable to format number using default version of the pattern"));
      explainStatus(status, "EXPLORE_NumberPattern");
    }
  else
    {
      u_fprintf(OUT, "<B><I>%U</I></B><BR>\r\n", defaultLanguageDisplayName());
      u_fprintf(OUT, "<FORM METHOD=GET ACTION=\"#EXPLORE_NumberPatterns\">\r\n");
      u_fprintf(OUT, "<INPUT NAME=_ TYPE=HIDDEN VALUE=%s>\r\n", locale);
      u_fprintf(OUT, "<INPUT TYPE=HIDDEN NAME=EXPLORE_NumberPatterns VALUE=\"");
      writeEscaped(pattern);
      u_fprintf(OUT, "\">\r\n");

      u_fprintf(OUT, "<TEXTAREA NAME=NP_DEF ROWS=1 COLS=20>");
      ucnv_setFromUCallBack((UConverter*)u_fgetConverter(OUT), &UCNV_FROM_U_CALLBACK_BACKSLASH_ESCAPE, &status); 
      u_fprintf(OUT, "%U", tempChars); 
      
      /* should 'get/restore' here. */
      /*  ucnv_setFromUCallBack((UConverter*)u_fgetConverter(OUT), &SubstituteWithValueHTML, &status); */
      ucnv_setFromUCallBack((UConverter*)u_fgetConverter(OUT), &UCNV_FROM_U_CALLBACK_DECOMPOSE, &status);

      status = U_ZERO_ERROR;
      
      u_fprintf(OUT, "</TEXTAREA><INPUT TYPE=SUBMIT VALUE=\"%U\"></FORM>", FSWF("EXPLORE_change", "Change"));
      
    }
  
  u_fprintf(OUT, "</TD><TD WIDTH=1 BGCOLOR=\"#EEEEEE\"><IMG SRC=/developerworks/opensource/icu/project/c.gif ALT=\"---\" WIDTH=0 HEIGHT=0></TD><TD>");

  /* ============ 'localized' side ================================= */

  unum_formatDouble(nf,value,tempChars, 1024, 0, &status);

  if(U_FAILURE(status))
    {
      u_fprintf(OUT, "%U<P>", FSWF("formatExample_errorFormat2", "Couldn't format the number normally"));
      explainStatus(status, "EXPLORE_NumberPattern");
    }
  else
    {
      /*  === local side */
      u_fprintf(OUT, "\r\n\r\n<!--  LOCALIZED SIDE -->\r\n<B>%U</B><BR>\r\n",curLocale?curLocale->ustr:FSWF("NoLocale","MISSING LOCALE NAME") );
      u_fprintf(OUT, "<FORM METHOD=GET ACTION=\"#EXPLORE_NumberPatterns\">\r\n");
      u_fprintf(OUT, "<INPUT NAME=_ TYPE=HIDDEN VALUE=%s>\r\n", locale);
      u_fprintf(OUT, "<INPUT TYPE=HIDDEN NAME=EXPLORE_NumberPatterns VALUE=\"");
      writeEscaped(pattern);
      u_fprintf(OUT, "\">\r\n");
      
      u_fprintf(OUT, "<TEXTAREA NAME=NP_LOC ROWS=1 COLS=20>");
      writeEscaped(tempChars);
      u_fprintf(OUT, "</TEXTAREA><INPUT TYPE=SUBMIT VALUE=\"%U\"></FORM>", FSWF("EXPLORE_change", "Change"));

      if(localValueErr)
	u_fprintf(OUT, "<P>%U", localValueErr);
    }
  /*  ============== End of the default/localized split =============== */

  u_fprintf(OUT, "</TD></TR>");
  u_fprintf(OUT, "</TABLE>");

  if(nf)
    unum_close(nf);

  if(nf_default)
    unum_close(nf_default);
  
  showExploreCloseButton(locale, "NumberPatterns");
  u_fprintf(OUT, "</TD><TD ALIGN=LEFT VALIGN=TOP>");
  printHelpTag("EXPLORE_NumberPatterns", NULL);
  u_fprintf(OUT, "</TD>\r\n");

  
  showKeyAndEndItem("EXPLORE_NumberPatterns", locale);
}


bool_t isSupportedLocale(const char *locale, bool_t includeChildren)
{
  UResourceBundle *newRB;
  UErrorCode       status = U_ZERO_ERROR;
  bool_t           supp   = TRUE;

  newRB = ures_open(myBundlePath(), locale, &status);
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
	  ures_get(newRB, "helpPrefix", &status);

	  if(status == U_USING_DEFAULT_ERROR)
	    supp = FALSE;
	  else if( (!includeChildren) && (status == U_USING_FALLBACK_ERROR))
	    supp = FALSE;
	}

      ures_close(newRB);
    }

  return supp;
}

#if 0
char standardFlags[] = "AAACAFAJALAMANAOAQARASATAUAVBABBBCBDBEBFBGBHBIBKBLBMBNBOBPBQBRBSBTBUBVBXBYCACBCDCECFCGCHCICJCKCMCNCOCQCRCSCTCUCVCWCYDJDODQDRECEGEIEKENERESETEUEZFAFGFIFJFMFOFPFQFRFSGAGBGGGHGIGJGKGLGMGOGPGQGRGTGVGYHAHKHMHOHQHRHUIDIMINIOIPIRISITIVIZJEJMJNJOJQJUKEKGKNKQKRKSKTKUKZLALELGLHLILOLQLSLTLULYMAMBMCMDMFMGMHMIMKMLMNMOMPMQMRMTMUMVMXMYMZNCNENFNGNHNINLNONPNRNSNTNUNZPAPCPEPKPLPMPOPPPSPUQARERMRORPRQRSRWSASBSCSESFSGSHSISLSMSNSOSPSTSUSVSWSXSYSZTCTDTETHTITKTLTNTOTPTSTUTVTWTXTZUGUKUPUSUVUYUZVCVEVIVMVQVTWAWFWQWSWZYMZAZIVNUATRSKPTLVJPILDZDK";

void showFlagImage(const char *locale, const char *extra)
{
  /* Show a little box with the currently viewed Locale */
    const  UChar  *flagImage;
    char flagPath[200];
    UResourceBundle *flagRB;
    UErrorCode status;
    char *ptr;

    status = U_ZERO_ERROR;

    flagPath[0] = 0;

    flagRB = ures_open(  myBundlePath(), locale, &status);
    if(U_SUCCESS(status) && (status != U_USING_DEFAULT_ERROR)) /* Important! don't want default flags.. */
      {
	flagImage = ures_get(flagRB, "flag", &status);
	
	if(U_SUCCESS(status) && (status != U_USING_DEFAULT_ERROR) && flagImage && *flagImage) /* if it's non null. Again, don't want default flags! */
	  {
	    u_austrcpy(flagPath, flagImage);
	  }
      }

    /* As a fallback (actually a common case), we can use a flag of the form
       'za-t.gif' (from the CIA world factbook). 
       Extract the country and see if it matches our list of flags.. 
    */
    if(!flagPath[0] && (strlen(locale) >= 4) && (locale[2]!='f') ) /* try our fallback.. */
      {
	flagPath[0] = toupper(locale[3]);
	flagPath[1] = toupper(locale[4]);
	
	for(ptr=standardFlags;*ptr;ptr += 2)
	  {
	    if((flagPath[0] == ptr[0]) && (flagPath[1] == ptr[1]))
	      break;
	  }
	
	if(*ptr) /* convert it */
	  {
	    flagPath[0] = tolower(flagPath[0]);
	    flagPath[1] = tolower(flagPath[1]);
	    strcpy(flagPath+2,"-t.gif");
	  }
	else
	  flagPath[0] = 0;
      }

    
    /* OK, if we got anything.. */
   if(flagPath[0])
     {
       u_fprintf(OUT,"<IMG SRC=\"%s/localeexplorer/flags/%s\" BORDER=1 %s ALT=[flag]>",
		 kStaticURLPrefix,
		 flagPath,
		 extra,
		 locale);
     }
   ures_close(flagRB);
   
   if(strstr(locale, "EURO"))
     {
       u_fprintf(OUT, "<A HREF=\"http://europa.eu.int/euro\"><IMG SRC=\"%s/localeexplorer/flags/euro.gif\" BORDER=0 ALT=[euro] HEIGHT=25 WIDTH=32></A> ",
		 kStaticURLPrefix); 
     }
}
#endif

#define kXKeyBGColor "\"#AAEEAA\" "

/* 
   pluggable UI
*/
void showKeyAndStartItemShort(const char *key, const UChar *keyName, const char *locale, UErrorCode showStatus)
{
      u_fprintf(OUT, "<P><TABLE BORDER=0 CELLSPACING=0 WIDTH=100%%>");
      u_fprintf(OUT, "<TR><TD HEIGHT=5 BGCOLOR=\"#AFA8AF\" COLSPAN=2><IMG SRC=/developerworks/opensource/icu/project/c.gif ALT=\"---\" WIDTH=0 HEIGHT=0></TD></TR>\r\n");
      u_fprintf(OUT, "<TR><TD COLSPAN=1 WIDTH=0 VALIGN=TOP BGCOLOR=" kXKeyBGColor "><A NAME=%s><B>", key);

      if(keyName == NULL)
	keyName = FSWF( key, key );

      printHelpTag(key, keyName);

      u_fprintf(OUT,"</B></A>", keyName);
      u_fprintf(OUT," </TD><TD BGCOLOR=" kXKeyBGColor " WIDTH=0 ALIGN=RIGHT>");
      u_fprintf(OUT, "&nbsp;");
      explainStatus(showStatus, key);
}

void showKeyAndStartItem(const char *key, const UChar *keyName, const char *locale, UErrorCode showStatus)
{
  showKeyAndStartItemShort(key,keyName,locale,showStatus);
  u_fprintf(OUT,"</TD><TR><TD COLSPAN=2>\r\n");
}

void showKeyAndEndItem(const char *key, const char *locale)
{
  u_fprintf(OUT, "</TR></TABLE>\r\n");
}
