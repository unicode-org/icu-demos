/********************************************************************
 * COPYRIGHT: 
 * Copyright (c) 1997-2001, International Business Machines Corporation and
 * others. All Rights Reserved.
 ********************************************************************/

#include "unicode/ustring.h"
#include "unicode/uchar.h"
#include "unicode/uniset.h"
#include "unicode/putil.h"
#include "cstring.h"
#include "uparse.h"
#include "ucdtest.h"

#ifdef ICU_UNICODE_CLASS_USE_DEPRECATES
#include "unicode/unicode.h"
#endif

#define LENGTHOF(array) (sizeof(array)/sizeof(array[0]))

UnicodeTest::UnicodeTest()
{
}

UnicodeTest::~UnicodeTest()
{
}

void UnicodeTest::runIndexedTest( int32_t index, UBool exec, const char* &name, char* /*par*/ )
{
    if (exec) logln("TestSuite UnicodeTest: ");
    switch (index) {
//%        case 0: name = "TestAdditionalProperties"; if(exec) TestAdditionalProperties(); break;  
#ifdef ICU_UNICODE_CLASS_USE_DEPRECATES
        case 0: name = "TestUpperLower"; if (exec) TestUpperLower(); break;
        case 1: name = "TestLetterNumber"; if (exec) TestLetterNumber(); break;
        case 2: name = "TestMisc"; if (exec) TestMisc(); break;
        case 3: name = "TestUnicodeData"; if (exec) TestUnicodeData(); break;
        case 4: name = "TestCodeUnit"; if(exec) TestCodeUnit(); break;
        case 5: name = "TestCodePoint"; if(exec) TestCodePoint(); break;
        case 6: name = "TestCharLength"; if(exec) TestCharLength(); break;
        case 7: name = "TestIdentifier"; if(exec) TestIdentifier(); break;  
        case 8: name = "TestScript"; if(exec) TestScript(); break;  
#endif
        default: name = ""; break; //needed to end loop
    }
}

//====================================================
// private data used by the tests
//====================================================

#ifdef ICU_UNICODE_CLASS_USE_DEPRECATES
const UChar  LAST_CHAR_CODE_IN_FILE = 0xFFFD;
const char tagStrings[] = "MnMcMeNdNlNoZsZlZpCcCfCsCoCnLuLlLtLmLoPcPdPsPePoSmScSkSoPiPf";
const int32_t tagValues[] =
    {
    /* Mn */ Unicode::NON_SPACING_MARK,
    /* Mc */ Unicode::COMBINING_SPACING_MARK,
    /* Me */ Unicode::ENCLOSING_MARK,
    /* Nd */ Unicode::DECIMAL_DIGIT_NUMBER,
    /* Nl */ Unicode::LETTER_NUMBER,
    /* No */ Unicode::OTHER_NUMBER,
    /* Zs */ Unicode::SPACE_SEPARATOR,
    /* Zl */ Unicode::LINE_SEPARATOR,
    /* Zp */ Unicode::PARAGRAPH_SEPARATOR,
    /* Cc */ Unicode::CONTROL,
    /* Cf */ Unicode::FORMAT,
    /* Cs */ Unicode::SURROGATE,
    /* Co */ Unicode::PRIVATE_USE,
    /* Cn */ Unicode::UNASSIGNED,
    /* Lu */ Unicode::UPPERCASE_LETTER,
    /* Ll */ Unicode::LOWERCASE_LETTER,
    /* Lt */ Unicode::TITLECASE_LETTER,
    /* Lm */ Unicode::MODIFIER_LETTER,
    /* Lo */ Unicode::OTHER_LETTER,
    /* Pc */ Unicode::CONNECTOR_PUNCTUATION,
    /* Pd */ Unicode::DASH_PUNCTUATION,
    /* Ps */ Unicode::START_PUNCTUATION,
    /* Pe */ Unicode::END_PUNCTUATION,
    /* Po */ Unicode::OTHER_PUNCTUATION,
    /* Sm */ Unicode::MATH_SYMBOL,
    /* Sc */ Unicode::CURRENCY_SYMBOL,
    /* Sk */ Unicode::MODIFIER_SYMBOL,
    /* So */ Unicode::OTHER_SYMBOL,
    /* Pi */ Unicode::INITIAL_PUNCTUATION,
    /* Pf */ Unicode::FINAL_PUNCTUATION
    };
const char dirStrings[][5] = {
    "L",
    "R",
    "EN",
    "ES",
    "ET",
    "AN",
    "CS",
    "B",
    "S",
    "WS",
    "ON",
    "LRE",
    "LRO",
    "AL",
    "RLE",
    "RLO",
    "PDF",
    "NSM",
    "BN"
};

//====================================================
// test toUpperCase() and toLowerCase()
//====================================================
void UnicodeTest::TestUpperLower()
{
    U_STRING_DECL(upperTest, "abcdefg123hij.?:klmno", 21);
    U_STRING_DECL(lowerTest, "ABCDEFG123HIJ.?:KLMNO", 21);
    uint16_t i;

    U_STRING_INIT(upperTest, "abcdefg123hij.?:klmno", 21);
    U_STRING_INIT(lowerTest, "ABCDEFG123HIJ.?:KLMNO", 21);

//Checks LetterLike Symbols which were previously a source of confusion
//[Bertrand A. D. 02/04/98]
    for (i=0x2100;i<0x2138;i++)
    {
        if(i!=0x2126 && i!=0x212a && i!=0x212b)
        {
            if (i != Unicode::toLowerCase(i)) // itself
                errln("Failed case conversion with itself: " + UCharToUnicodeString(i));
            if (i != Unicode::toUpperCase(i))
                errln("Failed case conversion with itself: " + UCharToUnicodeString(i));
        }
    }

    for (i = 0; i < 21; i++) {
//      logln((UnicodeString)"testing " + (int32_t)i + "...");
        if (Unicode::isLetter(upperTest[i]) && !Unicode::isLowerCase(upperTest[i]))
            errln("Failed isLowerCase test at " + UCharToUnicodeString(upperTest[i]));
        else if (Unicode::isLetter(lowerTest[i]) && !Unicode::isUpperCase(lowerTest[i]))
            errln("Failed isUpperCase test at " + UCharToUnicodeString(lowerTest[i]));
        else if (upperTest[i] != Unicode::toLowerCase(lowerTest[i]))
            errln("Failed case conversion : " + UCharToUnicodeString(upperTest[i]) + 
            " to " + UCharToUnicodeString(lowerTest[i]));
        else if (lowerTest[i] != Unicode::toUpperCase(upperTest[i]))
            errln("Failed case conversion : " + UCharToUnicodeString(upperTest[i]) +
            " to " + UCharToUnicodeString(lowerTest[i]));
        else if (upperTest[i] != Unicode::toLowerCase(upperTest[i])) // itself
            errln("Failed case conversion with itself: " + UCharToUnicodeString(upperTest[i]));
        else if (lowerTest[i] != Unicode::toUpperCase(lowerTest[i]))
            errln("Failed case conversion with itself: " + UCharToUnicodeString(lowerTest[i]));
    }
}

/* test isLetter() and isDigit() */
void UnicodeTest::TestLetterNumber()
{
    UChar i;

    for (i = 0x0041; i < 0x005B; i++) {
//      logln((UnicodeString)"testing " + (int32_t)i + "...");
        if (!Unicode::isLetter(i))
            errln("Failed isLetter test at " + UCharToUnicodeString(i));
    }
    for (i = 0x0660; i < 0x066A; i++) {
//      logln((UnicodeString)"testing " + (int32_t)i + "...");
        if (Unicode::isLetter(i))
            errln("Failed isLetter test with numbers at " + i);
    }
    for (i = 0x0660; i < 0x066A; i++) {
//      logln((UnicodeString)"testing " + (int32_t)i + "...");
        if (!Unicode::isDigit(i))
            errln("Failed isNumber test at " + i);
    }
}

/* Tests for isDefined(), isBaseForm(), isSpaceChar() and getCellWidth() */
void UnicodeTest::TestMisc()
{
    const UChar sampleSpaces[] = {0x0020, 0x00a0, 0x2000, 0x2001, 0x2005};
    const UChar sampleNonSpaces[] = {0x61, 0x62, 0x63, 0x64, 0x74};
    const UChar sampleWhiteSpaces[] = {0x2008, 0x2009, 0x200a, 0x001c, 0x000c};
    const UChar sampleNonWhiteSpaces[] = {0x61, 0x62, 0x3c, 0x28, 0x3f};
    const UChar sampleUndefined[] = {0xfff1, 0xfff7, 0xfa6b};
    const UChar sampleDefined[] = {0x523E, 0x4f88, 0xfffd};
    const UChar sampleBase[] = {0x0061, 0x0031, 0x03d2};
    const UChar sampleNonBase[] = {0x002B, 0x0020, 0x203B};
    const UChar sampleChars[] = {0x000a, 0x0045, 0x4e00, 0xDC00};
    const UChar sampleDigits[]= {0x0030, 0x0662, 0x0F23, 0x0ED5};
    const UChar sampleNonDigits[] = {0x0010, 0x0041, 0x0122, 0x68FE};
    const int32_t sampleDigitValues[] = {0, 2, 3, 5};
    const uint16_t sampleCellWidth[] = {Unicode::ZERO_WIDTH, 
                                        Unicode::HALF_WIDTH, 
                                        Unicode::FULL_WIDTH, 
                                        Unicode::NEUTRAL};
    int32_t i;
    for (i = 0; i < 5; i++) {
//      logln((UnicodeString)"testing " + (int32_t)i + "...");
        if (!(Unicode::isSpaceChar(sampleSpaces[i])) ||
                (Unicode::isSpaceChar(sampleNonSpaces[i])))
            errln((UnicodeString)"Space char test error : " + (int32_t)sampleSpaces[i] +
            " or " + (int32_t)sampleNonSpaces[i]);
    }
    for (i = 0; i < 5; i++) {
//      log_ln("Testing for isWhitespace and nonWhitespaces\n");
        if (!(Unicode::isWhitespace(sampleWhiteSpaces[i])) ||
                (Unicode::isWhitespace(sampleNonWhiteSpaces[i])))
        {
            errln((UnicodeString)"White Space char test error : " + (int32_t)sampleWhiteSpaces[i] +
                "or" + (int32_t)sampleNonWhiteSpaces[i]);
        }
    }
    for (i = 0; i < 3; i++) {
//      logln((UnicodeString)"testing " + (int32_t)i + "...");
        if ((Unicode::isDefined(sampleUndefined[i])) ||
                !(Unicode::isDefined(sampleDefined[i])))
            errln((UnicodeString)"Undefined char test error : " +
            (int32_t)sampleUndefined[i] + " or " + (int32_t)sampleDefined[i]);
    }
    for (i = 0; i < 3; i++) {
//      logln((UnicodeString)"testing " + (int32_t)i + "...");
        if ((Unicode::isBaseForm(sampleNonBase[i])) ||
                !(Unicode::isBaseForm(sampleBase[i])))
            errln((UnicodeString)"Non-baseform char test error : " +
            (int32_t)sampleNonBase[i] + " or " + (int32_t)sampleBase[i]);
    }
    for (i = 0; i < 4; i++) {
//      logln((UnicodeString)"testing " + (int32_t)i + "...");
        if (Unicode::getCellWidth(sampleChars[i]) != sampleCellWidth[i])
            errln((UnicodeString)"Cell width char test error : " +
            (int32_t)sampleChars[i]);
    }
    for (i = 0; i < 4; i++) {
        if ((Unicode::isDigit(sampleDigits[i]) && 
            (Unicode::digitValue(sampleDigits[i])!= sampleDigitValues[i])) ||
            (Unicode::isDigit(sampleNonDigits[i]))) {
            errln((UnicodeString)"Digit char test error : " +
            (int32_t)sampleDigits[i] + " or " + (int32_t)sampleNonDigits[i]);
        }
    }
}

