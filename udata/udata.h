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
*   file name:  udata.h
*   encoding:   US-ASCII
*   tab size:   8 (not used)
*   indentation:4
*
*   created on: 1999oct25
*   created by: Markus W. Scherer
*/

#ifndef __UDATA_H__
#define __UDATA_H__

#include "utypes.h"

/* utypes.h: values for UDataInfo.charsetFamily */
#define U_ASCII_FAMILY 0
#define U_EBCDIC_FAMILY 1

/* platform.h: character set family of this platform */
#define U_CHARSET_FAMILY 0

/**
 * Information about data memory.
 * This structure may grow in the future, indicated by the
 * <code>size</code> field.
 *
 * <p>The platform data property fields help determine if a data
 * file can be efficiently used on a given machine.
 * The particular fields are of importance only if the data
 * is affected by the properties - if there is integer data
 * with word sizes > 1 byte, char* text, or UChar* text.</p>
 *
 * <p>The <code>dataFormat</code> may be used to identify
 * the kind of data, e.g. a converter table.</p>
 *
 * <p>The <code>formatVersion</code> field should be used to
 * make sure that the format can be interpreted.
 * I may be a good idea to check only for the one or two highest
 * of the version elements to allow the data memory to
 * get more or somewhat rearranged contents, for as long
 * as the using code can still interpret the older contents.</p>
 *
 * <p>The <code>dataVersion</code> field is intended to be a
 * common place to store the source version of the data;
 * for data from the Unicode character database, this could
 * reflect the Unicode version.</p>
 */
typedef struct {
    /** @memo sizeof(UDataInfo) */
    uint16_t size;

    /** @memo unused, set to 0 */
    uint16_t reservedWord;

    /* platform data properties */
    /** @memo 0 for little-endian machine, 1 for big-endian */
    bool_t isBigEndian;

    /** @memo see U_CHARSET_FAMILY values in utypes.h */
    uint8_t charsetFamily;

    /** @memo sizeof(UChar), one of { 1, 2, 4 } */
    uint8_t sizeofUChar;

    /** @memo unused, set to 0 */
    uint8_t reservedByte;

    /** @memo data format identifier */
    uint8_t dataFormat[4];

    /** @memo versions: [0] major [1] minor [2] milli [3] micro */
    uint8_t formatVersion[4];
    uint8_t dataVersion[4];
} UDataInfo;

/* API for reading data -----------------------------------------------------*/

/* forward declaration of the data memory type */
typedef struct UDataMemory UDataMemory;

/* callback function for udata_openChoice(), returns TRUE if the current data memory is acceptable */
typedef bool_t
UDataMemoryIsAcceptable(void *context,
                        const char *type, const char *name,
                        UDataInfo *pInfo);


U_CAPI UDataMemory * U_EXPORT2
udata_open(const char *type, const char *name,
           UErrorCode *pErrorCode);

U_CAPI UDataMemory * U_EXPORT2
udata_openChoice(const char *type, const char *name,
                 UDataMemoryIsAcceptable *isAcceptable, void *context,
                 UErrorCode *pErrorCode);

U_CAPI void U_EXPORT2
udata_close(UDataMemory *pData);

U_CAPI const void * U_EXPORT2
udata_getMemory(UDataMemory *pData);

U_CAPI void U_EXPORT2
udata_getInfo(UDataMemory *pData, UDataInfo *pInfo);

#endif
