/***
 * 
 * Copyright (c) 2003 IBM Corporation and others, all rights reserved.
 *
 **
 *
 * 2003 - nov - 15   srl    New.
 *
 ***/

#ifndef REPUBCAL_H
#define REPUBCAL_H

#include "unicode/utypes.h"

#if !UCONFIG_NO_FORMATTING

#include "unicode/calendar.h"
#include <float.h>

/**
 * 
 * Le Calendrier Republicain
 *
 * 12 months of 30 days, divided into 10-day 'decades' (like weeks)
 * followed by 5 monthless days (6 in leap years).  
 * We will implement the proposed modified form, with Romme's leap year
 * formats. It was never actually implemented this way but is more 
 * precise.
 *
 * Source: "Calendrical Calculations" by Dershowitz and Reingold, 
 *  1st edition, p161
 * 
 * Epoch - Sunday, Sept 22, 1792
 * julian day = 2,375,840
 *     millis = -5594202000000.0  (PST - not correct)
 *
 * avg year length = 1460969 / 4000 
 * 
 *****
 * 
 * - sansculottides
 *  5 or 6 monthless days at the end of the year.
 * Each has its own name.
 * Unfortunately, without format registration, we have the choice of:
 * 
 * (a) putting each of these in their own 1-day 'month' 
 *     (so adding 6 1-day months)
 *     Pro:  the month can have the day name
 *     Con:  you wouldn't see the 'number' of the day  (1-6)
 *
 * (b) putting all of these in a 5-6 day long 'month' called SANSCULOTTIDES
 *     the 13th month (like haab). 
 *     Pro: simpler to calculate, gets day # in there
 *     Con: Formatting isn't as nice,  will just say 
 *             'Sansculottides 4'
 *
 *  For now I'm going with option B, but may come back to A.
 */

class RepublicainCalendar : public Calendar {
    public:
    enum {  SANSCULOTTIDES = 12  // Monthless days.  5-6 days depending on leap
    } EMonths;
  //-------------------------------------------------------------------------
  // Constructors...
  //-------------------------------------------------------------------------

  /**
   * Constructs a RepublicainCalendar based on the current time in the default time zone
   * with the given locale.
   *
   * @param aLocale  The given locale.
   * @param success  Indicates the status of RepublicainCalendar object construction.
   *                 Returns U_ZERO_ERROR if constructed successfully.
   * @internal
   */
  RepublicainCalendar(const Locale& aLocale, UErrorCode &success);

  /**
   * Copy Constructor
   * @internal
   */
  RepublicainCalendar(const RepublicainCalendar& other);

  /**
   * Destructor.
   * @internal
   */
  virtual ~RepublicainCalendar();

  /**
   * Determines whether this object uses the fixed-cycle Republicain Modified calendar
   * or an approximation of the religious, astronomical calendar.
   *
   * @param beModified   <code>true</code> to use the Modified calendar,
   *                  <code>false</code> to use the astronomical calendar.
   * @internal
   */
  void setModified(UBool beModified, UErrorCode &status);
    
  /**
   * Returns <code>true</code> if this object is using the fixed-cycle Modified
   * calendar, or <code>false</code> if using the religious, astronomical
   * calendar.
   * @internal
   */
  UBool isModified();


  // TODO: copy c'tor, etc

  // clone
  virtual Calendar* clone() const;

 private:
  /**
   * Determine whether a year is a leap year in the Republicain Modified calendar
   */
  static UBool modifiedLeapYear(int32_t year);
    

  //-------------------------------------------------------------------------
  // Internal data....
  //
    
  /**
   * <code>TRUE</code> if this object uses the revised modified calendar
   * and <code>FALSE</code> if it uses the original calendar. 
   */
  UBool modified;

  //----------------------------------------------------------------------
  // Calendar framework
  //----------------------------------------------------------------------
 protected:
  /**
   * @internal
   */
  virtual int32_t handleGetLimit(UCalendarDateFields field, ELimitType limitType) const;
  
  /**
   * Return the length (in days) of the given month.
   *
   * @param year  The hijri year
   * @param year  The hijri month, 0-based
   * @internal
   */
  virtual int32_t handleGetMonthLength(int32_t extendedYear, int32_t month) const;
  
  /**
   * Return the number of days in the given Republicain year
   * @internal
   */
  virtual int32_t handleGetYearLength(int32_t extendedYear) const;
    
  //-------------------------------------------------------------------------
  // Functions for converting from field values to milliseconds....
  //-------------------------------------------------------------------------

  // Return JD of start of given month/year
  /**
   * @internal
   */
  virtual int32_t handleComputeMonthStart(int32_t eyear, int32_t month, UBool useMonth) const;

  //-------------------------------------------------------------------------
  // Functions for converting from milliseconds to field values
  //-------------------------------------------------------------------------

  /**
   * @internal
   */
  virtual int32_t handleGetExtendedYear();

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
   * @internal
   */
  virtual void handleComputeFields(int32_t julianDay, UErrorCode &status);

  // UObject stuff
 public: 
  /**
   * @return   The class ID for this object. All objects of a given class have the
   *           same class ID. Objects of other classes have different class IDs.
   * @internal
   */
  virtual UClassID getDynamicClassID(void) const;

  /**
   * Return the class ID for this class. This is useful only for comparing to a return
   * value from getDynamicClassID(). For example:
   *
   *      Base* polymorphic_pointer = createPolymorphicObject();
   *      if (polymorphic_pointer->getDynamicClassID() ==
   *          Derived::getStaticClassID()) ...
   *
   * @return   The class ID for all objects of this class.
   * @internal
   */
  static inline UClassID getStaticClassID(void);

  /**
   * return the calendar type, "buddhist".
   *
   * @return calendar type
   * @internal
   */
  virtual const char * getType() const;

 private:
  RepublicainCalendar(); // default constructor not implemented
  static const char fgClassID; 

  // Default century.
 protected:

  /**
   * (Overrides Calendar) Return true if the current date for this Calendar is in
   * Daylight Savings Time. Recognizes DST_OFFSET, if it is set.
   *
   * @param status Fill-in parameter which receives the status of this operation.
   * @return   True if the current date for this Calendar is in Daylight Savings Time,
   *           false, otherwise.
   * @internal
   */
  virtual UBool inDaylightTime(UErrorCode& status) const;


  /**
   * Returns TRUE because the Republicain Calendar does have a default century
   * @internal
   */
  virtual UBool haveDefaultCentury() const;

  /**
   * Returns the date of the start of the default century
   * @return start of century - in milliseconds since epoch, 1970
   * @internal
   */
  virtual UDate defaultCenturyStart() const;

  /**
   * Returns the year in which the default century begins
   * @internal
   */
  virtual int32_t defaultCenturyStartYear() const;

  virtual int32_t getLimit(UCalendarDateFields field, ELimitType limitType) const;

 private: // default century stuff.
  /**
   * The system maintains a static default century start date.  This is initialized
   * the first time it is used.  Before then, it is set to SYSTEM_DEFAULT_CENTURY to
   * indicate an uninitialized state.  Once the system default century date and year
   * are set, they do not change.
   */
  static UDate         fgSystemDefaultCenturyStart;

  /**
   * See documentation for systemDefaultCenturyStart.
   */
  static int32_t          fgSystemDefaultCenturyStartYear;

  /**
   * Default value that indicates the defaultCenturyStartYear is unitialized
   */
  static const int32_t    fgSystemDefaultCenturyYear;

  /**
   * start of default century, as a date
   */
  static const UDate        fgSystemDefaultCentury;

  /**
   * Returns the beginning date of the 100-year window that dates 
   * with 2-digit years are considered to fall within.
   */
  UDate         internalGetDefaultCenturyStart(void) const;

  /**
   * Returns the first year of the 100-year window that dates with 
   * 2-digit years are considered to fall within.
   */
  int32_t          internalGetDefaultCenturyStartYear(void) const;

  /**
   * Initializes the 100-year window that dates with 2-digit years
   * are considered to fall within so that its start date is 80 years
   * before the current time.
   */
  static void  initializeSystemDefaultCentury(void);

 public:
  // deprecates
  /**
   * (Overrides Calendar) Rolls up or down by the given amount in the specified field.
   * For more information, see the documentation for Calendar::roll().
   *
   * @param field   The time field.
   * @param amount  Indicates amount to roll.
   * @param status  Output param set to success/failure code on exit. If any value
   *                previously set in the time field is invalid, this will be set to
   *                an error status.
   * @deprecated ICU 2.6. Use roll(UCalendarDateFields field, int32_t amount, UErrorCode& status) instead.
   */
  virtual void roll(EDateFields field, int32_t amount, UErrorCode& status);
 private:
  /**
   * Calculate approximate (+/- 1) French year from
   * Julian day
   * @param jd Julian day
   * @return approximate French extended year 
   * @internal
   */
  static int32_t approxYearFromJulian( int32_t jd );
};

inline UClassID
RepublicainCalendar::getStaticClassID(void)
{ return (UClassID)&fgClassID; }

inline UClassID
RepublicainCalendar::getDynamicClassID(void) const
{ return RepublicainCalendar::getStaticClassID(); }

#include "unicode/lx_utils.h"

#endif

#endif


