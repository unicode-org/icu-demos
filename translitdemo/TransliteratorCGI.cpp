
#include "TransliteratorCGI.h"
#include "unicode/translit.h"
#include "unicode/ustring.h"
#include "unicode/rbt.h"
#include <stdio.h>
#include <stdlib.h> // qsort (Linux)
#include <string.h> // strncmp (Linux)

// Encoding we use for interchange to/from the browser
#define ENCODING "UTF8"

// Special transliterator IDs
#define NULL_ID "Null"
#define COMPOUND_ID  "(Compound)"
#define INVERSE_ID "(Inverse)"

// emitAvailableIDs flags
enum { WITH_COMPOUND = 1, WITH_INVERSE = 2 };

static int _compareIDs(const void* arg1, const void* arg2);

TransliteratorCGI::TransliteratorCGI() {
    inputText = 0;
    availableIDs = 0;
    availableIDsCount = 0;
    isIntermediateInitialized = false;
    isIntermediateBogus = false;
    areUserTransliteratorsLoaded = false;
    translit1 = 0;
    translit2 = 0;
}

TransliteratorCGI::~TransliteratorCGI() {
    delete inputText;

    for (int i=0; i<availableIDsCount; ++i) {
        delete[] availableIDs[i];
    }
    delete[] availableIDs;

    delete translit1;
    delete translit2;
}

/**
 * Return the input text in its original encoding.  This is computed
 * the first time and cached thereafter.
 */
const char* TransliteratorCGI::getInputText() {
    if (inputText == 0) {
        const char* s = getParamValue("INPUT_TEXT");
        inputText = strdup(s ? s : "");
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
    }
    return inputText;
}

const char* TransliteratorCGI::getID1() {
    return getParamValue("TRANSLITERATOR_ID_1", NULL_ID);
}

const char* TransliteratorCGI::getID2() {
    return getParamValue("TRANSLITERATOR_ID_2", INVERSE_ID);
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
 */
void TransliteratorCGI::handleTemplateVariable(FILE* out, const char* var) {

    if (strcmp(var, "ids1") == 0) {
        emitAvailableIDs(out, getID1(), WITH_COMPOUND);
    }

    else if (strcmp(var, "ids2") == 0) {
        emitAvailableIDs(out, getID2(), WITH_COMPOUND | WITH_INVERSE);
    }

    else if (strcmp(var, "input") == 0) {
        fprintf(out, "%s", getInputText());
    }

    else if (strcmp(var, "out1") == 0) {
        extractTo(out, getIntermediate());
    }

    else if (strcmp(var, "out2") == 0) {
        UnicodeString temp, constructorError;
        Transliterator *trans = getTranslit2(constructorError);
        if (trans == 0) {
            extractTo(out, constructorError);
        } else {
            transliterate(trans, getIntermediate(), temp);
            if (!isIntermediateBogus) {
                extractTo(out, temp);
            }
        }
    }

    else if (strcmp(var, "compound1") == 0) {
        if (useCompound1()) {
            fprintf(out, getParamValue("COMPOUND_ID_1", ""));
        } else {
            char* id = createChars(getEffectiveTranslitID1());
            fprintf(out, "%s", id);
            delete[] id;
        }
    }

    else if (strcmp(var, "compound2") == 0) {
        // If Use inverse is selected then compound2 is the inverse of compound1;
        // otherwise it copies its original state.
        if (useInverse()) {
            UnicodeString constructorError;
            Transliterator* trans = getTranslit2(constructorError);
            if (trans != 0) {
                char* id = createChars(trans->getID());
                fprintf(out, "%s", id);
                delete[] id;
            }
        } else {
            fprintf(out, getParamValue("COMPOUND_ID_2", ""));
        }
    }

    // [translit_rule_edit]
    else if (strcmp(var, "MODE") == 0) {
        const char* mode = getParamValue("MODE", "");
        if (*mode) {
            // BUILD mode indicates that the window has been
            // initialized and the user has pressed the SUBMIT button.
            // We check the rules, and if they are valid, regenerate
            // the page with BUILD_OK mode.  If the rules are not
            // valid, we enter BUILD_ERROR mode.  When the JavaScript
            // detects BUILD_OK mode it submits the rules to the
            // parent page.  When the JavaScript detects BUILD_ERROR
            // mode it does a UI thing to indicate the error, and then
            // changes the mode back to BUILD.
            if (strcmp(mode, "BUILD") == 0) {
                // Get the rules and try to build them.
                UnicodeString id; // Not really used
                Transliterator *t = buildUserRules(id, "RULES");
                fprintf(out, "%s", (t==0) ? "BUILD_ERROR" : "BUILD_OK");
                delete t;
            }
        }
    }

    // The cgiStdout variable is used to return error information
    // etc. to the browser.
    else if (strcmp(var, "stdout") == 0) {
        char* s = createChars(cgiStdout);
        fprintf(out, "%s", s);
        delete[] s;
    }

    else if (strcmp(var, "icuversion") == 0) {
        char str[U_MAX_VERSION_STRING_LENGTH];
        UVersionInfo version;
        u_getVersion(version);
        u_versionToString(version, str);
        fprintf(out, "%s", str);
    }

#ifdef DEBUG
    else if (strcmp(var, "debug") == 0) {
        extractTo(out, debugLog);
    }
#endif

    else {
        TemplateCGI::handleTemplateVariable(out, var);
    }
}

/**
 * Return true if "Use inverse" is checked.
 */
bool TransliteratorCGI::useInverse() {
    // We used to have an explicit Inverse checkbox, so we did this:
    //|return *getParamValue("USE_INVERSE", "") != 0;
    // But now we have an INVERSE_ID in the second pop-up
    return strcmp(getID2(), INVERSE_ID) == 0;
}

/**
 * Return true if "Compound" box #1 is checked.
 */
bool TransliteratorCGI::useCompound1() {
    // We used to have an explicit Compound checkbox, so we did this:
    //|return *getParamValue("USE_COMPOUND_ID_1", "") != 0;
    // But now we have a COMPOUND_ID in the first pop-up
    return strcmp(getID1(), COMPOUND_ID) == 0;
}

/**
 * Return true if "Compound" box #2 will be checked.  This can
 * occur if the box is actually checked or if "Use inverse" is
 * checked and box #1 is checked.
 */
bool TransliteratorCGI::useCompound2() {
    // We used to have an explicit Compound checkbox, so we did this:
    //|// If Use inverse is selected then useCompound2 mirrors useCompound1;
    //|// otherwise it copies its original state.
    //|const char* tag = "USE_COMPOUND_ID_2";
    //|if (useInverse()) {
    //|    tag = "USE_COMPOUND_ID_1";
    //|}
    //|return *getParamValue(tag, "") != 0;
    // But now we have a COMPOUND_ID in the second pop-up
    return strcmp(getID2(), COMPOUND_ID) == 0;
}

/**
 * Return the effective ID of transliterator 1.
 */
UnicodeString TransliteratorCGI::getEffectiveTranslitID1() {
    UnicodeString id;
    if (useCompound1()) {
        const char *raw = getParamValue("COMPOUND_ID_1", NULL_ID);
        char *toDelete = copyAndCleanTranslitID(raw);
        id = UnicodeString(toDelete, ENCODING);
        delete[] toDelete;
    } else {
        id = getID1();
    }
    return id;
}

/**
 * Return the first transliterator.  If it has not been created yet, then
 * create it.  If there is a problem, return 0 and place an explanatory message
 * in constructorError.
 */
Transliterator* TransliteratorCGI::getTranslit1(UnicodeString& constructorError) {
    char *toDelete = 0;
    if (translit1 == 0) {
        UnicodeString id = getEffectiveTranslitID1();
        // We ALWAYS prepend an invisible Hex-Unicode transliterator.  This
        // just makes it possible for the user to type hex escapes in the
        // input area.
        id.insert(0, UnicodeString("Hex-Unicode;", ""));
        loadUserTransliterators();
        translit1 = Transliterator::createInstance(id);
        if (translit1 == 0) {
            constructorError = UnicodeString("Error: Unable to create transliterator \"", "");
            constructorError += getEffectiveTranslitID1();
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
        if (useInverse()) {
            UnicodeString id1 = getEffectiveTranslitID1();
            Transliterator *t = Transliterator::createInstance(id1);
            if (t != 0) {
                translit2 = t->createInverse();
                delete t;
                if (translit2 == 0) {
                    constructorError = UnicodeString("Error: Unable to create inverse of \"", "");
                    constructorError += id1;
                    constructorError += "\"";
                }
            }
        }
        else {
            UnicodeString id;
            if (useCompound2()) {
                const char *raw = getParamValue("COMPOUND_ID_2", NULL_ID);
                char *toDelete = copyAndCleanTranslitID(raw);
                id = UnicodeString(toDelete, ENCODING);
                delete[] toDelete;
            } else {
                id = getID2();
            }
            translit2 = Transliterator::createInstance(id);
            if (translit2 == 0) {
                constructorError = UnicodeString("Error: Unable to create transliterator \"", "");
                constructorError += id;
                constructorError += "\"";
            }
        }
    }
    return translit2;
}

/**
 * Given a transliterator ID (which may be compound, and may contain
 * inline UnicodeSet patterns), clean it of extraneous whitespace.
 * Return a new ID with no whitespace (other than that embedded in
 * UnicodeSet patterns).  User owns and must delete result!
 */
char* TransliteratorCGI::copyAndCleanTranslitID(const char* id) {
    char* p = new char[strlen(id) + 1];
	char* result = p;
    int unicodeStringPatDepth = 0;
    bool inEscape = false;
    if (p != 0) {
        for (; *id; ++id) {
            char c = *id;
            if (inEscape) {
                *p++ = c;
                inEscape = false;
                continue;
            }
            if (unicodeStringPatDepth) {
                *p++ = c;
                if (c == '\\') {
                    inEscape = true;
                }
                else if (c == '[') {
                    ++unicodeStringPatDepth;
                }
                else if (c == ']') {
                    --unicodeStringPatDepth;
                }
            } else {
                if (c == ' ' || c == '\t' || c == '\n' || c == '\r') {
                    continue;
                }
                *p++ = c;
                if (c == '[') {
                    unicodeStringPatDepth = 1;
                }
            }
        }
        // If unicodeStringPatDepth is != 0 or inEscape is true here, then
        // the id is bogus.
        *p = 0;
    }
    return result;
}

///**
// * Trim the spaces from str and return a newly allocated string
// * copy.
// */
//char* TransliteratorCGI::copyTrim(const char* str) {
//    char* p = new char[strlen(str) + 1];
//	char* result = p;
//    if (p != 0) {
//        while (*str) {
//            if (*str != ' ' && *str != '\t' &&
//                *str != '\n' && *str != '\r') {
//                *p++ = *str;
//            }
//            ++str;
//        }
//        *p = 0;
//    }
//    return result;
//}

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
            transliterate(trans, input, intermediate);
        }
    }
    return intermediate;
}

/**
 * Transliterate the given input string using a transliterator with
 * the given id.  Store the output in 'output'.
 */
void TransliteratorCGI::transliterate(Transliterator* trans,
                                      const UnicodeString& input,
                                      UnicodeString& output) {
    // assert(trans != 0)
    output = input;
    trans->transliterate(output);
}

/**
 * Given a tag (indicating a CGI param) retrieve the rules from that
 * tag and attempt to build them.  Return the resulting
 * transliterator, or NULL on failure.  On failure, put the relevant
 * error message in cgiStdout.
 *
 * Caller owns the result if it is non-NULL.
 */
Transliterator* TransliteratorCGI::buildUserRules(const UnicodeString& id,
                                                  const char* tag) {
    UnicodeString rules(getParamValue(tag, ""), ENCODING);
    UParseError err;
    UErrorCode status = U_ZERO_ERROR;
    Transliterator *t = new RuleBasedTransliterator(id, rules,
                                                    UTRANS_FORWARD,
                                                    0, err, status);
    if (U_FAILURE(status)) {
        char buf[256];
        sprintf(buf, "Syntax error in the rule \"");
        cgiStdout += buf;
        cgiStdout += err.preContext;
        cgiStdout += "\" ";
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

    // Currently we support only one user-defined RBT, in the hidden
    // fields USER_RULE and USER_RULE_ID.
    UnicodeString id(getParamValue("USER_RULE_ID", ENCODING));
    if (id.length() > 0) {
        Transliterator *t = buildUserRules(id, "USER_RULE");
        if (t != 0) {
            UErrorCode status = U_ZERO_ERROR;
            Transliterator::registerInstance(t, status);
        }
    }

    areUserTransliteratorsLoaded = true;
}

/**
 * Given a UnicodeString, extract its contents to a char* buffer using
 * ENCODING and return it.  Caller must delete returned pointer.
 * Return value will be 0 if out of memory.
 */
char* TransliteratorCGI::createChars(const UnicodeString& str) {
    // Preflight for size
    int32_t len = str.extract(0, str.length(), NULL, 0, ENCODING) + 16;
    char* charBuf = new char[len];
    if (charBuf != 0) {
        len = str.extract(0, str.length(), charBuf, len, ENCODING);
        charBuf[len] = 0;
    }
    return charBuf;
}

/**
 * Send the given UnicodeString to 'out' using ENCODING.
 */
void TransliteratorCGI::extractTo(FILE* out, const UnicodeString& str) {
    char* charBuf = createChars(str);
    if (charBuf == 0) {
        fprintf(out, "Error: Out of memory");
        return;
    }
    fprintf(out, "%s", charBuf);
    delete[] charBuf;
}

/**
 * Emit a list of available IDs as OPTION elements, with the given
 * ID selected.
 * @flags WITH_INVERSE or WITH_COMPOUND or both or neither
 */
void TransliteratorCGI::emitAvailableIDs(FILE* out, const char* selectedID,
                                         int flags) {
    int i;
    char** ids = getAvailableIDs();
    if (ids == 0) {
        fprintf(out, "Error: Out of memory");
        return;
    }

    // fprintf(out, "<!-- Selected: %s -->", selectedID);

    /*
    int selectedIndex = -1;
    // Check to see if no IDs match selectedID -- in that case
    // we want to select NULL_ID.
    for (i=0; i<availableIDsCount; ++i) {
        if (strcmp(selectedID, availableIDs[i]) == 0) {
            selectedIndex = i;
            break;
        }        
    }

    // This section could be eliminated -- all it does is make it so
    // that if the selectedID does not exist in our list, we select
    // Null.  If we don't care about this, we can delete this loop and
    // put the check (that happens above) directly into the loop
    // below.
    if (selectedIndex < 0) {
        for (i=0; i<availableIDsCount; ++i) {
            if (strcmp(NULL_ID, availableIDs[i]) == 0) {
                selectedIndex = i;
                break;
            }        
        }
    }
    */

    // fprintf(out, "<!-- Selected: %s -->", selectedID);

    // Output the HTML
    // Use index i==-2 to be <Inverse> and i==-1 to be <Compound>
    for (i=-2; i<availableIDsCount; ++i) {
        char *option = NULL;
        if (i==-2) {
            if (flags & WITH_INVERSE) {
                option = INVERSE_ID;
            } else {
                continue;
            }
        } else if (i==-1) {
            if (flags & WITH_COMPOUND) {
                option = COMPOUND_ID;
            } else {
                continue;
            }
        } else {
            option = ids[i];
        }
        //fprintf(out, "<OPTION%s>%s</OPTION>\n",
        //        selectedIndex == i ? " SELECTED" : "",
        //        option);
        fprintf(out, "<OPTION%s>%s</OPTION>\n",
                strcmp(option, selectedID)==0 ? " SELECTED" : "",
                option);
    }
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
    loadUserTransliterators();
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
