/**********************************************************************
*   Copyright (C) 1999-2003, International Business Machines
*   Corporation and others.  All Rights Reserved.
***********************************************************************/
#ifndef LOCEXP_
#define LOCEXP_

/** Headers. Watch out for too many unixy things, tends to break win32 **/

#include "unicode/utypes.h"

#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include <string.h>

#ifdef WIN32
/* Silly Windows compatibility naming */
#ifndef snprintf
#define snprintf _snprintf
#endif
#endif


#include "unicode/decompcb.h"
#include "unicode/lx_utils.h"
#include "unicode/translitcb.h"
#include "unicode/ucal.h"
#include "unicode/uchar.h"
#include "unicode/ucnv.h"
#include "unicode/ucol.h"
#include "unicode/udat.h"
#include "unicode/uloc.h"
#include "unicode/umsg.h"
#include "unicode/ures.h"
#include "unicode/ures_additions.h"
#include "unicode/uscript.h"
#include "unicode/ushape.h"
#include "unicode/usort.h"
#include "unicode/ustdio.h"
#include "unicode/ustring.h"
#include "unicode/utimzone.h"
#include "unicode/utrans.h"
#include <ctype.h>
/* #include "unicode/collectcb.h" */

/* for data */
#ifdef LX_STATIC
U_CFUNC char locexp_dat[];
#endif
/* end data */

#ifdef WIN32  /** Need file control to fix stdin/stdout issues **/
# include <fcntl.h>
# include <io.h>
#endif


typedef struct 
{
  /* ============= HTTP input - to be set up by context initializer */
  const char *scriptName;      /* Cached results of getenv("SCRIPT_NAME") */
  const char *queryString;     /* QUERY_STRING */
  const char *acceptCharset;   /* HTTP_ACCEPT_CHARSET */
  const char *acceptLanguage;  /* HTTP_ACCEPT_LANGUAGE */
  const char *serverName;      /* SERVER_NAME */
  char hostPort[256];          /* host:port  or just host  (if port 80) */
  const char *pathInfo;        /* PATH_INFO */
  uint16_t    port;            /* SERVER_PORT */
  const char *hostName;        /* HTTP_HOST */
  const char *postData;
  
  /* ============= IO */
  FILE  *fOUT;       /* low level file output */
  UFILE *OUT;        /* out stream */
  
  char    *headers;     /* append headers here (use appendHeader function, below) */
  int32_t  headerLen;  /* length of header *buffer*  */

  /* ============= ENCODING */
  const char *couldNotOpenEncoding;      /* contains error string if nonnull */

  const char *convName;      /* HTML friendly name of the current charset. was 'ourCharsetName' */
  const char *convRequested; /* The explicit charset in the URL, or the implied (detected) charset. Was 'chosenEncoding' */
  const char *convUsed;      /* Actual converter opened with ucnv_open() */
  UBool       convSet;       /* set to TRUE if user specified a converter */

  /* ============= OTHER STATE */
  UBool  inDemo;       /* are we in a 'demo' (EXPLORER) page? If so, don't show encoding and other options */
  const char *fileObj;    /* if set - then we're writing out a data object */

  /* ============= DISPLAY LOCALE */
  const char      *dispLocale; /* client (display) locale - was cLocale */
  UResourceBundle *dispRB;        /* RB in the display locale  - CACHE (was defaultRB)*/
  UBool            dispLocaleSet; /* Display locale has been set */

  /* ============= SELECTED LOCALE */
  UResourceBundle *curRB;            /* RB in the current locale */ 

  MySortable      *curLocale;     /* Current locale */
  MySortable      *parLocale;     /* Parent locale of current */
  char             curLocaleName[128];
  UChar           newZone[300];          /* Timezone to set to */
  const UChar          *timeZone;
  UChar displayName[1024];        /* Cache of current language display name */
  
  /* current locale's default calendar */
  char             defaultCalendar[1024];
  
  MySortable     specialParLocale; /* owns the parent IF it's not in the tree */

  /* === TODO: put the following  into a CACHE keyed on locale! */
  MySortable      *locales ;     /* tree of locales] */
  int32_t          numLocales;

  /* === Context information for callbacks..  */
  FromUBackslashContext   backslashCtx;
  FromUDecomposeContext   decomposeCtx;
  FromUTransliteratorContext   xlitCtx;
  
} LXContext;

/********************** Some Konstants **** and structs ***************/

/** Lengths and limits **/
#define UCA_LEN 110000           /* showCollationElements() */
#define SORTSIZE 32767            /* showSort() */


/** Tuning and appearance **/
#define kStatusBG "\"#EEEEEE\" " 
#define kXKeyBGColor "\"#AAEEAA\" "
#define kShowStringCutoffSize 200   /* size in chars before a string is 'too big'. */
#define kShowUnicodeSetCutoffSize 80   /* size in chars before a string is 'too big'. */
#define kShow2dArrayRowCutoff 5     /* size in rows before an array is too big */
#define kShow2dArrayColCutoff 5     /* size in cols before an array is too big */

#define G7COUNT 8  /* all 8 of the g7 locales. showSort() */
static const char   G7s[G7COUNT][10] = { "de_DE", "en_GB", "en_US", "fr_CA", "fr_FR", "it_IT", "ja_JP", "sv_SE" };

/** If we aren't on Win32, need to make up a hostname. **/
#ifdef WIN32
# define LXHOSTNAME "Win_NT"
# define URLPREFIX ""
#endif


/********************* prototypes ***************************/
/********************* prototypes ***************************/
/********************* prototypes ***************************/

/**
 * dump out a file that's ina  resource bundle
 */
extern void writeFileObject( LXContext *lx, const char *path );
extern void runLocaleExplorer(LXContext *myContext);
extern void setupLocaleExplorer(LXContext *lx);
extern void displayLocaleExplorer(LXContext *lx);
extern void initContext( LXContext *lx );

extern void initLX();
extern void openLX();
extern void closeLX(LXContext *lx);
extern void explainStatus( LXContext *lx, UErrorCode status, const char *tag );

typedef enum { kNormal, kCal } ECal;

/* setup the UFILE */

/* Setup the 'locales' structure */
extern void   setupLocaleTree(LXContext *lx);

/* some fcns for different parts of the screen */
extern void doFatal(LXContext *lx, const char *what, UErrorCode err);
extern void printStatusTable(LXContext *lx);
extern const UChar *defaultLanguageDisplayName(LXContext *lx);
/**
 * Print the path [ ICU LocaleExplorer > default > English > English (Canadian) ...] 
 * @param leaf Which node to start printing at
 * @param current The locale that should be selected
 * @param styled Should bold tags and links be put in?
 * @param suffix Any text to follow the URL
 */
extern void printPath(LXContext *lx, const MySortable *leaf, const MySortable *current, UBool styled, const char *suffix);
extern void printSubLocales(LXContext *lx, const char *suffix) ;

/* selection of locales and converter */
extern void chooseLocale(LXContext *lx, UBool toOpen, const char *current, const char *restored, UBool showAll);
extern void chooseConverter(LXContext *lx, const char *restored);

extern void chooseConverterMatching(LXContext *lx, const char *restored, UChar *sample);

extern void chooseConverterFrom(LXContext *lx, const char *restored, USort *list);
extern void showOneLocale(LXContext *lx);
void showExploreBreak(LXContext *lx, const char *locale);

/* fcns for dumping the contents of a particular rb */
extern void showCollationElements( LXContext *lx, UResourceBundle *rb, const char *locale, const char *whichString);
extern void showString( LXContext *lx, UResourceBundle *rb, const char *locale, const char *whichString, UBool PREformatted);
extern void showUnicodeSet( LXContext *lx, UResourceBundle *rb, const char *locale, const char *whichString, UBool PREformatted);
extern void showInteger( LXContext *lx, UResourceBundle *rb, const char *locale, const char *whichString, int radix);
extern void showLocaleCodes(LXContext *lx, UResourceBundle *myRB, const char *locale);
extern void showLocaleScript(LXContext *lx, UResourceBundle *myRB, const char *locale);
extern void showStringWithDescription( LXContext *lx, UResourceBundle *rb, const char *locale, const UChar *desc[], const char *whichString, UBool hidable);
extern void showArray( LXContext *lx, UResourceBundle *rb, const char *locale, const char *whichString, ECal isCal);
extern void showArrayWithDescription( LXContext *lx, UResourceBundle *rb, const char *locale, const UChar *desc[], const char *whichString, ECal);
extern void show2dArrayWithDescription( LXContext *lx, UResourceBundle *rb, const char *locale, const UChar *desc[], const char *whichString);
extern void showTaggedArray( LXContext *lx, UResourceBundle *rb, const char *locale, const char *whichString, UBool compareToDisplay);
extern void showCurrencies( LXContext *lx, UResourceBundle *rb, const char *locale);
extern void showShortLongCal( LXContext *lx, UResourceBundle *rb, const char *locale, const char *keyStem, const UChar *shortName, const UChar *longName, int32_t num);   /* Cal meaning, the defaultCalendar part of the context is taken into account */
extern void showDateTimeElements( LXContext *lx, UResourceBundle *rb, const char *locale);
extern void showSort( LXContext *lx, const char *locale);
extern void showSortStyle( LXContext *lx );

