#ifndef __UDATA_H__
#define __UDATA_H__

#include "utypes.h"

/* utypes.h: values for UDataInfo.charsetFamily */
#define U_ASCII_FAMILY 0
#define U_EBCDIC_FAMILY 1

/* platform.h: character set family of this platform */
#define U_CHARSET_FAMILY 0

/* information about data memory; this structure may grow in the future */
typedef struct {
    uint16_t size;              /* sizeof(UDataInfo) */
    bool_t isBigEndian;
    uint8_t charsetFamily;
    uint8_t dataFormat[4];
    uint8_t formatVersion[4];   /* versions: [0] major [1] minor [2] milli [3] micro */
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
