// Copyright (c) 2010-2013 IBM Corporation and Others. All Rights Reserved.
#include "../iucsamples.h"

int main()
{

  UErrorCode status = U_ZERO_ERROR;
  u_setDataDirectory("out"); // this is where the data is generated, see Makefile
  Locale locale = Locale::getDefault();
// BEGIN SAMPLE
  ResourceBundle resourceBundle("reshello", locale, status);
  UnicodeString thing = resourceBundle.getStringEx("hello", status);
  u_printf("%S\n", thing.getTerminatedBuffer());
  ASSERT_OK(status);
  return 0;
}
