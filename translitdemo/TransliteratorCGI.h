#include "TemplateCGI.h"
#include "unicode/utypes.h"
#include "unicode/unistr.h"
#include "unicode/utrans.h"
#include "TextCache.h"

class Transliterator;

class TransliteratorCGI : public TemplateCGI {

 public:

    TransliteratorCGI();

    virtual ~TransliteratorCGI();

 protected:

    // Implement TemplateCGI
    const char* getTemplateFile();

    // Override TemplateCGI
    void handleEmitHeader(FILE *out);

    // Implement TemplateCGI
    void handleTemplateVariable(FILE* out, const char* var, bool inQuote);

 private:

    static char* cleanupNewlines(const char*);

    static bool buildUserRules(const UnicodeString& id,
                               const UnicodeString& rules,
                               UnicodeString& errMsg);

    static Transliterator* xCreateFromRules(const UnicodeString& id,
                                            const UnicodeString& rules,
                                            UTransDirection dir,
                                            UParseError& err,
                                            UErrorCode& status);

    void loadUserTransliterators();
    char** getAvailableIDs();
    char** getAvailableRBTIDs();

    static void handle_USER_IDS(int32_t i, const UnicodeString& key,
                                void* context);
    static void registerUserRules(int32_t i, const UnicodeString& id,
                                  void* /*ignoredContext*/);

 private:

    // Cached alias; returned by getInputText()
    char* inputText;

    // Cached available IDs list
    char** availableIDs;
    int32_t availableIDsCount;

    // Cached RBT available IDs list
    char** availableRBTIDs;
    int32_t availableRBTIDsCount;

    // Cached transliterators
    Transliterator* translit1;
    Transliterator* translit2;

    bool isIntermediateInitialized;
    bool isIntermediateBogus;
    UnicodeString intermediate;

    bool areUserTransliteratorsLoaded;

    // Map of IDs to rules.  Only validated rules are stored.
    TextCache ruleCache;
};
