#include <stdio.h>

// Define the symbol DEBUG here to enable debugging output
//#define DEBUG

// The name of the debug output file (used if DEBUG is defined)
#define DEBUG_HTML_FILE "translitdebug.html"

#ifdef DEBUG
#include "unicode/unistr.h"
#endif

/**
 * A template-based CGI framework.
 */
class TemplateCGI {

    char** cgiParams;
    int cgiParamCount;

 protected:

    const char* getParamKey(int i);
    const char* getParamValue(int i);
    int getParamCount();

    /**
     * Return the value of the given key, or 'defValue' if the key does
     * not exist.
     */
    const char* getParamValue(const char* key, const char* defValue=NULL);

 public:

    TemplateCGI();

    virtual ~TemplateCGI();

    virtual void run(FILE* out);

    void run() { run(stdout); }

 protected:

    /**
     * Subclasses muse override this method to return the local path
     * to the template file.  A common implementation is to return the
     * value of a hidden field in the template file itself, allowing
     * the CGI to handle multiple template files.  If the hidden field
     * is not defined, then the default start template is returned.
     */
    virtual const char* getTemplateFile() = 0;

    /**
     * Subclasses should override this method to fprintf() to 'out'
     * the text to be substituted for variable number
     * 'variableNumber'.  The default implementation supplied by this
     * class simply outputs the value of the CGI variable of the same
     * name.  That is, $FOO is filled in with the value of
     * getParamValue("FOO", "").  Subclasses may invoke the default
     * implemenation using TemplateCGI::handleTemplateVariable().
     *
     * @param inQuote if TRUE, then this variable is being output
     * into a double-quoted string.  We detect this only in the case
     * of "$FOO..."; that is, we only look at the immediately preceding
     * character.  The JavaScript code must be aware of this.  When
     * in a double-quoted string, double quotes, backslashes, newlines,
     * and so on will be escaped properly.
     */
    virtual void handleTemplateVariable(FILE* out, const char* variableName,
                                        bool inQuote);
    
    /**
     * Subclasses may override this to specify an HTTP header.  The
     * default header is "Content-type: text/html\n\n";
     */
    virtual void handleEmitHeader(FILE* out);

    virtual void die(const char*);

    virtual void processTemplate(FILE* in, FILE* out);

    static size_t getWord(FILE* in, char* buf, size_t buflen);

#ifdef DEBUG
    UnicodeString debugLog;
#endif
};
