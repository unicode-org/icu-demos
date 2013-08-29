// Copyright (c) 2010-2013 IBM Corporation and Others. All Rights Reserved.

#include "../iucsamples.h"

int main()
{
  UErrorCode status = U_ZERO_ERROR;
  Locale loc = Locale::getDefault();
  LocalPointer<LocaleDisplayNames> ldn(LocaleDisplayNames::createInstance(loc, ULDN_DIALECT_NAMES));
  UnicodeString names[] = {
    "locale",
    "localestr",
    "today"
  };
  UnicodeString locName;
  Formattable args[] = {
    ldn->localeDisplayName(loc,locName),
    loc.getBaseName(),
    Calendar::getNow()
  };
  UnicodeString result;
  MessageFormat fmt("{localestr}: For {locale}, today is {today, date}!", loc, status);
  fmt.format(names, args, 3, result, status);
  ASSERT_OK(status);
  u_printf("%S\n", result.getTerminatedBuffer());
  return 0;
}

// Local Variables:
// compile-command: "make check"
// End:
