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
*   file name:  udata.c
*   encoding:   US-ASCII
*   tab size:   8 (not used)
*   indentation:4
*
*   created on: 1999oct25
*   created by: Markus W. Scherer
*/

#include "utypes.h"
#include "uloc.h"
#include "cmemory.h"
#include "cstring.h"
#include "udata.h"

static UDataMemory *
doOpenChoice(const char *type, const char *name,
             UDataMemoryIsAcceptable *isAcceptable, void *context,
             UErrorCode *pErrorCode);

U_CAPI UDataMemory * U_EXPORT2
udata_open(const char *type, const char *name,
           UErrorCode *pErrorCode) {
    if(pErrorCode==NULL || U_FAILURE(*pErrorCode)) {
        return NULL;
    } else if(name==NULL || *name==0) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        return NULL;
    } else {
        return doOpenChoice(type, name, NULL, NULL, pErrorCode);
    }
}

U_CAPI UDataMemory * U_EXPORT2
udata_openChoice(const char *type, const char *name,
                 UDataMemoryIsAcceptable *isAcceptable, void *context,
                 UErrorCode *pErrorCode) {
    if(pErrorCode==NULL || U_FAILURE(*pErrorCode)) {
        return NULL;
    } else if(name==NULL || *name==0 || isAcceptable==NULL) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        return NULL;
    } else {
        return doOpenChoice(type, name, isAcceptable, context, pErrorCode);
    }
}

/* platform-specific implementation ----------------------------------------- */

/*
 * Most implementations define a MappedData struct
 * and have a MappedData *p; in UDataMemory.
 * They share the source code for some functions.
 * Other implementations need to #undef the following #define.
 * See after the platform-specific code.
 */
#define UDATA_INDIRECT

#if defined(WIN32) /* Win32 implementations --------------------------------- */

#include <windows.h>

typedef struct {
    uint16_t headerSize;
    uint8_t magic1, magic2;
} MappedData;

#   if defined(UDATA_DLL) /* Win32 dll implementation ----------------------- */

struct UDataMemory {
    HINSTANCE lib;
    MappedData *p;
};

static UDataMemory *
doOpenChoice(const char *type, const char *name,
             UDataMemoryIsAcceptable *isAcceptable, void *context,
             UErrorCode *pErrorCode) {
    char buffer[40];
    UDataMemory *pData;
    MappedData *p;
    UDataInfo *info;
    int nameLimit;

    /* allocate the data structure */
    pData=(UDataMemory *)icu_malloc(sizeof(UDataMemory));
    if(pData==NULL) {
        *pErrorCode=U_MEMORY_ALLOCATION_ERROR;
        return NULL;
    }

    /* set up the dll filename */
    icu_strcpy(buffer, name);
    if(type!=NULL && *type!=0) {
        icu_strcat(buffer, "_");
        icu_strcat(buffer, type);
    }
    nameLimit=icu_strlen(buffer);
    icu_strcat(buffer, ".dll");

    /* open the dll */
    pData->lib=LoadLibrary(buffer);
    if(pData->lib==NULL) {
        icu_free(pData);
        *pErrorCode=U_FILE_ACCESS_ERROR;
        return NULL;
    }

    /* get the data pointer */
    buffer[nameLimit]=0;
    pData->p=p=(MappedData *)GetProcAddress(pData->lib, buffer);
    if(p==NULL) {
        FreeLibrary(pData->lib);
        icu_free(pData);
        *pErrorCode=U_FILE_ACCESS_ERROR;
        return NULL;
    }

    /* check magic1 & magic2 */
    if(p->magic1!=0xda || p->magic2!=0x27) {
        FreeLibrary(pData->lib);
        icu_free(pData);
        *pErrorCode=U_INVALID_FORMAT_ERROR;
        return NULL;
    }

    /* check for the byte ordering */
    info=(UDataInfo *)(p+1);
    if(info->isBigEndian!=U_IS_BIG_ENDIAN) {
        FreeLibrary(pData->lib);
        icu_free(pData);
        *pErrorCode=U_INVALID_FORMAT_ERROR;
        return NULL;
    }

    /* is this acceptable? */
    if(isAcceptable!=NULL && !isAcceptable(context, type, name, info)) {
        FreeLibrary(pData->lib);
        icu_free(pData);
        *pErrorCode=U_INVALID_FORMAT_ERROR;
        return NULL;
    }

    return pData;
}

