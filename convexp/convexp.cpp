/*
*******************************************************************************
*
*   Copyright (C) 2003, International Business Machines
*   Corporation and others.  All Rights Reserved.
*
*******************************************************************************
*   file name:  convexp.cpp
*   encoding:   US-ASCII
*   tab size:   4 (not used)
*   indentation:4
*
*   created on: 2003Mar21
*   created by: George Rhoten
*
*   This is a fairly crude converter explorer. It allows you to browse the
*   converter alias table without requiring the people to know the alias table syntax.
*
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "unicode/ucnv.h"
#include "unicode/ustring.h"
#include "uhash.h"
#include "cstring.h"

#define CGI_NAME "Converter Explorer"

static const char htmlHeader[]=
    "Content-Type: text/html; charset=utf-8\n"
    "\n"
    "<html lang=\"en-US\">\n"
    "<head>\n"
    "<title>ICU " CGI_NAME "</title>\n"
    "<meta name=\"robots\" content=\"nofollow\">\n"
    "<meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\">\n"
    "<style type=\"text/css\">\n"
    "th {white-space: nowrap; background-color: #EEEEEE; text-align: left;}\n"
    "th.standard {white-space: nowrap; background-color: #EEEEEE; text-align: center;}\n"
    "td {white-space: nowrap;}\n"
    "td.value {font-family: monospace;}\n"
    "</style>\n"
    "</head>\n"
    "<body bgcolor=\"#FFFFFF\">\n"
    "<table width=\"100%\"border=\"0\" cellspacing=\"0\" cellpadding=\"2\" summary=\"This is the navigation bar\">\n"
    "<tr><td>"
    "<a href=\"http://oss.software.ibm.com/icu/\">ICU</a> &gt;\n"
    "<a href=\"http://oss.software.ibm.com/icu/demo/\">Demo</a> &gt;";

static const char navigationMainHeader[]=
    "<strong>" CGI_NAME "</strong>\n";

static const char navigationSubHeader[]=
    "<a href=\"?\">" CGI_NAME "</a> &gt;\n"
    "<strong>%s</strong>\n";

static const char navigationEndHeader[]=
    "</td>"
    "<td align=\"right\">"
    "<a href=\"http://oss.software.ibm.com/icu/demo/convexp_help.html\">Help</a>"
    "</td></tr>\n"
    "</table>\n"
    "<hr>\n"
    "<h1>ICU " CGI_NAME "</h1>\n";

static const char htmlFooter[]=
    "</body>\n"
    "</html>\n";

/*static const char helpText[]=
    "<h2><a name=\"help\">About this demo</a></h2>\n"
    "<p>There is no help for this demo at this time.</p>"
    "<hr>";
*/

static const char *inputError="<p>Error parsing the input string: %s</p>\n";

static const char startByteEscape[]="\\x%02X";

static const char trailingByteEscape[]="\\x%02X";
static const char trailingByteEscapeSet[]="-\\x%02X";

static const char startUCharEscape[]="\\u%04X";

static const char trailingUCharEscape[]="\\u%04X";

static const char startForm[]=
    "<form method=\"GET\" action=\"%s\">\n"
    "<p>Select a standard to view:<br>\n"
//    "<input type=\"text\" name=\"t\" maxlength=\"500\" size=\"164\" value=\"%s\">\n"
    "</p>\n";

static const char endForm[]=
            "<input type=\"submit\" value=\"View Results\" size=\"100\">\n"
            "</form>\n";

static const char startTable[]=
    "<table border=\"1\" cellspacing=\"0\" cellpadding=\"2\">\n";

static const char endTable[]="</table>";

static const char versions[]=
    "<p>Powered by "
    "<a href=\"http://oss.software.ibm.com/icu/\">ICU</a> %s</p>\n";

static const char NBSP[]="\xC2\xA0";    // UTF-8 form of &nbsp;

static const char ALL[]="ALL";

int32_t gMaxStandards;
char gCurrConverter[UCNV_MAX_CONVERTER_NAME_LENGTH] = "";
UHashtable *gStandardsSelected = NULL;
const char *gScriptName = NULL;

#define OPTION_SEPARATOR  '&' 
#define VALUE_SEPARATOR   '='



static const char *getStandardOptionsURL(UErrorCode *status) {
    static char optionsURL[1024] = "";
    int32_t pos = -1;
    const UHashElement *e;
    const char *standard;
    int32_t len = 0;

    while ((e = uhash_nextElement(gStandardsSelected, &pos)) != NULL) {
        standard = (const char*) e->value.pointer;
        len += sprintf(optionsURL+len, "&s=%s", standard);
    }

    return optionsURL;
}

/*
 Only insert standards that we recognize into the hashtable
 All other standards could be bad/malicious data
*/
static void addStandard(const char *newStandard, int32_t nameSize, UErrorCode *status) {
    int32_t i;
    const char *standard;

    if (nameSize <= 0) {
        uhash_put(gStandardsSelected,
            (void*)ucnv_getStandard(gMaxStandards-1, status),
            (void*)ucnv_getStandard(gMaxStandards-1, status),
            status);
        if (U_FAILURE(*status)) {
            printf("ERROR: uhash_put() -> %s\n", u_errorName(*status));
        }
        return;
    }
    if (uprv_strnicmp(ALL, newStandard, nameSize) == 0) {
        uhash_put(gStandardsSelected, (void*)ALL, (void*)ALL, status);
        if (U_FAILURE(*status)) {
            printf("ERROR: uhash_put() -> %s\n", u_errorName(*status));
        }
        return;
    }
    for (i = 0; i < gMaxStandards; i++) {
        *status = U_ZERO_ERROR;
        standard = ucnv_getStandard((uint16_t)i, status);
        if (U_FAILURE(*status)) {
            printf("ERROR: ucnv_getStandard() -> %s\n", u_errorName(*status));
        }
        if (uprv_strnicmp(standard, newStandard, nameSize) == 0) {
            uhash_put(gStandardsSelected, (void*)standard, (void*)standard, status);
            if (U_FAILURE(*status)) {
                printf("ERROR: uhash_put() -> %s\n", u_errorName(*status));
            }
            return;  // I'm done, I already found it.
        }
    }
    printf("WARNING: Ignoring unknown standard %s<br>\n", newStandard);
}

static const char* getNextOption(const char* src, const char* srcLimit) {
    const char *nextOpt = strchr(src, OPTION_SEPARATOR);
    if (nextOpt != NULL) {
        src = nextOpt;
    }
    else {
        src = srcLimit;
    }
    return src;
}

static void parseAllStandards(const char *queryStr, UErrorCode *status) {
    const char* src = queryStr;
    int srcLen = strlen(queryStr);
    const char* srcLimit = queryStr + srcLen;

    while (src < srcLimit) {
        const char *nextVal = strchr(src, VALUE_SEPARATOR);
        if (!nextVal) {
            nextVal = srcLimit;
        }
        else {
            nextVal++;
        }
        const char *nextOpt = getNextOption(src, srcLimit);
        if (strncmp(src, "s=", 2) == 0) {
            addStandard(nextVal, nextOpt - nextVal, status);
        }
        else if (strncmp(src, "conv=", 5) == 0) {
            UErrorCode myStatus = U_ZERO_ERROR;
            strncpy(gCurrConverter, nextVal, nextOpt - nextVal);
            gCurrConverter[UCNV_MAX_CONVERTER_NAME_LENGTH-1] = 0;   // NULL terminate for safety

            UConverter *cnv = ucnv_open(gCurrConverter, &myStatus);
            if (U_SUCCESS(myStatus)) {
                strcpy(gCurrConverter, ucnv_getName(cnv, &myStatus));
                ucnv_close(cnv);
            }
            gCurrConverter[UCNV_MAX_CONVERTER_NAME_LENGTH-1] = 0;   // NULL terminate for safety
        }
        else {
            // Woah! I don't know what this option is.
        }
        src = nextOpt+1;
    }
}

/*static void addAllStandards(UHashtable *standardsHashtable, UErrorCode *status) {
    int32_t i;
    const char *standard;

    for (i = 0; i < gMaxStandards; i++) {
        *status = U_ZERO_ERROR;
        standard = ucnv_getStandard((uint16_t)i, status);
        if (U_FAILURE(*status)) {
            printf("ERROR: ucnv_getStandard() -> %s\n", u_errorName(*status));
        }
        uhash_put(standardsHashtable, (void*)standard, (void*)standard, status);
        if (U_FAILURE(*status)) {
            printf("ERROR: ucnv_getStandard() -> %s\n", u_errorName(*status));
        }
    }
}
*/

static void printOptions(UErrorCode *status) {
    int32_t i;
    const char *standard, *checked;

    if (*gCurrConverter) {
        printf("<input type=\"HIDDEN\" name=\"conv\" value=\"%s\" checked>", gCurrConverter);
    }
    for (i = 0; i < gMaxStandards; i++) {
        *status = U_ZERO_ERROR;
        standard = ucnv_getStandard((uint16_t)i, status);
        if (U_FAILURE(*status)) {
            printf("ERROR: ucnv_getStandard() -> %s\n", u_errorName(*status));
        }
        if (uhash_find(gStandardsSelected, standard) != NULL) {
            checked = " checked";
        }
        else {
            checked = "";
        }
        if (standard && *standard) {
            printf("<input type=\"checkbox\" name=\"s\" value=\"%s\"%s> %s<br>\n", standard, checked, standard);
        }
        else {
            printf("<input type=\"checkbox\" name=\"s\" value=\"\"%s> <em>Untagged Aliases</em><br>\n", checked);
        }
    }
    if (uhash_find(gStandardsSelected, ALL) != NULL) {
        checked = " checked";
    }
    else {
        checked = "";
    }
    printf("<input type=\"checkbox\" name=\"s\" value=\"ALL\"%s> <em>All Aliases</em><br>\n", checked);
    puts("<br>");
}

static const char *getConverterType(UConverterType convType) {
    switch (convType) {
    case UCNV_UNSUPPORTED_CONVERTER: return "UCNV_UNSUPPORTED_CONVERTER";
    case UCNV_SBCS: return "UCNV_SBCS";
    case UCNV_DBCS: return "UCNV_DBCS";
    case UCNV_MBCS: return "UCNV_MBCS";
    case UCNV_LATIN_1: return "UCNV_LATIN_1";
    case UCNV_UTF8: return "UCNV_UTF8";
    case UCNV_UTF16_BigEndian: return "UCNV_UTF16_BigEndian";
    case UCNV_UTF16_LittleEndian: return "UCNV_UTF16_LittleEndian";
    case UCNV_UTF32_BigEndian: return "UCNV_UTF32_BigEndian";
    case UCNV_UTF32_LittleEndian: return "UCNV_UTF32_LittleEndian";
    case UCNV_EBCDIC_STATEFUL: return "UCNV_EBCDIC_STATEFUL";
    case UCNV_ISO_2022: return "UCNV_ISO_2022";
    case UCNV_LMBCS_1: return "UCNV_LMBCS_1";
    case UCNV_LMBCS_2: return "UCNV_LMBCS_2"; 
    case UCNV_LMBCS_3: return "UCNV_LMBCS_3";
    case UCNV_LMBCS_4: return "UCNV_LMBCS_4";
    case UCNV_LMBCS_5: return "UCNV_LMBCS_5";
    case UCNV_LMBCS_6: return "UCNV_LMBCS_6";
    case UCNV_LMBCS_8: return "UCNV_LMBCS_8";
    case UCNV_LMBCS_11: return "UCNV_LMBCS_11";
    case UCNV_LMBCS_16: return "UCNV_LMBCS_16";
    case UCNV_LMBCS_17: return "UCNV_LMBCS_17";
    case UCNV_LMBCS_18: return "UCNV_LMBCS_18";
    case UCNV_LMBCS_19: return "UCNV_LMBCS_19";
    case UCNV_HZ: return "UCNV_HZ";
    case UCNV_SCSU: return "UCNV_SCSU";
    case UCNV_ISCII: return "UCNV_ISCII";
    case UCNV_US_ASCII: return "UCNV_US_ASCII";
    case UCNV_UTF7: return "UCNV_UTF7";
    case UCNV_BOCU1: return "UCNV_BOCU1";
    case UCNV_UTF16: return "UCNV_UTF16";
    case UCNV_UTF32: return "UCNV_UTF32";
    case UCNV_CESU8: return "UCNV_CESU8";
    case UCNV_IMAP_MAILBOX: return "UCNV_IMAP_MAILBOX";
    case UCNV_NUMBER_OF_SUPPORTED_CONVERTER_TYPES: return "UCNV_NUMBER_OF_SUPPORTED_CONVERTER_TYPES";
    }
    return "";
}

static void escapeSet(const char *source, int8_t len) {
    uint8_t lastByte = (uint8_t)*source;
    if (len > 0) {
        printf("[");
        printf(startByteEscape, (uint8_t)*(source++));
    }
    else {
        printf(NBSP);
        printf("<!-- Nothing to escape -->");
        return;
    }
    len--;
    while (len-- > 0) {
        if ((uint8_t)*source != lastByte+1) {
            printf(trailingByteEscapeSet, (uint8_t)lastByte);
            if (len > 0) {
                printf(startByteEscape, (uint8_t)*source);
            }
        }
        lastByte = *(source++);
    }
    if ((uint8_t)*source != lastByte+1) {
        printf(trailingByteEscapeSet, (uint8_t)lastByte);
    }
    printf("]");
}

static void escapeBytes(const char *source, int8_t len) {
    if (len > 0) {
        printf(startByteEscape, (uint8_t)*(source++));
    }
    else {
        printf(NBSP);
        printf("<!-- Nothing to escape -->");
    }
    len--;
    while (len-- > 0) {
        if (len > 8 && (*source & 0xF) == 0) {
            puts("<br>");
        }
        printf(trailingByteEscape, (uint8_t)*(source++));
    }
}

/*static void escapeUChars(const UChar *source, int8_t len) {
    if (len > 0) {
        printf(startUCharEscape, (uint8_t)*(source++));
    }
    else {
        printf(NBSP);
        printf("<!-- Nothing to escape -->");
    }
    len--;
    while (len-- > 0) {
        printf(trailingUCharEscape, (uint8_t)*(source++));
    }
}
*/

static void printAmbiguousAliasedConverters() {
    UErrorCode status = U_ZERO_ERROR;
    const char *alias;
    const char *canonicalName;
    const char *standard;
    uint16_t idx, stdIdx;
    uint16_t countAliases = ucnv_countAliases(gCurrConverter, &status);

    for (idx = 0; idx < countAliases; idx++) {
        status = U_ZERO_ERROR;
        alias = ucnv_getAlias(gCurrConverter, idx, &status);
        ucnv_getStandardName(alias, "", &status);
        if (status == U_AMBIGUOUS_ALIAS_WARNING) {
            for (stdIdx = 0; stdIdx < gMaxStandards; stdIdx++) {
                status = U_ZERO_ERROR;
                standard = ucnv_getStandard((uint16_t)stdIdx, &status);
                if (U_FAILURE(status)) {
                    printf("ERROR: ucnv_getStandard() -> %s\n", u_errorName(status));
                }
                canonicalName = ucnv_getCanonicalName(alias, standard, &status);
                if (canonicalName && strcmp(gCurrConverter, canonicalName) != 0) {
                    printf("<a href=\"?conv=%s%s\">%s<br>\n", canonicalName, getStandardOptionsURL(&status), canonicalName);
                }
            }
        }
    }
}

static UBool containsAmbiguousAliases() {
    UErrorCode status = U_ZERO_ERROR;
    const char * alias;
    uint16_t idx;
    uint16_t countAliases = ucnv_countAliases(gCurrConverter, &status);

    for (idx = 0; idx < countAliases; idx++) {
        status = U_ZERO_ERROR;
        alias = ucnv_getAlias(gCurrConverter, idx, &status);
        ucnv_getStandardName(alias, "", &status);
        if (status == U_AMBIGUOUS_ALIAS_WARNING) {
            return TRUE;
        }
    }
    return FALSE;
}

/* Is 0x20-7F always the same? */
static UBool isASCIIcompatible(UConverter *cnv) {
    UErrorCode status = U_ZERO_ERROR;
    static const char ascii[] =
        "\x20\x21\x22\x23\x24\x25\x26\x27\x28\x29\x2A\x2B\x2C\x2D\x2E\x2F"
        "\x30\x31\x32\x33\x34\x35\x36\x37\x38\x39\x3A\x3B\x3C\x3D\x3E\x3F"
        "\x40\x41\x42\x43\x44\x45\x46\x47\x48\x49\x4A\x4B\x4C\x4D\x4E\x4F"
        "\x50\x51\x52\x53\x54\x55\x56\x57\x58\x59\x5A\x5B\x5C\x5D\x5E\x5F"
        "\x60\x61\x62\x63\x64\x65\x66\x67\x68\x69\x6A\x6B\x6C\x6D\x6E\x6F"
        "\x70\x71\x72\x73\x74\x75\x76\x77\x78\x79\x7A\x7B\x7C\x7D\x7E";
    static const UChar expected[] = {
        0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F,
        0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F,
        0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4A, 0x4B, 0x4C, 0x4D, 0x4E, 0x4F,
        0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5A, 0x5B, 0x5C, 0x5D, 0x5E, 0x5F,
        0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6A, 0x6B, 0x6C, 0x6D, 0x6E, 0x6F,
        0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7A, 0x7B, 0x7C, 0x7D, 0x7E
    };
    UChar output[sizeof(ascii) * 8];    /* times 8 just in case escaping occurs */
    UChar *target = output;
    const char *source = ascii;
    ucnv_toUnicode(cnv,
                   &target, target+sizeof(output)/sizeof(output[0]),
                   &source, source+sizeof(ascii),
                   NULL, TRUE, &status);
    ucnv_reset(cnv);
    if (memcmp(expected, output, sizeof(expected)) == 0) {
        return TRUE;
    }
    return FALSE;
}

static void printConverterInfo(UErrorCode *status) {
    char buffer[64];    // It would be insane if it were lager than 64 bytes
    UBool starterBufferBool[256];
    UBool ambiguousAlias = FALSE;
    char starterBuffer[sizeof(starterBufferBool)+1];    // Add one for the NULL
    int8_t len;
    UConverter *cnv = ucnv_open(gCurrConverter, status);

    puts("<h2>Information about this converter</h2>");
    if (U_FAILURE(*status)) {
        printf("<p>Warning: Nothing is known about this converter.</p>");
        return;
    }
    puts(startTable);
    printf("<tr><th>Type of converter</th><td class=\"value\">%s</td></tr>\n", getConverterType(ucnv_getType(cnv)));
    printf("<tr><th>Minimum number of bytes</th><td class=\"value\">%d</td></tr>\n", ucnv_getMinCharSize(cnv));
    printf("<tr><th>Maximum number of bytes</th><td class=\"value\">%d</td></tr>\n", ucnv_getMaxCharSize(cnv));

    printf("<tr><th>Substitution character</th><td class=\"value\">");
    buffer[0] = 0;
    len = sizeof(buffer)/sizeof(buffer[0]);
    ucnv_getSubstChars(cnv, buffer, &len, status);
    escapeBytes(buffer, len);
    if (ucnv_getType(cnv) == UCNV_UTF16 || ucnv_getType(cnv) == UCNV_UTF32) {
        printf(" <strong><em>(Note: This byte sequence is platform dependent)</em></strong>");
    }
    printf("</td></tr>\n");

    printf("<tr><th>Is ASCII [\\x20-\\x7E] compatible?</th><td class=\"value\">%s</td></tr>\n", (isASCIIcompatible(cnv) ? "TRUE" : "FALSE"));

    if (ucnv_getType(cnv) == UCNV_MBCS) {
        UErrorCode myStatus = U_ZERO_ERROR;
        printf("<tr><th>Starter bytes</th><td class=\"value\">");
        ucnv_getStarters(cnv, starterBufferBool, &myStatus);
        starterBuffer[0] = 0;
        len = 0;

        int32_t idx;
        for (idx = 0; idx < (int32_t)(sizeof(starterBufferBool)/sizeof(starterBufferBool[0])); idx++) {
            if (starterBufferBool[idx]) {
                starterBuffer[len++] = (char)idx;
            }
        }
        escapeSet(starterBuffer, len);
        printf("</td></tr>\n");
    }

    printf("<tr><th>Is converter ambiguous?</th><td class=\"value\">%s</td></tr>\n", (ucnv_isAmbiguous(cnv) ? "TRUE" : "FALSE"));
    ambiguousAlias = containsAmbiguousAliases();
    printf("<tr><th>Contains ambiguous aliases?</th><td class=\"value\">%s</td></tr>\n", (ambiguousAlias ? "TRUE" : "FALSE"));
    if (ambiguousAlias) {
        puts("<tr><th>Converters with conflicting aliases</th><td>");
        printAmbiguousAliasedConverters();
        puts("</td></tr>");
    }

    puts(endTable);

    ucnv_close(cnv);
}

static void printStandardHeaders(UErrorCode *status) {
    int32_t i;
    const char *standard;

    puts("<tr><th class=\"standard\">Internal<br>Converter Name</th>");
    for (i = 0; i < gMaxStandards; i++) {
        *status = U_ZERO_ERROR;
        standard = ucnv_getStandard((uint16_t)i, status);
        if (U_FAILURE(*status)) {
            printf("ERROR: ucnv_getStandard() -> %s\n", u_errorName(*status));
        }
        if (uhash_find(gStandardsSelected, standard) != NULL) {
            if (*standard) {
                printf("<th class=\"standard\">%s</th>\n", standard);
            }
            else {
                puts("<th class=\"standard\"><em>Untagged Aliases</em></th>");
            }
        }
    }
    if (uhash_find(gStandardsSelected, ALL) != NULL) {
        puts("<th class=\"standard\"><em>All Aliases</em></th>");
    }
    puts("</tr>");
}

static void printAllAliasList(const char *canonicalName, UErrorCode *status) {
    const char *alias;
    uint16_t idx;
    uint16_t countAliases = ucnv_countAliases(canonicalName, status);

    puts("<td>");
    for (idx = 0; idx < countAliases; idx++) {
        alias = ucnv_getAlias(canonicalName, idx, status);
        printf("%s<br>\n", alias);
        if (U_FAILURE(*status)) {
            printf("ERROR: ucnv_getAlias() -> %s\n", u_errorName(*status));
        }
    }
    puts("</td>");
}

static void printStandardAliasList(const char *canonicalName, const char *standardName, UErrorCode *status) {
    UEnumeration *stdConvEnum = ucnv_openStandardNames(canonicalName, standardName, status);
    const char *alias;
    if (U_FAILURE(*status)) {
//        printf("ERROR: ucnv_openStandardNames() -> %s\n", u_errorName(*status));
    }
    else {
        UErrorCode myStatus = U_ZERO_ERROR;

        puts("<td>");
        if (uenum_count(stdConvEnum, status) == 0) {
            printf(NBSP);
        }
        while ((alias = uenum_next(stdConvEnum, NULL, &myStatus))) {
            printf("%s<br>\n", alias);
        }
        puts("</td>");
    }
    uenum_close(stdConvEnum);
}

static void printAliases(const char *canonicalName, UErrorCode *status) {
    int32_t i;
    const char *standard;

    if (U_FAILURE(*status)) {
        printf("ERROR: ucnv_countAliases() -> %s\n", u_errorName(*status));
    }
    for (i = 0; i < gMaxStandards; i++) {
        *status = U_ZERO_ERROR;
        standard = ucnv_getStandard((uint16_t)i, status);
        if (uhash_find(gStandardsSelected, standard) != NULL) {
            printStandardAliasList(canonicalName, standard, status);
        }
    }
    if (uhash_find(gStandardsSelected, ALL) != NULL) {
        printAllAliasList(canonicalName, status);
    }
}

static void printAliasTable() {
    UErrorCode status = U_ZERO_ERROR;
    UEnumeration *convEnum;

    printf(startTable);
    printStandardHeaders(&status);

    convEnum = ucnv_openAllNames(&status);
    if (U_FAILURE(status)) {
        printf("ERROR: ucnv_openAllNames() -> %s\n", u_errorName(status));
    }
    else {
        const char *canonicalName = NULL;
        int32_t len = 0;
        int32_t allNamesCount = uenum_count(convEnum, &status);
        while ((canonicalName = uenum_next(convEnum, NULL, &status))) {
//        if ((canonicalName = uenum_next(convEnum, NULL, &status))) {
            if (*gCurrConverter && strcmp(canonicalName, gCurrConverter) != 0) {
                continue;
            }
            if (*gCurrConverter) {
                printf("<tr>\n<th>%s</th>\n", canonicalName);
            }
            else {
                printf("<tr>\n<th><a href=\"?conv=%s%s\">%s</a></th>\n",
                    canonicalName, getStandardOptionsURL(&status), canonicalName);
            }
            status = U_ZERO_ERROR;
            printAliases(canonicalName, &status);
            puts("</tr>\n");
        }
        uenum_close(convEnum);
    }
    printf(endTable);
}

extern int
main(int argc, const char *argv[]) {
    int32_t inputLength;
    UErrorCode errorCode = U_ZERO_ERROR;
    const char *cgi;

    gScriptName=getenv("SCRIPT_NAME"); 

    puts(htmlHeader);
    
/* on win32 systems to debug uncomment the block below
 * Invoke the CGI application. 
 * Attach a debugger (such as Visual C) to the CGI process while a message box is on the screen. 
 * When the debugger is attached, open the source file and set a break point. 
 * Click OK to dismiss the message box. When the message box is dismissed, 
 * the CGI execution will resume and the break point will be hit. 
 */
#if 0
#   ifdef WIN32
  
    char szMessage [256];
    
    wsprintf (szMessage, "Please attach a debugger to the process 0x%X (%s) and click OK",
              GetCurrentProcessId(),"idnbrowser");
    MessageBox(NULL, szMessage, "CGI Debug Time!",
               MB_OK|MB_SERVICE_NOTIFICATION);
    
#   endif
#endif
    gStandardsSelected = uhash_open(uhash_hashLong, uhash_compareLong, &errorCode);
    gMaxStandards = ucnv_countStandards();

//    if((cgi=getenv("QUERY_STRING"))!=NULL && *cgi) {
//    if((cgi="s=IBM&s=windows&s=&s=ALL")!=NULL) {
//    if((cgi="conv=ISO_2022,locale=ja,version=0&s=IBM&s=windows&s=&s=ALL")!=NULL) {
//    if((cgi="conv=ibm-943_P130-2000&s=IBM&s=windows&s=&s=ALL")!=NULL) {
    if((cgi="conv=ibm-949")!=NULL) {
//    if((cgi="conv=UTF-8&s=IBM&s=windows&s=&s=ALL")!=NULL) {
//    if((cgi="conv=ibm-930_P120-1999&s=IBM&s=windows&s=&s=ALL")!=NULL) {
//    if((cgi="conv=UTF-8&s=IBM&s=windows&s=&s=ALL")!=NULL) {
//        puts(cgi);
        parseAllStandards(cgi, &errorCode);
    }
    if (uhash_count(gStandardsSelected) == 0) {
        /* Didn't specify a standard. Give the person something to look at. */
        uhash_put(gStandardsSelected, (void*)ALL, (void*)ALL, &errorCode);
        if (U_FAILURE(errorCode)) {
            printf("ERROR: uhash_put() -> %s\n", u_errorName(errorCode));
        }
    }

    if(U_FAILURE(errorCode)) {
        printf(inputError, u_errorName(errorCode));
        inputLength=0;
    }

    if (*gCurrConverter) {
        printf(navigationSubHeader, gCurrConverter);
    }
    else {
        printf(navigationMainHeader);
    }

    puts(navigationEndHeader);

    printf(startForm, gScriptName ? gScriptName : "");

    printOptions(&errorCode);

    printf(endForm);

    printAliasTable();

    if (*gCurrConverter) {
        printConverterInfo(&errorCode);
    }

    
//    puts(helpText);
    puts("<br>\n<hr>");

    char icuVString[16];
    UVersionInfo icuVer;

    u_getVersion(icuVer);
    u_versionToString(icuVer, icuVString);
    printf(versions, icuVString);
    
    puts(htmlFooter);

    uhash_close(gStandardsSelected);
    return 0;
}
