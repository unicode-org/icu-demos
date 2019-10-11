// © 2018 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html#License

#include <unicode/errorcode.h>
#include <unicode/locid.h>
#include <unicode/msgfmt.h>
#include <unicode/uclean.h>
#include <unicode/ustdio.h>
#include <unicode/ustream.h>
#include <iostream>

#define ASSERT_OK(status) \
  if(status.isFailure()) { \
    printf("Error on line %d:\n", __LINE__); \
    puts(status.errorName()); \
    return 1; \
  }

icu::UnicodeString run(const char16_t* pattern, double quantity) {
  icu::ErrorCode status;

  icu::MessageFormat mfmt(pattern, status);
  ASSERT_OK(status);
  icu::UnicodeString result;
  icu::Formattable args[] = {quantity};
  mfmt.format({args, 1}, result, status);
  ASSERT_OK(status);
  return result;
}

int main() {
  const char16_t* pattern1 = u"The room measures "
    "{0, plural, one{1 meter} other{# meters}}"
    " wide.";
  const char16_t* pattern2 = u"The room measures "
    "{0, number, ::measure-unit/length-meter unit-width-full-name}"
    " wide.";

  std::cout << run(pattern1, 1) << std::endl;
  std::cout << run(pattern1, 5) << std::endl;
  std::cout << run(pattern2, 1) << std::endl;
  std::cout << run(pattern2, 5) << std::endl;

  u_cleanup();
  return 0;
}
