/***
 * 
 * Copyright (c) 2003 IBM Corporation and others, all rights reserved.
 *
 **
 *
 * 2003 - nov - 15   srl    New.
 *
 ***/

#if 0
//Put this in initLX()
    ucal_registerRepublicain(&calError);
    if(U_FAILURE(calError)) {
      fprintf(stderr, "reg fail: %s\n", u_errorName(calError));
    } else {
      fprintf(stderr, "reg OK : %s\n", u_errorName(calError));
    }
#endif


#define U_DEBUG_RC 1
#include <stdio.h>

#include "unicode/repubcal.h"

#if !UCONFIG_NO_FORMATTING

#include "unicode/calendar.h"
#include "gregoimp.h"
#include "mutex.h"
#include "iculserv.h"
#include "cstring.h"

#ifdef U_DEBUG_RC
# include <stdio.h>
# include "uresimp.h" // for debugging

static void debug_tz_loc(const char *f, int32_t l)
{
  fprintf(stderr, "%s:%d: ", f, l);
}

static void debug_tz_msg(const char *pat, ...)
{
  va_list ap;
  va_start(ap, pat);
  vfprintf(stderr, pat, ap);
  fflush(stderr);
}
static char gStrBuf[256];
#define U_DEBUG_RC_STR(x) u_austrncpy(gStrBuf,x,sizeof(gStrBuf)-1)
// must use double parens, i.e.:  U_DEBUG_RC_MSG(("four is: %d",4));
#define U_DEBUG_RC_MSG(x) {debug_tz_loc(__FILE__,__LINE__);debug_tz_msg x;}
#else
#define U_DEBUG_RC_MSG(x)
#endif

static const int32_t kFrenchEpoch = 2375840; // Sun, Sept 22, 1792
static const double  kAvgYearLen  = 1460969. / 4000.; // ~365.24225
static const int32_t kDaysInMonth = 30;
const char RepublicainCalendar::fgClassID = 0; // Value is irrelevant

int32_t
RepublicainCalendar::approxYearFromJulian( int32_t jd )  {
  U_DEBUG_RC_MSG(("%.8lf %.8lf\n", (double)(jd-kFrenchEpoch), kAvgYearLen));
  return (int32_t)Math::floorDivide( (double)(jd-kFrenchEpoch), kAvgYearLen );
}

const char *RepublicainCalendar::getType() const { 
  //  if(modified) { 
  //    return "republicain-modified";
  //  } else {
    return "republicain";
    //}
}

Calendar* RepublicainCalendar::clone() const {
  return new RepublicainCalendar(*this);
}

RepublicainCalendar::RepublicainCalendar(const Locale& aLocale, UErrorCode& success)
  :   Calendar(TimeZone::createDefault(), aLocale, success),
      modified(TRUE)
{
  setTimeInMillis(getNow(), success); // Call this again now that the vtable is set up properly.
}

RepublicainCalendar::RepublicainCalendar(const RepublicainCalendar& other) : Calendar(other), modified(other.modified) {
}

RepublicainCalendar::~RepublicainCalendar()
{
}

/**
 * Determines whether this object uses original or modified calendar
 *
 * @param beModified   <code>true</code> to use the civil calendar,
 *                  <code>false</code> to use the astronomical calendar.
 * @draft ICU 2.4
 */
void RepublicainCalendar::setModified(UBool beModified, UErrorCode &status)
{
  if (modified != beModified) {
    // The fields of the calendar will become invalid, because the calendar
    // rules are different
    UDate m = getTimeInMillis(status);
    modified = beModified;
    clear();
    setTimeInMillis(m, status);
  }
}
    
/**
 * Returns <code>true</code> if this object is using the modified
 * calendar, or <code>false</code> if using the original, astronomical
 * calendar.
 * @draft ICU 2.4
 */
UBool RepublicainCalendar::isModified() {
  return modified;
}
    
//-------------------------------------------------------------------------
// Minimum / Maximum access functions
//-------------------------------------------------------------------------

static const int32_t LIMITS[UCAL_FIELD_COUNT][4] = {
  // Minimum  Greatest    Least  Maximum
  //           Minimum  Maximum
  {        0,        0,       0,       0 }, // ERA
  {        1,        1, 5000000, 5000000 }, // YEAR
  {        0,        0,      12,      12 }, // MONTH (0..11, 12)
  {        1,        1,      55,      55 }, // WEEK_OF_YEAR //??
  {        0,        0,       1,       6 }, // WEEK_OF_MONTH
  {        1,        1,       5,kDaysInMonth}, // DAY_OF_MONTH
  {        1,        1,     365,     366 }, // DAY_OF_YEAR
  {1,1,10,10}, // DAY_OF_WEEK
  {       -1,       -1,       4,       5 }, // DAY_OF_WEEK_IN_MONTH
  {/*N/A*/-1,/*N/A*/-1,/*N/A*/-1,/*N/A*/-1}, // AM_PM
  {/*N/A*/-1,/*N/A*/-1,/*N/A*/-1,/*N/A*/-1}, // HOUR
  {/*N/A*/-1,/*N/A*/-1,/*N/A*/-1,/*N/A*/-1}, // HOUR_OF_DAY
  {/*N/A*/-1,/*N/A*/-1,/*N/A*/-1,/*N/A*/-1}, // MINUTE
  {/*N/A*/-1,/*N/A*/-1,/*N/A*/-1,/*N/A*/-1}, // SECOND
  {/*N/A*/-1,/*N/A*/-1,/*N/A*/-1,/*N/A*/-1}, // MILLISECOND
  {/*N/A*/-1,/*N/A*/-1,/*N/A*/-1,/*N/A*/-1}, // ZONE_OFFSET
  {/*N/A*/-1,/*N/A*/-1,/*N/A*/-1,/*N/A*/-1}, // DST_OFFSET
  { -5000001, -5000001, 5000001, 5000001 }, // YEAR_WOY
  {/*N/A*/-1,/*N/A*/-1,/*N/A*/-1,/*N/A*/-1}, // DOW_LOCAL
  { -5000000, -5000000, 5000000, 5000000 }, // EXTENDED_YEAR
  {/*N/A*/-1,/*N/A*/-1,/*N/A*/-1,/*N/A*/-1}, // JULIAN_DAY
  {/*N/A*/-1,/*N/A*/-1,/*N/A*/-1,/*N/A*/-1} // MILLISECONDS_IN_DAY
};

/**
 * @draft ICU 2.4
 */
int32_t RepublicainCalendar::handleGetLimit(UCalendarDateFields field, ELimitType limitType) const {
  return LIMITS[field][limitType];
}

int32_t RepublicainCalendar::getLimit(UCalendarDateFields field, ELimitType limitType) const {
    switch (field) {
    case UCAL_DAY_OF_WEEK:
        return handleGetLimit(field, limitType);
    default:
      return Calendar::getLimit(field, limitType);
    }
}


//-------------------------------------------------------------------------
// Assorted calculation utilities
//

/**
 * Determine whether a year is a leap year in the Republicain civil calendar
 */
UBool RepublicainCalendar::modifiedLeapYear(int32_t year)
{
    return(  ((year % 4)==0) && 
             !(  (year%100==0) && !((year % 400) == 0)) &&  // year%400 NOT 100,200,300
             ((year%4000) != 0) );
}
    
/**
 * Return the day # on which the given year starts.  Days are counted
 * from the Hijri epoch, origin 0.
 */

    
/**
 * Return the day # on which the given month starts.  Days are counted
 * from the Hijri epoch, origin 0.
 *
 * @param year  The hijri year
 * @param year  The hijri month, 0-based
 */

    
/**
 * Find the day number on which a particular month of the true/lunar
 * Republicain calendar starts.
 *
 * @param month The month in question, origin 0 from the Hijri epoch
 *
 * @return The day number on which the given month starts.
 */


/**
 * Return the "age" of the moon at the given time; this is the difference
 * in ecliptic latitude between the moon and the sun.  This method simply
 * calls CalendarAstronomer.moonAge, converts to degrees, 
 * and adjusts the resultto be in the range [-180, 180].
 *
 * @param time  The time at which the moon's age is desired,
 *              in millis since 1/1/1970.
 */


//----------------------------------------------------------------------
// Calendar framework
//----------------------------------------------------------------------

/**
 * Return the length (in days) of the given month.
 *
 * @param year  The hijri year
 * @param year  The hijri month, 0-based
 * @draft ICU 2.4
 */
