// © 2018 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html#License

#include <iostream>
#include <unicode/errorcode.h>
#include <unicode/uclean.h>
#include <unicode/unistr.h>
#include <unicode/ustream.h>

int main() {
    icu::ErrorCode status;
    icu::UnicodeString message(u"Welcome to ICU! 😼");
    std::cout << message << std::endl;
    u_init(status);
    if (status.isFailure()) {
        std::cout
            << icu::UnicodeString(u"error 😕 ")
            << status.errorName()
            << std::endl;
    } else {
        std::cout
            << icu::UnicodeString(u"everything is OK 🎉")
            << std::endl;
    }
    return status.get();
}

/** Emacs Local Variables: **/
/** Emacs compile-command: "icurun s10_test_cpp.cpp" **/
/** Emacs End: **/
