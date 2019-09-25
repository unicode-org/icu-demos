/**********************************************************************
*   Copyright (C) 1999, International Business Machines
*   Corporation and others.  All Rights Reserved.
***********************************************************************/
#include "unicode/utimzone.h"
#include "unicode/timezone.h"
#include "unicode/simpletz.h"
#include "unicode/strenum.h"
#include "unicode/unistr.h"

#include <stdlib.h>

icu::TimeZone *gmtZone  = NULL; // for comparison

U_CAPI UTimeZone *utz_open(const UChar* id)
{
  icu::TimeZone *z = icu::TimeZone::createTimeZone(icu::UnicodeString(id));

  if(gmtZone == NULL)
    {
      icu::TimeZone *aZone = icu::TimeZone::createTimeZone("GMT");
      gmtZone = aZone; // no concurrency problem here, possible leak.
    }

  if(z->hasSameRules(*gmtZone) && id != NULL)
    {
      // ARGH.
      // Of course, they could ask for any number of aliases to GMT. 
      // broken for now.

      delete z;
      return NULL;
    }

  return (UTimeZone*)z;
}

U_CAPI UTimeZone *utz_openDefault() {
    icu::TimeZone *z = icu::TimeZone::createDefault();
    return z;
}

U_CAPI int utz_getID(const UTimeZone *zone, char *idbuf, int idlen) {
    icu::UnicodeString id;
    ((icu::TimeZone*)zone)->getID(id);
    idbuf[0]=0;
    return id.extract(0,id.length(),idbuf);
}


U_CAPI void utz_close(UTimeZone* zone)
{
  delete ((icu::TimeZone*)zone);
}

U_CAPI int32_t utz_getRawOffset(const UTimeZone *zone)
{
  return ((icu::TimeZone*)zone)->getRawOffset();
}

// int32_t utz_getDisplayName(zone, daylight, style, locale, result, resultLength, status)

// UBool utz_useDayightTime(zone);

//U_CAPI UBool utz_hasSameRules(const UTimeZone *zone, const UTimeZone *other);

U_CAPI const UChar* utz_hackyGetDisplayName(const UTimeZone *zone)
{
  if(!zone)
    return NULL;

  return ((const icu::TimeZone*)zone)->getDisplayName(*(new icu::UnicodeString())).getBuffer();
}

U_CAPI void utz_setDefault(const UTimeZone *zone)
{
    icu::TimeZone::setDefault(*((icu::TimeZone*)zone));
}

U_CAPI UStringEnumeration * utz_createEnumerationForTerritory(const char *territory) {
    return (UStringEnumeration*)icu::TimeZone::createEnumeration(territory);
}

U_CAPI const char *ustre_next(UStringEnumeration *e, int32_t *resultLength, UErrorCode *status) {
    return ((icu::StringEnumeration*)e)->next(resultLength, *status);
}

U_CAPI const UChar *ustre_unext(UStringEnumeration *e, int32_t *resultLength, UErrorCode *status) {
    return ((icu::StringEnumeration*)e)->unext(resultLength, *status);
}

U_CAPI const char *ustre_nextz(UStringEnumeration *e, int32_t *resultLength, UErrorCode *status) {
    const icu::UnicodeString *us =  ((icu::StringEnumeration*)e)->snext(*status);
    if(us == NULL) {
        return NULL;
    }
    char *f = (char*)malloc(212);
    *resultLength = us->length();
    f[0]=0;
    us->extract(0,*resultLength,f);
    return f;
}
 

U_CAPI void ustre_close(UStringEnumeration *e) {
    delete ((icu::StringEnumeration*)e);
}
U_CAPI int32_t ustre_count(UStringEnumeration *e, UErrorCode *status) {
    return ((icu::StringEnumeration*)e)->count(*status);
}
