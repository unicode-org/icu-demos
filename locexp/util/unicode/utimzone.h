/**********************************************************************
*   Copyright (C) 1999-2007, International Business Machines
*   Corporation and others.  All Rights Reserved.
***********************************************************************/
#include "unicode/utypes.h"

#ifndef _UTIMZONE
#define _UTIMZONE

typedef void UTimeZone;
typedef void UStringEnumeration;

/**
 * create a tz.  Thanks to the broken TimeZone::createTimeZone() api,
 * this function will only return a GMT zone if you pass in NULL.
 * (there's no way to distinguish between a bad timezone and GMT)
 * @param id the id to create
 * @returns NULL if the tz doesn't exist. [ NOT gmt! ]
 * TODO: change this to char*
 */
U_CAPI UTimeZone *utz_open(const UChar *id);

U_CAPI UTimeZone *utz_openDefault();

U_CAPI int getID(const UTimeZone *zone, char *idbuf, int idlen);

U_CAPI void utz_close(UTimeZone* zone);

U_CAPI int32_t utz_getRawOffset(const UTimeZone *zone);

U_CAPI const UChar *utz_hackyGetDisplayName(const UTimeZone *zone);


U_CAPI void utz_setDefault(const UTimeZone *zone);

U_CAPI UStringEnumeration * utz_createEnumerationForTerritory(const char *territory);

U_CAPI const char *ustre_next(UStringEnumeration *e, int32_t *resultLength, UErrorCode *status);

U_CAPI const char *ustre_nextz(UStringEnumeration *e, int32_t *resultLength, UErrorCode *status);

U_CAPI const UChar *ustre_unext(UStringEnumeration *e, int32_t *resultLength, UErrorCode *status);

U_CAPI void ustre_close(UStringEnumeration *e);

U_CAPI int32_t ustre_count(UStringEnumeration *e, UErrorCode *status);

#endif
