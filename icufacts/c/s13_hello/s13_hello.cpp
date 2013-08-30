// Copyright (c) 2010-2013 IBM Corporation and Others. All Rights Reserved.

#include "../iucsamples.h"

int main() {
  UErrorCode status = U_ZERO_ERROR;
  ULocaleDisplayNames *names = uldn_open(NULL, // = default
                                         ULDN_DIALECT_NAMES,
                                         &status);
  UChar result[256];
  int32_t len = uldn_regionDisplayName(names, "001", result, 256, &status);
  uldn_close(names);
  ASSERT_OK(status);
  UnicodeString msg("Hello, ");
  msg.append(result, len);
  msg.append(0x2603);
  std::cout << msg << std::endl;
  return 0;
}

// Local Variables:
// compile-command: "icurun s13_hello.cpp"
// End:

