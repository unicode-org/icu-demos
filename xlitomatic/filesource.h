/**********************************************************************
*   Copyright (C) 2000, International Business Machines
*   Corporation and others.  All Rights Reserved.
***********************************************************************/


#ifndef FILESOURCE_H_
#define FILESOURCE_H_

#include "unicode/utypes.h"
#include "dsource.h"
#include "unicode/ucnv.h"

class FileSource: public RawSource
{
 public:
  FileSource(const char *path);
  virtual void rewind();
  virtual int32_t read(const uint8_t* &start, const uint8_t* &end);
  virtual ~FileSource();
private:
  FILE *f;
};

class ToUSource: public UnicodeSource
{
 public:
  ToUSource(RawSource& source, const char *enc = "utf-8");
  virtual ~ToUSource();
  virtual void rewind();
  virtual int32_t read(const UChar* &start, const UChar* &end);
 private:
  UConverter  *u;
  RawSource   &source;
};


#endif
