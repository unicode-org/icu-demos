#include "TextCache.h"
#include "util.h"

void visitor(int32_t i, const UnicodeString& key, void* ccache) {
    printf(" %ld: ", i);
    util_fprintf(stdout, key);
    printf(" => \"");
    UnicodeString str;
    ((TextCache*)ccache)->get(key, str);
    util_fprintf(stdout, str);
    printf("\"\n");
}

void usage() {
    printf("Usage: tct <cache-dir> [commands]\n");
    printf(" <cache-dir> Path to cache\n");
    printf("Commands:\n");
    printf(" put <key> <value>\n");
    printf(" get <key>\n");
    printf(" rem <key>\n");
    printf(" dump\n");
    exit(1);
}

/**
 * A simple tool for TextCache.  Used for testing TextCache
 * and manipulating caches.
 */
int main(int argc, char* argv[]) {

    if (argc < 2) usage();

    int argi = 1;

    TextCache cache(argv[argi++]);

    while (argi < argc) {
        if (strcmp(argv[argi], "put") == 0) {
            ++argi;
            if ((argi+2) > argc) usage();
            const char* key = argv[argi++];
            const char* value = argv[argi++];
            cache.put(key, value);
        }
        
        else if (strcmp(argv[argi], "get") == 0) {
            ++argi;
            if ((argi+1) > argc) usage();
            const char* key = argv[argi++];
            UnicodeString str;
            cache.get(key, str);
            printf("%s => \"", key);
            util_fprintf(stdout, str);
            printf("\"\n");
        }
        
        else if (strcmp(argv[argi], "rem") == 0) {
            ++argi;
            if ((argi+1) > argc) usage();
            const char* key = argv[argi++];
            cache.remove(key);
        }

        else if (strcmp(argv[argi], "dump") == 0) {
            ++argi;
            printf("dump:\n");
            cache.visitKeys(visitor, &cache);
        }

        else {
            usage();
        }
    }

    return 0;
}
