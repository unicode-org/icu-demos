/**********************************************************************
*   Copyright (C) 2000, International Business Machines
*   Corporation and others.  All Rights Reserved.
***********************************************************************/


#ifndef XLITSOURCE_H_
#define XLITSOURCE_H_

#include "unicode/utypes.h"
#include "dsource.h"
#include "unicode/translit.h"

class TranslitUSource: public UnicodeSource
{
 public:
  TranslitUSource(UnicodeSource& source, const char *xlit = "Null");
  virtual ~TranslitUSource();
  virtual void rewind();
  virtual int32_t read(const UChar* &start, const UChar* &end);
 private:
  Transliterator *trans;
  UnicodeSource   &source;
  UnicodeString   result;
  bool_t inTag;
};


#endif
