/**********************************************************************
*   Copyright (C) 1999, International Business Machines
*   Corporation and others.  All Rights Reserved.
***********************************************************************/
#include "unicode/utypes.h"

#ifndef _UTIMZONE
#define _UTIMZONE

typedef void *UTimeZone;

/**
 * create a tz.  Thanks to the broken TimeZone::createTimeZone() api,
 * this function will only return a GMT zone if you pass in NULL.
 * (there's no way to distinguish between a bad timezone and GMT)
 * @param id the id to create
 * @returns NULL if the tz doesn't exist. [ NOT gmt! ]
 * TODO: change this to char*
 */
U_CAPI UTimeZone *utz_open(const UChar *id);



U_CAPI void utz_close(UTimeZone* zone);

U_CAPI int32_t utz_getRawOffset(const UTimeZone *zone);

// int32_t utz_getDisplayName(zone, daylight, style, locale, result, resultLength, status)

U_CAPI const UChar *utz_hackyGetDisplayName(const UTimeZone *zone);

// bool_t utz_useDayightTime(zone);

//U_CAPI bool_t utz_hasSameRules(const UTimeZone *zone, const UTimeZone *other);

#endif
