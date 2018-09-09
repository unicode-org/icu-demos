// © 2018 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html#License

#include <unicode/errorcode.h>
#include <unicode/locid.h>
#include <unicode/ustdio.h>
#include <unicode/ustream.h>
#include <iostream>

#define ASSERT_OK(status) \
	if(status.isFailure()) { \
		puts(status.errorName()); \
		return 1; \
	}

int main() {
  icu::ErrorCode status;
  icu::Locale locale("und_001");
  icu::UnicodeString world;
  locale.getDisplayCountry(world);
  ASSERT_OK(status);

  std::cout << "Hello, " << world << "!" << std::endl;
  return 0;
}
