// Copyright (c) 2010-2012 IBM Corporation and Others. All Rights Reserved.

#include <unicode/ustdio.h>
#include <unicode/uloc.h>

int main() {
  UErrorCode status = U_ZERO_ERROR;
  UChar world[256];
  uloc_getDisplayCountry("und_001", NULL, world, 256, &status);
  if(U_FAILURE(status)) { puts(u_errorName(status)); return 1; }

  u_printf("Hello, %S!\n", world);
  return 0;
}