extern void showExploreDateTimePatterns( LXContext *lx, UResourceBundle *rb, const char *locale);
extern void showExploreNumberPatterns  ( LXContext *lx, const char *locale);

extern void showExploreButton( LXContext *lx, UResourceBundle *rb, const char *locale, const UChar *sampleString, const char *key);
extern void showExploreButtonSort( LXContext *lx, UResourceBundle *rb, const char *locale, const char *key, UBool rightAlign);
extern void showExploreLink( LXContext *lx, UResourceBundle *rb, const char *locale, const UChar *sampleString, const char *key);
extern void showExploreCloseButton(LXContext *lx, const char *locale, const char *frag);
extern void showExploreCalendar(LXContext *lx); /* in calexp.c */
extern void showExploreSearch(LXContext *lx);   /* in srchexp.c */
void showExploreSearchForm(LXContext *lx, const UChar *valueString);
extern void showSpelloutExample(LXContext *lx, UResourceBundle *rb, const char *locale);


extern UBool didUserAskForKey(LXContext *lx, const char *key);

/*  Pluggable UI.  Put these before and after each item. */
extern void showKeyAndStartItem(LXContext *lx, const char *key, const UChar *keyName, const char *locale, UBool cumulative, UErrorCode showStatus);
extern void showKeyAndStartItemShort(LXContext *lx, const char *key, const UChar *keyName, const char *locale, UBool cumulative, UErrorCode showStatus);
extern void showKeyAndEndItem(LXContext *lx, const char *key, const char *locale);

/**
 * (for CGI programs)
 * Build a UFILE based on the user's preference for locale and encoding.
 * Try to figure out what a good encoding to use is.
 * 
 * @param chosenEncoding (on return) the encoding that was chosen
 * @param didSetLocale   (on return) TRUE if a locale was chosen
 * @return the new UFILE. Doesn't set any callbacks
 */
extern UFILE *openUFILE(LXContext *lx);
extern void setLocaleAndEncoding(LXContext *lx);

/* write a string in \uXXXX format */
extern void writeEscaped(LXContext *lx, const UChar *s);
extern char *createEscapedSortList(const UChar *source);

/* is this a locale we should advertise as supported? */
extern UBool isSupportedLocale(const char *locale, UBool includeChildren); /* for LX interface */
extern UBool isExperimentalLocale(const char *local); /* for real data */
extern void printLocaleAndSubs(LXContext *lx, UBool toOpen, MySortable *l, const char *current, const char *restored, UBool *hadUnsupportedLocales);
extern void printLocaleLink(LXContext *lx, UBool toOpen, MySortable *l, const char *current, const char *restored, UBool *hadUnsupportedLocales);

extern void printHelpTag(LXContext *lx, const char *helpTag, const UChar *str);
/* ex: printHelpImg("coffee", L"[coffee help symbol]", L"coffee-help.gif", L"BORDER=3"); */
extern void printHelpImg(LXContext *lx, const char *helpTag, const UChar *alt, const UChar *img, const UChar *options);

extern void exploreFetchNextPattern(LXContext *lx, UChar *dstPattern,const char *patternText);
extern void exploreShowPatternForm(LXContext *lx, UChar *dstPattern, const char *locale, const char *key, const char *patternText, double value, UNumberFormat *valueFmt);

/* get bundle for current locale */
extern UResourceBundle *getCurrentBundle(LXContext* lx, UErrorCode *status);
/* get bundle for display locale */
extern UResourceBundle *getDisplayBundle(LXContext* lx, UErrorCode *status);

/* CGI (or CGI-like) helper functions */
void initCGIVariables(LXContext* lx);
void initPOSTFromFILE(LXContext* lx, FILE *f);
void closeCGIVariables(LXContext* lx);
void closePOSTFromFILE(LXContext* lx);

/* lxurl */
/* find start of field from a query like string */
const char *fieldInQuery(LXContext* lx, const char *query, const char *field);
const char *queryField(LXContext* lx, const char *field);
UBool hasQueryField(LXContext* lx, const char *field);

/* header management */
/**
 * Add a header to the list.  Do not add trailing cr/lf.
 * @param lx the context
 * @param header the header name (e.g. "Content-type")
 * @param fmt stdio (host, not unicode!) format string + params (e.g. "text/plain; charset=%s", "btf-5") 
 */
void appendHeader(LXContext* lx, const char *header, const char *fmt, ...);


#endif