int32_t RepublicainCalendar::handleGetMonthLength(int32_t extendedYear, int32_t month) const {
    if(month == SANSCULOTTIDES) {
        if (modified) {
            return modifiedLeapYear(extendedYear) ? 6 : 5;
        } else {
            // ?? TODO original
            return modifiedLeapYear(extendedYear) ? 6 : 5;
        }
    } else {
        return kDaysInMonth;
    }
}

/**
 * Return the number of days in the given Republicain year
 * @draft ICU 2.4
 */
int32_t RepublicainCalendar::handleGetYearLength(int32_t extendedYear) const {
  if (modified) {
      return modifiedLeapYear(extendedYear) ? (kDaysInMonth*12)+6 : (kDaysInMonth*12)+5;
  } else {
      // ?? TODO original
      return modifiedLeapYear(extendedYear) ? (kDaysInMonth*12)+6 : (kDaysInMonth*12)+5;
  }
}
    
//-------------------------------------------------------------------------
// Functions for converting from field values to milliseconds....
//-------------------------------------------------------------------------

    // Return JD of start of given month/year
    /**
     * @draft ICU 2.4
     */
int32_t RepublicainCalendar::handleComputeMonthStart(int32_t eyear, int32_t month, UBool useMonth) const {
  return  (kFrenchEpoch-1)  // days before calendar
    + (365*(eyear-1))  // ordinary days before calendar
    + Math::floorDivide(eyear-1, 4)
    - Math::floorDivide(eyear-1, 100)
    + Math::floorDivide(eyear-1, 400)
    - Math::floorDivide(eyear-1, 4000)
    + (kDaysInMonth*month)
    + 0; // day 0
}

//-------------------------------------------------------------------------
// Functions for converting from milliseconds to field values
//-------------------------------------------------------------------------

/**
 * @draft ICU 2.4
 */
int32_t RepublicainCalendar::handleGetExtendedYear() {
  int32_t year;
  if (newerField(UCAL_EXTENDED_YEAR, UCAL_YEAR) == UCAL_EXTENDED_YEAR) {
    year = internalGet(UCAL_EXTENDED_YEAR, 1); // Default to year 1
  } else {
    year = internalGet(UCAL_YEAR, 1); // Default to year 1
  }
  return year;
}

/**
 * Override Calendar to compute several fields specific to the Republicain
 * calendar system.  These are:
 *
 * <ul><li>ERA
 * <li>YEAR
 * <li>MONTH
 * <li>DAY_OF_MONTH
 * <li>DAY_OF_YEAR
 * <li>EXTENDED_YEAR</ul>
 * 
 * The DAY_OF_WEEK and DOW_LOCAL fields are already set when this
 * method is called. The getGregorianXxx() methods return Gregorian
 * calendar equivalents for the given Julian day.
     * @draft ICU 2.4
     */
void RepublicainCalendar::handleComputeFields(int32_t julianDay, UErrorCode &status) {
    int32_t year, month, dayOfMonth, dayOfYear;
    UDate startDate;

    U_DEBUG_RC_MSG(("jd%d\n", julianDay));
    //if(modified) 
      { 
        int32_t approxYear = approxYearFromJulian(julianDay);
        // approx could be +/- 1.  Find year which 
        U_DEBUG_RC_MSG(("jd%d approx=%d\n", julianDay, approxYear));
        for(year = approxYear-1; year<=approxYear; year++) {
          int32_t tryy = handleComputeMonthStart(year+1, 0, FALSE);
          U_DEBUG_RC_MSG(("jd%d trying y%d -> %d\n", julianDay, year+1, tryy));
          if(tryy > julianDay) {
            break;
          }
        }
        
    }
    // Now, we have the year.
    int32_t yearStart = handleComputeMonthStart(year, 0, FALSE);
    U_DEBUG_RC_MSG(("jd%d y=%d yearst=%d\n", julianDay, year,yearStart));
    
    // month..
    month = Math::floorDivide(julianDay - yearStart,  kDaysInMonth);
    U_DEBUG_RC_MSG(("jd%d month=%d\n", julianDay, month));
    
    // dom
    dayOfMonth = julianDay - (yearStart + month*kDaysInMonth);
    U_DEBUG_RC_MSG(("jd%d dom=%d\n", julianDay, dayOfMonth));

    // Now figure out the day of the year.
    dayOfYear = julianDay - yearStart;
    U_DEBUG_RC_MSG(("jd%d doy=%d\n", julianDay, dayOfYear));

    // OK, setup..
    internalSet(UCAL_ERA, 0);
    internalSet(UCAL_YEAR, year);
    internalSet(UCAL_EXTENDED_YEAR, year);
    internalSet(UCAL_MONTH, month);
    internalSet(UCAL_DAY_OF_MONTH, dayOfMonth);
    internalSet(UCAL_DAY_OF_YEAR, dayOfYear);       
}    

