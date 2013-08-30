// Copyright (c) 2010-2013 IBM Corporation and Others. All Rights Reserved.

#include "../iucsamples.h"
#include <unicode/ustream.h>

int main() {
  UErrorCode status = U_ZERO_ERROR;
  u_init(&status);
  if(U_FAILURE(status)) { puts(u_errorName(status)); return 1; } // hereafter: ASSERT_OK
  UnicodeString msg("Hello, ");
  msg.append("World");
  msg.append(0x2603);
  std::cout << msg << std::endl;
  return 0;
}

// Local Variables:
// compile-command: "icurun s13_hello.cpp"
// End:
