// Copyright (c) 2010-2011 IBM Corporation and Others. All Rights Reserved.

#include <unicode/ustdio.h>
#include <unicode/uloc.h>

#include "../iucsamples.h"
int main(int argc, const char *argv[])
{
  UFILE *out;
  UErrorCode status = U_ZERO_ERROR;
  const char *locale = "de"; // Get the user's locale from somewhere
  
  out = u_finit(stdout, locale, NULL);

  UChar world[256];
  uloc_getDisplayCountry("und_001", locale, world, 256, &status);
  if(U_FAILURE(status)) { puts("Fail!"); return 1; }

  u_fprintf(out, "%s: Hello, %S!\n", locale, world);

  u_fclose(out);

  return 0;
}