UBool
RepublicainCalendar::inDaylightTime(UErrorCode& status) const
{
// copied from GregorianCalendar
if (U_FAILURE(status) || !getTimeZone().useDaylightTime()) 
  return FALSE;

    // Force an update of the state of the Calendar.
((RepublicainCalendar*)this)->complete(status); // cast away const

return (UBool)(U_SUCCESS(status) ? (internalGet(UCAL_DST_OFFSET) != 0) : FALSE);
}


// deprecated
void 
RepublicainCalendar::roll(EDateFields field, int32_t amount, UErrorCode& status) {
  Calendar::roll((UCalendarDateFields) field, amount, status); 
}




// default century
const UDate     RepublicainCalendar::fgSystemDefaultCentury        = DBL_MIN;
const int32_t   RepublicainCalendar::fgSystemDefaultCenturyYear    = -1;

UDate           RepublicainCalendar::fgSystemDefaultCenturyStart       = DBL_MIN;
int32_t         RepublicainCalendar::fgSystemDefaultCenturyStartYear   = -1;


UBool RepublicainCalendar::haveDefaultCentury() const
{
  return TRUE;
}

UDate RepublicainCalendar::defaultCenturyStart() const
{
  return internalGetDefaultCenturyStart();
}

int32_t RepublicainCalendar::defaultCenturyStartYear() const
{
  return internalGetDefaultCenturyStartYear();
}

UDate
RepublicainCalendar::internalGetDefaultCenturyStart() const
{
  // lazy-evaluate systemDefaultCenturyStart
  UBool needsUpdate;
  { 
    Mutex m;
    needsUpdate = (fgSystemDefaultCenturyStart == fgSystemDefaultCentury);
  }

  if (needsUpdate) {
    initializeSystemDefaultCentury();
  }

  // use defaultCenturyStart unless it's the flag value;
  // then use systemDefaultCenturyStart
  
  return fgSystemDefaultCenturyStart;
}

int32_t
RepublicainCalendar::internalGetDefaultCenturyStartYear() const
{
  // lazy-evaluate systemDefaultCenturyStartYear
  UBool needsUpdate;
  { 
    Mutex m;
    needsUpdate = (fgSystemDefaultCenturyStart == fgSystemDefaultCentury);
  }

  if (needsUpdate) {
    initializeSystemDefaultCentury();
  }

  // use defaultCenturyStart unless it's the flag value;
  // then use systemDefaultCenturyStartYear
  
  return    fgSystemDefaultCenturyStartYear;
}

void
RepublicainCalendar::initializeSystemDefaultCentury()
{
  // initialize systemDefaultCentury and systemDefaultCenturyYear based
  // on the current time.  They'll be set to 80 years before
  // the current time.
  // No point in locking as it should be idempotent.
  if (fgSystemDefaultCenturyStart == fgSystemDefaultCentury)
  {
    UErrorCode status = U_ZERO_ERROR;
    Calendar *calendar = new RepublicainCalendar(Locale("fr@calendar=republicain"),status);
    if (calendar != NULL && U_SUCCESS(status))
    {
      calendar->setTime(Calendar::getNow(), status);
      calendar->add(UCAL_YEAR, -80, status);
      UDate    newStart =  calendar->getTime(status);
      int32_t  newYear  =  calendar->get(UCAL_YEAR, status);
      {
        Mutex m;
        fgSystemDefaultCenturyStart = newStart;
        fgSystemDefaultCenturyStartYear = newYear;
      }
      delete calendar;
    }
    // We have no recourse upon failure unless we want to propagate the failure
    // out.
  }
}

/* registry */
/**
 * a Calendar Factory which creates the "basic" calendar types, that is, those 
 * shipped with ICU.
 */

