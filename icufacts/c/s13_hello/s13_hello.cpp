// Copyright (c) 2010-2013 IBM Corporation and Others. All Rights Reserved.

#include "../iucsamples.h"

int main() {
  UErrorCode status = U_ZERO_ERROR;
  UChar world[256];
  u_init(&status);
  if(U_FAILURE(status)) { puts(u_errorName(status)); return 1; } // hereafter: ASSERT_OK
  uloc_getDisplayCountry("und_001", NULL, world, 256, &status);
  ASSERT_OK(status);

  u_printf("Hello, %S!\n", world);
  return 0;
}

// Local Variables:
// compile-command: "icurun s13_hello.cpp"
// End:
