/*
*******************************************************************************
*
*   Copyright (C) 1998-2003, International Business Machines
*   Corporation and others.  All Rights Reserved.
*
*******************************************************************************
*   file name:  unistrob.h
*   encoding:   US-ASCII
*   tab size:   8 (not used)
*   indentation:4
*
*   created on: 2003apr30
*   created by: Markus W. Scherer
*
*   This file contains obsolete definitions from unistr.h.
*/

#ifndef __UNISTROB_H__
#define __UNISTROB_H__

#include "unicode/utypes.h"
#include "unicode/unistr.h"
#include "unicode/obsolete.h"

#ifdef U_USE_DEPRECATED_UCHAR_REFERENCE

// UnicodeString in unistr.h had the following method
#if 0
  /**
   * Return a modifiable reference to a code unit of the string.
   * This is unsafe because copying the UCharReference can leave it with
   * a dangling pointer to this UnicodeString object.
   * It also causes inefficient code because in most cases, the r-value
   * operator[] const is intended to be used instead of creating
   * a UCharReference object.
   *
   * @param pos The index of the code unit to refer to.
   * @return A modifiable UCharReference to that code unit.
   * @obsolete ICU 2.4. Use charAt(), setCharAt(), and operator[] const instead since this API will be removed in that release.
   */
  UCharReference operator[] (int32_t pos);
#endif

//========================================
// class UCharReference
//========================================

/**
 * A proxy class to allow the UnicodeString::operator[] function to
 * work as a readable and a writable operator.
 * @obsolete ICU 2.4. Use charAt(), setCharAt(), and operator[] const instead since this API will be removed in that release.
 */
class U_COMMON_API UCharReference : public UObject {
public:
  UCharReference();
  inline UCharReference(UnicodeString *string,
         int32_t pos);
  inline UCharReference(const UCharReference& that);
  ~UCharReference();

  inline UCharReference& operator= (const UCharReference& that);
  inline UCharReference& operator= (UChar c);

  inline operator UChar();

  /**
   * ICU "poor man's RTTI", returns a UClassID for the actual class.
   *
   * @draft ICU 2.2
   */
  virtual inline UClassID getDynamicClassID() const { return getStaticClassID(); }

  /**
   * ICU "poor man's RTTI", returns a UClassID for this class.
   *
   * @draft ICU 2.2
   */
  static inline UClassID getStaticClassID() { return (UClassID)&fgClassID; }

private:
  UnicodeString *fString;
  int32_t fPos;

  /**
   * The address of this static class variable serves as this class's ID
   * for ICU "poor man's RTTI".
   */
  static const char fgClassID;
};


//========================================
// Inline members
//========================================
inline
UCharReference::UCharReference(UnicodeString *string, 
                   int32_t pos)
  : UObject(), fString(string), fPos(pos)
{}

inline
UCharReference::UCharReference(const UCharReference& that)
: UObject(that)
{ this->operator=(that); }

inline
UCharReference::~UCharReference()
{}

inline UCharReference&
UCharReference::operator= (const UCharReference& that)
{ fString->setCharAt(fPos, that.fString->charAt(that.fPos)); return *this; }

inline UCharReference& 
UCharReference::operator= (UChar c)
{ fString->setCharAt(fPos, c); return *this; }

inline
UCharReference::operator UChar()
{ return fString->charAt(fPos); }

#endif // U_USE_DEPRECATED_UCHAR_REFERENCE

#endif // __UNISTROB_H__
