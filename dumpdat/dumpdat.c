/******************************************************************************
*
*   Copyright (C) 1999-2000, International Business Machines
*   Corporation and others.  All Rights Reserved.
*
*******************************************************************************
*   file name:  dumpdat.c
*   encoding:   US-ASCII
*   tab size:   8 (not used)
*   indentation:4
*
*   created on: 1999nov25
*   created by: Markus W. Scherer  - gencmn
*   modified by: steven r loomis [srl]
*
* 
*/

#ifdef WIN32
#include <ctype.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include "unicode/utypes.h"
#include "unicode/udata.h"

/* prototypes --------------------------------------------------------------- */
void printVersion(const uint8_t  *v);
void printInfo(const UDataInfo *info);


/* -------------------------------------------------------------------------- 

 */
void printVersion(const uint8_t  *v)
{
  int i;
  for(i=0;i<4;i++)
    printf("%3d%c", (int)v[i], (i==3?' ':'.'));

  for(i=0;i<4;i++)
    printf("%c", isprint(v[i])?v[i]:'_');

  printf("\n");
}

void printInfo(const UDataInfo *info)
{
    printf("Size: %d, Endianness: %c, Charset family: %c, ",
	   (int)info->size,
	   "lB?"[info->isBigEndian],
	   "AE?"[info->charsetFamily]);
    
    printf("UChar=%d bytes.\n", info->sizeofUChar);

    printf("dataFormat   =");
    printVersion(info->dataFormat);
    printf("formatVersion=");
    printVersion(info->formatVersion);
    printf("dataVersion  =");
    printVersion(info->dataVersion);
}

static UBool
isAcceptable(void *context,
             const char *type, const char *name,
             const UDataInfo *pInfo) {
  printf("isAcceptable() called on:\n");
  printInfo(pInfo);
  printf("\n");
    return TRUE;
}

extern int
main(int argc, char *argv[]) {
    static uint8_t buffer[4096];
/*    char line[512];*/
/*    char *s;*/
    UErrorCode errorCode=U_ZERO_ERROR;
    UDataInfo  info;
/*    uint32_t i, fileOffset, basenameOffset, length; */
    UDataMemory *data;
    const uint16_t *table=NULL;


    if( (argc<3) || (argc >4) ) {
        fprintf(stderr,
            "usage: %s <type> <name>\n" /* [ path ] */
            "\tPrint the UDATA header for a data type. [try '%s dat unames']\n",
            argv[0],argv[0]);
        return U_ILLEGAL_ARGUMENT_ERROR;
    }

    /* open the data outside the mutex block */
    data=udata_openChoice(NULL, argv[1], argv[2], isAcceptable, NULL, &errorCode);
    if(U_FAILURE(errorCode)) {
        fprintf(stderr, "%s: error %s [%d]\n", argv[0],u_errorName(errorCode),
                (int)errorCode);
        return errorCode;
    }

    table=(const uint16_t *)udata_getMemory(data);

    info.size = sizeof(info);
    udata_getInfo(data, &info);

    printInfo(&info);

    udata_close(data);

    return 0;
}

