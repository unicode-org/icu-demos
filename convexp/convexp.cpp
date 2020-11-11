/*
*******************************************************************************
*
*   Copyright (C) 2003-2015, International Business Machines
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

/*
TODO:
* Make printAmbiguousAliasedConverters() highlight the default converter mapping for an alias.

*/
#include "unicode/ucnv.h"
#include "unicode/ustring.h"
#include "unicode/uset.h"
#include "unicode/uloc.h"
#include "unicode/ulocdata.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "printcp.h"
#include "params.h"

#include "demo_settings.h"
#include "demoutil.h"

/*
IANA states that an alias may be up to 40 characters.
The only ones that fails this specification are
    1) Extended_UNIX_Code_Packed_Format_for_Japanese
    2) Extended_UNIX_Code_Fixed_Width_for_Japanese
We wrap such unusually long names onto multiple lines.
*/
#define MAX_NOWRAP_ALIAS 40

static const char htmlHeader[]=
    "Content-Type: text/html; charset=utf-8\n"
    "\n"
    "<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Transitional//EN\" \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd\">\n"
    "<html xmlns=\"http://www.w3.org/1999/xhtml\" xml:lang=\"en\" lang=\"en\">\n"
    "<head>\n"
    "\n";

static const char defaultHeader[]=
    "<meta http-equiv=\"Content-Type\" content=\"text/html; charset=UTF-8\"/>\n"
    "<title>ICU Demonstration - " PROGRAM_NAME "</title>\n";

static const char endHeaderBeginBody[]=
    // TODO: This is a custom style that should be changed in the future.
    "<style type=\"text/css\">\n"
    "/*<![CDATA[*/\n"
    "div.unicodeset {font-family: monospace; }\n"
    "span.autobr:after {font-family: monospace; content: \"\\00200B\"; }\n"
    "table.data-table-2 { border-right: #000 solid 1px; border-bottom: #000 solid 1px; border-left: 0px; border-top: 0px; }\n"
    "table.data-table-2 td,\n"
    "table.data-table-2 th{ padding-left:3px; padding-right:3px; padding-top:3px; border-left: #000 solid 1px; border-top: #000 solid 1px; border-right: 0px; border-bottom: 0px; }\n"
    "table.data-table-2 caption {border-bottom:#fff solid 0px;}\n"
    "table.data-table-2 td.alias {white-space: nowrap;}\n"
    "table.data-table-2 td.reserved {min-width: 5.25%; padding-top: 0.85em; padding-bottom: 0.85em; white-space: nowrap; background-color: #EEEEEE; text-align: center; font-size: 115%; font-family: monospace;}\n"
    "table.data-table-2 td.continue {min-width: 5.25%; padding-top: 0.85em; padding-bottom: 0.85em; white-space: nowrap; background-color: #EEEEEE; text-align: center; font-size: 115%; font-family: monospace;}\n"
    "table.data-table-2 div.iso {margin-top: 0.2em; margin-bottom: 0.2em; border: solid; border-width: 1px; font-size: 100%; font-family: monospace;}\n"
    "table.data-table-2 div.glyph {font-size: 160%; font-family: initial;}\n" // don't use serif or the backslash will look funny on Windows Vista.
    "/*]]>*/\n"
    "</style>\n"
    "</head>\n"
    "<body>\n";

static const char breadCrumbMainHeader[]=
    "<div style=\"font-size: 6px\">" NBSP "</div>\n"
    "<a class=\"bctl\" href=\"//www.icu-project.org/\">ICU</a><span class=\"bct\">" NBSP NBSP "&gt;" NBSP "</span>\n"
    "<a class=\"bctl\" href=\"icudemos\">Demo</a><span class=\"bct\">" NBSP NBSP "&gt;" NBSP "</span>\n";

static const char navigationMainHeader[]=
    "<strong class=\"bctl\">" PROGRAM_NAME "</strong>\n";

