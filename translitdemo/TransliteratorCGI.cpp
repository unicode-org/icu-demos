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
 * $AVAILABLE_RBT_IDS
 * Replaced by semicolon delimited list of available system
 * RuleBasedTransliterator IDs.  This is an SLOW call because
 * it instantiates all the IDs to see which are rule based.
 *
 * $USER_IDS
 * Replaced by semicolon delimited list of user-defined transliterator
 * IDs (from the TextCache).
 *
 * $OUTPUT1 / $OUTPUT2
 * Replaced by the result of transliterating a field ARG_INPUT using
 * ARG_ID and then ARG_ID2.  ARG_DIRECTION2 may be set to "REVERSE"
 * to invoke ARG_ID2 in the reverse direction.
 *
 * $RULE_SOURCE
 * Replaced by the text for user or system rule ARG_ID, unless
 * ARG_OPERATION is "COMPILE".  In the latter case, $RULE_SOURCE
 * is replaced by itself (the field value is left unchanged); this
 * preserves the input text so the user can correct syntax errors.
 *
 * $RESULT
 * Replaced by either "OK" or an error string.  This is a generic
 * portal for operations whose primary result is not text output, but
 * instead a side effect.  Takes argument ARG_OPERATION, which can
 * have any of the values:
 *
 *   "COMPILE"
 *   Takes arguments ARG_ID and ARG_RULES.  On success, stores the
 *   given ID/rules pair in the user cache.
 *
 *   "REMOVE"
 *   Takes argument ARG_ID.  On success, removes the given ID from the
 *   user cache.
 *
 * List of fields:
 * ARG_DIRECTION2
 * ARG_ID
 * ARG_ID2
 * ARG_INPUT
 * ARG_OPERATION
 * ARG_RULES
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
        c = "data/translit_template.html";
    }
    return c;
}

/**
 * TemplateCGI framework method to emit the CGI header.
 */
void TransliteratorCGI::handleEmitHeader(FILE* out) {
	fprintf(out, "Content-type: text/html; charset=UTF-8\n\n");
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

    else if (strcmp(var, "RULE_SOURCE") == 0) {
        // If this is a compile operation, then replace RULE_SOURCE
        // with itself.  That way we don't blow away rules the user
        // has entered that contain a syntax error.
        const char* opcode = getParamValue("ARG_OPERATION", "");
        if (strcmp(opcode, "COMPILE") == 0) {
            TemplateCGI::handleTemplateVariable(out, var, inQuote);
            return;
        }

        UnicodeString id(getParamValue("ARG_ID", ""), ENCODING);
        UnicodeString rule;
        if (ruleCache.get(id, rule)) {
            util_fprintf(out, rule);
        } else {
            Transliterator *t = Transliterator::createInstance(id);
            if (t == NULL) {
                fprintf(out, "// Cannot create transliterator ");
                util_fprintf(out, id);
            } else {
                if (t->getDynamicClassID() ==
                    RuleBasedTransliterator::getStaticClassID()) {
                    ((RuleBasedTransliterator*)t)->toRules(rule, FALSE);
                    util_fprintf(out, rule);
                } else {
                    fprintf(out, "// ");
                    util_fprintf(out, id);
                    fprintf(out, " is not a RuleBasedTransliterator");
                }
                delete t;
            }
		}
    }

    else if (strcmp(var, "OUTPUT1") == 0) {
        util_fprintf(out, getIntermediate());
    }

    else if (strcmp(var, "OUTPUT2") == 0) {
        UnicodeString constructorError;
        Transliterator *trans = getTranslit2(constructorError);
        if (trans == 0) {
            util_fprintf(out, constructorError);
        } else {
            UnicodeString output2(getIntermediate());
            if (!isIntermediateBogus) {
                trans->transliterate(output2);
                util_fprintf(out, output2);
            }
        }
    }

    else if (strcmp(var, "RESULT") == 0) {
        const char* opcode = getParamValue("ARG_OPERATION", "");

        if (strcmp(opcode, "COMPILE") == 0) {
            // Get the rules and try to build them.
            UnicodeString id(getParamValue("ARG_ID", ""), ENCODING);
            UnicodeString rules(getParamValue("ARG_RULES", ""), ENCODING);
            UnicodeString errMsg;
            Transliterator *t = buildUserRules(id, rules, errMsg);
            if (t != 0) {
                delete t;
                // We have a validated rule set; save it
                UBool ok = ruleCache.put(id, rules);
                fprintf(out, ok ? "OK" : "INTERNAL ERROR");
            } else {
                util_fprintf(out, errMsg, inQuote);
            }
        }

        else if (strcmp(opcode, "REMOVE") == 0) {
            UnicodeString id(getParamValue("ARG_ID", ""), ENCODING);
            ruleCache.remove(id);
            util_fprintf(out, "OK");
        }
    }

#ifdef _WIN32
    // On a Win32 server we need to munge the input to avoid the
    // insertion of spurious newlines.
    else if (strcmp(var, "ARG_INPUT") == 0) {
        fprintf(out, getInputText());
    }
#endif

    else if (strcmp(var, "BUILD_TIME") == 0) {
        fprintf(out, __TIME__ " " __DATE__);
    }

    else {
        TemplateCGI::handleTemplateVariable(out, var, inQuote);
    }
}


/**
 * Return the input text in its original encoding.  This is computed
 * the first time and cached thereafter.
 */
const char* TransliteratorCGI::getInputText() {
    if (inputText == 0) {
        const char* s = getParamValue("ARG_INPUT");
        inputText = strdup(s ? s : "");
#ifdef _WIN32
        // For some reason, on IE5.5/Win2K we are seeing (^M)+^J
        // Delete all ^M's
        char *p = inputText;
        char *q = p;
        while (*q) {
            if (*q != 13) {
                *p++ = *q;
            }
            ++q;
        }
        *p = 0;
#endif
#if 0
        // Enable this (and disable the above block) to see what
        // control characters the browser/server is inserting.  On
        // Win32, it's ^M^J for every newline.
        char *copy = (char*) malloc(strlen(inputText) * 2);
        char *src = inputText;
        char *dst = copy;
        while (*src) {
            if (*src < ' ') {
                *dst++ = '^';
                *dst++ = 'A'+(*src)-1;
            } else {
                *dst++ = *src;
            }
            ++src;
        }
        *dst = 0;
        free(inputText);
        inputText = copy;
#endif
    }
    return inputText;
}

/**
 * Return the first transliterator.  If it has not been created yet, then
 * create it.  If there is a problem, return 0 and place an explanatory message
 * in constructorError.
 */
Transliterator* TransliteratorCGI::getTranslit1(UnicodeString& constructorError) {
    if (translit1 == 0) {
        UnicodeString id(getParamValue("ARG_ID", NULL_ID), ENCODING);
        // We ALWAYS prepend an invisible Hex-Any transliterator.  This
        // just makes it possible for the user to type hex escapes in the
        // input area.
        id.insert(0, UnicodeString("Hex-Any;", ""));
        loadUserTransliterators();
        translit1 = Transliterator::createInstance(id);
        if (translit1 == 0) {
            constructorError = UnicodeString("Error: Unable to create transliterator \"", "");
            id = UnicodeString(getParamValue("ARG_ID", NULL_ID), ENCODING);
            constructorError += id;
            constructorError += "\"";
        }
    }
    return translit1;
}

/**
 * Return the second transliterator.  If it has not been created yet, then
 * create it.  If there is a problem, return 0 and place an explanatory message
 * in constructorError.
 */
Transliterator* TransliteratorCGI::getTranslit2(UnicodeString& constructorError) {
    if (translit2 == 0) {
        loadUserTransliterators();
        UnicodeString id(getParamValue("ARG_ID2", NULL_ID), ENCODING);
        UTransDirection dir = UTRANS_FORWARD;
        if (strcmp(getParamValue("ARG_DIRECTION2", ""), "REVERSE") == 0) {
            dir = UTRANS_REVERSE;
        }
        translit2 = Transliterator::createInstance(id, dir);
        if (translit2 == 0) {
            constructorError = UnicodeString("Error: Unable to create transliterator \"", "");
            constructorError += id;
            constructorError += "\"";
        }
    }
    return translit2;
}

/**
 * Return the intermediate text.  Compute it if needed, or fetch it
 * from the cache.
 */
UnicodeString& TransliteratorCGI::getIntermediate() {
    if (!isIntermediateInitialized) {
        isIntermediateInitialized = true;
        UnicodeString input(getInputText(), ENCODING), constructorError;
        Transliterator *trans = getTranslit1(constructorError);
        if (trans == 0) {
            isIntermediateBogus = true;
            intermediate = constructorError;
        } else {
            intermediate = input;
            trans->transliterate(intermediate);
        }
    }
    return intermediate;
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
        sprintf(buf, "Syntax error in the rule \"");
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
