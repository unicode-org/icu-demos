#include "stdio.h"
#include "utypes.h"
#include "cmemory.h"
#include "cstring.h"
#include "udata.h"
#include <uloc.h>

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

#if 0 /* Win32 dll implementation */
#elif 0 /* POSIX dll implementation */
#elif WIN32 /* Win32 memory map implementation */
#include <windows.h>

typedef struct {
    uint16_t headerSize;
    uint8_t magic1, magic2;
} MappedData;

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
        /* open the input file */
        HANDLE file=CreateFile(buffer+4, GENERIC_READ, FILE_SHARE_READ, NULL,
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
#elif defined (LINUX)||defined(POSIX)||defined(SOLARIS)||defined(AIX)||defined(HPUX) /* POSIX memory map implementation */
 
/* srl's mmap implementation */
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
 

#else /* stdio fopen()/fread() implementation */
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
    const char *dataDir;
    
    dataDir = uloc_getDataDirectory();

    /* open the input file */
    if(type!=NULL && *type!=0) {
        char filename[512];

	icu_strcpy(filename, dataDir);
        icu_strcat(filename, name);
        icu_strcat(filename, ".");
        icu_strcat(filename, type);
        file=fopen(filename, "rb");
    } else {
      char filename[512];
      icu_strcpy(filename, dataDir);
      icu_strcat(filename, name);
      file=fopen(filename, "rb");
    }

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
