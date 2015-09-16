/*
*******************************************************************************
* Copyright (C) 2014-2015, International Business Machines
* Corporation and others.  All Rights Reserved.
*******************************************************************************
* icuapps/webdemo/collation/available.cpp
*
* created on: 2014mar14
* created by: Markus W. Scherer
*/

#include <assert.h>
#include <stdio.h>

#include <map>
#include <memory>
#include <string>

#include "unicode/utypes.h"
#include "unicode/coll.h"
#include "unicode/errorcode.h"
#include "unicode/locdspnm.h"
#include "unicode/locid.h"
#include "unicode/strenum.h"
#include "unicode/ucol.h"
#include "unicode/uloc.h"
#include "unicode/ures.h"

#define LENGTHOF(array) (int32_t)(sizeof(array)/sizeof((array)[0]))

using std::string;

using icu::Collator;
using icu::Locale;
using icu::LocalUResourceBundlePointer;
using icu::UnicodeString;

namespace {

/**
 * Private ErrorCode subclass.
 * The destructor calls handleFailure() which calls exit(errorCode) when isFailure().
 */
class MyErrorCode : public icu::ErrorCode {
public:
    /**
     * @param loc A short string describing where the MyErrorCode is used.
     */
    MyErrorCode(const char *loc) : location(loc) {}
    virtual ~MyErrorCode();
protected:
    virtual void handleFailure() const;
private:
    const char *location;
};

MyErrorCode::~MyErrorCode() {
    // Safe because our handleFailure() does not throw exceptions.
    if(isFailure()) { handleFailure(); }
}

void MyErrorCode::handleFailure() const {
    fprintf(stderr, "error at %s: %s\n", location, errorName());
    exit(errorCode);
}

void appendAttribute(const UnicodeString &attr, const UnicodeString &value, UnicodeString &dest) {
    if(!dest.isEmpty()) {
        dest.append((const UChar *)u", ");
    }
    dest.append(attr);
    if(!value.isEmpty()) {
        dest.append((UChar)'=').append(value);
    }
}

struct {
    const char *name;
    UColAttribute attr;
    UColAttributeValue defaultValue;
} collAttributes[UCOL_ATTRIBUTE_COUNT] = {
    { "strength", UCOL_STRENGTH, UCOL_TERTIARY },
    { "backwards", UCOL_FRENCH_COLLATION, UCOL_OFF },
    { "caseLevel", UCOL_CASE_LEVEL, UCOL_OFF },
    { "caseFirst", UCOL_CASE_FIRST, UCOL_OFF },
    { "alternate", UCOL_ALTERNATE_HANDLING, UCOL_NON_IGNORABLE },
    { "hiraganaQuaternary", UCOL_HIRAGANA_QUATERNARY_MODE, UCOL_OFF },
    { "normalization", UCOL_NORMALIZATION_MODE, UCOL_OFF },
    { "numeric", UCOL_NUMERIC_COLLATION, UCOL_OFF }
};

const char *const collValueNames[UCOL_ATTRIBUTE_VALUE_COUNT] = {
    "primary",
    "secondary",
    "tertiary",
    "quaternary",
    "?",
    "?",
    "?",
    "?",
    "?",
    "?",
    "?",
    "?",
    "?",
    "?",
    "?",
    "identical",
    "off",
    "on",
    "?",
    "?",
    "shifted",
    "non-ignorable",
    "?",
    "?",
    "lower",
    "upper"
};

}  // namespace

