// Copyright (c) 2000-2001 IBM, Inc. and others.
#include "TextCache.h"
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <string.h>
#include "util.h"

#ifdef _WIN32
  #include <direct.h>
  #define mkdir(a,b) _mkdir(a)
  #include <windows.h>
  #define sleep(a) Sleep(a)
#else
  #include <unistd.h>
#endif

#define INDEX_FILENAME "index"
#define LOCK_FILENAME "lock"

TextCache::TextCache(const char* path) : lockCount(0) {
    root = new char[4+strlen(path)]; // 1 for zero byte, 3 for extra stuff
    strcpy(root, path);
    if (root[strlen(root)-1] != '/') {
        strcat(root, "/");
    }
    mkdir(root, 0777);

    lockPath = new char[8+strlen(root)];
    strcpy(lockPath, root);
    strcat(lockPath, LOCK_FILENAME);

    hash.setValueDeleter(deleteCacheObj);

    readIndex();
}

TextCache::~TextCache() {
    releaseLock(true);
    delete[] root;
    delete[] lockPath;
}

void TextCache::readIndex() {
    char path[256];
    strcpy(path, root);
    strcat(path, INDEX_FILENAME);
    acquireLock();
    FILE *indexFile = fopen(path, "rb");
    if (indexFile != NULL) {
        UErrorCode status = U_ZERO_ERROR;
        UnicodeString key;
        for (;;) {
            // Read key
            if (!util_readFrom(indexFile, key) || key.length() == 0) {
                // Empty key string indicates end of index
                break;
            }

            // Read filename
            int32_t length = 0;
            char* name = util_readFrom(indexFile, length);
            if (name == 0 || length>MAX_FILENAME_LENGTH) {
                break;
            }

            // Create the cache object
            CacheObj *obj = new CacheObj();
            if (obj == 0) {
                delete[] name;
                break;
            }
            strcpy(obj->filename, name);
            delete[] name;
            obj->isLoaded = FALSE;

            // Successfully read a key-value pair; store it
            hash.put(key, obj, status);
            if (U_FAILURE(status)) {
                break;
            }
        }
        fclose(indexFile);
    }
    releaseLock();
}

UBool TextCache::writeIndex() {
    char path[256];
    strcpy(path, root);
    strcat(path, INDEX_FILENAME);
    acquireLock();
    FILE *indexFile = fopen(path, "w+b");
    UBool success = TRUE;
    if (indexFile == NULL) {
        success = FALSE;
    } else {
        int32_t pos = -1;
        for (;;) {
            const UHashElement* elem = hash.nextElement(pos);
            if (elem == NULL) {
                break;
            }
            UnicodeString* key = (UnicodeString*) elem->key.pointer;
            CacheObj* obj = (CacheObj*) elem->value.pointer;
             
            success &= util_writeTo(indexFile, *key);
            success &= util_writeTo(indexFile, obj->filename, strlen(obj->filename) + 1);
        }
        UnicodeString empty;
        success &= util_writeTo(indexFile, empty); // empty string is end mark
        fclose(indexFile);
    }
    releaseLock();
    return success;
}

void TextCache::visitKeys(KeyVisitor visit, void* context) const {
    int32_t pos = -1;
    int32_t i = 0;
    for (;;) {
        const UHashElement* elem = hash.nextElement(pos);
        if (elem == NULL) {
            break;
        }
        UnicodeString* key = (UnicodeString*) elem->key.pointer;
        visit(i++, *key, context);
    }
}

UBool TextCache::put(const UnicodeString& key, const UnicodeString& value) {
    UBool doUpdateIndex = FALSE;
    CacheObj* obj = (CacheObj*) hash.get(key);
    UErrorCode status = U_ZERO_ERROR;
    if (obj == 0) {
        obj = new CacheObj();
        obj->generateFilename(root);
        hash.put(key, obj, status);
        doUpdateIndex = TRUE;
    }
    obj->isLoaded = TRUE;
    obj->text = value;
    acquireLock();
    UBool result = obj->save(root);
    releaseLock();
    if (doUpdateIndex) {
        result &= writeIndex();
    }
    return result && U_SUCCESS(status);
}

