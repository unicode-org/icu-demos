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

typedef struct
{
  UConverterFromUCallback  subCallback;
  const void                    *subContext;

  /* special data here */
} FromUDecomposeContext;



U_CAPI void 
  UCNV_FROM_U_CALLBACK_DECOMPOSE  (const void *context,
                                UConverterFromUnicodeArgs *fromUArgs,
                                const UChar* codeUnits,
                                int32_t length,
                                UChar32 codePoint,
                                UConverterCallbackReason reason,
                                UErrorCode *err);


/* NOTE: returns USING_FALLBACK_ERROR instead of ZERO_ERROR if 
   decomposition occurs. (does NOT occur in INDEX_OUTOFBOUNDS cases) */


#endif