U_CAPI void U_EXPORT2
udata_close(UDataMemory *pData) {
    if(pData!=NULL) {
        FreeLibrary(pData->lib);
        icu_free(pData);
    }
}

#   else /* Win32 memory map implementation --------------------------------- */

struct UDataMemory {
    HANDLE map;
    MappedData *p;
};

static UDataMemory *
doOpenChoice(const char *type, const char *name,
             UDataMemoryIsAcceptable *isAcceptable, void *context,
             UErrorCode *pErrorCode) {
    char buffer[40];
    UDataMemory *pData;
    MappedData *p;
    UDataInfo *info;

    /* allocate the data structure */
    pData=(UDataMemory *)icu_malloc(sizeof(UDataMemory));
    if(pData==NULL) {
        *pErrorCode=U_MEMORY_ALLOCATION_ERROR;
        return NULL;
    }

    /* set up the mapping name and the filename */
    icu_strcpy(buffer, "icu ");
    icu_strcat(buffer, name);
    if(type!=NULL && *type!=0) {
        icu_strcat(buffer, ".");
        icu_strcat(buffer, type);
    }

    /* open the mapping */
    pData->map=OpenFileMapping(FILE_MAP_READ, FALSE, buffer);
    if(pData->map==NULL) {
        /* the mapping has not been created */
        char filename[512];
        const char *path;
        HANDLE file;

        /* open the input file */
        path=uloc_getDataDirectory();
        if(path!=NULL) {
            icu_strcpy(filename, path);
            icu_strcat(filename, buffer+4);
            path=filename;
        } else {
            path=buffer+4;
        }
        file=CreateFile(path, GENERIC_READ, FILE_SHARE_READ, NULL,
                        OPEN_EXISTING,
                        FILE_ATTRIBUTE_NORMAL|FILE_FLAG_RANDOM_ACCESS, NULL);
        if(file==INVALID_HANDLE_VALUE) {
            icu_free(pData);
            *pErrorCode=U_FILE_ACCESS_ERROR;
            return NULL;
        }

        /* create the mapping */
        pData->map=CreateFileMapping(file, NULL, PAGE_READONLY, 0, 0, buffer);
        CloseHandle(file);
        if(pData->map==NULL) {
            icu_free(pData);
            *pErrorCode=U_FILE_ACCESS_ERROR;
            return NULL;
        }
    }

    /* get a view of the mapping */
    pData->p=p=(MappedData *)MapViewOfFile(pData->map, FILE_MAP_READ, 0, 0, 0);
    if(p==NULL) {
        CloseHandle(pData->map);
        icu_free(pData);
        *pErrorCode=U_FILE_ACCESS_ERROR;
        return NULL;
    }

    /* check magic1 & magic2 */
    if(p->magic1!=0xda || p->magic2!=0x27) {
        CloseHandle(pData->map);
        icu_free(pData);
        *pErrorCode=U_INVALID_FORMAT_ERROR;
        return NULL;
    }

    /* check for the byte ordering */
    info=(UDataInfo *)(p+1);
    if(info->isBigEndian!=U_IS_BIG_ENDIAN) {
        CloseHandle(pData->map);
        icu_free(pData);
        *pErrorCode=U_INVALID_FORMAT_ERROR;
        return NULL;
    }

    /* is this acceptable? */
    if(isAcceptable!=NULL && !isAcceptable(context, type, name, info)) {
        CloseHandle(pData->map);
        icu_free(pData);
        *pErrorCode=U_INVALID_FORMAT_ERROR;
        return NULL;
    }

    return pData;
}

U_CAPI void U_EXPORT2
udata_close(UDataMemory *pData) {
    if(pData!=NULL) {
        UnmapViewOfFile(pData->p);
        CloseHandle(pData->map);
        icu_free(pData);
    }
}

#   endif

/* POSIX implementations ---------------------------------------------------- */

#elif defined (LINUX)||defined(POSIX)||defined(SOLARIS)||defined(AIX)||defined(HPUX)

#   if defined(UDATA_DLL) /* POSIX dll implementation ----------------------- */

#       error the UDATA_DLL configuration is not implemented yet

#   else /* POSIX memory map implementation --------------------------------- */

#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/fcntl.h>

typedef struct {
    uint16_t headerSize;
    uint8_t magic1, magic2;
} MappedData;

struct UDataMemory {
  size_t length;
  MappedData *p;
};

static UDataMemory *
doOpenChoice(const char *type, const char *name,
             UDataMemoryIsAcceptable *isAcceptable, void *context,
             UErrorCode *pErrorCode) {
    char buffer[512];
    UDataMemory *pData;
    MappedData *p;
    UDataInfo *info;
    int fd;
    int length;
    const char *dataDir;
    struct stat mystat;
    void *data;

    /* allocate the data structure */
    pData=(UDataMemory *)icu_malloc(sizeof(UDataMemory));
    if(pData==NULL) {
        *pErrorCode=U_MEMORY_ALLOCATION_ERROR;
        return NULL;
    }

    /* set up the mapping name and the filename */
    dataDir = uloc_getDataDirectory();
    icu_strcpy(buffer, dataDir);
    icu_strcat(buffer, name);
    if(type!=NULL && *type!=0) {
        icu_strcat(buffer, ".");
        icu_strcat(buffer, type);
    }

    if(stat(buffer, &mystat))
      {
        icu_free(pData);
        *pErrorCode=U_FILE_ACCESS_ERROR;
        return NULL;
      }

    length = mystat.st_size;

    fd = open(buffer, O_RDONLY);

    if(fd == -1)
      {
        icu_free(pData);
        *pErrorCode=U_FILE_ACCESS_ERROR;
        return NULL;
      }

    data = mmap(0, length, PROT_READ, MAP_SHARED, fd, 0);

    if(data == MAP_FAILED)
      {
        perror("mmap");
        close(fd);
        icu_free(pData);
        *pErrorCode=U_FILE_ACCESS_ERROR;
        return NULL;
      }

    fprintf(stderr, "mmap of %s [%d bytes] succeeded, -> 0x%X\n",
            buffer, length, data); fflush(stderr);
    
    close(fd); /* no longer needed */

    /* get a view of the mapping */
    pData->p =(MappedData *)data;
    p = pData->p;
    pData->length = length;

    /* check magic1 & magic2 */
    if(p->magic1!=0xda || p->magic2!=0x27) {
       udata_close(pData);
        *pErrorCode=U_INVALID_FORMAT_ERROR;
        return NULL;
    }

    /* check for the byte ordering */
    info=(UDataInfo *)(p+1);
    if(info->isBigEndian!=U_IS_BIG_ENDIAN) {
       udata_close(pData);
        *pErrorCode=U_INVALID_FORMAT_ERROR;
        return NULL;
    }

    /* is this acceptable? */
    if(isAcceptable!=NULL && !isAcceptable(context, type, name, info)) {
      udata_close(pData);
        *pErrorCode=U_INVALID_FORMAT_ERROR;
        return NULL;
    }

    return pData;
}

U_CAPI void U_EXPORT2
udata_close(UDataMemory *pData) {
    if(pData!=NULL) {
      if(munmap(pData->p, pData->length) == -1)
        perror("munmap");

        icu_free(pData);
    }
}
#   endif 

#else /* unknown platform - stdio fopen()/fread() implementation ------------ */

#include <stdio.h>

#undef UDATA_INDIRECT

struct UDataMemory {
    uint16_t headerSize;
    uint8_t magic1, magic2;
};

static UDataMemory *
doOpenChoice(const char *type, const char *name,
             UDataMemoryIsAcceptable *isAcceptable, void *context,
             UErrorCode *pErrorCode) {
    FILE *file;
    UDataMemory *pData;
    UDataInfo *info;
    long fileLength;
    char filename[512];
    const char *dataDir;
    
    /* open the input file */
    dataDir=uloc_getDataDirectory();
    if(dataDir!=NULL) {
        icu_strcpy(filename, dataDir);
    } else {
        filename[0]=0;
    }
    icu_strcat(filename, name);
    if(type!=NULL && *type!=0) {
        icu_strcat(filename, ".");
        icu_strcat(filename, type);
    }
    file=fopen(filename, "rb");
    if(file==NULL) {
        *pErrorCode=U_FILE_ACCESS_ERROR;
        return NULL;
    }

    /* get the file length */
    fseek(file, 0, SEEK_END);
    fileLength=ftell(file);
    fseek(file, 0, SEEK_SET);
    if(ferror(file) || fileLength<=20) {
        fclose(file);
        *pErrorCode=U_FILE_ACCESS_ERROR;
        return NULL;
    }

    /* allocate the data structure */
    pData=(UDataMemory *)icu_malloc(fileLength);
    if(pData==NULL) {
        fclose(file);
        *pErrorCode=U_MEMORY_ALLOCATION_ERROR;
        return NULL;
    }

    /* read the file */
    if((size_t)fileLength!=fread(pData, 1, fileLength, file)) {
        icu_free(pData);
        fclose(file);
        *pErrorCode=U_FILE_ACCESS_ERROR;
        return NULL;
    }

    fclose(file);

    /* check magic1 & magic2 */
    if(pData->magic1!=0xda || pData->magic2!=0x27) {
        icu_free(pData);
        *pErrorCode=U_INVALID_FORMAT_ERROR;
        return NULL;
    }

    /* get the header size */
    info=(UDataInfo *)(pData+1);
    if(info->isBigEndian!=U_IS_BIG_ENDIAN) {
        pData->headerSize=pData->headerSize<<8|pData->headerSize>>8;
        info->size=info->size<<8|info->size>>8;
    }

    /* is this acceptable? */
    if(isAcceptable!=NULL && !isAcceptable(context, type, name, info)) {
        icu_free(pData);
        *pErrorCode=U_INVALID_FORMAT_ERROR;
        return NULL;
    }

    return pData;
}

U_CAPI void U_EXPORT2
udata_close(UDataMemory *pData) {
    if(pData!=NULL) {
        icu_free(pData);
    }
}

U_CAPI const void * U_EXPORT2
udata_getMemory(UDataMemory *pData) {
    if(pData!=NULL) {
        return (char *)pData+pData->headerSize;
    } else {
        return NULL;
    }
}

U_CAPI void U_EXPORT2
udata_getInfo(UDataMemory *pData, UDataInfo *pInfo) {
    if(pInfo!=NULL) {
        if(pData!=NULL) {
            UDataInfo *info=(UDataInfo *)(pData+1);
            uint16_t size=pInfo->size;
            if(size>info->size) {
                pInfo->size=info->size;
            }
            icu_memcpy((uint16_t *)pInfo+1, (uint16_t *)info+1, size-2);
        } else {
            pInfo->size=0;
        }
    }
}
#endif

/* common function implementations if UDATA_INDIRECT was not #undef'ed ------ */

#ifdef UDATA_INDIRECT
U_CAPI const void * U_EXPORT2
udata_getMemory(UDataMemory *pData) {
    if(pData!=NULL) {
        return (char *)(pData->p)+pData->p->headerSize;
    } else {
        return NULL;
    }
}

U_CAPI void U_EXPORT2
udata_getInfo(UDataMemory *pData, UDataInfo *pInfo) {
    if(pInfo!=NULL) {
        if(pData!=NULL) {
            UDataInfo *info=(UDataInfo *)(pData->p+1);
            uint16_t size=pInfo->size;
            if(size>info->size) {
                pInfo->size=info->size;
            }
            icu_memcpy((uint16_t *)pInfo+1, (uint16_t *)info+1, size-2);
        } else {
            pInfo->size=0;
        }
    }
}
#endif
