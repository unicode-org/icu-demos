// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

#include <unicode/locdspnm.h>
#include <unicode/ustream.h>
#include <unicode/msgfmt.h>
#include <iostream>
#include "../iucsamples.h"
using namespace icu;

/**
 * run with LC_ALL=es, LC_ALL=en etc.
 */

int main() {
    UErrorCode status = U_ZERO_ERROR;
    LocalPointer<LocaleDisplayNames> 
        names(LocaleDisplayNames::createInstance(Locale::getDefault(), ULDN_DIALECT_NAMES));
    if(!names.isValid()) return 1; // hereafter: ASSERT_VALID(names);
    UnicodeString world;
    names->regionDisplayName("001", world);

    // New code, use a message
    const int kArgCount=1;
    Formattable arguments[kArgCount]     = { world };
    UnicodeString      argnames[kArgCount]     = {"world"};
    FieldPosition fpos = 0;
    
    MessageFormat msg_en("Hello, {world}", 
        Locale("en"), status);
    UnicodeString result_en;
    msg_en.format(argnames, arguments, kArgCount, result_en, status);
    ASSERT_OK(status);
    std::cout << "en: " << result_en << std::endl;

    MessageFormat msg_es("¡Hola, {world}!",
        Locale("es"), status);
    UnicodeString result_es;
    msg_es.format(argnames, arguments, kArgCount, result_es, status);
    ASSERT_OK(status);
    std::cout << "es: " << result_es << std::endl;
    return 0;
}
