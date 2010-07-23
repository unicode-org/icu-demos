/* Copyright (c) 2010 IBM Corporation and Others. All Rights Reserved. */

#ifndef _UCD_RESULT
#define _UCD_RESULT

#include "ucd.h"
#include "unicode/uobject.h"
#include "unicode/utext.h"

/* === Classes === */
class UCDResult : public UObject {
 public:
  /**
   * Get a UText object. Will heap allocate.
   */
  UText* getUText() { return getUText(NULL); }
  /**
   * Get a UText object, possibly reusing the fillin
   */
  virtual UText* getUText(UText *fillin) = 0;
 private:
  UClassID getDynamicClassID() const { return NULL; } // No poor man's RTTI
};

#endif
