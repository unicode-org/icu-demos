/**********************************************************************
*   Copyright (C) 1999, International Business Machines
*   Corporation and others.  All Rights Reserved.
***********************************************************************/
#include "unicode/utimzone.h"
#include "unicode/timezone.h"
#include "unicode/simpletz.h"

TimeZone *gmtZone  = NULL; // for comparison

U_CAPI UTimeZone *utz_open(const UChar* id)
{
  TimeZone *z = TimeZone::createTimeZone(UnicodeString(id));

  if(gmtZone == NULL)
    {
      TimeZone *aZone = TimeZone::createTimeZone("GMT");
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

U_CAPI void utz_close(UTimeZone* zone)
{
  delete ((TimeZone*)zone);
}

U_CAPI int32_t utz_getRawOffset(const UTimeZone *zone)
{
  return ((TimeZone*)zone)->getRawOffset();
}

// int32_t utz_getDisplayName(zone, daylight, style, locale, result, resultLength, status)

// UBool utz_useDayightTime(zone);

//U_CAPI UBool utz_hasSameRules(const UTimeZone *zone, const UTimeZone *other);

U_CAPI const UChar* utz_hackyGetDisplayName(const UTimeZone *zone)
{
  if(!zone)
    return NULL;

  return ((const TimeZone*)zone)->getDisplayName(*(new UnicodeString())).getBuffer();
}

U_CAPI void utz_setDefault(const UTimeZone *zone)
{
    TimeZone::setDefault(*((TimeZone*)zone));
}
