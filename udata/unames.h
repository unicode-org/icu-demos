/*
*******************************************************************************
*                                                                             *
* COPYRIGHT:                                                                  *
*   (C) Copyright International Business Machines Corporation, 1999           *
*   Licensed Material - Program-Property of IBM - All Rights Reserved.        *
*   US Government Users Restricted Rights - Use, duplication, or disclosure   *
*   restricted by GSA ADP Schedule Contract with IBM Corp.                    *
*                                                                             *
*******************************************************************************
*   file name:  unames.h
*   encoding:   US-ASCII
*   tab size:   8 (not used)
*   indentation:4
*
*   created on: 1999oct25
*   created by: Markus W. Scherer
*/

#ifndef __UNAMES_H__
#define __UNAMES_H__

/* public API --------------------------------------------------------------- */

typedef enum UCharNameChoice {
    U_UNICODE_CHAR_NAME,
    U_UNICODE_10_CHAR_NAME,
    UCharNameChoiceCount
} UCharNameChoice;

U_CAPI UTextOffset U_EXPORT2
u_charName(uint32_t code, UCharNameChoice nameChoice,
           char *buffer, UTextOffset bufferLength,
           UErrorCode *pErrorCode);

#endif
