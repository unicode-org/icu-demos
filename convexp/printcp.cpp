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

#include "printcp.h"
#include "unicode/ucnv.h"
#include "unicode/ustring.h"
#include "unicode/uchar.h"

#include <stdio.h>

//#define CELL_WIDTH " width=\"5%\""
#define CELL_WIDTH ""
#define ISO_BEGIN "<div class=\"iso\">"
#define ISO_END "</div>"

static void printHeader() {
    int32_t idx;
    puts("<tr><th></th>");
    for (idx = 0; idx < 16; idx++) {
        printf("<th " CELL_WIDTH ">%02X</th>", idx);
    }
    puts("</tr>");
}

static const char *getEscapeChar(UChar32 uniVal) {
    switch (uniVal) {
    case '<':   return "&lt;<br>";
    case '>':   return "&gt;<br>";
    case '&':   return "&amp;<br>";
    case 0x00:  return ISO_BEGIN "NUL" ISO_END;
    case 0x01:  return ISO_BEGIN "STX" ISO_END;
    case 0x02:  return ISO_BEGIN "SOT" ISO_END;
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
    case 0x80:  return ISO_BEGIN "&nbsp;" ISO_END;
    case 0x81:  return ISO_BEGIN "&nbsp;" ISO_END;
    case 0x82:  return ISO_BEGIN "BPH" ISO_END;
    case 0x83:  return ISO_BEGIN "NBH" ISO_END;
    case 0x84:  return ISO_BEGIN "IND" ISO_END;
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
    case 0x98:  return ISO_BEGIN "SAS" ISO_END;
    case 0x99:  return ISO_BEGIN "&nbsp;" ISO_END;
    case 0x9A:  return ISO_BEGIN "SCI" ISO_END;
    case 0x9B:  return ISO_BEGIN "CSI" ISO_END;
    case 0x9C:  return ISO_BEGIN "ST" ISO_END;
    case 0x9D:  return ISO_BEGIN "OSC" ISO_END;
    case 0x9E:  return ISO_BEGIN "PM" ISO_END;
    case 0x9F:  return ISO_BEGIN "APC" ISO_END;
    }

    return NULL;
}

void printCPTable(const char *convName, UErrorCode *status) {
    int32_t col, row, utf8Size;
    UConverter *cnv = ucnv_open(convName, status);
    char *sourceBuffer;
    char *source;
    char *sourceLimit;
    UChar *targetBuffer;
    UChar *target;
    UChar *targetLimit;
    UChar32 uniVal;
    char utf8[32];
    int8_t maxCharSize;

    if (U_FAILURE(*status)) {
        return;
    }

    UConverterType convType = ucnv_getType(cnv);
    switch (convType) {
    case UCNV_SBCS:
//    case UCNV_DBCS:
//    case UCNV_MBCS:
    case UCNV_LATIN_1:
    case UCNV_US_ASCII:
        puts("<h2>Codepage Layout</h2>");
        break;
    default:
        puts("<p>Codepage layout information is not available for this converter at this time.</p>");
        return;
    }

    ucnv_setToUCallBack(cnv, UCNV_TO_U_CALLBACK_SKIP, NULL, NULL, NULL, status);
    maxCharSize = ucnv_getMaxCharSize(cnv);
    sourceBuffer = new char[maxCharSize];
    sourceLimit = sourceBuffer + maxCharSize;
    targetBuffer = new UChar[maxCharSize*2];
    targetLimit = targetBuffer + maxCharSize;

    puts("<table width=\"100%\" border=\"1\" cellspacing=\"0\" cellpadding=\"2\" summary=\"This is the layout of the codepage\">");
    printHeader();

    for (row = 0; row < 16; row++) {
        puts("<tr>");
        printf("<th>%X0</th>", row);
        for (col = 0; col < 16; col++) {
            *status = U_ZERO_ERROR;
            ucnv_reset(cnv);

            sourceBuffer[0] = (char)(row << 4 | col);
            targetBuffer[0] = 0;
            source = sourceBuffer;
            target = targetBuffer;
            ucnv_toUnicode(cnv, &target, targetLimit, (const char **)&source, (const char *)sourceLimit, NULL, TRUE, status);

            U16_GET_UNSAFE(targetBuffer, 0, uniVal);
            if (U_SUCCESS(*status) && (target - targetBuffer) > 0) {
                const char *escapedChar = getEscapeChar(uniVal);
                if (!escapedChar) {
                    u_strToUTF8(utf8, sizeof(utf8)/sizeof(utf8[0]), &utf8Size, targetBuffer, target - targetBuffer, status);
                    printf("<td align=\"center\" " CELL_WIDTH "><font size=\"+2\">%s</font><br>", utf8);
                }
                else {
                    printf("<td align=\"center\" " CELL_WIDTH ">%s", escapedChar);
                }
                printf("<font size=\"-2\">%04X</font></td>\n", uniVal);
            }
            else {
                // show nothing
                printf("<td class=\"reserved\" " CELL_WIDTH ">&nbsp;</td>");
            }
        }
        puts("</tr>");
    }
    puts("</table>");
    ucnv_close(cnv);
}
