
#ifndef TEXTCACHE_H
#define TEXTCACHE_H

#include "unicode/unistr.h"
#include "hash.h"

/**
 * A cache of text objects.  The cache is stored on disk.
 *
 * TODO Add locking so multiple processes can access the
 * cache safely.
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

    void remove(const UnicodeString& key);

    int32_t count() const { return hash.count(); }

    typedef void (*KeyVisitor)(int32_t i, const UnicodeString& key, void* context);

    void visitKeys(KeyVisitor visit, void* context) const;

 private:

    static void deleteCacheObj(void* o);

    void readIndex();
    UBool writeIndex();

    enum { MAX_FILENAME_LENGTH = 31 };

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

    char* root; // path supplied to ct (owned copy)

    Hashtable hash; // map of keys to CacheObjs
};

#endif
