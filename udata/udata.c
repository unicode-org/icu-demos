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
#include "umutex.h"
#include "cmemory.h"
#include "cstring.h"
#include "udata.h"

#if !defined(UDATA_DLL) && !defined(UDATA_MAP)
#   define UDATA_DLL
#endif

#define COMMON_DATA_NAME "icudata"
#define DATA_TYPE "dat"

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

typedef HINSTANCE Library;

#define GET_ENTRY(lib, entryName) (MappedData *)GetProcAddress(lib, entry)

static Library
loadLibrary(const char *name) {
    char buffer[40];

    /* set up the dll filename */
    icu_strcpy(buffer, name);
    icu_strcat(buffer, ".dll");
    return LoadLibrary(buffer);
}

#define NO_LIBRARY NULL
#define IS_LIBRARY(lib) ((lib)!=NULL)
#define UNLOAD_LIBRARY(lib) FreeLibrary(lib)

#   else /* Win32 memory map implementation --------------------------------- */

struct UDataMemory {
    HANDLE map;
    MappedData *p;
};

typedef UDataMemory *Library;

#define GET_ENTRY(lib, entryName) getCommonMapData(lib, entry)

static UDataMemory *
doOpenMap(const char *type, const char *name,
          UErrorCode *pErrorCode) {
    char buffer[40];
    UDataMemory *pData;
    MappedData *p;

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

    return pData;
}

U_CAPI void U_EXPORT2
udata_close(UDataMemory *pData) {
    if(pData!=NULL) {
        if(pData->map!=NULL) {
            UnmapViewOfFile(pData->p);
            CloseHandle(pData->map);
        }
        icu_free(pData);
    }
}

#   endif

/* POSIX implementations ---------------------------------------------------- */

#elif defined (LINUX)||defined(POSIX)||defined(SOLARIS)||defined(AIX)||defined(HPUX)

typedef struct {
    uint16_t headerSize;
    uint8_t magic1, magic2;
} MappedData;

#if defined(UDATA_DEBUG)
#include <stdio.h>
#endif

#   if defined(UDATA_DLL) /* POSIX dll implementation ----------------------- */

struct UDataMemory {
    void *lib;
    MappedData *p;
};

#ifndef UDATA_SO_SUFFIX
#   error Please define UDATA_SO_SUFFIX to the shlib suffix (i.e. '.so' )
#endif

/* Do we need to check the platform here? */
#include <dlfcn.h>

typedef void *Library;

#define GET_ENTRY(lib, entryName) (MappedData *)dlsym(lib, entry)

static Library
loadLibrary(const char *name) {
    char buffer[40];

    /* set up the dll filename */
    icu_strcpy(buffer, "lib");
    icu_strcat(buffer, name);
    icu_strcat(buffer, UDATA_SO_SUFFIX);
    return dlopen("lib" COMMON_DATA_NAME UDATA_SO_SUFFIX, RTLD_LAZY|RTLD_GLOBAL);
}

#define NO_LIBRARY NULL
#define IS_LIBRARY(lib) ((lib)!=NULL)
#define UNLOAD_LIBRARY(lib) dlclose(lib)

#   else /* POSIX memory map implementation --------------------------------- */

#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/fcntl.h>

struct UDataMemory {
  size_t length;
  MappedData *p;
};

typedef UDataMemory *Library;

#define GET_ENTRY(lib, entryName) getCommonMapData(lib, entry)

static UDataMemory *
doOpenMap(const char *type, const char *name,
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
    if(dataDir != NULL) {
        icu_strcpy(buffer, dataDir);
    } else {
        buffer[0] = 0;
    }
    icu_strcat(buffer, name);
    if(type!=NULL && *type!=0) {
        icu_strcat(buffer, ".");
        icu_strcat(buffer, type);
    }

    /* determine the length of the file */
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

#ifdef UDATA_DEBUG
    fprintf(stderr, "mmap of %s [%d bytes] succeeded, -> 0x%X\n",
            buffer, length, data); fflush(stderr);
#endif
    
    close(fd); /* no longer needed */

    /* get a view of the mapping */
    pData->p =(MappedData *)data;
    p = pData->p;
    pData->length = length;

    return pData;
}

