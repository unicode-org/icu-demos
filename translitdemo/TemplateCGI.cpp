#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "TemplateCGI.h"
#include "util.h"

extern "C" char **getcgivars();

TemplateCGI::TemplateCGI() {
    // Need to add some error checking -- getcgivars() can die
    cgiParams = getcgivars();
    cgiParamCount = 0;
    while (cgiParams[2*cgiParamCount] != NULL) {
        cgiParamCount++;
    }
#ifdef DEBUG
    for (int i=0; i<cgiParamCount; ++i) {
        debugLog += "param ";
        debugLog += (UChar)('0'+i);
        debugLog += ":";
        debugLog += getParamKey(i);
        debugLog += "=";
        debugLog += getParamValue(i);
        debugLog += "<br>\n";
    }
#endif
}

TemplateCGI::~TemplateCGI() {
    for (int i=0; cgiParams[i] != NULL; ++i) {
        free(cgiParams[i]);
        cgiParams[i] = NULL;
    }
    free(cgiParams);
}

const char* TemplateCGI::getParamKey(int i) {
    return (const char*) cgiParams[2*i];
}

const char* TemplateCGI::getParamValue(int i) {
    return (const char*) cgiParams[(2*i)+1];
}

const char* TemplateCGI::getParamValue(const char* key, const char* defValue) {
    for (int i=0; i<cgiParamCount; ++i) {
        if (strcmp(key, cgiParams[2*i]) == 0) {
#ifdef DEBUG
            debugLog += "getParamValue(";
            debugLog += key;
            debugLog += ")=";
            debugLog += (UChar) ('0' + i);
            debugLog += ":";
            debugLog += cgiParams[(2*i)+1];
            debugLog += "<br>\n";
#endif
            return (const char*) cgiParams[(2*i)+1];
        }
    }
#ifdef DEBUG
    debugLog += "getParamValue(";
    debugLog += key;
    debugLog += ")=default:";
    debugLog += defValue;
    debugLog += "<br>\n";
#endif
    return defValue;
}

int TemplateCGI::getParamCount() {
    return cgiParamCount;
}

void TemplateCGI::run(FILE* out) {

    handleEmitHeader(out);

    FILE *templateFile = fopen(getTemplateFile(), "r");
    if (templateFile == NULL) { die("Can't open template"); }
    processTemplate(templateFile, out);

#ifdef DEBUG
    // Debugging
    fseek(templateFile, 0, SEEK_SET);
    FILE *log = fopen(DEBUG_HTML_FILE, "w");
    if (log != NULL) {
        handleEmitHeader(log);
        processTemplate(templateFile, log);
        fclose(log);
    }
#endif

    fclose(templateFile);
}

void TemplateCGI::handleEmitHeader(FILE* out) {
	fprintf(out, "Content-type: text/html\n\n");
}

/**
 * TemplateCGI framework method default implementation.
 */
void TemplateCGI::handleTemplateVariable(FILE* out, const char* var,
                                         bool inQuote) {
    util_fprintf(out, getParamValue(var, ""), inQuote);
}

void TemplateCGI::processTemplate(FILE* in, FILE* out) {
    char last = 0;
    for (;;) {
        char c = getc(in);
        if (c == EOF) {
            break;
        } else if (c == '$') {
            char varName[257]; // Keep var names shorter than 256!
            int len=0;
            for (;len<256;) {
                c = getc(in);
                if ((c >= '0' && c <= '9') ||
                    (c >= 'a' && c <= 'z') ||
                    (c >= 'A' && c <= 'Z') ||
                    (c == '_')) {
                    varName[len++] = c;
                } else {
                    break;
                }
            }
            varName[len] = 0;

            if (len == 0) {
                // Not a variable, so output the '$' literal
                fprintf(out, "$");
                // Replace the subsequent char unless it is '$',
                // that is, "$$" => "$".
                if (c != '$') {
                    ungetc(c, in); // ungetc ignores EOF
                }
            } else {
                ungetc(c, in); // ungetc ignores EOF
                handleTemplateVariable(out, varName, last=='"');
            }
        } else {
            fprintf(out, "%c", c);
            last = c;
        }
    }
}

void TemplateCGI::die(const char* msg) {
    printf("<h1>Fatal Error</h1><p>");
    printf(msg);
    exit(0);
}
