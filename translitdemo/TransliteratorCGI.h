#include "TemplateCGI.h"
#include "unicode/utypes.h"
#include "unicode/unistr.h"

class Transliterator;

class TransliteratorCGI : public TemplateCGI {

 public:

    TransliteratorCGI();

    virtual ~TransliteratorCGI();

 protected:

    // Implement TemplateCGI
    const char* getTemplateFile();

    // Implement TemplateCGI
    void handleTemplateVariable(FILE* out, const char* var);

    // Override TemplateCGI
    void handleEmitHeader(FILE *out);

 private:

    UnicodeString& getIntermediate();

    void emitAvailableIDs(FILE* out, const char* selectedID, int flags);

    void transliterate(Transliterator* trans, const UnicodeString& input,
                       UnicodeString& output);

    const char* getInputText();

    Transliterator* getTranslit1(UnicodeString& constructorError);
    Transliterator* getTranslit2(UnicodeString& constructorError);

    char** getAvailableIDs();

    const char* getID1();
    const char* getID2();

    bool useInverse();
    bool useCompound1();
    bool useCompound2();

    UnicodeString getEffectiveTranslitID1();

    Transliterator* buildUserRules(const UnicodeString& id, const char* tag);
    void loadUserTransliterators();

    // Utility methods
    static char* createChars(const UnicodeString& str);
    static void extractTo(FILE* out, const UnicodeString& str);
    //static char* copyTrim(const char* str);
    static char* copyAndCleanTranslitID(const char* id);

    // Cached alias; returned by getInputText()
    char* inputText;

    // Cached available IDs list
    char** availableIDs;
    int32_t availableIDsCount;

    // Cached transliterators
    Transliterator* translit1;
    Transliterator* translit2;

    bool isIntermediateInitialized;
    bool isIntermediateBogus;
    UnicodeString intermediate;

    bool areUserTransliteratorsLoaded;

    UnicodeString cgiStdout;
};
