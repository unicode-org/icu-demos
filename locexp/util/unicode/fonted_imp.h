/**********************************************************************
*   Copyright (C) 1999, International Business Machines
*   Corporation and others.  All Rights Reserved.
***********************************************************************/

/*

  fonted callback internal header

*/

#ifndef FONTEDIMP_H
#define FONTEDIMP_H

#include "unicode/ucnv.h"

/* Sub modules. Return TRUE on success. */

#define U_FONTED_DEVANAGARI_START 0x0900
#define U_FONTED_DEVANAGARI_END   0x097F

U_CAPI bool_t
  fonted_devanagari (UConverter * _this,
		     unsigned char **target,
		     const unsigned char *targetLimit,
		     const UChar ** source,
		     const UChar * sourceLimit,
		     int32_t *offsets,
		     bool_t flush,
		     UErrorCode * err);

#define U_FONTED_KANNADA_START 0x0C80
#define U_FONTED_KANNADA_END   0x0CEF

U_CAPI bool_t
  fonted_kannada    (UConverter * _this,
		     unsigned char **target,
		     const unsigned char *targetLimit,
		     const UChar ** source,
		     const UChar * sourceLimit,
		     int32_t *offsets,
		     bool_t flush,
		     UErrorCode * err);


#endif
