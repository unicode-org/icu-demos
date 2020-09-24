// © 2018 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html#License

#include <unicode/errorcode.h>
#include <unicode/locid.h>
#include <unicode/msgfmt.h>
#include <unicode/numberformatter.h>
#include <unicode/uclean.h>
#include <unicode/ustdio.h>
#include <unicode/ustream.h>
#include <iostream>

#if U_ICU_VERSION_MAJOR_NUM < 62
# error for ICU before 62.x, change toString(status) to toString()
#endif


#define ASSERT_OK(status) \
  if(status.isFailure()) { \
    printf("Error on line %d:\n", __LINE__); \
    puts(status.errorName()); \
    return 1; \
  }

int main() {
  icu::ErrorCode status;
  const int32_t quantity = 987654321;

  // Create NumberFormatter
  icu::number::LocalizedNumberFormatter lnf = icu::number::NumberFormatter::with()
    .notation(icu::number::Notation::compactShort())
    .locale("en-us");

  // Check NumberFormatter (optional: error also set in format method)
  lnf.copyErrorTo(status);
  ASSERT_OK(status);

  // Run NumberFormatter
  std::cout
    << lnf.formatDouble(quantity, status).toString(status)
    << std::endl;

  // NumberFormatter all in one line
  std::cout
    << icu::number::NumberFormatter::with()
      .notation(icu::number::Notation::compactShort())
      .locale("en-us")
      .formatDouble(quantity, status)
      .toString(status)
    << std::endl;

  // Create MessageFormat
  icu::MessageFormat mfmt(u"{0, number, ::compact-short}", status);
  ASSERT_OK(status);

  // Run MessageFormat
  icu::UnicodeString result;
  icu::Formattable args[] = {quantity};
  mfmt.format({args, 1}, result, status);
  ASSERT_OK(status);
  std::cout << result << std::endl;

  u_cleanup();
  return 0;
}
