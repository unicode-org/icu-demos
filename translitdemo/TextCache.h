/* Copyright (C) 2006 IBM and Others */

#ifndef TEXTCACHE_H
#define TEXTCACHE_H

#include "unicode/unistr.h"
#include "hash.h"
using namespace icu; //TODO:

/**
 * A cache of text objects.  The cache is stored on disk.
 */
class TextCache {

 public:

    /**
     * @param path the path to the directory where the cache will be
     * stored, with a trailing path delimiter, e.g., "data/cache/".
     */
    TextCache(const char* path);

    virtual ~TextCache();

    UBool get(const UnicodeString& key, UnicodeString& value);

    UBool put(const UnicodeString& key, const UnicodeString& value);

	/**
	 * - Write the item to a UTF-8 file located in '<path>/flat'
	 * - Add the item to SVN if it didn't exist
	 * - Commit
	 * @return false if error.  Returns true and silently fails if /flat does not exist.
	 */
    UBool reposWrite(const UnicodeString& key, const UnicodeString& value, const char *author);

    void remove(const UnicodeString& key);

    int32_t count() const { return hash.count(); }

    typedef void (*KeyVisitor)(int32_t i, const UnicodeString& key, void* context);

    void visitKeys(KeyVisitor visit, void* context) const;

 private:

    // Multiple process contention is handled in a simple-minded
    // fashion with a lock file.  The semantics of acquire and
    // release are that they must be balanced.  A call to acquire
    // where the lock is already held simply increments the lockCount
    // and returns.
    // LOCKING IS FOR BETWEEN DIFFERENT PROCESSES, NOT multiple
    // threads within one process.
    // TODO: Add timeout param?
    int32_t lockCount;
    char* lockPath;
    void acquireLock();
    // @param allTheWay if true, automatically balance ALL previous
    // calls to acquireLock (delete the lock file)
    void releaseLock(UBool allTheWay=false);

    static UBool fileExists(const char* fullpath);

    static void deleteCacheObj(void* o);

    void readIndex();
    UBool writeIndex();

 public:

    enum { MAX_FILENAME_LENGTH = 31 };

 private:

    class CacheObj {
    public:
        char filename[MAX_FILENAME_LENGTH + 1];
        UnicodeString text;
        UBool isLoaded;
    public:
        UBool load(const char* root);
        UBool save(const char* root);
        void generateFilename(const char* root);
    };
    friend class CacheObj;

    char* root; // path supplied to ct (owned copy)

    Hashtable hash; // map of keys to CacheObjs
};

#endif