/* Tests for isControl() and isPrintable() */
void UnicodeTest::TestControlPrint()
{
    const UChar sampleControl[] = {0x001b, 0x0097, 0x0082};
    const UChar sampleNonControl[] = {0x61, 0x0031, 0x00e2};
    const UChar samplePrintable[] = {0x0042, 0x005f, 0x2014};
    const UChar sampleNonPrintable[] = {0x200c, 0x009f, 0x001b};
    int32_t i;
    for (i = 0; i < 3; i++) {
//      logln((UnicodeString)"testing " + (int32_t)i + "...");
        if (!(Unicode::isControl(sampleControl[i])) ||
                (Unicode::isControl(sampleNonControl[i])))
            errln((UnicodeString)"Control char test error : " + (int32_t)sampleControl[i] +
            " or " + (int32_t)sampleNonControl[i]);
    }
    for (i = 0; i < 3; i++) {
//      logln((UnicodeString)"testing " + (int32_t)i + "...");
        if ((Unicode::isPrintable(samplePrintable[i])) ||
                !(Unicode::isPrintable(samplePrintable[i])))
            errln((UnicodeString)"Printable char test error : " +
            (int32_t)samplePrintable[i] + " or " + (int32_t)sampleNonPrintable[i]);
    }
}

/* Tests for isJavaIdentifierStart(), isJavaIdentifierPart(),
 * isUnicodeIdentifierStart() and isUnicodeIdentifierPart() */
void UnicodeTest::TestIdentifier()
{
    const UChar sampleJavaIDStart[] = {0x0071, 0x00e4, 0x005f};
    const UChar sampleNonJavaIDStart[] = {0x0020, 0x2030, 0x0082};
    const UChar sampleUnicodeIDStart[] = {0x0250, 0x00e2, 0x0061};
    const UChar sampleNonUnicodeIDStart[] = {0x2000, 0x000a, 0x2019};
    const UChar sampleJavaIDPart[] = {0x005f, 0x0032, 0x0045};
    const UChar sampleNonJavaIDPart[] = {0x2030, 0x2020, 0x0020};
    const UChar sampleUnicodeIDPart[] = {0x005f, 0x0032, 0x0045};
    const UChar sampleNonUnicodeIDPart[] = {0x2030, 0x00a3, 0x0020};
    const UChar sampleIDIgnore[] = {0x0006, 0x0010, 0x206b};
    const UChar sampleNonIDIgnore[] = {0x0075, 0x00a3, 0x0061};

    int32_t i;
    for (i = 0; i < 3; i++) {
//      logln((UnicodeString)"testing " + (int32_t)i + "...");
        if (!(Unicode::isJavaIdentifierStart(sampleJavaIDStart[i])) ||
                (Unicode::isJavaIdentifierStart(sampleNonJavaIDStart[i])))
            errln((UnicodeString)"Java ID Start char test error : " + (int32_t)sampleJavaIDStart[i] +
            " or " + (int32_t)sampleNonJavaIDStart[i]);
    }
    for (i = 0; i < 3; i++) {
//      logln((UnicodeString)"testing " + (int32_t)i + "...");
        if (!(Unicode::isJavaIdentifierPart(sampleJavaIDPart[i])) ||
                (Unicode::isJavaIdentifierPart(sampleNonJavaIDPart[i])))
            errln((UnicodeString)"Java ID Part char test error : " + (int32_t)sampleJavaIDPart[i] +
            " or " + (int32_t)sampleNonJavaIDPart[i]);
    }
    for (i = 0; i < 3; i++) {
//      logln((UnicodeString)"testing " + (int32_t)i + "...");
        if (!(Unicode::isUnicodeIdentifierStart(sampleUnicodeIDStart[i])) ||
                (Unicode::isUnicodeIdentifierStart(sampleNonUnicodeIDStart[i])))
            errln((UnicodeString)"Unicode ID Start char test error : " + (int32_t)sampleUnicodeIDStart[i] +
            " or " + (int32_t)sampleNonUnicodeIDStart[i]);
    }
    for (i = 0; i < 3; i++) {
//      logln((UnicodeString)"testing " + (int32_t)i + "...");
        if (!(Unicode::isUnicodeIdentifierPart(sampleUnicodeIDPart[i])) ||
                (Unicode::isUnicodeIdentifierPart(sampleNonUnicodeIDPart[i])))
            errln((UnicodeString)"Unicode ID Part char test error : " + (int32_t)sampleUnicodeIDPart[i] +
            " or " + (int32_t)sampleNonUnicodeIDPart[i]);
    }
    for (i = 0; i < 3; i++) {
//      logln((UnicodeString)"testing " + (int32_t)i + "...");
        if (!(Unicode::isIdentifierIgnorable(sampleIDIgnore[i])) ||
                (Unicode::isIdentifierIgnorable(sampleNonIDIgnore[i])))
            errln((UnicodeString)"ID ignorable char test error : " + (int32_t)sampleIDIgnore[i] +
            " or " + (int32_t)sampleNonIDIgnore[i]);
    }
}

/* for each line of UnicodeData.txt, check some of the properties */
U_CAPI void U_CALLCONV
unicodeDataLineFn(void *context,
                  char *fields[][2], int32_t fieldCount,
                  UErrorCode *pErrorCode)
{
    char *end;
    uint32_t value;
    UChar32 c;
    int8_t type;
    UnicodeTest *me=(UnicodeTest *)context;

    if(U_FAILURE(*pErrorCode)) {
        me->errln("error: unicodeDataLineFn called with pErrorCode=%d\n", pErrorCode);
        return;
    }

    if(fieldCount != 15) {
        me->errln("error in UnicodeData.txt: The data format changed. fieldCount=%lu and it should be 15\n", fieldCount);
        *pErrorCode = U_PARSE_ERROR;
        return;
    }

    /* get the character code, field 0 */
    c=(UChar32)uprv_strtoul(fields[0][0], &end, 16);
    if(end<=fields[0][0] || end!=fields[0][1]) {
        me->errln("error: syntax error in field 0 at %s\n" + UnicodeString(fields[0][0], ""));
        *pErrorCode = U_PARSE_ERROR;
        return;
    }
    if((uint32_t)c>=0x110000) {
        me->errln("error in UnicodeData.txt: code point %lu out of range\n", c);
        *pErrorCode = U_PARSE_ERROR;
        return;
    }

    /* get general category, field 2 */
    *fields[2][1]=0;
    type = (int8_t)tagValues[me->MakeProp(fields[2][0])];
    if(Unicode::getType(c)!=type) {
        me->errln("error: Unicode::getType(U+%04lx)==%u instead of %u\n", c, Unicode::getType(c), type);
        *pErrorCode = U_PARSE_ERROR;
        return;
    }

    /* get canonical combining class, field 3 */
    value=(uint32_t)uprv_strtoul(fields[3][0], &end, 10);
    if(end<=fields[3][0] || end!=fields[3][1]) {
        me->errln("error: syntax error in field 3 at code 0x%lx\n", c);
        *pErrorCode = U_PARSE_ERROR;
        return;
    }
    if(value>255) {
        me->errln("error in UnicodeData.txt: combining class %lu out of range\n", value);
        *pErrorCode = U_PARSE_ERROR;
        return;
    }
    if(value!=Unicode::getCombiningClass(c)) {
        me->errln("error: Unicode::getCombiningClass(U+%04lx)==%hu instead of %lu\n", c, Unicode::getCombiningClass(c), value);
        *pErrorCode = U_PARSE_ERROR;
        return;
    }

    /* get BiDi category, field 4 */
    *fields[4][1]=0;
    if(Unicode::characterDirection(c)!=me->MakeDir(fields[4][0])) {
        me->errln("error: Unicode::characterDirection(U+%04lx)==%u instead of %u (%s)\n", c, Unicode::characterDirection(c), me->MakeDir(fields[4][0]), fields[4][0]);
        *pErrorCode = U_PARSE_ERROR;
        return;
    }

    /* get uppercase mapping, field 12 */
    if(fields[12][0]!=fields[12][1]) {
        value=(uint32_t)uprv_strtoul(fields[12][0], &end, 16);
        if(end!=fields[12][1]) {
            me->errln("error: syntax error in field 12 at code 0x%lx\n", c);
            *pErrorCode = U_PARSE_ERROR;
            return;
        }
        if((UChar32)value!=Unicode::toUpperCase(c)) {
            me->errln("error: Unicode::toUpperCase(U+%04lx)==U+%04lx instead of U+%04lx\n", c, Unicode::toUpperCase(c), value);
            *pErrorCode = U_PARSE_ERROR;
            return;
        }
    } else {
        /* no case mapping: the API must map the code point to itself */
        if(c!=Unicode::toUpperCase(c)) {
            me->errln("error: U+%04lx does not have an uppercase mapping but Unicode::toUpperCase()==U+%04lx\n", c, Unicode::toUpperCase(c));
            *pErrorCode = U_PARSE_ERROR;
            return;
        }
    }

    /* get lowercase mapping, field 13 */
    if(fields[13][0]!=fields[13][1]) {
        value=(uint32_t)uprv_strtoul(fields[13][0], &end, 16);
        if(end!=fields[13][1]) {
            me->errln("error: syntax error in field 13 at code 0x%lx\n", c);
            *pErrorCode = U_PARSE_ERROR;
            return;
        }
        if((UChar32)value!=Unicode::toLowerCase(c)) {
            me->errln("error: Unicode::toLowerCase(U+%04lx)==U+%04lx instead of U+%04lx\n", c, Unicode::toLowerCase(c), value);
            *pErrorCode = U_PARSE_ERROR;
            return;
        }
    } else {
        /* no case mapping: the API must map the code point to itself */
        if(c!=Unicode::toLowerCase(c)) {
            me->errln("error: U+%04lx does not have a lowercase mapping but Unicode::toLowerCase()==U+%04lx\n", c, Unicode::toLowerCase(c));
            *pErrorCode = U_PARSE_ERROR;
            return;
        }
    }

    /* get titlecase mapping, field 14 */
    if(fields[14][0]!=fields[14][1]) {
        value=(uint32_t)uprv_strtoul(fields[14][0], &end, 16);
        if(end!=fields[14][1]) {
            me->errln("error: syntax error in field 14 at code 0x%lx\n", c);
            *pErrorCode = U_PARSE_ERROR;
            return;
        }
        if((UChar32)value!=Unicode::toTitleCase(c)) {
            me->errln("error: Unicode::toTitleCase(U+%04lx)==U+%04lx instead of U+%04lx\n", c, Unicode::toTitleCase(c), value);
            *pErrorCode = U_PARSE_ERROR;
            return;
        }
    } else {
        /* no case mapping: the API must map the code point to itself */
        if(c!=Unicode::toTitleCase(c)) {
            me->errln("error: U+%04lx does not have a titlecase mapping but Unicode::toTitleCase()==U+%04lx\n", c, Unicode::toTitleCase(c));
            *pErrorCode = U_PARSE_ERROR;
            return;
        }
    }
}

/* tests for several properties */
void UnicodeTest::TestUnicodeData()
{
    char newPath[256];
    char backupPath[256];
    char *fields[15][2];
    UErrorCode errorCode = U_ZERO_ERROR;

    /* Look inside ICU_DATA first */
    strcpy(newPath, u_getDataDirectory());
    strcat(newPath, "unidata" U_FILE_SEP_STRING "UnicodeData.txt");


    // As a fallback, try to guess where the source data was located
    //   at the time ICU was built, and look there.
#   if defined (U_TOPSRCDIR)
        strcpy(backupPath, U_TOPSRCDIR  U_FILE_SEP_STRING "data");
#   else
        strcpy(backupPath, loadTestData(errorCode));
        strcat(backupPath, U_FILE_SEP_STRING ".." U_FILE_SEP_STRING ".." U_FILE_SEP_STRING ".." U_FILE_SEP_STRING ".." U_FILE_SEP_STRING "data");
#   endif
    strcat(backupPath, U_FILE_SEP_STRING);
    strcat(backupPath, "unidata" U_FILE_SEP_STRING "UnicodeData.txt");

    u_parseDelimitedFile(newPath, ';', fields, 15, unicodeDataLineFn, this, &errorCode);

    if(errorCode==U_FILE_ACCESS_ERROR) {
        errorCode=U_ZERO_ERROR;
        u_parseDelimitedFile(backupPath, ';', fields, 15, unicodeDataLineFn, this, &errorCode);

    }
    if(U_FAILURE(errorCode)) {
        errln("error parsing UnicodeData.txt: %s\n" + UnicodeString(u_errorName(errorCode), ""));
        return;
    }

    // test Unicode::getCharName()
    // a more thorough test of u_charName() is in cintltst/cucdtst.c
    char buffer[100];
    int32_t length=Unicode::getCharName(0x284, buffer, (int32_t)sizeof(buffer));

    // use invariant-character conversion to Unicode
    UnicodeString name(buffer, length, "");
    if(name!=UNICODE_STRING("LATIN SMALL LETTER DOTLESS J WITH STROKE AND HOOK", 49)) {
        errln("Unicode character name lookup failed\n");
    }

    // test Unicode::isMirrored() and charMirror()
    // see also cintltst/cucdtest.c
    if(!(Unicode::isMirrored(0x28) && Unicode::isMirrored(0xbb) && Unicode::isMirrored(0x2045) && Unicode::isMirrored(0x232a) &&
         !Unicode::isMirrored(0x27) && !Unicode::isMirrored(0x61) && !Unicode::isMirrored(0x284) && !Unicode::isMirrored(0x3400)
        )
    ) {
        errln("Unicode::isMirrored() does not work correctly\n");
    }

    if(!(Unicode::charMirror(0x3c)==0x3e && Unicode::charMirror(0x5d)==0x5b && Unicode::charMirror(0x208d)==0x208e && Unicode::charMirror(0x3017)==0x3016 &&
         Unicode::charMirror(0x2e)==0x2e && Unicode::charMirror(0x6f3)==0x6f3 && Unicode::charMirror(0x301c)==0x301c && Unicode::charMirror(0xa4ab)==0xa4ab
        )
    ) {
        errln("Unicode::charMirror() does not work correctly\n");
    }
}

int32_t UnicodeTest::MakeProp(char* str) 
{
    int32_t result = 0;
    const char* matchPosition;
    
    matchPosition = strstr(tagStrings, str);
    if (matchPosition == 0) errln((UnicodeString)"unrecognized type letter " + str);
    else result = ((matchPosition - tagStrings) / 2);
    return result;
}

int32_t UnicodeTest::MakeDir(char* str) 
{
    int32_t pos = 0;
    for (pos = 0; pos < 19; pos++) {
        if (strcmp(str, dirStrings[pos]) == 0) {
            return pos;
        }
    }
    return -1;
}
/*Tests added by Madhu*/

/* Tests for isSingle(), isLead(), isTrial(), isSurrogate */
void UnicodeTest::TestCodeUnit(){
    const UChar codeunit[]={0x0000,0xe065,0x20ac,0xd7ff,0xd800,0xd841,0xd905,0xdbff,0xdc00,0xdc02,0xddee,0xdfff,0};

    int32_t i;

    for(i=0; i<(int32_t)(sizeof(codeunit)/sizeof(codeunit[0])); i++){
        UChar c=codeunit[i];
        UnicodeString msg;
        msg.append((UChar32)c);
        logln((UnicodeString)"Testing code unit value of " + prettify(msg));
        if(i<4){
            if(!(Unicode::isSingle(c)) || (Unicode::isLead(c)) || (Unicode::isTrail(c)) ||(Unicode::isSurrogate(c))){
                errln((UnicodeString)"ERROR:" + prettify(msg) + " is a single");
            }

        }
        if(i >= 4 && i< 8){
            if(!(Unicode::isLead(c)) || Unicode::isSingle(c) || Unicode::isTrail(c) || !(Unicode::isSurrogate(c))){
                errln((UnicodeString)"ERROR:" + prettify(msg) + " is a first surrogate");
            }
        }
        if(i >= 8 && i< 12){
            if(!(Unicode::isTrail(c)) || Unicode::isSingle(c) || Unicode::isLead(c) || !(Unicode::isSurrogate(c))){
                errln((UnicodeString)"ERROR:" + prettify(msg) + " is a second surrogate");
            }
        }
    }

}
/* Tests for isSurrogate(), isUnicodeChar(), isError(), isValid() */
void UnicodeTest::TestCodePoint(){
    const UChar32 codePoint[]={
        //surrogate, notvalid(codepoint), not a UnicodeChar, not Error
        0xd800,
        0xdbff,
        0xdc00,
        0xdfff,
        0xdc04,
        0xd821,
        //not a surrogate, valid, isUnicodeChar , not Error
        0x20ac,
        0xd7ff,
        0xe000,
        0xe123,
        0x0061,
        0xe065, 
        0x20402,
        0x24506,
        0x23456,
        0x20402,
        0x10402,
        0x23456,
        //not a surrogate, not valid, isUnicodeChar, isError
        0x0015,
        0x009f,
        //not a surrogate, not valid, not isUnicodeChar, isError
        0xffff,
        0xfffe,
    };
    int32_t i;
    for(i=0; i<(int32_t)(sizeof(codePoint)/sizeof(codePoint[0])); i++){
        UChar32 c=codePoint[i];
        UnicodeString msg;
        msg.append(c);
        logln((UnicodeString)"Testing code Point value of " + prettify(msg));
        if(i<6){
            if(!Unicode::isSurrogate(c)){
                errln((UnicodeString)"ERROR: isSurrogate() failed for" + prettify(msg));
            }
            if(Unicode::isValid(c)){
                errln((UnicodeString)"ERROR: isValid() failed for "+ prettify(msg));
            }
            if(Unicode::isUnicodeChar(c)){
                errln((UnicodeString)"ERROR: isUnicodeChar() failed for "+ prettify(msg));
            }
            if(Unicode::isError(c)){
                errln((UnicodeString)"ERROR: isError() failed for "+ prettify(msg));
            }
        }else if(i >=6 && i<18){
            if(Unicode::isSurrogate(c)){
                errln((UnicodeString)"ERROR: isSurrogate() failed for" + prettify(msg));
            }
            if(!Unicode::isValid(c)){
                errln((UnicodeString)"ERROR: isValid() failed for "+ prettify(msg));
            }
            if(!Unicode::isUnicodeChar(c)){
                errln((UnicodeString)"ERROR: isUnicodeChar() failed for "+ prettify(msg));
            }
            if(Unicode::isError(c)){
                errln((UnicodeString)"ERROR: isError() failed for "+ prettify(msg));
            }
        }else if(i >=18 && i<20){
            if(Unicode::isSurrogate(c)){
                errln((UnicodeString)"ERROR: isSurrogate() failed for" + prettify(msg));
            }
            if(Unicode::isValid(c)){
                errln((UnicodeString)"ERROR: isValid() failed for "+ prettify(msg));
            }
            if(!Unicode::isUnicodeChar(c)){
                errln((UnicodeString)"ERROR: isUnicodeChar() failed for "+ prettify(msg));
            }
            if(!Unicode::isError(c)){
                errln((UnicodeString)"ERROR: isError() failed for "+ prettify(msg));
            }
        }
        else if(i >=18 && i<(int32_t)(sizeof(codePoint)/sizeof(codePoint[0]))){
            if(Unicode::isSurrogate(c)){
                errln((UnicodeString)"ERROR: isSurrogate() failed for" + prettify(msg));
            }
            if(Unicode::isValid(c)){
                errln((UnicodeString)"ERROR: isValid() failed for "+ prettify(msg));
            }
            if(Unicode::isUnicodeChar(c)){
                errln((UnicodeString)"ERROR: isUnicodeChar() failed for "+ prettify(msg));
            }
            if(!Unicode::isError(c)){
                errln((UnicodeString)"ERROR: isError() failed for "+ prettify(msg));
            }
        }
    }

}

void UnicodeTest::TestCharLength()
{
    const int32_t codepoint[]={
        1, 0x0061,
        1, 0xe065,
        1, 0x20ac,
        2, 0x20402,
        2, 0x23456,
        2, 0x24506,
        2, 0x20402,
        2, 0x10402,
        1, 0xd7ff,
        1, 0xe000
    };

    int32_t i;
    UBool multiple;
    for(i=0; i<(int32_t)(sizeof(codepoint)/sizeof(codepoint[0])); i=(int16_t)(i+2)){
        UChar32 c=codepoint[i+1];
        UnicodeString msg;
        msg.append(c);
        if(Unicode::charLength(c) != codepoint[i]){
            errln((UnicodeString)"The no: of code units for" + prettify(msg)+
                ":- Expected: " + (int32_t)codepoint[i] + " Got: " + Unicode::charLength(c));
        }else{
            logln((UnicodeString)"The no: of code units for" + prettify(msg) + " is " + Unicode::charLength(c)); 
        }
        multiple = codepoint[i] != 1;
        if(Unicode::needMultipleUChar(c) != multiple){
            errln("ERROR: Unicode::needMultipleUChar() failed for" + prettify(msg));
        }
    }
}

/*
Various script value testing.
This makes sure that the Unicode::EUnicodeScript
and UCharScript enum values are the same.
*/
void UnicodeTest::TestScript()
{
    if ((int32_t)Unicode::kScriptCount != (int32_t)UBLOCK_COUNT) {
        errln("ERROR: Unicode::EUnicodeScript is not the same size as UCharScript");
    }

    if ((int32_t)Unicode::kBasicLatin != (int32_t)U_BASIC_LATIN) {
        errln("ERROR: Different Basic Latin values in EUnicodeScript and UCharScript");
    }

    if ((int32_t)Unicode::kHighSurrogate != (int32_t)U_HIGH_SURROGATES) {
        errln("ERROR: Different High Surrogate values in EUnicodeScript and UCharScript");
    }

    if ((int32_t)Unicode::kLowSurrogate != (int32_t)U_LOW_SURROGATES) {
        errln("ERROR: Different Low Surrogate values in EUnicodeScript and UCharScript");
    }

    if ((int32_t)Unicode::kCJKRadicalsSupplement != (int32_t)U_CJK_RADICALS_SUPPLEMENT) {
        errln("ERROR: Different CJK Radicals Supplement values in EUnicodeScript and UCharScript");
    }

    if ((int32_t)Unicode::kGreek != (int32_t)U_GREEK) {
        errln("ERROR: Different Greek values in EUnicodeScript and UCharScript");
    }

    if ((int32_t)Unicode::kThai != (int32_t)U_THAI) {
        errln("ERROR: Diffe rent Thai values in EUnicodeScript and UCharScript");
    }
}
#endif

// test DerivedCoreProperties.txt -------------------------------------------

