#ifndef LOCEXP_
#define LOCEXP_

/** Headers. Watch out for too many unixy things, tends to break win32 **/

#include "unicode/utypes.h"

#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include <string.h>


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
  FILE  *fOUT;       /* low level file output */
  UFILE *OUT;        /* out stream */
  char cLocale[200]; /* client locale */

  const char *couldNotOpenEncoding;      /* contains error string if nonnull */
  const char *ourCharsetName; /* HTML friendly name of the current charset */
  char        chosenEncoding[128];
  
  UBool setEncoding;            /* what is our state? What's setup? */
  UBool setLocale;

  UResourceBundle *defaultRB;        /* RB in the selected locale  - CACHE*/

  UBool  inDemo;       /* are we in a 'demo' page? If so, don't show encoding/ecc options */

  /* ====== locale navigation ===== */
  MySortable      *curLocale;     /* Current locale */
  MySortable      *parLocale;     /* Parent locale of current */
  char             curLocaleName[128];
  UChar           newZone[300];          /* Timezone to set to */
  const UChar          *timeZone;
    UChar displayName[1024];        /* Cache of current language display name */

  MySortable     specialParLocale; /* owns the parent IF it's not in the tree */

  /* === Put the following  into a CACHE - keyed on locale! */
  MySortable      *locales ;     /* tree of locales */
  int32_t          numLocales;

  /* === Context information for callbacks.. woo hoo ! */
  FromUBackslashContext   backslashCtx;
  FromUDecomposeContext   decomposeCtx;
  FromUTransliteratorContext   xlitCtx;
  
} LXContext;

/********************** Some Konstants **** and structs ***************/

/** Lengths and limits **/
#define UCA_LEN 110000           /* showCollationElements() */
#define SORTSIZE 8192            /* showSort() */


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

extern void openLX();
extern void closeLX(LXContext *lx);
extern void explainStatus( LXContext *lx, UErrorCode status, const char *tag );



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
 */
extern void printPath(LXContext *lx, const MySortable *leaf, const MySortable *current, UBool styled);

/* selection of locales and converter */
extern void chooseLocale(LXContext *lx, const char *qs, UBool toOpen, const char *current, const char *restored, UBool showAll);
extern void chooseConverter(LXContext *lx, const char *restored);

extern void chooseConverterMatching(LXContext *lx, const char *restored, UChar *sample);

extern void chooseConverterFrom(LXContext *lx, const char *restored, USort *list);
extern void showOneLocale(LXContext *lx, char *b);

/* fcns for dumping the contents of a particular rb */
extern void showCollationElements( LXContext *lx, UResourceBundle *rb, const char *locale, const char *qs, const char *whichString);
extern void showString( LXContext *lx, UResourceBundle *rb, const char *locale, const char *qs, const char *whichString, UBool PREformatted);
extern void showUnicodeSet( LXContext *lx, UResourceBundle *rb, const char *locale, const char *qs, const char *whichString, UBool PREformatted);
extern void showInteger( LXContext *lx, UResourceBundle *rb, const char *locale, const char *whichString, int radix);
extern void showLocaleCodes(LXContext *lx, UResourceBundle *myRB, const char *locale);
extern void showLocaleScript(LXContext *lx, UResourceBundle *myRB, const char *locale);
extern void showStringWithDescription( LXContext *lx, UResourceBundle *rb, const char *locale, const char *qs, const UChar *desc[], const char *whichString, UBool hidable);
extern void showArray( LXContext *lx, UResourceBundle *rb, const char *locale, const char *whichString);
extern void showArrayWithDescription( LXContext *lx, UResourceBundle *rb, const char *locale, const UChar *desc[], const char *whichString);
extern void show2dArrayWithDescription( LXContext *lx, UResourceBundle *rb, const char *locale, const UChar *desc[], const char *queryString, const char *whichString);
extern void showTaggedArray( LXContext *lx, UResourceBundle *rb, const char *locale, const char *queryString, const char *whichString);
extern void showShortLong( LXContext *lx, UResourceBundle *rb, const char *locale, const char *keyStem, const UChar *shortName, const UChar *longName, int32_t num);
extern void showDateTimeElements( LXContext *lx, UResourceBundle *rb, const char *locale);
extern void showSort( LXContext *lx, const char *locale, const char *b);

extern void showExploreDateTimePatterns( LXContext *lx, UResourceBundle *rb, const char *locale, const char *b);
extern void showExploreNumberPatterns  ( LXContext *lx, const char *locale, const char *b);

extern void showExploreButton( LXContext *lx, UResourceBundle *rb, const char *locale, const UChar *sampleString, const char *key);
extern void showExploreButtonSort( LXContext *lx, UResourceBundle *rb, const char *locale, const char *sampleString, const char *key, UBool rightAlign);
extern void showExploreLink( LXContext *lx, UResourceBundle *rb, const char *locale, const UChar *sampleString, const char *key);
extern void showExploreCloseButton(LXContext *lx, const char *locale, const char *frag);
extern void showExploreCalendar(LXContext *lx, const char *qs); /* in calexp.c */
extern void showExploreSearch(LXContext *lx, const char *qs);   /* in srchexp.c */
void showExploreSearchForm(LXContext *lx, const UChar *valueString);
extern void showSpelloutExample(LXContext *lx, UResourceBundle *rb, const char *locale);


extern UBool didUserAskForKey(const char *key, const char *queryString);

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
extern UFILE *setLocaleAndEncodingAndOpenUFILE(LXContext *lx, char *chosenEncoding, UBool *didSetLocale, UBool *didSetEncoding, const char **fileObject);

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

extern void exploreFetchNextPattern(LXContext *lx, UChar *dstPattern, const char* qs);
extern void exploreShowPatternForm(LXContext *lx, UChar *dstPattern, const char *locale, const char *key, const char* qs, double value, UNumberFormat *valueFmt);

#endif