extern "C" int
main(int argc, char* argv[]) {
    (void)argc;
    (void)argv;
    MyErrorCode errorCode("webdemo/collation/available");
    std::map<string, Collator *> locToColl;
    std::map<string, string> locToDefaultType;
    locToDefaultType[string()] = "standard";
    const Locale &root = Locale::getRoot();
    locToColl[string(" ") + root.getName()] = Collator::createInstance(root, errorCode);
    errorCode.assertSuccess();

    std::unique_ptr<icu::StringEnumeration> locales(Collator::getAvailableLocales());
    const char *localeID = "root";
    bool isRoot = true;
    do {
        Locale locale(localeID);
        std::unique_ptr<icu::StringEnumeration> types(
                Collator::getKeywordValuesForLocale("collation", locale, FALSE, errorCode));
        errorCode.assertSuccess();
        const char *type = NULL;  // Ask ICU for the default collation type.
        for(;;) {
            Locale localeWithType(locale);
            if(type != NULL) {
                localeWithType.setKeywordValue("collation", type, errorCode);
            }
            errorCode.assertSuccess();
            std::unique_ptr<Collator> coll(Collator::createInstance(localeWithType, errorCode));
            errorCode.assertSuccess();
            Locale actual = coll->getLocale(ULOC_ACTUAL_LOCALE, errorCode);
            string actualName(actual.getName());
            bool skip = false;
            if(isRoot) {
                // Make root locales sort before others.
                actualName.insert(0, " ");
            } else if(actualName.empty() || actualName.compare(0, 4, "root") == 0) {
                skip = true;
            }
            if(!skip && locToColl.find(actualName) == locToColl.end()) {
                locToColl[actualName] = coll.release();
            }
            const char *nextType = types->next(NULL, errorCode);
            if(nextType == NULL) { break; }
            if(type == NULL) {
                // There is always at least the default type which is the first list item.
                locToDefaultType[localeID] = nextType;
            }
            type = nextType;
        }
        isRoot = false;
    } while((localeID = locales->next(NULL, errorCode)) != NULL);

    std::unique_ptr<icu::LocaleDisplayNames> ldn(
        icu::LocaleDisplayNames::createInstance(Locale::getEnglish(), ULDN_DIALECT_NAMES));
    assert(ldn.get() != NULL);

    for(const auto &entry : locToColl) {
        UnicodeString text;

        // Write the BCP 47 language tag.
        const char *locID = entry.first.c_str();
        if(*locID == ' ') {
            ++locID;  // root locale with prepended space
        }
        char langTag[100];
        uloc_toLanguageTag(locID, langTag, LENGTHOF(langTag), /* strict= */ FALSE, errorCode);
        errorCode.assertSuccess();
        text.append(UnicodeString(langTag, -1, US_INV));

        UnicodeString attributes;

        // We want to know the collation type if it was suppressed.
        Locale locale(locID);
        char type[16];
        int32_t typeLength = locale.getKeywordValue("collation", type, LENGTHOF(type), errorCode);
        errorCode.assertSuccess();
        if(typeLength == 0) {
            string &defaultType = locToDefaultType[locID];
            UnicodeString uType(defaultType.data(), defaultType.length(), US_INV);
            appendAttribute((const UChar *)u"type", uType, attributes);
            locale.setKeywordValue("collation", defaultType.c_str(), errorCode);
            errorCode.assertSuccess();
        }

        Collator *coll = entry.second;
        for(const auto &collAttr : collAttributes) {
            UColAttributeValue value = coll->getAttribute(collAttr.attr, errorCode);
            errorCode.assertSuccess();
            if(value != collAttr.defaultValue) {
                appendAttribute(UnicodeString(collAttr.name, -1, US_INV),
                                UnicodeString(collValueNames[value], -1, US_INV),
                                attributes);
            }
        }

        // TODO: maxVariable
        // TODO: reorder codes

        if(!attributes.isEmpty()) {
            text.append((const UChar *)u" (").append(attributes).append((UChar)')');
        }

        // English display name.
        UnicodeString displayName;
        text.append((const UChar *)u": \u00A0 ");
        text.append(ldn->localeDisplayName(locale, displayName));

        string textUTF8;
        text.toUTF8String(textUTF8);
        printf("<option value=\"%s\">%s</option>\n", locID, textUTF8.c_str());
        delete coll;
    }
    return 0;
}
