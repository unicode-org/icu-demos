/**********************************************************************
*   Copyright (C) 1999, International Business Machines
*   Corporation and others.  All Rights Reserved.
***********************************************************************/
/*


  'TRANSLIT' callback. Uses (special) fonts to render different scripts
  in HTML..

  Steven R. Loomis <srl@monkey.sbay.org>

  **NONE OF THIS CODE IS MULTITHREAD SAFE [yet] **
  **NONE OF THIS CODE IS MULTITHREAD SAFE [yet] **

*/

#ifndef TRANSLITCB_H
#define TRANSLITCB_H

#include "unicode/ucnv.h"

typedef struct
{
  UConverterFromUCallback  subCallback;

  /* PUBLIC */
  const void    *subContext;  /* For Chaining */
  const char    *locale;      /* source locale, will translit  locale-Latin */
  UBool          html;

  /* PRIVATE */
  UConverter *utf8; 
} FromUTransliteratorContext;


U_CAPI void 
  UCNV_FROM_U_CALLBACK_TRANSLITERATED  (const void *context,
                                UConverterFromUnicodeArgs *fromUArgs,
                                const UChar* codeUnits,
                                int32_t length,
                                UChar32 codePoint,
                                UConverterCallbackReason reason,
                                UErrorCode *err);

#endif





