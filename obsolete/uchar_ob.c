/*
*******************************************************************************
*
*   Copyright (C) 1996-2003, International Business Machines
*   Corporation and others.  All Rights Reserved.
*
*******************************************************************************
*   file name:  uchar_ob.c
*   encoding:   US-ASCII
*   tab size:   8 (not used)
*   indentation:4
*
*   created on: 2003apr30
*   created by: Markus W. Scherer
*
*   This file contains obsolete definitions from uchar.c.
*/

#include "unicode/utypes.h"
#include "unicode/uchar.h"
#include "unicode/obsolete.h"
#include "unicode/uchar_ob.h"

/* static data tables ------------------------------------------------------- */

static const UChar cellWidthRanges[] =
{
    0x0000, /* general scripts area*/
    0x1100, /* combining Hangul choseong*/
    0x1160, /* combining Hangul jungseong and jongseong*/
    0x1e00, /* Latin Extended Additional, Greek Extended*/
    0x2000, /* symbols and punctuation*/
    0x3000, /* CJK phonetics & symbols, CJK ideographs, Hangul syllables*/
    0xd800, /* surrogates, private use*/
    0xf900, /* CJK compatibility ideographs*/
    0xfb00, /* alphabetic presentation forms, Arabic presentations forms A, combining half marks*/
    0xfe30, /* CJK compatibility forms, small form variants*/
    0xfe70, /* Arabic presentation forms B*/
    0xff00, /* fullwidth ASCII*/
    0xff60, /* halfwidth, CJK punctuation, Katakana, Hangul Jamo*/
    0xffe0, /* fullwidth punctuation and currency signs*/
    0xffe8, /* halfwidth forms, arrows, and shapes*/
    0xfff0  /* specials*/
};

static const UChar cellWidthValues[] =
{
    U_HALF_WIDTH,    /* general scripts area*/
    U_FULL_WIDTH,    /* combining Hangul choseong*/
    U_ZERO_WIDTH,    /* combining Hangul jungseong and jongseong*/
    U_HALF_WIDTH,    /* Latin extended aAdditional, Greek extended*/
    U_NEUTRAL_WIDTH, /* symbols and punctuation*/
    U_FULL_WIDTH,    /* CJK phonetics & symbols, CJK ideographs, Hangul syllables*/
    U_NEUTRAL_WIDTH, /* surrogates, private use*/
    U_FULL_WIDTH,    /* CJK compatibility ideographs*/
    U_HALF_WIDTH,    /* alphabetic presentation forms, Arabic presentations forms A, combining half marks*/
    U_FULL_WIDTH,    /* CJK compatibility forms, small form variants*/
    U_HALF_WIDTH,    /* Arabic presentation forms B*/
    U_FULL_WIDTH,    /* fullwidth ASCII*/
    U_HALF_WIDTH,    /* halfwidth CJK punctuation, Katakana, Hangul Jamo*/
    U_FULL_WIDTH,    /* fullwidth punctuation and currency signs*/
    U_HALF_WIDTH,    /* halfwidth forms, arrows, and shapes*/
    U_ZERO_WIDTH     /* specials*/
};

#define NUM_CELL_WIDTH_VALUES (sizeof(cellWidthValues)/sizeof(cellWidthValues[0]))
/* Gets table cell width of the Unicode character.*/
U_CAPI uint16_t U_EXPORT2
u_charCellWidth(UChar32 ch)
{
    int16_t i;
    int32_t type = u_charType(ch);

    /* surrogate support is still incomplete */
    if((uint32_t)ch>0xffff) {
        return U_ZERO_WIDTH;
    }

    /* these Unicode character types are scattered throughout the Unicode range, so
     special-case for them*/
    switch (type) {
        case U_UNASSIGNED:
        case U_NON_SPACING_MARK:
        case U_ENCLOSING_MARK:
        case U_LINE_SEPARATOR:
        case U_PARAGRAPH_SEPARATOR:
        case U_CONTROL_CHAR:
        case U_FORMAT_CHAR:
            return U_ZERO_WIDTH;

        default:
            /* for all remaining characters, find out which Unicode range they belong to using
               the table above, and then look up the appropriate return value in that table*/
            for (i = 0; i < (int16_t)NUM_CELL_WIDTH_VALUES; ++i) {
                if (ch < cellWidthRanges[i]) {
                    break;
                }
            }
            --i;
            return cellWidthValues[i];
    }
}

/* from UnicodeString ------------------------------------------------------- */
#if 0

/* from unistr.h */
  /**
   * Returns the number of display cells occupied by the range
   * [<TT>start</TT>, <TT>length</TT>).
   * This function is designed for Asian text and properly takes into account
   * halfwidth and fullwidth variants of various CJK characters and the 
   * combining behavior of the Hangul Jamo characters (with some limitations;
   * see documentation for Unicode::getCellWidth()).<BR>
   * In order to avoid dealing with fractions, this function can either be
   * construed to return twice the actual number of display cells or to 
   * treat a "cell" as the width of a halfwidth character rather than the
   * width of a fullwidth character.
   * @param start the start of the range
   * @param length the number of characters to measure
   * @param asian The <TT>asian</TT> parameter controls whether characters
   * considered NEUTRAL by the Unicode class are treated as halfwidth or 
   * fullwidth here.  If you set <TT>asian</TT> to FALSE, neutrals are 
   * treated as halfwidth, and this function returns a close approximation
   * of how many Latin display cells the text will take up in a monospaced
   * font.
   * @return the number of display cells occupied by the specified substring.
   * @obsolete ICU 2.6. Use UCHAR_EAST_ASIAN_WIDTH instead since this API will be removed in that release.
   */
  int32_t numDisplayCells(int32_t start = 0,
              int32_t length = INT32_MAX,
              UBool asian = TRUE) const;

/* from unistr.cpp */
int32_t
UnicodeString::numDisplayCells( int32_t start,
                int32_t length,
                UBool asian) const
{
  // pin indices to legal values
  pinIndices(start, length);

  UChar32 c;
  int32_t result = 0;
  int32_t limit = start + length;

  while(start < limit) {
    UTF_NEXT_CHAR(fArray, start, limit, c);
    switch(u_charCellWidth(c)) {
    case U_ZERO_WIDTH:
      break;

    case U_HALF_WIDTH:
      result += 1;
      break;

    case U_FULL_WIDTH:
      result += 2;
      break;

    case U_NEUTRAL_WIDTH:
      result += (asian ? 2 : 1);
      break;
    }
  }

  return result;
}

#endif
