/**********************************************************************
*   Copyright (C) 2000, International Business Machines
*   Corporation and others.  All Rights Reserved.
***********************************************************************/


#ifndef URLSOURCE_H_
#define URLSOURCE_H_

#include "unicode/utypes.h"
#include "dsource.h"

struct URLSourceImp;

class URLSource: public RawSource
{
 public:
  URLSource(const char *url);
  virtual void rewind();
  virtual int32_t read(const uint8_t* &start, const uint8_t* &end);
  virtual ~URLSource();
  const char *getEncoding();
private:
  URLSource(const char *url, bool_t encodingTry);
  const char *u;
  const char *encoding;
  URLSourceImp *imp;
};


#endif
