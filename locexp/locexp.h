#ifndef LOCEXP_
#define LOCEXP_

#include "unicode/utypes.h"
#include "unicode/ustdio.h"
#include "unicode/lx_utils.h"
#include "unicode/decompcb.h"
#include "unicode/translitcb.h"

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

  MySortable     specialParLocale; /* owns the parent IF it's not in the tree */

  /* === Put the following  into a CACHE - keyed on locale! */
  MySortable      *locales ;     /* tree of locales */
  int32_t          numLocales;

  /* === Context information for callbacks.. woo hoo ! */
  FromUBackslashContext   backslashCtx;
  FromUDecomposeContext   decomposeCtx;
  FromUTransliteratorContext   xlitCtx;
  
} LXContext;

/**
 * dump out a file that's ina  resource bundle
 */
extern void writeFileObject( LXContext *lx, const char *path );
extern void runLocaleExplorer(LXContext *myContext);
extern void initContext( LXContext *lx );

extern void initLX();
extern void closeLX();
extern void explainStatus( LXContext *lx, UErrorCode status, const char *tag );


#endif
