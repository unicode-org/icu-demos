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


/* probably not needed */
#ifdef WIN32
UConverterFromUCallback TRANSLITERATED_lastResortCallback;
#else
extern UConverterFromUCallback TRANSLITERATED_lastResortCallback;
#endif

U_CAPI void 
  UCNV_FROM_U_CALLBACK_TRANSLITERATED (UConverter * _this,
			       char **target,
					    const char *targetLimit,
					    const UChar ** source,
					    const UChar * sourceLimit,
					    int32_t *offsets,
					    bool_t flush,
					    UErrorCode * err);


#endif