U_CAPI void U_EXPORT2
udata_close(UDataMemory *pData) {
    if(pData!=NULL) {
        if(pData->length!=0 && munmap(pData->p, pData->length)==-1) {
            perror("munmap");
        }
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

static MappedData *
getChoice(Library lib, const char *entry,
          const char *type, const char *name,
          UDataMemoryIsAcceptable *isAcceptable, void *context,
          UErrorCode *pErrorCode);

#   ifdef UDATA_DLL

/* common DLL implementations */

static Library commonLib=NULL;

static UDataMemory *
doOpenChoice(const char *type, const char *name,
             UDataMemoryIsAcceptable *isAcceptable, void *context,
             UErrorCode *pErrorCode) {
    char buffer[40];
    UDataMemory *pData;
    MappedData *p;
    UErrorCode errorCode=U_ZERO_ERROR;

    if(!IS_LIBRARY(commonLib)) {
        /* load the common library if necessary - outside the mutex block */
        Library lib=loadLibrary(COMMON_DATA_NAME);
        if(IS_LIBRARY(lib)) {
            /* in the mutex block, set the common library for this process */
            umtx_lock(NULL);
            if(!IS_LIBRARY(commonLib)) {
                commonLib=lib;
                lib=NO_LIBRARY;
            }
            umtx_unlock(NULL);

            /* if a different thread set it first, then free the extra library instance */
            if(IS_LIBRARY(lib)) {
                UNLOAD_LIBRARY(lib);
            }
        }
    }

    /* allocate the data structure */
    pData=(UDataMemory *)icu_malloc(sizeof(UDataMemory));
    if(pData==NULL) {
        *pErrorCode=U_MEMORY_ALLOCATION_ERROR;
        return NULL;
    }

    /* set up the entry point name */
    icu_strcpy(buffer, name);
    if(type!=NULL && *type!=0) {
        icu_strcat(buffer, "_");
        icu_strcat(buffer, type);
    }

    /* look for it in the common library */
    if(commonLib!=NULL) {
        p=getChoice(commonLib, buffer, type, name, isAcceptable, context, &errorCode);
        if(p!=NULL) {
            pData->lib=NULL;
            pData->p=p;
            return pData;
        }
    }

    /* try to open a specific dll */
    pData->lib=loadLibrary(buffer);
    if(pData->lib==NULL) {
        icu_free(pData);
        *pErrorCode=U_FILE_ACCESS_ERROR;
        return NULL;
    }

    p=getChoice(pData->lib, buffer, type, name, isAcceptable, context, pErrorCode);
    if(p!=NULL) {
        pData->p=p;
        return pData;
    } else {
        UNLOAD_LIBRARY(pData->lib);
        icu_free(pData);
        return NULL;
    }
}

U_CAPI void U_EXPORT2
udata_close(UDataMemory *pData) {
    if(pData!=NULL) {
        if(IS_LIBRARY(pData->lib)) {
            UNLOAD_LIBRARY(pData->lib);
        }
        icu_free(pData);
    }
}

#   else

/* common implementation of use of common memory map */

static UDataMemory *commonData=NULL;

static bool_t
isCommonDataAcceptable(void *context,
                       const char *type, const char *name,
                       UDataInfo *pInfo);

static UDataMemory *
doOpenChoice(const char *type, const char *name,
             UDataMemoryIsAcceptable *isAcceptable, void *context,
             UErrorCode *pErrorCode) {
    UDataMemory *pData;
    MappedData *p;
    UErrorCode errorCode;

    if(commonData==NULL) {
        /* load the common library if necessary - outside the mutex block */
        errorCode=U_ZERO_ERROR;
        pData=doOpenMap(DATA_TYPE, COMMON_DATA_NAME, &errorCode);
        if(pData!=NULL) {
            p=getChoice(pData, NULL, DATA_TYPE, COMMON_DATA_NAME, isCommonDataAcceptable, NULL, &errorCode);
            if(p!=NULL) {
                /* in the mutex block, set the common data for this process */
                umtx_lock(NULL);
                if(commonData==NULL) {
                    commonData=pData;
                    pData=NULL;
                }
                umtx_unlock(NULL);
            }

            /* if an error occured or a different thread set it first, then close the extra data instance */
            if(pData!=NULL) {
                udata_close(pData);
            }
        }
    }

    /* look in the common data first */
    if(commonData!=NULL) {
        char buffer[40];
        const char *dataName;

        /* get the data name */
        if(type==NULL || *type==0) {
            dataName=name;
        } else {
            icu_strcpy(buffer, name);
            icu_strcat(buffer, ".");
            icu_strcat(buffer, type);
            dataName=buffer;
        }

        errorCode=U_ZERO_ERROR;
        p=getChoice(commonData, dataName, type, name, isAcceptable, context, &errorCode);
        if(p!=NULL) {
            /* allocate the data structure */
            UDataMemory *pData=(UDataMemory *)icu_malloc(sizeof(UDataMemory));
            if(pData==NULL) {
                /* U_MEMORY_ALLOCATION_ERROR */
            } else {
                icu_memset(pData, 0, sizeof(UDataMemory));
                pData->p=p;
                return pData;
            }
        }
    }

    /* look for a specific map file */
    pData=doOpenMap(type, name, pErrorCode);
    if(pData!=NULL) {
        p=getChoice(pData, NULL, type, name, isAcceptable, context, pErrorCode);
        if(p=NULL) {
            udata_close(pData);
            pData=NULL;
        }
    }

    return pData;
}

/* this is the memory-map version of GET_ENTRY(), used by getChoice() */
static MappedData *
getCommonMapData(const UDataMemory *data, const char *dataName) {
    /* dataName==NULL if no lookup in a table of contents is necessary */
    if(dataName!=NULL) {
        const char *base=(const char *)(data->p)+data->p->headerSize;
        uint32_t *toc=(uint32_t *)base;
        uint32_t start, limit, number;

        /* perform a binary search for the data in the common data's table of contents */
        start=0;
        limit=*toc++;   /* number of names in this table of contents */
        while(start<limit-1) {
            number=(start+limit)/2;
            if(icu_strcmp(dataName, (const char *)(base+toc[2*number]))<0) {
                limit=number;
            } else {
                start=number;
            }
        }

        if(icu_strcmp(dataName, (const char *)(base+toc[2*start]))==0) {
            /* found it */
            return (MappedData *)(base+toc[2*start+1]);
        } else {
            return NULL;
        }
    } else {
        return data->p;
    }
}

static bool_t
isCommonDataAcceptable(void *context,
                       const char *type, const char *name,
                       UDataInfo *pInfo) {
    return
        pInfo->size>=20 &&
        pInfo->isBigEndian==U_IS_BIG_ENDIAN &&
        pInfo->charsetFamily==U_CHARSET_FAMILY &&
        pInfo->sizeofUChar==sizeof(UChar) &&
        pInfo->dataFormat[0]==0x43 &&   /* dataFormat="CmnD" */
        pInfo->dataFormat[1]==0x6d &&
        pInfo->dataFormat[2]==0x6e &&
        pInfo->dataFormat[3]==0x44 &&
        pInfo->formatVersion[0]==1;
}

#   endif

/* common implementations of other functions */

static MappedData *
getChoice(Library lib, const char *entry,
          const char *type, const char *name,
          UDataMemoryIsAcceptable *isAcceptable, void *context,
          UErrorCode *pErrorCode) {
    MappedData *p;
    UDataInfo *info;

    /* get the data pointer */
    p=GET_ENTRY(lib, entry);
    if(p==NULL) {
        *pErrorCode=U_FILE_ACCESS_ERROR;
        return NULL;
    }
    info=(UDataInfo *)(p+1);

    /* check magic1 & magic2 */
    /* check for the byte ordering */
    /* is this acceptable? */
    if( p->magic1!=0xda || p->magic2!=0x27 ||
        info->isBigEndian!=U_IS_BIG_ENDIAN ||
        isAcceptable!=NULL && !isAcceptable(context, type, name, info)
    ) {
        *pErrorCode=U_INVALID_FORMAT_ERROR;
        return NULL;
    }

    return p;
}

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
