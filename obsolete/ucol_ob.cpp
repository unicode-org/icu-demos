/*
*******************************************************************************
*
*   Copyright (C) 1996-2003, International Business Machines
*   Corporation and others.  All Rights Reserved.
*
*******************************************************************************
*   file name:  ucol_ob.cpp
*   encoding:   US-ASCII
*   tab size:   8 (not used)
*   indentation:4
*
*   created on: 2003apr30
*   created by: Markus W. Scherer
*
*   This file contains obsolete definitions from ucol.cpp.
*/

#include "unicode/utypes.h"

#if !UCONFIG_NO_COLLATION

#include <string.h>
#include "unicode/uchar.h"
#include "unicode/obsolete.h"
#include "unicode/ucol_ob.h"

U_CAPI UCollator * U_EXPORT2
ucol_openVersion(const char *loc,
                 UVersionInfo version,
                 UErrorCode *status) {
  UCollator *collator;
  UVersionInfo info;

  collator=ucol_open(loc, status);
  if(U_SUCCESS(*status)) {
    ucol_getVersion(collator, info);
    if(0!=memcmp(version, info, sizeof(UVersionInfo))) {
      ucol_close(collator);
      *status=U_MISSING_RESOURCE_ERROR;
      return NULL;
    }
  }
  return collator;
}

U_CAPI void U_EXPORT2
ucol_setNormalization(  UCollator            *coll,
            UNormalizationMode    mode)
{
  UErrorCode status = U_ZERO_ERROR;
  switch(mode) {
  case UNORM_NONE:
    ucol_setAttribute(coll, UCOL_NORMALIZATION_MODE, UCOL_OFF, &status);
    break;
  case UNORM_NFD:
    ucol_setAttribute(coll, UCOL_NORMALIZATION_MODE, UCOL_ON, &status);
    break;
  default:
    /* Shouldn't get here. */
    /* This is quite a bad API */
    /* *status = U_ILLEGAL_ARGUMENT_ERROR; */
    return;
  }
}

U_CAPI UNormalizationMode U_EXPORT2
ucol_getNormalization(const UCollator* coll)
{
  UErrorCode status = U_ZERO_ERROR;
  if(ucol_getAttribute(coll, UCOL_NORMALIZATION_MODE, &status) == UCOL_ON) {
    return UNORM_NFD;
  } else {
    return UNORM_NONE;
  }
}

#endif /* #if !UCONFIG_NO_COLLATION */
