/*
*******************************************************************************
*
*   Copyright (C) 1999-2003, International Business Machines
*   Corporation and others.  All Rights Reserved.
*
*******************************************************************************
*   file name:  unistrob.cpp
*   encoding:   US-ASCII
*   tab size:   8 (not used)
*   indentation:4
*
*   created on: 2003apr30
*   created by: Markus W. Scherer
*
*   This file contains obsolete definitions from unistr.cpp.
*/

#include "unicode/utypes.h"
#include "unicode/unistr.h"
#include "unicode/obsolete.h"
#include "unicode/unistrob.h"

U_NAMESPACE_BEGIN

const char UCharReference::fgClassID=0;

// unistr.cpp implemented the following:
#if 0
UCharReference
UnicodeString::operator[] (int32_t pos)
{
  return UCharReference(this, pos);
}
#endif

U_NAMESPACE_END