static const char navigationSubHeader[]=
    "<a class=\"bctl\" href=\"%s?%s\">" PROGRAM_NAME "</a><span class=\"bct\">" NBSP NBSP "&gt;" NBSP "</span>\n"
    "\n";

static const char navigationEndHeader[]=
    "<h1>%s</h1>\n";

static const char htmlFooter[]=
    "</body>\n"
    "</html>\n";

static const char *inputError="<p>Error parsing the input string: %s</p>\n";

static const char startByteEscape[]="\\x%02X";

static const char trailingByteEscape[]="\\x%02X";
static const char trailingByteEscapeSet[]="-\\x%02X";

static const char startUCharEscape[]="\\u%04X";

static const char trailingUCharEscape[]="\\u%04X";

static const char startForm[]=
    "<table width=\"97%%\" border=\"0\" cellspacing=\"0\" style=\"margin: 1em 0em 1em 1em\">\n"
    "<tr><td style=\"white-space: nowrap\">\n"
    "<form method=\"get\" action=\"%s\">\n"
    "<p>Select a standard to view:<br />\n"
    "<br />\n"
    "\n";

static const char endForm[]=
    "<input type=\"submit\" value=\"Show Aliases\" />\n"
    "</p>"
    "</form>\n"
    "</td>\n"
    "<td align=\"right\" valign=\"top\">\n"

    "<table cellspacing=\"0\" cellpadding=\"0\" border=\"0\" width=\"175\" class=\"v14-gray-table-border\">\n"
    "<tr><th colspan=\"3\" class=\"v14-header-1-small\">Related Topics</th></tr>\n"
    "<tr><td colspan=\"3\"><span style=\"font-size: 5px\">" NBSP "</span></td></tr>\n"
    "<tr><td align=\"right\" valign=\"top\">" NBSP "</td><td align=\"left\">&middot;" NBSP "<a href=\"http://www.icu-project.org/docs/demo/convexp_help.html\">" PROGRAM_NAME " Help</a></td><td width=\"5\">" NBSP "</td></tr>\n"
    "<tr><td align=\"right\" valign=\"top\">" NBSP "</td><td align=\"left\">&middot;" NBSP "<a href=\"http://www.icu-project.org/charts/charset/\">ICU Charset Information</a></td><td width=\"5\">" NBSP "</td></tr>\n"
    "<tr><td><span style=\"font-size: 5px\">" NBSP "</span></td></tr>\n"
    "</table>\n"

    "</td></tr>\n"
    "</table>\n";

static const char startTable[]=
    "<table border=\"1\" cellspacing=\"0\" cellpadding=\"2\">\n";

static const char endTable[]="</table>";

static const char versions[]=
    "<p style=\"border-top: 1px solid silver; margin-top: 2em; margin-bottom: 0; width: 99%\">Powered by "
    "<a href=\"//www.icu-project.org/\">ICU</a> %s</p><br />\n";

static void printOptions(UErrorCode *status) {
    int32_t i;
    const char *standard, *checked;

    if (*gCurrConverter) {
        printf("<input type=\"hidden\" name=\"conv\" value=\"%s\" checked=\"checked\" />\n", gCurrConverter);
    }
    if (*gStartBytes) {
        printf("<input type=\"hidden\" name=\"b\" value=\"%s\"  checked=\"checked\" />\n", gStartBytes);
    }
    for (i = 0; i < gMaxStandards; i++) {
        *status = U_ZERO_ERROR;
        standard = ucnv_getStandard((uint16_t)i, status);
        if (U_FAILURE(*status)) {
            printf("ERROR: ucnv_getStandard() -> %s\n", u_errorName(*status));
        }
        if (gStandardsSelected->count(std::string(standard))) {
            checked = " checked";
        }
        else {
            checked = "";
        }
        if (standard && *standard) {
            printf("<input type=\"checkbox\" name=\"s\" value=\"%s\" id=\"%s\"%s /> <label for=\"%s\">%s</label><br />\n",
                standard, standard, checked, standard, standard);
        }
        else {
            printf("<input type=\"checkbox\" name=\"s\" value=\"-\" id=\"UntaggedAliases\"%s /> <label for=\"UntaggedAliases\"><em>Untagged Aliases</em></label><br />\n",
                checked);
        }
    }
    if (gStandardsSelected->count(std::string(ALL))) {
        checked = " checked=\"checked\"";
    }
    else {
        checked = "";
    }
    printf("<input type=\"checkbox\" name=\"s\" value=\"ALL\" id=\"AllAliases\"%s /> <label for=\"AllAliases\"><em>All Aliases</em></label><br />\n",
        checked);
    printf("<input type=\"checkbox\" name=\"" SHOW_UNAVAILABLE "\" value=\"\" id=\"" SHOW_UNAVAILABLE "\"%s /> <label for=\"" SHOW_UNAVAILABLE "\"><em>Show Unavailable Converters</em></label><br />\n",
        ( gShowUnavailable ? " checked=\"checked\"" : ""));
    puts("<br />");
}

static const char *getConverterType(UConverterType convType) {
    switch (convType) {
    case UCNV_COMPOUND_TEXT:  return "UCNV_UNSUPPORTED_CONVERTER (compound text)";
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
            puts("<br />");
        }
        printf(trailingByteEscape, (uint8_t)*(source++));
    }
}

static void printAmbiguousAliasedConverters() {
    UErrorCode status = U_ZERO_ERROR;
    const char *alias;
    const char *canonicalName;
    const char *standard;
    uint16_t idx, stdIdx;
    uint16_t countAliases = ucnv_countAliases(gCurrConverter, &status);

    /* Do not include the startBytes in the address. It's specific to the current converter. */

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
                    printf("<a href=\"%s?conv=%s" OPTION_SEP_STR "%s\">%s</a> %s { %s }<br />\n",
                        gScriptName, canonicalName, getStandardOptionsURL(&status), canonicalName, alias, standard);
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
            return true;
        }
    }
    return false;
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
                   NULL, true, &status);
    ucnv_reset(cnv);
    if (U_SUCCESS(status) && memcmp(expected, output, sizeof(expected)) == 0) {
        return true;
    }
    return false;
}

/*
returns a new UnicodeSet that is a flattened form of the original
UnicodeSet.
TODO: Some codepages have multi-codepoint mappings, and we presume
    that the codepage data is in NFKC. Right now, none of the languages
    require the multicode point mappings (e.g. one codepage codepoint
    to n Unicode codepoints). Fix this in the future when this changes.
*/
static USet *createFlattenSet(USet *origSet, UErrorCode *status) {
    if (U_FAILURE(*status)) {
        printf("createFlattenSet called with %s\n", u_errorName(*status));
        return NULL;
    }

    USet *newSet = uset_open(1, 0);
    int32_t origItemCount = uset_getItemCount(origSet);
    int32_t idx, graphmeSize;
    UChar32 start, end;
    UChar graphme[64];

    for (idx = 0; idx < origItemCount; idx++) {
        graphmeSize = uset_getItem(origSet, idx,
            &start, &end, 
            graphme, (int32_t)(sizeof(graphme)/sizeof(graphme[0])),
            status);
        if (U_FAILURE(*status)) {
            printf("ERROR: uset_getItem returned %s\n", u_errorName(*status));
            *status = U_ZERO_ERROR;
        }
        if (graphmeSize) {
            uset_addAllCodePoints(newSet, graphme, graphmeSize);
        }
        else {
            uset_addRange(newSet, start, end);
        }
    }
    return newSet;
}

