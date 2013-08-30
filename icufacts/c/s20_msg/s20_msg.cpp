// Copyright (c) 2010-2013 IBM Corporation and Others. All Rights Reserved.

#include "../iucsamples.h"

int main()
{
  UErrorCode status = U_ZERO_ERROR;
  Locale defaultLocaleId = Locale::getDefault();
  LocalPointer<LocaleDisplayNames>
    ldn(LocaleDisplayNames::createInstance(defaultLocaleId, ULDN_DIALECT_NAMES));
  UnicodeString defaultLocaleName;
  ldn->localeDisplayName(defaultLocaleId, defaultLocaleName);
  UnicodeString world;
  ldn->regionDisplayName("001",world);

  UnicodeString names[] = { "mylocale", "world", "today", "part" };
  Formattable args[] = { defaultLocaleName, world, Calendar::getNow(), 1.00 };
  UnicodeString result;
  MessageFormat fmt("A hello to {part, number, percent} of the {world}, in {mylocale}, on {today, date}!",
                    defaultLocaleId, status);
  fmt.format(names, args, 4, result, status);
  ASSERT_OK(status);
  std::cout << result << std::endl;
  return 0;
}

// Local Variables:
// compile-command: "make check"
// End:
