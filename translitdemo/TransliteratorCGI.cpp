/**
 * The CGI interacts with the template file by filling in variables of
 * the form $FOO and reading text from fields.  Here is a complete
 * list of operations.
 *
 * $ICU_VERSION
 * Replaced by ICU version string.
 *
 * $BUILD_TIME
 * Replaced by the time and date THIS FILE was compiled.
 *
 * $AVAILABLE_IDS
 * Replaced by semicolon delimited list of available system
 * transliterator IDs.
 *
 * $AVAILABLE_SOURCES
 * Replaced by semicolon delimited list of available system
 * source specs.
 *
 * $AVAILABLE_RBT_IDS
 * Replaced by semicolon delimited list of available system
 * RuleBasedTransliterator IDs.  This is an SLOW call because
 * it instantiates all the IDs to see which are rule based.
 *
 * $USER_IDS
 * Replaced by semicolon delimited list of user-defined transliterator
 * IDs (from the TextCache).
 *
 * // TODO
 * Add docs for the OPCODE operations
 *
 * QUOTING:
 * All of the strings are output raw, with no escaping, unless otherwise
 * specified here:
 * - $RESULT has double quotes escaped with backslashes
 *
 * TEMPLATE_FILE is a special field that is used to tell the CGI what
 * template file to use to generate its output.  It is a
 * self-referential field.
 */

#include "TransliteratorCGI.h"
#include "unicode/translit.h"
#include "unicode/ustring.h"
#include "unicode/rbt.h"
#include "util.h"
#include <stdio.h>
#include <stdlib.h> // qsort (Linux)
#include <string.h> // strncmp (Linux)

// Encoding we use for interchange to/from the browser
#define ENCODING "UTF8"

// Special transliterator IDs
#define NULL_ID "Null"

static int _compareIDs(const void* arg1, const void* arg2);

TransliteratorCGI::TransliteratorCGI() :
    ruleCache("data/cache/") {
    inputText = 0;
    availableIDs = 0;
    availableIDsCount = 0;
    availableRBTIDs = 0;
    availableRBTIDsCount = 0;
    isIntermediateInitialized = false;
    isIntermediateBogus = false;
    areUserTransliteratorsLoaded = false;
    translit1 = 0;
    translit2 = 0;
}

TransliteratorCGI::~TransliteratorCGI() {
    delete inputText;
    int i;
    for (i=0; i<availableIDsCount; ++i) {
        delete[] availableIDs[i];
    }
    delete[] availableIDs;
    for (i=0; i<availableRBTIDsCount; ++i) {
        delete[] availableRBTIDs[i];
    }
    delete[] availableRBTIDs;

    delete translit1;
    delete translit2;
}

/**
 * TemplateCGI framework method to get the template file name.
 */
const char* TransliteratorCGI::getTemplateFile() {
    const char* c = getParamValue("TEMPLATE_FILE");
    if (!c || !*c) {
        c = "data/translit_main.html";
    }
    return c;
}

/**
 * TemplateCGI framework method to emit the CGI header.
 */
void TransliteratorCGI::handleEmitHeader(FILE* out) {
	fprintf(out, "Content-type: text/html; charset=UTF-8\n\n");
}

char* TransliteratorCGI::cleanupNewlines(const char* text) {
#ifdef _WIN32
    char* newtext = new char[1+strlen(text ? text : "")];
    // For some reason, on IE5.5/Win2K we are seeing (^M)+^J
    // Delete all ^M's
    const char *src = text;
    char *dst = newtext;
    while (*src) {
        if (*src != 13) {
            *dst++ = *src;
        }
        ++src;
    }
    *dst = 0;
    return newtext;
#else
    return strdup(text ? text : "");
#endif
}

/**
 * TemplateCGI framework method to fill in variables.
 * TODO: Handle inQuote arg properly.
 */
void TransliteratorCGI::handleTemplateVariable(FILE* out, const char* var,
                                               bool inQuote) {

    if (strcmp(var, "AVAILABLE_IDS") == 0) {
        char** ids = getAvailableIDs();
        if (ids != 0) {
            int i;
            for (i=0; i<availableIDsCount; ++i) {
                fprintf(out, ((i==0)?"%s":";%s"), ids[i]);
            }
        }
    }

    else if (strcmp(var, "AVAILABLE_RBT_IDS") == 0) {
        char** ids = getAvailableRBTIDs();
        if (ids != 0) {
            int i;
            for (i=0; i<availableRBTIDsCount; ++i) {
                fprintf(out, ((i==0)?"%s":";%s"), ids[i]);
            }
        }
    }

    else if (strcmp(var, "AVAILABLE_SOURCES") == 0) {
        UnicodeString src;
        loadUserTransliterators();
        int32_t n = Transliterator::countAvailableSources();
        for (int32_t i=0; i<n; ++i) {
            if (i) fprintf(out, ";");
            util_fprintf(out, Transliterator::getAvailableSource(i, src));
        }
    }

    else if (strcmp(var, "ICU_VERSION") == 0) {
        char str[U_MAX_VERSION_STRING_LENGTH];
        UVersionInfo version;
        u_getVersion(version);
        u_versionToString(version, str);
        fprintf(out, "%s", str);
    }

    else if (strcmp(var, "USER_IDS") == 0) {
        ruleCache.visitKeys(handle_USER_IDS, (void*) out);
    }

    else if (strcmp(var, "OPRESULT") == 0) {
        const char* opcode = getParamValue("OPCODE", "");
        const char* arg1 = getParamValue("OPARG1", "");
        const char* arg2 = getParamValue("OPARG2", "");
        const char* arg3 = getParamValue("OPARG3", "");

        if (strcmp(opcode, "GETAVAILABLESOURCES") == 0) {
            UnicodeString src;
            loadUserTransliterators();
            int32_t n = Transliterator::countAvailableSources();
            for (int32_t i=0; i<n; ++i) {
                if (i) fprintf(out, ";");
                util_fprintf(out, Transliterator::getAvailableSource(i, src));
            }            
        }

        else if (strcmp(opcode, "GETAVAILABLETARGETS") == 0) {
            UnicodeString src(arg1, ENCODING);
            UnicodeString trg;
            loadUserTransliterators();
            int32_t n = Transliterator::countAvailableTargets(src);
            for (int32_t i=0; i<n; ++i) {
                if (i) fprintf(out, ";");
                util_fprintf(out, Transliterator::getAvailableTarget(i, src, trg));
            }            
        }

        else if (strcmp(opcode, "GETAVAILABLEVARIANTS") == 0) {
            UnicodeString src(arg1, ENCODING);
            UnicodeString trg(arg2, ENCODING);
            UnicodeString var;
            loadUserTransliterators();
            int32_t n = Transliterator::countAvailableVariants(src, trg);
            for (int32_t i=0; i<n; ++i) {
                if (i) fprintf(out, ";");
                util_fprintf(out, Transliterator::getAvailableVariant(i, src, trg, var));
            }            
        }

        else if (strcmp(opcode, "TRANSLITERATE") == 0) {
            UnicodeString text;
            UnicodeString id(arg2, ENCODING);
            UTransDirection dir = UTRANS_FORWARD;
            // Leading ' indicates inverse
            if (id.charAt(0) == 39) {
                id.remove(0,1);
                dir = UTRANS_REVERSE;
            } else {
                // We prepend an invisible Hex-Any transliterator.
                // This just makes it possible for the user to type
                // hex escapes in the input area.
                id.insert(0, UnicodeString("Hex-Any;", ""));
            }
            loadUserTransliterators();
            Transliterator *t = Transliterator::createInstance(id, dir);
            if (t != 0) {
                char* s = cleanupNewlines(arg1);
                text = UnicodeString(s, ENCODING);
                delete[] s;
                t->transliterate(text);
            } else {
                UnicodeString id(arg2, ENCODING);
                if (id.charAt(0) == 39) {
                    id.remove(0,1);
                }
                text = "Error: Unable to create ";
                text += (dir == UTRANS_FORWARD) ? "" : "inverse of ";
                text += id;
                text += "";
            }
            util_fprintfq(out, text);
        }

        else if (strcmp(opcode, "COMPILE") == 0) {
            UnicodeString id(arg1, ENCODING);
            char *r = cleanupNewlines(arg2);
            UnicodeString rules(r, ENCODING);
            delete[] r;
            UnicodeString errMsg;
            Transliterator *t = buildUserRules(id, rules, errMsg);
            if (t != 0) {
                delete t;
                // We have a validated rule set; save it
                UBool ok = ruleCache.put(id, rules);
                fprintf(out, ok ? "" : "Error: Internal CGI failure");
            } else {
                util_fprintf(out, errMsg, inQuote);
            }            
        }

        else if (strcmp(opcode, "TORULES") == 0) {
            UnicodeString id(arg1, ENCODING);
            UnicodeString rule;
            if (ruleCache.get(id, rule)) {
                util_fprintf(out, rule);
            } else {
                Transliterator *t = Transliterator::createInstance(id);
                if (t == NULL) {
                    fprintf(out, "// Cannot create ");
                    util_fprintf(out, id);
                } else {
                    if (t->getDynamicClassID() ==
                        RuleBasedTransliterator::getStaticClassID()) {
                        ((RuleBasedTransliterator*)t)->toRules(rule, FALSE);
                        util_fprintfq(out, rule);
                    } else {
                        fprintf(out, "// ");
                        util_fprintf(out, id);
                        fprintf(out, " is not a RuleBasedTransliterator");
                    }
                    delete t;
                }
            }
        }

        else if (strcmp(opcode, "GETUSERIDS") == 0) {
            ruleCache.visitKeys(handle_USER_IDS, (void*) out);
        }

        else if (strcmp(opcode, "DELETEUSERID") == 0) {
            UnicodeString id(arg1, ENCODING);
            ruleCache.remove(id);
        }
    }

    else if (strcmp(var, "BUILD_TIME") == 0) {
        fprintf(out, __TIME__ " " __DATE__);
    }

    else {
        TemplateCGI::handleTemplateVariable(out, var, inQuote);
    }
}

/**
 * Given a tag (indicating a CGI param) retrieve the rules from that
 * tag and attempt to build them.  Return the resulting
 * transliterator, or NULL on failure.  On failure, put the relevant
 * error message in errMsg.
 *
 * Caller owns the result if it is non-NULL.
 */
Transliterator* TransliteratorCGI::buildUserRules(const UnicodeString& id,
                                                  const UnicodeString& rules,
                                                  UnicodeString& errMsg) {
    UParseError err;
    UErrorCode status = U_ZERO_ERROR;
    Transliterator *t = new RuleBasedTransliterator(id, rules,
                                                    UTRANS_FORWARD,
                                                    0, err, status);
    if (U_FAILURE(status)) {
        char buf[256];
        sprintf(buf, "Error: Syntax error in the rule \"");
        errMsg += buf;
        errMsg += err.preContext;
        errMsg += "\"";
        delete t;
        t = 0;
    }
    return t;
}

/**
 * Load transliterators the user has defined in the browser.  These
 * may be loaded from cookies, or typed into a text area.
 */
void TransliteratorCGI::loadUserTransliterators() {
    if (areUserTransliteratorsLoaded) {
        return;
    }

    ruleCache.visitKeys(registerUserRules, this);

    areUserTransliteratorsLoaded = true;
}

/**
 * Return the available RuleBasedTransliterator IDs as a sorted
 * list of char* strings.  This list is created once and cached
 * thereafter.
 *
 * THIS IS AN EXPENSIVE CALL because it instantiates all the IDs.
 */
char** TransliteratorCGI::getAvailableRBTIDs() {
    if (availableRBTIDs != 0) {
        return availableRBTIDs;
    }

    char** all = getAvailableIDs();

    availableRBTIDs = new char*[availableIDsCount];
    if (availableRBTIDs == 0) {
        availableRBTIDsCount = 0;
        return 0;
    }
    
    int32_t i;
    availableRBTIDsCount = 0;
    for (i=0; i<availableIDsCount; ++i) {
        UnicodeString id(all[i], ENCODING);
        Transliterator *t = Transliterator::createInstance(id);
        if (t != NULL) {
            if (t->getDynamicClassID() ==
                RuleBasedTransliterator::getStaticClassID()) {
                availableRBTIDs[availableRBTIDsCount++] = strdup(all[i]);
            }
            delete t;
        }
    }

    return availableRBTIDs;
}

