#include "TextCache.h"
#include "util.h"

void visitor(int32_t i, const UnicodeString& key, void* context) {
    printf(" %ld: \"", i);
    util_fprintf(stdout, key);
    printf("\"\n");
}

/**
 * A simple test for TextCache.  Invoke with and without
 * the "-put" option and inspect the files in the
 * testcache/ subdirectory.
 */
int main(int argc, char* argv[]) {

    TextCache cache("testcache/");

    if (argc >= 2 && strcmp(argv[1], "-put") == 0) {
        cache.put("alpha", "apple");
        cache.put("beta", "banana");
        cache.put("gamma", "grape");
    }

    UnicodeString str;

    cache.get("alpha", str);
    printf("alpha->");
    util_fprintf(stdout, str);
    printf("\n");

    cache.get("beta", str);
    printf("beta->");
    util_fprintf(stdout, str);
    printf("\n");

    cache.get("gamma", str);
    printf("gamma->");
    util_fprintf(stdout, str);
    printf("\n");

    printf("index:\n");
    cache.visitKeys(visitor, NULL);
}
