/*
*******************************************************************************
*
*   Copyright (C) 2003, International Business Machines
*   Corporation and others.  All Rights Reserved.
*
*******************************************************************************
*   file name:  printcp.cpp
*   encoding:   US-ASCII
*   tab size:   4 (not used)
*   indentation:4
*
*   created on: 2003Mar28
*   created by: George Rhoten
*
*   This is a fairly crude converter explorer. It allows you to browse the
*   converter alias table without requiring the people to know the alias table syntax.
*
*/

#include "unicode/ucnv.h"
#include "unicode/ustring.h"
#include "unicode/uchar.h"

#include "printcp.h"
#include "params.h"

#include <stdio.h>
#include <string.h>

//#define CELL_WIDTH " width=\"5%\""
#define CELL_WIDTH ""
#define ISO_BEGIN "<div class=\"iso\">"
#define ISO_END "</div>"

static void printHeader() {
    int32_t idx;
    puts("<tr><td></td>");
    for (idx = 0; idx < 16; idx++) {
        printf("<th class=\"standard\"" CELL_WIDTH ">%02X</th>", idx);
    }
    puts("<td></td></tr>");
}

static const char *getEscapeChar(UChar32 uniVal) {
    switch (uniVal) {
    case '<':   return "&lt;<br>";
    case '>':   return "&gt;<br>";
    case '&':   return "&amp;<br>";
    case 0x00:  return ISO_BEGIN "NUL" ISO_END;
    case 0x01:  return ISO_BEGIN "SOH" ISO_END;
    case 0x02:  return ISO_BEGIN "STX" ISO_END;
    case 0x03:  return ISO_BEGIN "ETX" ISO_END;
    case 0x04:  return ISO_BEGIN "EOT" ISO_END;
    case 0x05:  return ISO_BEGIN "ENQ" ISO_END;
    case 0x06:  return ISO_BEGIN "ACK" ISO_END;
    case 0x07:  return ISO_BEGIN "BEL" ISO_END;
    case 0x08:  return ISO_BEGIN "BS" ISO_END;
    case 0x09:  return ISO_BEGIN "HT" ISO_END;
    case 0x0A:  return ISO_BEGIN "LF" ISO_END;
    case 0x0B:  return ISO_BEGIN "VT" ISO_END;
    case 0x0C:  return ISO_BEGIN "FF" ISO_END;
    case 0x0D:  return ISO_BEGIN "CR" ISO_END;
    case 0x0E:  return ISO_BEGIN "SO" ISO_END;
    case 0x0F:  return ISO_BEGIN "SI" ISO_END;
    case 0x10:  return ISO_BEGIN "DLE" ISO_END;
    case 0x11:  return ISO_BEGIN "DC1" ISO_END;
    case 0x12:  return ISO_BEGIN "DC2" ISO_END;
    case 0x13:  return ISO_BEGIN "DC3" ISO_END;
    case 0x14:  return ISO_BEGIN "DC4" ISO_END;
    case 0x15:  return ISO_BEGIN "NAK" ISO_END;
    case 0x16:  return ISO_BEGIN "SYN" ISO_END;
    case 0x17:  return ISO_BEGIN "ETB" ISO_END;
    case 0x18:  return ISO_BEGIN "CAN" ISO_END;
    case 0x19:  return ISO_BEGIN "EM" ISO_END;
    case 0x1A:  return ISO_BEGIN "SUB" ISO_END;
    case 0x1B:  return ISO_BEGIN "ESC" ISO_END;
    case 0x1C:  return ISO_BEGIN "FS" ISO_END;
    case 0x1D:  return ISO_BEGIN "GS" ISO_END;
    case 0x1E:  return ISO_BEGIN "RS" ISO_END;
    case 0x1F:  return ISO_BEGIN "US" ISO_END;
    case 0x7F:  return ISO_BEGIN "DEL" ISO_END;
//    case 0x80:  return ISO_BEGIN "&nbsp;" ISO_END;
//    case 0x81:  return ISO_BEGIN "&nbsp;" ISO_END;
    case 0x82:  return ISO_BEGIN "BPH" ISO_END;
    case 0x83:  return ISO_BEGIN "NBH" ISO_END;
//    case 0x84:  return ISO_BEGIN "IND" ISO_END;
    case 0x85:  return ISO_BEGIN "NEL" ISO_END;
    case 0x86:  return ISO_BEGIN "SSA" ISO_END;
    case 0x87:  return ISO_BEGIN "ESA" ISO_END;
    case 0x88:  return ISO_BEGIN "HTS" ISO_END;
    case 0x89:  return ISO_BEGIN "HTJ" ISO_END;
    case 0x8A:  return ISO_BEGIN "VTS" ISO_END;
    case 0x8B:  return ISO_BEGIN "PLD" ISO_END;
    case 0x8C:  return ISO_BEGIN "PLU" ISO_END;
    case 0x8D:  return ISO_BEGIN "RI" ISO_END;
    case 0x8E:  return ISO_BEGIN "SS2" ISO_END;
    case 0x8F:  return ISO_BEGIN "SS3" ISO_END;
    case 0x90:  return ISO_BEGIN "DCS" ISO_END;
    case 0x91:  return ISO_BEGIN "PU1" ISO_END;
    case 0x92:  return ISO_BEGIN "PU2" ISO_END;
    case 0x93:  return ISO_BEGIN "STS" ISO_END;
    case 0x94:  return ISO_BEGIN "CCH" ISO_END;
    case 0x95:  return ISO_BEGIN "MW" ISO_END;
    case 0x96:  return ISO_BEGIN "SPA" ISO_END;
    case 0x97:  return ISO_BEGIN "EPA" ISO_END;
    case 0x98:  return ISO_BEGIN "SOS" ISO_END;
//    case 0x99:  return ISO_BEGIN "&nbsp;" ISO_END;
    case 0x9A:  return ISO_BEGIN "SCI" ISO_END;
    case 0x9B:  return ISO_BEGIN "CSI" ISO_END;
    case 0x9C:  return ISO_BEGIN "ST" ISO_END;
    case 0x9D:  return ISO_BEGIN "OSC" ISO_END;
    case 0x9E:  return ISO_BEGIN "PM" ISO_END;
    case 0x9F:  return ISO_BEGIN "APC" ISO_END;
    }
    if (u_iscntrl(uniVal)) {
        return ISO_BEGIN NBSP ISO_END;
    }

    return NULL;
}

static const int8_t
basicToDigit[256]={
     0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
     0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,

     0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
     0,  1,  2,  3,  4,  5,  6,  7,  8,  9,  0,  0,  0,  0,  0,  0,

     0, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24,
    15, 16, 17, 18, 19, 20, 21, 22, 23, 24,  0,  0,  0,  0,  0,  0,

     0, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24,
    15, 16, 17, 18, 19, 20, 21, 22, 23, 24,  0,  0,  0,  0,  0,  0,

     0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
     0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,

     0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
     0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,

     0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
     0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,

     0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
     0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0
};

char *parseAndCopy(char *dest, const char *source, int32_t len) {
    len-=2;
    while (len >= 0) {
        dest[len/2] = (basicToDigit[source[len]] << 4) + basicToDigit[source[len+1]];
        len-=2;
    }
    return dest;
}

/* The UTF-8 converter will try to convert non-shortest form, but it will return a
   U_TRUNCATED_CHAR_FOUND error when you try to flush the character. The non-shortest
   form of UTF-8 is illegal in Unicode, but the callback isn't called in that case
   because the character isn't complete yet. This function tries to prevent showing
   non-shortest form in the layout.
   Please see http://www.unicode.org/unicode/reports/tr28/ for details. */
static UBool isShortestUTF8(char *source, int32_t size) {
    if (source[0] == (char)0xc0 || source[0] == (char)0xc1 || source[0] > (char)0xf4
        || (size >= 2
            && ((source[0] == (char)0xE0 && source[1] < (char)0xA0)
            || (source[0] == (char)0xED && source[1] > (char)0x9F)
            || (source[0] == (char)0xF0 && source[1] < (char)0x90)
            || (source[0] == (char)0xF4 && source[1] > (char)0x8F))))
    {
        return FALSE;
    }
    return TRUE;
}

static inline void printUChar(const UChar *targetBuffer, int32_t targetSize, UChar32 uniVal, UErrorCode *status) {
    int32_t utf8Size;
    static char utf8[128];
    static char uniName[256];
    const char *escapedChar = getEscapeChar(uniVal);

    u_charName(uniVal, U_EXTENDED_CHAR_NAME, uniName, sizeof(uniName), status);
    if (!escapedChar) {
        u_strToUTF8(utf8, sizeof(utf8)/sizeof(utf8[0]), &utf8Size, targetBuffer, targetSize, status);
        printf("<td align=\"center\" title=\"%s\"" CELL_WIDTH "><font size=\"+2\">%s</font><br>",
            uniName, utf8);
    }
    else {
        printf("<td align=\"center\" title=\"%s\"" CELL_WIDTH ">%s", uniName, escapedChar);
    }
    printf("<font size=\"-2\">%04X</font></td>\n", uniVal);
}

static inline void printContinue(const char *startBytes, uint8_t currCh, UErrorCode *status) {
    printf("<td class=\"continue\"" CELL_WIDTH "><a href=\"" CGI_NAME "?conv=%s"OPTION_SEP_STR"b=%s%02X"OPTION_SEP_STR"%s#layout\">%02X</a></td>\n",
        gCurrConverter, startBytes, (uint32_t)currCh,
        getStandardOptionsURL(status),
        (uint32_t)currCh);
}

static inline void printNothing() {
    puts("<td class=\"reserved\"" CELL_WIDTH ">"NBSP"</td>");
}