/**
 * Return the available transliterator IDs as a sorted list of
 * char* strings.  This list is created once and cached thereafter.
 */
char** TransliteratorCGI::getAvailableIDs() {
    if (availableIDs != 0) {
        return availableIDs;
    }

    int32_t i;
    availableIDsCount = Transliterator::countAvailableIDs();
    availableIDs = new char*[availableIDsCount];
    if (availableIDs == 0) {
        availableIDsCount = 0;
        return 0;
    }

    for (i=0; i<availableIDsCount; ++i) {
        availableIDs[i] = 0;
    }

    // Extract the IDs into char** array
    for (i=0; i<availableIDsCount; ++i) {
        const UnicodeString& id = Transliterator::getAvailableID(i);
        int32_t len = id.length();
        availableIDs[i] = new char[len+1];
        len = id.extract(0, id.length(), availableIDs[i], len+1, ENCODING);
        availableIDs[i][len] = 0;
    }

    // Sort the array
    qsort((void*)availableIDs, availableIDsCount,
          sizeof(availableIDs[0]), _compareIDs);

    return availableIDs;
}

// TextCache::KeyVisitor
void TransliteratorCGI::handle_USER_IDS(int32_t i, const UnicodeString& key,
                                        void* context) {
    FILE* out = (FILE*) context;
    if (i != 0) {
        fprintf(out, ";");
    }
    util_fprintf(out, key);
}

// TextCache::KeyVisitor
void TransliteratorCGI::registerUserRules(int32_t i, const UnicodeString& id,
                                          void* context) {
    UnicodeString rules;
    TransliteratorCGI* _this = (TransliteratorCGI*) context;
    if (_this->ruleCache.get(id, rules)) {
        UErrorCode status = U_ZERO_ERROR;
        Transliterator *t = new RuleBasedTransliterator(id, rules,
                                                        UTRANS_FORWARD,
                                                        status);
        if (U_SUCCESS(status)) {
            Transliterator::registerInstance(t);
        } else {
            delete t;
        }
    }
}

/**
 * Given an id of the form <x>-<y>, store it into the buffer as either
 * <x>-<y> or <y>-<x>a.  The latter form is chosen if y precedes x
 * lexically.  The 'a' is appended to make a reversed ID sort after an
 * unreversed ID.
 *
 * As a special case, we want to put Latin-<x> at the top of the list,
 * with <x>-Latin immediately below it.  To do this Latin-<x> is
 * transformed to \001-<x> (and <x>-Latin to \001-<x>a).
 */
static void _mungeID(char* buf, const char* id) {
    // Handle Latin-<x>
    if (strncmp(id, "Latin-", 6) == 0) {
        strcpy(buf, "\001-");
        strcat(buf, id+6);
        return;
    }
    char* dash = strchr(id, '-');
    if (dash != 0) {
        // Handle <x>-Latin
        if (strcmp(dash+1, "Latin") == 0) {
            strcpy(buf, "\001-");
            strncat(buf, id, dash-id);
            strcat(buf, "a");
            return;
        }
        // Handle <x>-<y> where <y> precedes <x>; change to
        // <y>-<x>a.
        if (strcmp(id, dash+1) > 0) {
            strcpy(buf, dash+1);
            strcat(buf, "-");
            strncat(buf, id, dash-id);
            strcat(buf, "a");
            return;
        }
    }
    strcpy(buf, id);
}

/**
 * qsort comparison function.
 */
static int _compareIDs(const void* arg1, const void* arg2) {
    char buf1[256];
    char buf2[256];
    _mungeID(buf1, *(char**)arg1);
    _mungeID(buf2, *(char**)arg2);
    return strcmp(buf1, buf2);
}