class RepublicainFactory : public LocaleKeyFactory {
public:
  /**
   * @param calendarType static const string (caller owns storage - will be aliased) to calendar type
   */
  RepublicainFactory(const char *calendarType)
    : LocaleKeyFactory(LocaleKeyFactory::INVISIBLE), fType(calendarType), fID(calendarType,"")  { }
  
  virtual ~RepublicainFactory() {}
  
protected:
  virtual UBool isSupportedID( const UnicodeString& id, UErrorCode& /* status */) const { return (id == fID); }
  
  virtual void updateVisibleIDs(Hashtable& result, UErrorCode& status) const
  {
    if (U_SUCCESS(status)) {
      const UnicodeString& id = fID;
      result.put(id, (void*)this, status);
    }
  }
  
  virtual UObject* create(const ICUServiceKey& key, const ICUService* /*service*/, UErrorCode& status) const {
    const LocaleKey& lkey = (LocaleKey&)key;
    Locale curLoc;  // current locale
    Locale canLoc;  // Canonical locale
    
    lkey.currentLocale(curLoc);
    lkey.canonicalLocale(canLoc);
    
    UnicodeString str;
    key.currentID(str);
    
#ifdef U_DEBUG_CALSVC
    fprintf(stderr, "RepublicainFactory[%s] - cur %s, can %s\n", fType, (const char*)curLoc.getName(), (const char*)canLoc.getName());
#endif
    
    if(str != fID) {  // Do we handle this type?
#ifdef U_DEBUG_CALSVC
      fprintf(stderr, "RepublicainFactory[%s] - not handling %s.\n", fType, (const char*) curLoc.getName() );
#endif
      return NULL;
    }
    
#ifdef U_DEBUG_CALSVC
    fprintf(stderr, "RepublicainFactory %p: creating %s type for %s\n", 
            this, fType, (const char*)curLoc.getName());
    fflush(stderr);
#endif
    
    if(!fType || !*fType || (!uprv_strcmp(fType, "republicain-modified"))) {
#ifdef U_DEBUG_CALSVC
      fprintf(stderr, "RepublicainFactory - creating R-M\n");
#endif
      return new RepublicainCalendar(canLoc, status);
    } else if(!uprv_strcmp(fType, "republicain")) {  
      RepublicainCalendar *i = new RepublicainCalendar(canLoc, status);
      //i->setModified(FALSE, status);
#ifdef U_DEBUG_CALSVC
      fprintf(stderr, "RepublicainFactory - creating R %p [status %s]\n", i, u_errorName(status));
#endif      
      return i;
    } else { 
      status = U_UNSUPPORTED_ERROR;
      return NULL;
    }
  }
private:
  const char *fType;
  const UnicodeString fID;
};





URegistryKey gReg1;
URegistryKey gReg2;

U_CAPI void U_EXPORT2
ucal_registerRepublicain(UErrorCode *status) {
  if(U_FAILURE(*status)) return;
  gReg1 = Calendar::registerFactory(new RepublicainFactory("republicain-modified"), *status);
  gReg2 = Calendar::registerFactory(new RepublicainFactory("republicain"), *status);
}

/* unregister ? */

U_CAPI void U_EXPORT2
ucal_unregisterRepublicain(UErrorCode *status) {
  UBool ok;
  if(U_FAILURE(*status)) return;
  ok = Calendar::unregister(gReg1, *status);
  ok = Calendar::unregister(gReg2, *status);
  if(!ok) { 
    *status = U_INTERNAL_PROGRAM_ERROR;
  }
}

#endif

/*


For le calendrier republicain:

ADD TO ROOT.TXT:
----------------

    DayAbbreviations_republicain {
        "1","2","3","4","5","6","7","8","9","10"
    }
    DayNames_republicain {
        "1","2","3","4","5","6","7","8","9","10"
    }    
    MonthNames_republicain {
       "Vendemiaire","Brumaire","Frimaire","Nivose","Pluviose","Ventose","Germinal","Floreal","Prairal","Messidor","Thermidor","Fructidor","Complem."
    }
    MonthAbbreviations_republicain {
       "Vendemiaire","Brumaire","Frimaire","Nivose","Pluviose","Ventose","Germinal","Floreal","Prairal","Messidor","Thermidor","Fructidor","Complem."
    }

    Eras_republicain {
       "AL"  // Anee Liberte
     }

*/
