/**********************************************************************
*   Copyright (C) 1999, International Business Machines
*   Corporation and others.  All Rights Reserved.
***********************************************************************/

/* 
   Non-multithread safe callback.

   This callback will record a list of all the uchars that failed
   to convert, up to whatever you set in COLLECT_setSize().

   COLLECT_getChars() will return the chars as a null terminated list.

*/

#ifndef COLLECTCB_H
#define COLLECTCB_H

#include "unicode/utypes.h"
#include "unicode/ucnv.h"

extern UConverterFromUCallback COLLECT_lastResortCallback;

/* if TRUE, collect will only record chars that pass the u_isalnum() 
   test in uchars.h. if FALSE (default), collect will record all chars.
*/
#ifdef WIN32
UBool                  COLLECT_alnum;
#else
extern UBool                  COLLECT_alnum;
#endif

U_CAPI void 
  UCNV_FROM_U_CALLBACK_COLLECT (UConverter * _this,
				char **target,
				const char *targetLimit,
				const UChar ** source,
				const UChar * sourceLimit,
				int32_t *offsets,
				UBool flush,
				UErrorCode * err);


/* set the # of bad chars to be stored, also clear the list. */
U_CAPI void COLLECT_setSize(int32_t sz);

/* Return a null terminated list of bad UChars (up to the size set in setSize) */
U_CAPI const UChar* COLLECT_getChars();


#endif
