// Copyright (c) 2010-2012 IBM Corporation and Others. All Rights Reserved.

#include <unicode/ustdio.h>
#include <unicode/uloc.h>
#include <unicode/uldnames.h>
int main() {
  UErrorCode status = U_ZERO_ERROR;

  char theirLocale[ULOC_FULLNAME_CAPACITY];
  uloc_forLanguageTag("es-419-u-nu-roman", // IETF BCP 47
     theirLocale, ULOC_FULLNAME_CAPACITY, NULL, &status);
  // theirLocale is now: "en_@numbers=roman"
  UChar str[256];
  const char *ourLocale = uloc_getDefault(); // Change this to override display
  ULocaleDisplayNames *ldn = uldn_open(ourLocale, ULDN_DIALECT_NAMES, &status);
  uldn_localeDisplayName(ldn, theirLocale, str, 256, &status);
  uldn_close(ldn);

  if(U_FAILURE(status)) { puts(u_errorName(status)); return 1; }

  u_printf("ourLocale=%s, theirLocale=%s\n\n", ourLocale, theirLocale);

  u_printf("\n==> %S!\n \n", str);

  return 0;
}