UBool TextCache::get(const UnicodeString& key, UnicodeString& value) {
    CacheObj* obj = (CacheObj*) hash.get(key);
    if (obj == 0) {
        return FALSE;
    }
    acquireLock();
    UBool loadOk = obj->load(root);
    releaseLock();
    if (!loadOk) {
        return FALSE;
    }
    value = obj->text;
    return TRUE;
}

void TextCache::remove(const UnicodeString& key) {
    CacheObj* obj = (CacheObj*) hash.get(key);
    if (obj != 0) {
        char path[256];
        strcpy(path, root);
        strcat(path, obj->filename);
        acquireLock();
        unlink(path);
        releaseLock();
        hash.remove(key);
        writeIndex();
    }
}

/**
 * Load a cache object into the cache by reading the given file.
 * @return TRUE on success
 */
UBool TextCache::CacheObj::load(const char* root) {
    if (isLoaded) return TRUE;
    char path[256];
    strcpy(path, root);
    strcat(path, filename);
    FILE* file = fopen(path, "rb");
    if (file == NULL) return FALSE;
    UBool result = util_readFrom(file, text);
    fclose(file);
    return result;
}

/**
 * Save a cache object into the cache by writing the given file.
 * @return TRUE on success
 */
UBool TextCache::CacheObj::save(const char* root) {
    char path[256];
    strcpy(path, root);
    strcat(path, filename);
    FILE* file = fopen(path, "w+b");
    if (file == NULL) return FALSE;
    UBool result = util_writeTo(file, text);
    fclose(file);
    return result;
}

static char* FILENAMECHARS = "abcdefghijklmnopqrstuvwxyz0123456789";

/**
 * Generate a filename that doesn't already exist.
 */
void TextCache::CacheObj::generateFilename(const char* root) {
    char path[256];
    strcpy(path, root);
    strcat(path, "cache_");
    int code[4];
    char *p = path + strlen(path);
    p[4] = 0;
    int pos;
    for (pos=0; pos<4; ++pos) {
        code[pos] = 0;
    }
    for (;;) {
        for (pos=0; pos<4; ++pos) {
            p[pos] = FILENAMECHARS[code[pos]];
        }
        if (!fileExists(path)) {
            break;
        }
        for (pos=3; pos>=0; --pos) {
            ++code[pos];
            if (code[pos] < 36) break;
            code[pos] = 0;
        }
    }
    strcpy(filename, path + strlen(root));
}

void TextCache::deleteCacheObj(void* o) {
    delete (CacheObj*) o;
}

// See below
#ifdef _WIN32
#define TIMEOUT 5
#else
#define TIMEOUT 15
#endif

void TextCache::acquireLock() {
    if (lockCount) {
        ++lockCount;
    } else {
        // If a process crashes it can leave a lock file around.  To
        // handle this eventuality, apply a maximum wait interval.
        // After this interval, assume that a process crashed, and
        // acquire the lock anyway.  On Win32, we are running locally,
        // so make the interval short.  Otherwise have it be longer.
        int32_t timeout = TIMEOUT;
        while (fileExists(lockPath)) {
            if (timeout-- == 0) {
                ++lockCount;
                return;
            }
            sleep(1); // 1 second
        }
        FILE* file = fopen(lockPath, "w");
        if (file != NULL) {
            fclose(file);
        }
        ++lockCount;
    }
}

void TextCache::releaseLock(bool allTheWay) {
    if (lockCount > 0) {
        --lockCount;
        if (allTheWay || lockCount == 0) {
            unlink(lockPath);
        }
    }
}

bool TextCache::fileExists(const char* fullpath) {
    bool result = false;
    FILE* file = fopen(fullpath, "r");
    if (file != NULL) {
        result = true;
        fclose(file);
    }
    return result;
}
