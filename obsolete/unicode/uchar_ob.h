/*
*******************************************************************************
*
*   Copyright (C) 1997-2003, International Business Machines
*   Corporation and others.  All Rights Reserved.
*
*******************************************************************************
*   file name:  uchar_ob.h
*   encoding:   US-ASCII
*   tab size:   8 (not used)
*   indentation:4
*
*   created on: 2003mar02
*   created by: Markus W. Scherer
*
*   This file contains obsolete definitions from uchar.h.
*/

#ifndef __UCHAR_OB_H__
#define __UCHAR_OB_H__

#include "unicode/utypes.h"
#include "unicode/uchar.h"
#include "unicode/obsolete.h"

/* These UBlockCode constants were renamed to ones with a UBLOCK_ prefix. */
enum {
    /** @obsolete ICU 2.4. Use the enum that begins with UBLOCK instead since this API will be removed in that release. */
    U_BASIC_LATIN = 1,
    /** @obsolete ICU 2.4. Use the enum that begins with UBLOCK instead since this API will be removed in that release. */
    U_LATIN_1_SUPPLEMENT=2,
    /** @obsolete ICU 2.4. Use the enum that begins with UBLOCK instead since this API will be removed in that release. */
    U_LATIN_EXTENDED_A=3,
    /** @obsolete ICU 2.4. Use the enum that begins with UBLOCK instead since this API will be removed in that release. */
    U_LATIN_EXTENDED_B=4,
    /** @obsolete ICU 2.4. Use the enum that begins with UBLOCK instead since this API will be removed in that release. */
    U_IPA_EXTENSIONS=5,
    /** @obsolete ICU 2.4. Use the enum that begins with UBLOCK instead since this API will be removed in that release. */
    U_SPACING_MODIFIER_LETTERS=6,
    /** @obsolete ICU 2.4. Use the enum that begins with UBLOCK instead since this API will be removed in that release. */
    U_COMBINING_DIACRITICAL_MARKS=7,
    /** @obsolete ICU 2.4. Use the enum that begins with UBLOCK instead since this API will be removed in that release. */
    U_GREEK=8,
    /** @obsolete ICU 2.4. Use the enum that begins with UBLOCK instead since this API will be removed in that release. */
    U_CYRILLIC=9,
    /** @obsolete ICU 2.4. Use the enum that begins with UBLOCK instead since this API will be removed in that release. */
    U_ARMENIAN=10,
    /** @obsolete ICU 2.4. Use the enum that begins with UBLOCK instead since this API will be removed in that release. */
    U_HEBREW=11,
    /** @obsolete ICU 2.4. Use the enum that begins with UBLOCK instead since this API will be removed in that release. */
    U_ARABIC=12,
    /** @obsolete ICU 2.4. Use the enum that begins with UBLOCK instead since this API will be removed in that release. */
    U_SYRIAC=13,
    /** @obsolete ICU 2.4. Use the enum that begins with UBLOCK instead since this API will be removed in that release. */
    U_THAANA=14,
    /** @obsolete ICU 2.4. Use the enum that begins with UBLOCK instead since this API will be removed in that release. */
    U_DEVANAGARI=15,
    /** @obsolete ICU 2.4. Use the enum that begins with UBLOCK instead since this API will be removed in that release. */
    U_BENGALI=16,
    /** @obsolete ICU 2.4. Use the enum that begins with UBLOCK instead since this API will be removed in that release. */
    U_GURMUKHI=17,
    /** @obsolete ICU 2.4. Use the enum that begins with UBLOCK instead since this API will be removed in that release. */
    U_GUJARATI=18,
    /** @obsolete ICU 2.4. Use the enum that begins with UBLOCK instead since this API will be removed in that release. */
    U_ORIYA=19,
    /** @obsolete ICU 2.4. Use the enum that begins with UBLOCK instead since this API will be removed in that release. */
    U_TAMIL=20,
    /** @obsolete ICU 2.4. Use the enum that begins with UBLOCK instead since this API will be removed in that release. */
    U_TELUGU=21,
    /** @obsolete ICU 2.4. Use the enum that begins with UBLOCK instead since this API will be removed in that release. */
    U_KANNADA=22,
    /** @obsolete ICU 2.4. Use the enum that begins with UBLOCK instead since this API will be removed in that release. */
    U_MALAYALAM=23,
    /** @obsolete ICU 2.4. Use the enum that begins with UBLOCK instead since this API will be removed in that release. */
    U_SINHALA=24,
    /** @obsolete ICU 2.4. Use the enum that begins with UBLOCK instead since this API will be removed in that release. */
    U_THAI=25,
    /** @obsolete ICU 2.4. Use the enum that begins with UBLOCK instead since this API will be removed in that release. */
    U_LAO=26,
    /** @obsolete ICU 2.4. Use the enum that begins with UBLOCK instead since this API will be removed in that release. */
    U_TIBETAN=27,
    /** @obsolete ICU 2.4. Use the enum that begins with UBLOCK instead since this API will be removed in that release. */
    U_MYANMAR=28,
    /** @obsolete ICU 2.4. Use the enum that begins with UBLOCK instead since this API will be removed in that release. */
    U_GEORGIAN=29,
    /** @obsolete ICU 2.4. Use the enum that begins with UBLOCK instead since this API will be removed in that release. */
    U_HANGUL_JAMO=30,
    /** @obsolete ICU 2.4. Use the enum that begins with UBLOCK instead since this API will be removed in that release. */
    U_ETHIOPIC=31,
    /** @obsolete ICU 2.4. Use the enum that begins with UBLOCK instead since this API will be removed in that release. */
    U_CHEROKEE=32,
    /** @obsolete ICU 2.4. Use the enum that begins with UBLOCK instead since this API will be removed in that release. */
    U_UNIFIED_CANADIAN_ABORIGINAL_SYLLABICS=33,
    /** @obsolete ICU 2.4. Use the enum that begins with UBLOCK instead since this API will be removed in that release. */
    U_OGHAM=34,
    /** @obsolete ICU 2.4. Use the enum that begins with UBLOCK instead since this API will be removed in that release. */
    U_RUNIC=35,
    /** @obsolete ICU 2.4. Use the enum that begins with UBLOCK instead since this API will be removed in that release. */
    U_KHMER=36,
    /** @obsolete ICU 2.4. Use the enum that begins with UBLOCK instead since this API will be removed in that release. */
    U_MONGOLIAN=37,
    /** @obsolete ICU 2.4. Use the enum that begins with UBLOCK instead since this API will be removed in that release. */
    U_LATIN_EXTENDED_ADDITIONAL=38,
    /** @obsolete ICU 2.4. Use the enum that begins with UBLOCK instead since this API will be removed in that release. */
    U_GREEK_EXTENDED=39,
    /** @obsolete ICU 2.4. Use the enum that begins with UBLOCK instead since this API will be removed in that release. */
    U_GENERAL_PUNCTUATION=40,
    /** @obsolete ICU 2.4. Use the enum that begins with UBLOCK instead since this API will be removed in that release. */
    U_SUPERSCRIPTS_AND_SUBSCRIPTS=41,
    /** @obsolete ICU 2.4. Use the enum that begins with UBLOCK instead since this API will be removed in that release. */
    U_CURRENCY_SYMBOLS=42,
    /** @obsolete ICU 2.4. Use the enum that begins with UBLOCK instead since this API will be removed in that release. */
    U_COMBINING_MARKS_FOR_SYMBOLS=43,
    /** @obsolete ICU 2.4. Use the enum that begins with UBLOCK instead since this API will be removed in that release. */
    U_LETTERLIKE_SYMBOLS=44,
    /** @obsolete ICU 2.4. Use the enum that begins with UBLOCK instead since this API will be removed in that release. */
    U_NUMBER_FORMS=45,
    /** @obsolete ICU 2.4. Use the enum that begins with UBLOCK instead since this API will be removed in that release. */
    U_ARROWS=46,
    /** @obsolete ICU 2.4. Use the enum that begins with UBLOCK instead since this API will be removed in that release. */
    U_MATHEMATICAL_OPERATORS=47,
    /** @obsolete ICU 2.4. Use the enum that begins with UBLOCK instead since this API will be removed in that release. */
    U_MISCELLANEOUS_TECHNICAL=48,
    /** @obsolete ICU 2.4. Use the enum that begins with UBLOCK instead since this API will be removed in that release. */
    U_CONTROL_PICTURES=49,
    /** @obsolete ICU 2.4. Use the enum that begins with UBLOCK instead since this API will be removed in that release. */
    U_OPTICAL_CHARACTER_RECOGNITION=50,
    /** @obsolete ICU 2.4. Use the enum that begins with UBLOCK instead since this API will be removed in that release. */
    U_ENCLOSED_ALPHANUMERICS=51,
    /** @obsolete ICU 2.4. Use the enum that begins with UBLOCK instead since this API will be removed in that release. */
    U_BOX_DRAWING=52,
    /** @obsolete ICU 2.4. Use the enum that begins with UBLOCK instead since this API will be removed in that release. */
    U_BLOCK_ELEMENTS=53,
    /** @obsolete ICU 2.4. Use the enum that begins with UBLOCK instead since this API will be removed in that release. */
    U_GEOMETRIC_SHAPES=54,
    /** @obsolete ICU 2.4. Use the enum that begins with UBLOCK instead since this API will be removed in that release. */
    U_MISCELLANEOUS_SYMBOLS=55,
    /** @obsolete ICU 2.4. Use the enum that begins with UBLOCK instead since this API will be removed in that release. */
    U_DINGBATS=56,
    /** @obsolete ICU 2.4. Use the enum that begins with UBLOCK instead since this API will be removed in that release. */
    U_BRAILLE_PATTERNS=57,
    /** @obsolete ICU 2.4. Use the enum that begins with UBLOCK instead since this API will be removed in that release. */
    U_CJK_RADICALS_SUPPLEMENT=58,
    /** @obsolete ICU 2.4. Use the enum that begins with UBLOCK instead since this API will be removed in that release. */
    U_KANGXI_RADICALS=59,
    /** @obsolete ICU 2.4. Use the enum that begins with UBLOCK instead since this API will be removed in that release. */
    U_IDEOGRAPHIC_DESCRIPTION_CHARACTERS=60,
    /** @obsolete ICU 2.4. Use the enum that begins with UBLOCK instead since this API will be removed in that release. */
    U_CJK_SYMBOLS_AND_PUNCTUATION=61,
    /** @obsolete ICU 2.4. Use the enum that begins with UBLOCK instead since this API will be removed in that release. */
    U_HIRAGANA=62,
    /** @obsolete ICU 2.4. Use the enum that begins with UBLOCK instead since this API will be removed in that release. */
    U_KATAKANA=63,
    /** @obsolete ICU 2.4. Use the enum that begins with UBLOCK instead since this API will be removed in that release. */
    U_BOPOMOFO=64,
    /** @obsolete ICU 2.4. Use the enum that begins with UBLOCK instead since this API will be removed in that release. */
    U_HANGUL_COMPATIBILITY_JAMO=65,
    /** @obsolete ICU 2.4. Use the enum that begins with UBLOCK instead since this API will be removed in that release. */
    U_KANBUN=66,
    /** @obsolete ICU 2.4. Use the enum that begins with UBLOCK instead since this API will be removed in that release. */
    U_BOPOMOFO_EXTENDED=67,
    /** @obsolete ICU 2.4. Use the enum that begins with UBLOCK instead since this API will be removed in that release. */
    U_ENCLOSED_CJK_LETTERS_AND_MONTHS=68,
    /** @obsolete ICU 2.4. Use the enum that begins with UBLOCK instead since this API will be removed in that release. */
    U_CJK_COMPATIBILITY=69,
    /** @obsolete ICU 2.4. Use the enum that begins with UBLOCK instead since this API will be removed in that release. */
    U_CJK_UNIFIED_IDEOGRAPHS_EXTENSION_A=70,
    /** @obsolete ICU 2.4. Use the enum that begins with UBLOCK instead since this API will be removed in that release. */
    U_CJK_UNIFIED_IDEOGRAPHS=71,
    /** @obsolete ICU 2.4. Use the enum that begins with UBLOCK instead since this API will be removed in that release. */
    U_YI_SYLLABLES=72,
    /** @obsolete ICU 2.4. Use the enum that begins with UBLOCK instead since this API will be removed in that release. */
    U_YI_RADICALS=73,
    /** @obsolete ICU 2.4. Use the enum that begins with UBLOCK instead since this API will be removed in that release. */
    U_HANGUL_SYLLABLES=74,
    /** @obsolete ICU 2.4. Use the enum that begins with UBLOCK instead since this API will be removed in that release. */
    U_HIGH_SURROGATES=75,
    /** @obsolete ICU 2.4. Use the enum that begins with UBLOCK instead since this API will be removed in that release. */
    U_HIGH_PRIVATE_USE_SURROGATES=76,
    /** @obsolete ICU 2.4. Use the enum that begins with UBLOCK instead since this API will be removed in that release. */
    U_LOW_SURROGATES=77,
    /** @obsolete ICU 2.4. Use the enum that begins with UBLOCK instead since this API will be removed in that release. */
    U_PRIVATE_USE_AREA=78,
    /** @obsolete ICU 2.4. Use the enum that begins with UBLOCK instead since this API will be removed in that release. */
    U_CJK_COMPATIBILITY_IDEOGRAPHS=79,
    /** @obsolete ICU 2.4. Use the enum that begins with UBLOCK instead since this API will be removed in that release. */
    U_ALPHABETIC_PRESENTATION_FORMS=80,
    /** @obsolete ICU 2.4. Use the enum that begins with UBLOCK instead since this API will be removed in that release. */
    U_ARABIC_PRESENTATION_FORMS_A=81,
    /** @obsolete ICU 2.4. Use the enum that begins with UBLOCK instead since this API will be removed in that release. */
    U_COMBINING_HALF_MARKS=82,
    /** @obsolete ICU 2.4. Use the enum that begins with UBLOCK instead since this API will be removed in that release. */
    U_CJK_COMPATIBILITY_FORMS=83,
    /** @obsolete ICU 2.4. Use the enum that begins with UBLOCK instead since this API will be removed in that release. */
    U_SMALL_FORM_VARIANTS=84,
    /** @obsolete ICU 2.4. Use the enum that begins with UBLOCK instead since this API will be removed in that release. */
    U_ARABIC_PRESENTATION_FORMS_B=85,
    /** @obsolete ICU 2.4. Use the enum that begins with UBLOCK instead since this API will be removed in that release. */
    U_SPECIALS=86,
    /** @obsolete ICU 2.4. Use the enum that begins with UBLOCK instead since this API will be removed in that release. */
    U_HALFWIDTH_AND_FULLWIDTH_FORMS=87,
    /** @obsolete ICU 2.4. Use the enum that begins with UBLOCK instead since this API will be removed in that release. */
    U_SCRIPT_COUNT=UBLOCK_COUNT,
    /** @obsolete ICU 2.4. Use the enum that begins with UBLOCK instead since this API will be removed in that release. */
    U_CHAR_SCRIPT_COUNT =UBLOCK_COUNT,
    /** @obsolete ICU 2.4. Use the enum that begins with UBLOCK instead since this API will be removed in that release. */
    U_NO_SCRIPT = UBLOCK_COUNT
};

/**
 * @obsolete ICU 2.4. Use ublock_getCode() instead since this API will be removed in that release.
 */
#define u_charScript ublock_getCode
/** @obsolete ICU 2.4. Use UBlockCode instead since this API will be removed in that release. */
typedef UBlockCode UCharScript;

/**
 * Values returned by the u_getCellWidth() function.
 * @obsolete ICU 2.6. Use UCHAR_EAST_ASIAN_WIDTH instead since this API will be removed in that release.
 */
enum UCellWidth
{
    /** @obsolete ICU 2.6. Use UCHAR_EAST_ASIAN_WIDTH instead since this API will be removed in that release. */
    U_ZERO_WIDTH              = 0,
    /** @obsolete ICU 2.6. Use UCHAR_EAST_ASIAN_WIDTH instead since this API will be removed in that release. */
    U_HALF_WIDTH              = 1,
    /** @obsolete ICU 2.6. Use UCHAR_EAST_ASIAN_WIDTH instead since this API will be removed in that release. */
    U_FULL_WIDTH              = 2,
    /** @obsolete ICU 2.6. Use UCHAR_EAST_ASIAN_WIDTH instead since this API will be removed in that release. */
    U_NEUTRAL_WIDTH           = 3,
    /** @obsolete ICU 2.6. Use UCHAR_EAST_ASIAN_WIDTH instead since this API will be removed in that release. */
    U_CELL_WIDTH_COUNT
};

typedef enum UCellWidth UCellWidth; /**< C typedef for struct UCellWidth. @obsolete ICU 2.6. Use UCHAR_EAST_ASIAN_WIDTH instead since this API will be removed in that release. */

/**
 * Obsolete because the "cell width" functions and implementation are
 * out of date compared with Unicode Standard Annex #11.
 * Use u_getIntPropertyValue with UCHAR_EAST_ASIAN_WIDTH.
 * See http://www.unicode.org/reports/tr11/
 *
 * Returns a value indicating the display-cell width of the character
 * when used in Asian text, according to the Unicode standard (see p. 6-130
 * of The Unicode Standard, Version 2.0).  The results for various characters
 * are as follows:
 * <P>
 *      U_ZERO_WIDTH: Characters which are considered to take up no display-cell space:
 *          control characters
 *          format characters
 *          line and paragraph separators
 *          non-spacing marks
 *          combining Hangul jungseong
 *          combining Hangul jongseong
 *          unassigned Unicode values
 * <P>
 *      U_HALF_WIDTH: Characters which take up half a cell in standard Asian text:
 *          all characters in the General Scripts Area except combining Hangul choseong
 *              and the characters called out specifically above as ZERO_WIDTH
 *          alphabetic and Arabic presentation forms
 *          halfwidth CJK punctuation
 *          halfwidth Katakana
 *          halfwidth Hangul Jamo
 *          halfwidth forms, arrows, and shapes
 * <P>
 *      U_FULL_WIDTH:  Characters which take up a full cell in standard Asian text:
 *          combining Hangul choseong
 *          all characters in the CJK Phonetics and Symbols Area
 *          all characters in the CJK Ideographs Area
 *          all characters in the Hangul Syllables Area
 *          CJK compatibility ideographs
 *          CJK compatibility forms
 *          small form variants
 *          fullwidth ASCII
 *          fullwidth punctuation and currency signs
 * <P>
 *      U_NEUTRAL_WIDTH:  Characters whose cell width is context-dependent:
 *          all characters in the Symbols Area, except those specifically called out above
 *          all characters in the Surrogates Area
 *          all charcaters in the Private Use Area
 * <P>
 * For Korean text, this algorithm should work properly with properly normalized Korean
 * text.  Precomposed Hangul syllables and non-combining jamo are all considered full-
 * width characters.  For combining jamo, we treat we treat choseong (initial consonants)
 * as double-width characters and junseong (vowels) and jongseong (final consonants)
 * as non-spacing marks.  This will work right in text that uses the precomposed
 * choseong characters instead of teo choseong characters in a row, and which uses the
 * choseong filler character at the beginning of syllables that don't have an initial
 * consonant.  The results may be slightly off with Korean text following different
 * conventions.
 * @param c The character to be tested
 * @return a value indicating the display-cell width of the character when used in Asian text
 * @obsolete ICU 2.6. Use UCHAR_EAST_ASIAN_WIDTH instead since this API will be removed in that release.
 */
U_CAPI uint16_t U_EXPORT2
u_charCellWidth(UChar32 c);

#endif
