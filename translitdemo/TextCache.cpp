
#include "TextCache.h"
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include "util.h"

#define INDEX_FILENAME "index"

TextCache::TextCache(const char* path) {
    root = new char[4+strlen(path)]; // 1 for zero byte, 3 for extra stuff
    strcpy(root, path);
    if (root[strlen(root)-1] != '/') {
        strcat(root, "/");
    }
    mkdir(root, 0777);

    hash.setValueDeleter(deleteCacheObj);

    readIndex();
}

TextCache::~TextCache() {
    delete[] root;
}

void TextCache::readIndex() {
    char path[256];
    strcpy(path, root);
    strcat(path, INDEX_FILENAME);
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
}

UBool TextCache::writeIndex() {
    char path[256];
    strcpy(path, root);
    strcat(path, INDEX_FILENAME);
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
            UnicodeString* key = (UnicodeString*) elem->key;
            CacheObj* obj = (CacheObj*) elem->value;
            success &= util_writeTo(indexFile, *key);
            success &= util_writeTo(indexFile, obj->filename, strlen(obj->filename) + 1);
        }
        UnicodeString empty;
        success &= util_writeTo(indexFile, empty); // empty string is end mark
        fclose(indexFile);
    }
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
        UnicodeString* key = (UnicodeString*) elem->key;
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
    UBool result = obj->save(root);
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
    if (!obj->load(root)) {
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
        unlink(path);
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
        FILE* file = fopen(path, "r");
        if (file == NULL) {
            break;
        }
        fclose(file);
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