void printCPTable(UConverter *cnv, char *startBytes, UErrorCode *status) {
    int32_t col, row, startBytesLen, maxCharSize;
    char *sourceBuffer;
    char *source;
    char *sourceLimit;
    UChar *targetBuffer;
    UChar *target;
    UChar *targetLimit;
    UChar32 uniVal;
    int8_t cnvMaxCharSize;

    if (U_FAILURE(*status)) {
        return;
    }

    if (!startBytes) {
        puts("<p>ERROR: startBytes == NULL</p>");
        return;
    }

    UConverterType convType = ucnv_getType(cnv);
    switch (convType) {
    case UCNV_SBCS:
//    case UCNV_DBCS:
    case UCNV_MBCS:   /* Be careful. This can also act like DBCS */
    case UCNV_LATIN_1:
    case UCNV_UTF8:
//    case UCNV_UTF16_BigEndian:
//    case UCNV_UTF16_LittleEndian:
//    case UCNV_UTF32_BigEndian:
//    case UCNV_UTF32_LittleEndian:
    case UCNV_EBCDIC_STATEFUL:
//    case UCNV_ISO_2022:
//    case UCNV_LMBCS_1:
//    case UCNV_LMBCS_2: 
//    case UCNV_LMBCS_3:
//    case UCNV_LMBCS_4:
//    case UCNV_LMBCS_5:
//    case UCNV_LMBCS_6:
//    case UCNV_LMBCS_8:
//    case UCNV_LMBCS_11:
//    case UCNV_LMBCS_16:
//    case UCNV_LMBCS_17:
//    case UCNV_LMBCS_18:
//    case UCNV_LMBCS_19:
//    case UCNV_HZ:
//    case UCNV_SCSU:
//    case UCNV_ISCII:
    case UCNV_US_ASCII:
//    case UCNV_UTF7:
//    case UCNV_BOCU1:
//    case UCNV_UTF16:
//    case UCNV_UTF32:
//    case UCNV_CESU8:
//    case UCNV_IMAP_MAILBOX:
        puts("<h2><a name=\"layout\">Codepage Layout</a></h2>");
        break;
    default:
        puts("<p>Codepage layout information is not available for this converter at this time.</p>");
        return;
    }

    /* The string has two digits per byte */
    /* If it's an odd number, ignore the last digit */
    startBytesLen = ((strlen(startBytes)>>1)<<1);
    maxCharSize = startBytesLen/2;
	cnvMaxCharSize = ucnv_getMaxCharSize(cnv);
	if (convType == UCNV_EBCDIC_STATEFUL || convType == UCNV_ISO_2022) {
		/* Add one for the shift */
		cnvMaxCharSize++;
	}
    if (maxCharSize >= cnvMaxCharSize) {
        puts("<p>WARNING: startBytes > maximum number of characters. startBytes is truncated.</p>");
        maxCharSize = cnvMaxCharSize - 1;
        startBytesLen = (cnvMaxCharSize-1) * 2;
    }
    maxCharSize++;
    if (startBytesLen > 0) {
        printf("<p>Currently showing the codepage starting with the bytes %s</p>\n", startBytes);
    }

    ucnv_setToUCallBack(cnv, UCNV_TO_U_CALLBACK_STOP, NULL, NULL, NULL, status);
    sourceBuffer = new char[maxCharSize];
    sourceLimit = sourceBuffer + maxCharSize;
    targetBuffer = new UChar[maxCharSize*UTF_MAX_CHAR_LENGTH];
    targetLimit = targetBuffer + maxCharSize;
    parseAndCopy(sourceBuffer, startBytes, startBytesLen);

    puts("<table width=\"100%\" border=\"1\" cellspacing=\"0\" cellpadding=\"2\" summary=\"This is the layout of the codepage\">");
    printHeader();

    for (row = 0; row < 16; row++) {
        puts("<tr>");
        printf("<th class=\"standard\">%X0</th>\n", row);
        for (col = 0; col < 16; col++) {
            ucnv_resetToUnicode(cnv);
            uint8_t currCh = (uint8_t)(row << 4 | col);
            int32_t targetSize = 0;

            *status = U_ZERO_ERROR;
            sourceBuffer[startBytesLen/2] = currCh;
            targetBuffer[0] = 0;
            source = sourceBuffer;
            target = targetBuffer;
            ucnv_toUnicode(cnv, &target, targetLimit, (const char **)&source, (const char *)sourceLimit, NULL, TRUE, status);
            targetSize = (target - targetBuffer);

            if (convType == UCNV_UTF8 && *status == U_TRUNCATED_CHAR_FOUND && !isShortestUTF8(sourceBuffer, source - sourceBuffer))
            {
                *status = U_ILLEGAL_CHAR_FOUND;
            }
            if (U_SUCCESS(*status) && targetSize > 0) {
                U16_GET(targetBuffer, 0, 0, targetSize, uniVal);
                printUChar(targetBuffer, targetSize, uniVal, status);
            }
            else if (*status == U_TRUNCATED_CHAR_FOUND
				|| (convType == UCNV_EBCDIC_STATEFUL && currCh == UCNV_SO && startBytesLen == 0))
            {
                printContinue(startBytes, currCh, status);
            }
            else {
                // show nothing
                printNothing();
            }
        }
        printf("<th class=\"standard\">%X0</th>\n", row);
        puts("</tr>");
    }

    printHeader();
    puts("</table>");

    delete sourceBuffer;
    delete targetBuffer;

    ucnv_resetToUnicode(cnv);
}
