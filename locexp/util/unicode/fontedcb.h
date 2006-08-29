/**********************************************************************
*   Copyright (C) 1998-2006, International Business Machines
*   Corporation and others.  All Rights Reserved.
***********************************************************************/
/*
  'fonted' callback. Uses (special) fonts to render different scripts
  in HTML..

  Steven R. Loomis <srl [at] icu-project.org>

*/

#ifndef FONTEDCB_H
#define FONTEDCB_H

#include "unicode/ucnv.h"

/* #include a bunch of stuff before this file... */

/* probably not needed */
extern UConverterFromUCallback FONTED_lastResortCallback;

U_CAPI void 
  UCNV_FROM_U_CALLBACK_FONTED (UConverter * _this,
			       char **target,
					    const char *targetLimit,
					    const UChar ** source,
					    const UChar * sourceLimit,
					    int32_t *offsets,
					    UBool flush,
					    UErrorCode * err);

/* NOTE: returns USING_FALLBACK_ERROR instead of ZERO_ERROR if 
   substitution occurs. (does NOT occur in INDEX_OUTOFBOUNDS cases).

   Note #2: 
     You MUST make sure this callback is called with valid chunks of
     text. It can't remember the font state across calls!

  Note #3, does not support offsets..

*/

U_CAPI void 
  UCNV_FROM_U_CALLBACK_NAMED (UConverter * _this,
			       char **target,
					    const char *targetLimit,
					    const UChar ** source,
					    const UChar * sourceLimit,
					    int32_t *offsets,
					    UBool flush,
					    UErrorCode * err);


#endif
