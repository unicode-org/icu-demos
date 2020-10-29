/**********************************************************************
*   Copyright (C) 2000, International Business Machines
*   Corporation and others.  All Rights Reserved.
***********************************************************************/

/* this file describes unicode and raw data sources abstractly */

#include "unicode/utypes.h"

#ifndef DSOURCE_H_
#define DSOURCE_H_

#define RBUFSIZ 1024
#define UBUFSIZ 3000

class Source
{
 public:
  virtual ~Source();
  virtual void rewind() = 0;
  inline bool_t eof() { return fEOF; }
 protected:
  bool_t fEOF;
  inline Source();
};

class RawSource : public Source
{
 public:
  virtual int32_t read(const uint8_t* &start, const uint8_t* &end) = 0;
 protected:
  inline RawSource();
  uint8_t buffer[RBUFSIZ];
};

class UnicodeSource : public Source
{
 public:
  virtual int32_t read(const UChar* &start, const UChar* &end) = 0;
 protected:
  inline UnicodeSource();
  UChar buffer[UBUFSIZ];
};

/********* inline imps ************/
inline Source::Source()
: fEOF(false)
{
}

inline RawSource::RawSource()
{
}

inline UnicodeSource::UnicodeSource()
{
}


#endif
