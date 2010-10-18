// Copyright (c) 2010 IBM Corporation and Others. All Rights Reserved.

#include <unicode/ustdio.h>
#include <unicode/uloc.h>

#include "../iucsamples.h"
int main(int argc, const char *argv[])
{
  UFILE *out;
  UErrorCode status = U_ZERO_ERROR;
#if 1
  const char *loc = uloc_getDefault();
  if(argc==2) loc = argv[1];
#endif

  out = u_finit(stdout, loc, NULL);

  UChar world[256];
  uloc_getDisplayCountry("und_001", "de", world, 256, &status);
  ASSERT_OK(status);

  u_fprintf(out, "%s: Hello, %S!\n", "de", world);

  u_fclose(out);

  return 0;
}