static void printUnicodeSet(USet *cnvSet, UErrorCode *status, UBool alwaysShowSet) {
    int32_t patLen;
    UChar *patBuffer;
    char *patBufferUTF8;
    char setStrBuf[64];
    UChar setUStrBuf[sizeof(setStrBuf)];
    UErrorCode myStatus = U_ZERO_ERROR;
    patLen = uset_toPattern(cnvSet, NULL, 0, true, &myStatus) + 1;
    if (alwaysShowSet || patLen < (int32_t)(sizeof(setUStrBuf)/sizeof(setUStrBuf[0]))) {
        if (patLen < (int32_t)(sizeof(setUStrBuf)/sizeof(setUStrBuf[0]))) {
            patBuffer = setUStrBuf;
            patBufferUTF8 = setStrBuf;
        }
        else {
            patBuffer = (UChar*)malloc((patLen + 1) * sizeof(UChar));
            patBufferUTF8 = (char*)malloc((patLen + 1) * sizeof(char));
        }
        if (U_SUCCESS(*status) && patBuffer && patBufferUTF8) {
            myStatus = U_ZERO_ERROR;
            patLen = uset_toPattern(cnvSet, patBuffer, patLen, true, &myStatus) + 1;
            u_strToUTF8(patBufferUTF8, patLen, NULL, patBuffer, patLen, &myStatus);

            /*
            This is a hack solely for Firefox because it doesn't do proper line breaking.
            So we suggest locations to break with the autobr span.
            This implicitly inserts whitespace without inserting real whitespace
            for copy and paste operations.
            */
            printf("<div class=\"unicodeset\">");
            char *currStr = patBufferUTF8;
            char *lastStr = patBufferUTF8;
            if (patLen >= (int32_t)(sizeof(setUStrBuf)/sizeof(setUStrBuf[0]))) {
                while ((currStr = strchr(currStr, '\\')) != NULL) {
                    printf("<span class=\"autobr\">%.*s</span>", currStr-lastStr, lastStr);
                    lastStr = currStr;
                    currStr++;
                }
            }
            /* else don't insert breaks for the small list. */
            printf("%s</div>\n", lastStr);
        }
        else {
            puts("<p>Not Available</p>");
        }
        if (patBuffer != setUStrBuf) {
            free(patBuffer);
            free(patBufferUTF8);
        }
        *status = U_ZERO_ERROR;
    }
    else {
        printf("<p><a href=\"%s?conv=%s" OPTION_SEP_STR SHOW_UNICODESET OPTION_SEP_STR "%s#" SHOW_UNICODESET "\">View Complete Set...</a></p>\n",
            gScriptName, gCurrConverter, getStandardOptionsURL(&myStatus));
    }
}

static void printLanguages(UConverter * /*cnv*/, USet *cnvSet, UErrorCode *status) {
    UChar patBuffer[128];
    char patBufferUTF8[1024]; /* 4 times as large as patBuffer */
    int32_t patLen;
    int32_t locCount = uloc_countAvailable();
    int32_t locIndex;
    UBool localeFound = false;
    UErrorCode myStatus = U_ZERO_ERROR;

    if (U_FAILURE(*status)) {
        return;
    }
    puts("<br /><table border=\"0\" cellpadding=\"0\" cellspacing=\"0\" class=\"data-table-1\">\n");
    puts("<caption><em><a name=\"" SHOW_LOCALES "\"></a>List of Languages Representable By This Codepage</em></caption>\n");
    if (!gShowLanguages) {
        printf("<tr><td><a href=\"%s?conv=%s" OPTION_SEP_STR SHOW_LOCALES OPTION_SEP_STR "%s#ShowLocales\">View Complete Set...</a></td></tr>\n",
            gScriptName, gCurrConverter, getStandardOptionsURL(&myStatus));
    }
    else {
        myStatus = U_ZERO_ERROR;
        USet *locSet = uset_open(1, 0);

        for (locIndex = 0; locIndex < locCount; locIndex++) {
            const char *locale = uloc_getAvailable(locIndex);
            if (locale) {
                myStatus = U_ZERO_ERROR;
                ULocaleData *localeDataObj = ulocdata_open(locale, &myStatus);
                ulocdata_getExemplarSet(localeDataObj, locSet, 0, ULOCDATA_ES_STANDARD, &myStatus);
                ulocdata_close(localeDataObj);
                if (myStatus != U_USING_FALLBACK_WARNING) {
                    // Flatten glyphs like "ch" of a non-falling back set.
                    USet *flatLocSet = createFlattenSet(locSet, status);
                    if (U_SUCCESS(*status) && uset_containsAll(cnvSet, flatLocSet)) {
                        patLen = uloc_getDisplayName(locale, "en", patBuffer, sizeof(patBuffer)/sizeof(patBuffer[0]), &myStatus);
                        if (U_SUCCESS(*status)) {
                            /* Make sure that the string is NULL terminated in case really bad things happen. */
                            patBuffer[sizeof(patBuffer)/sizeof(patBuffer[0])-1] = 0;
                            patBufferUTF8[0] = 0;
                            u_strToUTF8(patBufferUTF8, sizeof(patBufferUTF8)/sizeof(patBufferUTF8[0]), NULL, patBuffer, patLen, &myStatus);
                            patBufferUTF8[sizeof(patBufferUTF8)/sizeof(patBufferUTF8[0])-1] = 0;
                            if (!localeFound) {
                                localeFound = true;
                                puts("<tr><th>Locale</th><th>Locale Name</th></tr>");
                            }
                            printf("<tr><td>%s</td><td>%s</td></tr>\n",
                                locale, patBufferUTF8);
                        }
                    }
                    /*else {
                        uset_removeAll(flatLocSet, cnvSet);
                        printf("<tr><td>%s</td><td>", locale);
                        printUnicodeSet(flatLocSet, status, true);
                        printf("</td></tr>\n");
                    }*/
                    /*else {
                        printf("<tr><td>%s</td><td>" NBSP " -- Skipped --</td></tr>\n",
                            locale);
                    }*/
                    uset_close(flatLocSet);
                }
            }
        }
        if (!localeFound) {
            puts("<tr><td>Not Available</td><td></td></tr>\n");
        }

        *status = U_ZERO_ERROR;
    }
    printf(endTable);
}

static void printConverterInfo(UErrorCode *status) {
    char buffer[64];    // It would be insane if it were lager than 64 bytes
    UBool starterBufferBool[256];
    UBool ambiguousAlias = false;
    char starterBuffer[sizeof(starterBufferBool)+1];    // Add one for the NULL
    int8_t len;
    UConverter *cnv = ucnv_open(gCurrConverter, status);
    UConverterType convType;
    UErrorCode myStatus = U_ZERO_ERROR;

    printCPTable(cnv, gStartBytes, status);

    /* Note: If this set changes, you need to update the help text too. */
    USet *nfcSet = uset_openPattern(UNICODE_STRING_SIMPLE("[[:NFC_Quick_Check=yes:]&[:ccc=0:]]").getTerminatedBuffer(), -1, status);
    USet *bidiSet = uset_openPattern(UNICODE_STRING_SIMPLE("[[:Bidi_Class=AL:][:Bidi_Class=R:][:Bidi_Class=RLE:][:Bidi_Class=RLO:]]").getTerminatedBuffer(), -1, status);
    USet *cnvSet = uset_open(1, 0);
    ucnv_getUnicodeSet(cnv, cnvSet, UCNV_ROUNDTRIP_SET, status);

    puts("<br />\n");
    if (U_FAILURE(*status)) {
        puts("<h2>Information About This Converter</h2>");
        printf("<p>Warning: Nothing is known about this converter.</p>");
        return;
    }
    puts("<table border=\"0\" cellpadding=\"0\" cellspacing=\"0\" class=\"data-table-1\">\n");
    puts("<caption><em>Information About This Converter</em></caption>\n");
    convType = ucnv_getType(cnv);
    printf("<tr><th>Type of converter</th><td>%s</td></tr>\n", getConverterType(convType));
    printf("<tr><th>Minimum number of bytes per UChar</th><td>%d</td></tr>\n", ucnv_getMinCharSize(cnv));
    printf("<tr><th>Maximum number of bytes per UChar</th><td>%d</td></tr>\n", ucnv_getMaxCharSize(cnv));

    printf("<tr><th>Substitution character</th><td>");
    buffer[0] = 0;
    len = sizeof(buffer)/sizeof(buffer[0]);
    ucnv_getSubstChars(cnv, buffer, &len, status);
    if (U_SUCCESS(*status)) {
        escapeBytes(buffer, len);
    }
    else {
        printf(NBSP);
    }
    if (convType == UCNV_UTF16 || convType == UCNV_UTF16_BigEndian
        || convType == UCNV_UTF16_LittleEndian || convType == UCNV_UTF32
        || convType == UCNV_UTF32_BigEndian || convType == UCNV_UTF32_LittleEndian )
    {
        printf(" <strong><em>(See note below)</em></strong>\n");
    }
    printf("</td></tr>\n");

    if (ucnv_getType(cnv) == UCNV_MBCS) {
        printf("<tr><th>Starter bytes</th><td>");
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

    printf("<tr><th>Is ASCII [\\x20-\\x7E] compatible?</th><td>%s</td></tr>\n", (isASCIIcompatible(cnv) ? "TRUE" : "FALSE"));
    printf("<tr><th>Is ASCII [\\u0020-\\u007E] ambiguous?</th><td>%s</td></tr>\n", (ucnv_isAmbiguous(cnv) ? "TRUE" : "FALSE"));

    ambiguousAlias = containsAmbiguousAliases();
    printf("<tr><th>Contains ambiguous aliases?</th><td>%s</td></tr>\n", (ambiguousAlias ? "TRUE" : "FALSE"));
    if (ambiguousAlias) {
        puts("<tr><th>Converters with conflicting aliases</th><td>");
        printAmbiguousAliasedConverters();
        puts("</td></tr>");
    }

    printf("<tr><th>Always generates Unicode NFC?</th><td>%s</td></tr>\n", (uset_containsAll(nfcSet, cnvSet) ? "TRUE": "UNKNOWN"));
    printf("<tr><th>Contains BiDi characters?</th><td>%s</td></tr>\n", (uset_containsSome(bidiSet, cnvSet) ? "TRUE": "FALSE"));
    puts(endTable);

    printLanguages(cnv, cnvSet, status);

    puts("<br /><h2><a name=\"" SHOW_UNICODESET "\"></a>Set of Unicode Characters Representable By This Codepage</h2>");
    printUnicodeSet(cnvSet, status, gShowUnicodeSet);

    if (convType == UCNV_UTF16 || convType == UCNV_UTF16_BigEndian
        || convType == UCNV_UTF16_LittleEndian || convType == UCNV_UTF32
        || convType == UCNV_UTF32_BigEndian || convType == UCNV_UTF32_LittleEndian )
    {
        puts("<p><strong><em>Note:</em></strong> The substitution byte sequence can be platform dependent.\n"
             "It depends on the endianess of the platform.\n"
             "Please see the <a href=\"http://www.unicode.org/faq/utf_bom.html\">Unicode FAQ</a> for details.</p>");
    }

    ucnv_close(cnv);
    uset_close(cnvSet);
    uset_close(nfcSet);
    uset_close(bidiSet);
}

static void printStandardHeaders(UErrorCode *status) {
    int32_t i;
    const char *standard;

    puts("<tr><th style=\"text-align: center\">Internal<br />Converter Name</th>");
    for (i = 0; i < gMaxStandards; i++) {
        *status = U_ZERO_ERROR;
        standard = ucnv_getStandard((uint16_t)i, status);
        if (U_FAILURE(*status)) {
            printf("ERROR: ucnv_getStandard() -> %s\n", u_errorName(*status));
        }
        if (gStandardsSelected->count(std::string(standard))) {
            if (*standard) {
                printf("<th style=\"text-align: center\">%s</th>\n", standard);
            }
            else {
                puts("<th style=\"text-align: center\"><em>Untagged Aliases</em></th>");
            }
        }
    }
    if (gStandardsSelected->count(std::string(ALL))) { 
        puts("<th style=\"text-align: center\"><em>All Aliases</em></th>");
    }
    puts("</tr>");
}

static inline void printAlias(const char *alias, UBool isFinal) {
    if (strlen(alias) > MAX_NOWRAP_ALIAS) {
        char *nextUnderscore = (char *)alias;
        puts("<!-- This alias is way too long. So we do special formatting so that the browser can word wrap. -->");
        printf("<span style=\"font-style: italic\" title=\"%s\">", alias);
        while ((nextUnderscore = strchr(nextUnderscore, '_'))) {
            printf("%.*s<br />\n" NBSP NBSP NBSP NBSP, nextUnderscore-alias, alias);
            alias = nextUnderscore;
            nextUnderscore++; // Skip the current '_'
        }
        printf("%s</span>", alias);
    }
    else {
        printf("%s", alias);
    }
    if (!isFinal) { /* Don't print a break after the last item. */
        puts("<br />");
    }
}

static void printAllAliasList(const char *canonicalName, UErrorCode *status) {
    const char *alias;
    uint16_t idx;
    uint16_t countAliases = ucnv_countAliases(canonicalName, status);

    puts("<td class=\"alias\">");
    for (idx = 0; idx < countAliases; idx++) {
        alias = ucnv_getAlias(canonicalName, idx, status);
        printAlias(alias, (UBool)(idx + 1 == countAliases));
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
        int32_t aliasCount = uenum_count(stdConvEnum, status);

        puts("<td class=\"alias\">");
        if (aliasCount == 0) {
            printf(NBSP);
        }
        while ((alias = uenum_next(stdConvEnum, NULL, &myStatus))) {
            printAlias(alias, (UBool)(--aliasCount == 0));
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
        if (gStandardsSelected->count(std::string(standard))) {
            printStandardAliasList(canonicalName, standard, status);
        }
    }
    if (gStandardsSelected->count(std::string(ALL))) {
        printAllAliasList(canonicalName, status);
    }
}

static void printAliasTable() {
    UErrorCode status = U_ZERO_ERROR;
    UEnumeration *convEnum;

    puts("<br /><table cellspacing=\"0\" class=\"data-table-2\">\n");
    puts("<caption><em>List of Converter Aliases</em></caption>");
    printStandardHeaders(&status);

    convEnum = ucnv_openAllNames(&status);
    if (U_FAILURE(status)) {
        printf("ERROR: ucnv_openAllNames() -> %s\n", u_errorName(status));
    }
    else {
        const char *canonicalName = NULL;
        //int32_t len = 0;
        //int32_t allNamesCount = uenum_count(convEnum, &status);
        while ((canonicalName = uenum_next(convEnum, NULL, &status))) {
            if (*gCurrConverter && strcmp(canonicalName, gCurrConverter) != 0) {
                continue;
            }
            if (*gCurrConverter) {
                printf("<tr>\n<th style=\"white-space: nowrap\">%s</th>\n", canonicalName);
            }
            else {
                if (!gShowUnavailable) {
                    // When listing all the converters, skip the unavailable.
                    UErrorCode openStatus = U_ZERO_ERROR;
                    ucnv_close(ucnv_open(canonicalName, &openStatus));
                    if (U_FAILURE(openStatus)) {
                        continue;
                    }
                }
                printf("<tr>\n<th style=\"white-space: nowrap\"><a href=\"%s?conv=%s" OPTION_SEP_STR "%s\">%s</a></th>\n",
                    gScriptName, canonicalName, getStandardOptionsURL(&status), canonicalName);
            }
            status = U_ZERO_ERROR;
            printAliases(canonicalName, &status);
            puts("</tr>");
            if (*gCurrConverter) {
                // We were only looking for one converter. There is no need to look further.
                break;
            }
        }
        uenum_close(convEnum);
    }
    printf(endTable);
}

extern int
main(int /*argc*/, const char * /*argv*/[]) {
    int32_t inputLength;
    UErrorCode errorCode = U_ZERO_ERROR;
    const char *cgi;

	alarm(10);

    gScriptName=getenv("SCRIPT_NAME");
    if (gScriptName == NULL) {
        gScriptName = "convexp";
    }

    puts(htmlHeader);
    if (!printHTMLFragment(NULL, NULL, DEMO_COMMON_DIR "convexp-header.html")) {
        puts(defaultHeader);
    }
    puts(endHeaderBeginBody);
    if (printHTMLFragment(NULL, NULL, DEMO_COMMON_MASTHEAD)) {
        puts(DEMO_BEGIN_LEFT_NAV);
        printHTMLFragment(NULL, NULL, DEMO_COMMON_LEFTNAV);
        puts(DEMO_END_LEFT_NAV);
        puts(DEMO_BEGIN_CONTENT);
    }
    puts(breadCrumbMainHeader);

    gStandardsSelected = new std::set<std::string>();
    gMaxStandards = ucnv_countStandards();

    if((cgi=getenv("QUERY_STRING"))!=NULL && *cgi) {
//    if((cgi="conv=utf-16be")!=NULL) {
//    if((cgi="conv=ISO_2022%2Clocale%3Dja%2Cversion0")!=NULL) {
//    if((cgi="s=IBM&s=windows&s=&s=ALL")!=NULL) {
//    if((cgi="conv=ibm-1388&b=0e")!=NULL) {
//    if((cgi="conv=ISO_2022,locale=ja,version=0&b=&ShowLocales&s=IBM&s=windows&s=&s=ALL")!=NULL) {
//    if((cgi="conv=ibm-943_P130-2000&s=IBM&s=windows&s=&s=ALL")!=NULL) {
//    if((cgi="conv=ibm-949")!=NULL) {
//    if((cgi="conv=windows-1256&b=")!=NULL) {
//    if((cgi="conv=ibm-950&ShowLocales")!=NULL) {
//    if((cgi="conv=ASCII&ShowLocales")!=NULL) {
//    if((cgi="conv=iso-8859-9&ShowLocales")!=NULL) {
//    if((cgi="conv=ibm-949_P11A-2000")!=NULL) {
//    if((cgi="conv=UTF-8&s=IBM&s=windows&s=&s=ALL")!=NULL) {
//    if((cgi="conv=ibm-930_P120-1999&s=IBM&s=windows&s=&s=ALL")!=NULL) {
//    if((cgi="conv=UTF-8&ShowLocales&s=IBM&s=windows&s=&s=ALL")!=NULL) {
//        puts(cgi);
        parseAllOptions(cgi, &errorCode);
    }
    if (gStandardsSelected->size() == 0) {
        /* Didn't specify a standard. Give the person something to look at. */
        gStandardsSelected->insert(std::string(ALL));
        if (U_FAILURE(errorCode)) {
            printf("ERROR: uhash_put() -> %s\n", u_errorName(errorCode));
        }
    }

    if(U_FAILURE(errorCode)) {
        printf(inputError, u_errorName(errorCode));
        inputLength=0;
    }

    if (*gCurrConverter) {
        printf(navigationSubHeader, gScriptName, getStandardOptionsURL(&errorCode));
        printf(navigationEndHeader, gCurrConverter);
    }
    else {
        printf(navigationEndHeader, PROGRAM_NAME);
    }

    printf(startForm, gScriptName ? gScriptName : "");

    printOptions(&errorCode);

    printf(endForm);

    printAliasTable();

    if (*gCurrConverter) {
        printConverterInfo(&errorCode);
    }

    char icuVString[16];
    UVersionInfo icuVer;

    u_getVersion(icuVer);
    u_versionToString(icuVer, icuVString);
    printf(versions, icuVString);
    
    puts(DEMO_END_CONTENT);
    printHTMLFragment(NULL, NULL, DEMO_COMMON_FOOTER);
    puts(htmlFooter);

    delete gStandardsSelected;
    return 0;
}
