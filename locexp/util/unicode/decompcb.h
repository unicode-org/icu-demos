/**********************************************************************
*   Copyright (C) 1999, International Business Machines
*   Corporation and others.  All Rights Reserved.
***********************************************************************/

/*

  A callback that applies Unicode decomposition and other tables
  looking for a better character fit.

*/

#ifndef _DECOMPCB
#define _DECOMPCB

#include "unicode/ucnv.h"

extern  UConverterFromUCallback DECOMPOSE_lastResortCallback;

U_CAPI void 
  UCNV_FROM_U_CALLBACK_DECOMPOSE (UConverter * _this,
					    char **target,
					    const char *targetLimit,
					    const UChar ** source,
					    const UChar * sourceLimit,
					    int32_t *offsets,
					    bool_t flush,
					    UErrorCode * err);

/* NOTE: returns USING_FALLBACK_ERROR instead of ZERO_ERROR if 
   decomposition occurs. (does NOT occur in INDEX_OUTOFBOUNDS cases) */


#endif