//%// copied from genprops.c
//%static int32_t
//%getTokenIndex(const char *const tokens[], int32_t countTokens, const char *s) {
//%    const char *t, *z;
//%    int32_t i, j;
//%
//%    s=u_skipWhitespace(s);
//%    for(i=0; i<countTokens; ++i) {
//%        t=tokens[i];
//%        if(t!=NULL) {
//%            for(j=0;; ++j) {
//%                if(t[j]!=0) {
//%                    if(s[j]!=t[j]) {
//%                        break;
//%                    }
//%                } else {
//%                    z=u_skipWhitespace(s+j);
//%                    if(*z==';' || *z==0) {
//%                        return i;
//%                    } else {
//%                        break;
//%                    }
//%                }
//%            }
//%        }
//%    }
//%    return -1;
//%}
//%
//%static const char *const
//%derivedCorePropsNames[]={
//%    "Math",
//%    "Alphabetic",
//%    "Lowercase",
//%    "Uppercase",
//%    "ID_Start",
//%    "ID_Continue",
//%    "XID_Start",
//%    "XID_Continue",
//%    "Default_Ignorable_Code_Point",
//%    "Grapheme_Extend",
//%    "Grapheme_Base"
//%};
//%
//%static const UProperty
//%derivedCorePropsIndex[]={
//%    UCHAR_MATH,
//%    UCHAR_ALPHABETIC,
//%    UCHAR_LOWERCASE,
//%    UCHAR_UPPERCASE,
//%    UCHAR_ID_START,
//%    UCHAR_ID_CONTINUE,
//%    UCHAR_XID_START,
//%    UCHAR_XID_CONTINUE,
//%    UCHAR_DEFAULT_IGNORABLE_CODE_POINT,
//%    UCHAR_GRAPHEME_EXTEND,
//%    UCHAR_GRAPHEME_BASE
//%};
//%
//%U_CAPI void U_CALLCONV
//%derivedCorePropsLineFn(void *context,
//%                        char *fields[][2], int32_t /* fieldCount */,
//%                        UErrorCode *pErrorCode)
//%{
//%    UnicodeTest *me=(UnicodeTest *)context;
//%    uint32_t start, end;
//%    int32_t i;
//%
//%    u_parseCodePointRange(fields[0][0], &start, &end, pErrorCode);
//%    if(U_FAILURE(*pErrorCode)) {
//%        me->errln("UnicodeTest: syntax error in DerivedCoreProperties.txt field 0 at %s\n", fields[0][0]);
//%        return;
//%    }
//%
//%    /* parse derived binary property name, ignore unknown names */
//%    i=getTokenIndex(derivedCorePropsNames, LENGTHOF(derivedCorePropsNames), fields[1][0]);
//%    if(i<0) {
//%        me->errln("UnicodeTest warning: unknown property name '%s' in \n", fields[1][0]);
//%        return;
//%    }
//%
//%    me->derivedCoreProps[i].add(start, end);
//%}
//%
//%void UnicodeTest::TestAdditionalProperties() {
//%    // test DerivedCoreProperties.txt
//%    if(LENGTHOF(derivedCoreProps)<LENGTHOF(derivedCorePropsNames)) {
//%        errln("error: UnicodeTest::derivedCoreProps[] too short, need at least %d UnicodeSets\n",
//%              LENGTHOF(derivedCorePropsNames));
//%        return;
//%    }
//%    if(LENGTHOF(derivedCorePropsIndex)!=LENGTHOF(derivedCorePropsNames)) {
//%        errln("error in ucdtest.cpp: LENGTHOF(derivedCorePropsIndex)!=LENGTHOF(derivedCorePropsNames)\n");
//%        return;
//%    }
//%
//%    char newPath[256];
//%    char backupPath[256];
//%    char *fields[2][2];
//%    int32_t length;
//%    UErrorCode errorCode=U_ZERO_ERROR;
//%
//%    /* Look inside ICU_DATA first */
//%    strcpy(newPath, u_getDataDirectory());
//%
//%    // remove trailing "out/"
//%    length=uprv_strlen(newPath);
//%    if(length>=4 && uprv_strcmp(newPath+length-4, "out" U_FILE_SEP_STRING)==0) {
//%        newPath[length-4]=0;
//%    }
//%
//%    strcat(newPath, "unidata" U_FILE_SEP_STRING "DerivedCoreProperties.txt");
//%
//%    // As a fallback, try to guess where the source data was located
//%    // at the time ICU was built, and look there.
//%#   ifdef U_TOPSRCDIR
//%        strcpy(backupPath, U_TOPSRCDIR  U_FILE_SEP_STRING "data");
//%#   else
//%        strcpy(backupPath, loadTestData(errorCode));
//%        strcat(backupPath, U_FILE_SEP_STRING ".." U_FILE_SEP_STRING ".." U_FILE_SEP_STRING ".." U_FILE_SEP_STRING ".." U_FILE_SEP_STRING "data");
//%#   endif
//%    strcat(backupPath, U_FILE_SEP_STRING);
//%    strcat(backupPath, "unidata" U_FILE_SEP_STRING "DerivedCoreProperties.txt");
//%
//%    u_parseDelimitedFile(newPath, ';', fields, 2, derivedCorePropsLineFn, this, &errorCode);
//%
//%    if(errorCode==U_FILE_ACCESS_ERROR) {
//%        errorCode=U_ZERO_ERROR;
//%        u_parseDelimitedFile(backupPath, ';', fields, 2, derivedCorePropsLineFn, this, &errorCode);
//%    }
//%    if(U_FAILURE(errorCode)) {
//%        errln("error parsing DerivedCoreProperties.txt: %s\n", u_errorName(errorCode));
//%        return;
//%    }
//%
//%    // now we have all derived core properties in the UnicodeSets
//%    // run them all through the API
//%    int32_t rangeCount, range;
//%    uint32_t i;
//%    UChar32 start, end;
//%
//%    // test all true properties
//%    for(i=0; i<LENGTHOF(derivedCorePropsNames); ++i) {
//%        rangeCount=derivedCoreProps[i].getRangeCount();
//%        for(range=0; range<rangeCount; ++range) {
//%            start=derivedCoreProps[i].getRangeStart(range);
//%            end=derivedCoreProps[i].getRangeEnd(range);
//%            for(; start<=end; ++start) {
//%                if(!u_hasBinaryProperty(start, derivedCorePropsIndex[i])) {
//%                    errln("UnicodeTest error: u_hasBinaryProperty(U+%04lx, %s)==false is wrong\n", start, derivedCorePropsNames[i]);
//%                }
//%            }
//%        }
//%    }
//%
//%    // invert all properties
//%    for(i=0; i<LENGTHOF(derivedCorePropsNames); ++i) {
//%        derivedCoreProps[i].complement();
//%    }
//%
//%    // test all false properties
//%    for(i=0; i<LENGTHOF(derivedCorePropsNames); ++i) {
//%        rangeCount=derivedCoreProps[i].getRangeCount();
//%        for(range=0; range<rangeCount; ++range) {
//%            start=derivedCoreProps[i].getRangeStart(range);
//%            end=derivedCoreProps[i].getRangeEnd(range);
//%            for(; start<=end; ++start) {
//%                if(u_hasBinaryProperty(start, derivedCorePropsIndex[i])) {
//%                    errln("UnicodeTest error: u_hasBinaryProperty(U+%04lx, %s)==true is wrong\n", start, derivedCorePropsNames[i]);
//%                }
//%            }
//%        }
//%    }
//%}
