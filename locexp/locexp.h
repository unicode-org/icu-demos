#ifndef LOCEXP_
#define LOCEXP_

#include "unicode/utypes.h"
#include "unicode/ustdio.h"

typedef struct 
{
  UFILE *OUT;        /* out stream */
  char cLocale[200]; /* client locale */
  
  const char *couldNotOpenEncoding;      /* contains error string if nonnull */
  const char *ourCharsetName; /* HTML friendly name of the current charset */
  char        chosenEncoding[128];
  
  bool_t setEncoding;            /* what is our state? What's setup? */
  bool_t setLocale;

  UResourceBundle *defaultRB;        /* RB in the selected locale  - CACHE*/

  /* ====== locale navigation ===== */
  MySortable      *curLocale;     /* Current locale */
  MySortable      *parLocale;     /* Parent locale of current */
  char             curLocaleName[128];
  UChar           newZone[300];          /* Timezone to set to */
  const UChar          *timeZone;

  /* === Put the following  into a CACHE - keyed on locale! */
  MySortable      *locales ;     /* tree of locales */
  int32_t          numLocales;
  
} LXContext;

#endif
