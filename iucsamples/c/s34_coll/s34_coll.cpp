// Copyright (c) 2010-2012 IBM Corporation and Others. All Rights Reserved.

#include <unicode/ures.h>
#include <unicode/unistr.h>
#include <unicode/resbund.h>
#include <unicode/ustdio.h>
#include <unicode/putil.h>
#include <unicode/msgfmt.h>
#include <unicode/calendar.h>
#include <unicode/ucol.h>
#include <unicode/coll.h>

#include "../iucsamples.h"



void s34_coll_init() 
{
  // C
  {
    UErrorCode status = U_ZERO_ERROR;
    UCollator *coll = ucol_open("en_US", &status);
    if(U_SUCCESS(status)) {
      /* do useful things with a collator */
      ucol_close(coll);
    }
  }
  
  // C++
  {
    UErrorCode status = U_ZERO_ERROR;
    Collator *coll = Collator::createInstance(Locale("en", "US"), status); 
    if(U_SUCCESS(status)) {
      // do useful things with a collator
      delete coll;
    } 
  }
}

/* for now, noop */
#define swap(x,y) 

void s36_csort() {
  UChar *s [] = { /* list of Unicode strings */ };
  uint32_t listSize = sizeof(s)/sizeof(s[0]);
  UErrorCode status = U_ZERO_ERROR;
  UCollator *coll = ucol_open("en_US", &status);
  uint32_t i, j;
  if(U_SUCCESS(status)) {
    for(i=listSize-1; i>=1; i--) {
      for(j=0; j<i; j++) {
        if(ucol_strcoll(coll, s[j], -1, s[j+1], -1) == UCOL_LESS) {
          swap(s[j], s[j+1]);
        }
      }
    }
    ucol_close(coll);
  } 
}

void s36_cppsort() {
  UnicodeString s[] = { /* list of Unicode strings */ };
  uint32_t listSize = sizeof(s)/sizeof(s[0]);
  UErrorCode status = U_ZERO_ERROR;
  Collator *coll = Collator::createInstance(Locale("en", "US"), status);
  uint32_t i, j;
  if(U_SUCCESS(status)) {
    for(i=listSize-1; i>=1; i--) {
      for(j=0; j<i; j++) {
        if(coll->compare(s[j], s[j+1]) == UCOL_LESS) {
          swap(s[j], s[j+1]);
        }
      }
    }
    delete coll;
  }
}


int main() {
  s34_coll_init(); /* just show API */

  return 0;
}
