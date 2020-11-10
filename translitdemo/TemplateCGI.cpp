/*
*******************************************************************************
*
*   Copyright (C) 2003-2006, International Business Machines
*   Corporation and others.  All Rights Reserved.
*
*******************************************************************************
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "TemplateCGI.h"
#include "util.h"
#include "demo_settings.h"
#include "demoutil.h"
#include <ctype.h>

#ifdef _WIN32
  #include <direct.h>
  #define getcwd _getcwd
#else
  #include <unistd.h>
#endif

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
    if(getenv(key) && *getenv(key))
    {
      return getenv(key);
    }

    return defValue;
}

int TemplateCGI::getParamCount() {
    return cgiParamCount;
}

void TemplateCGI::run(FILE* out) {
    handleEmitHeader(out);
    const char *fileName = getTemplateFile();
    for(int i=0;fileName[i];i++)
    {
        if(!isprint(fileName[i])) /* reject nonprinting chars */
        {
            die("Bad template name.");
        }
    }
    if(fileName[0]=='/')
    {
        die("Bad template name.");
    }
    if(strstr(fileName,"..")) /* reject dot-dots */
    {
        die("Bad template name.");
    }

    FILE *templateFile = fopen(getTemplateFile(), "r");

    if (templateFile == NULL) {
        char buf[256], cwd[256] = "";
#ifdef DEBUG
        getcwd(cwd, sizeof(cwd));  /* finding CWD could help compromise us */
#endif
        sprintf(buf, "Can't open template \"%s\" in dir \"%s\"",
                getTemplateFile(),
                cwd);
        die(buf);
    }
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
    fprintf(out, "Content-Type: text/html; charset=UTF-8\n\n");
}

/**
 * TemplateCGI framework method default implementation.
 */
void TemplateCGI::handleTemplateVariable(FILE* out, const char* var,
                                         UBool inQuote)
{
    if (strcmp(var, "DEMO_COMMON_MASTHEAD") == 0) {
        printHTMLFragment(NULL, NULL, DEMO_COMMON_MASTHEAD);
    }
    if (strcmp(var, "DEMO_BEGIN_LEFT_NAV") == 0) {
        fputs(DEMO_BEGIN_LEFT_NAV, out);
    }
    if (strcmp(var, "DEMO_COMMON_LEFTNAV") == 0) {
        printHTMLFragment(NULL, NULL, DEMO_COMMON_LEFTNAV);
    }
    if (strcmp(var, "DEMO_END_LEFT_NAV") == 0) {
        fputs(DEMO_END_LEFT_NAV, out);
    }
    if (strcmp(var, "DEMO_BEGIN_CONTENT") == 0) {
        fputs(DEMO_BEGIN_CONTENT, out);
    }
    if (strcmp(var, "DEMO_END_CONTENT") == 0) {
        fputs(DEMO_END_CONTENT, out);
    }
    if (strcmp(var, "DEMO_COMMON_FOOTER") == 0) {
        printHTMLFragment(NULL, NULL, DEMO_COMMON_FOOTER);
    }
    else {
        util_fprintf(out, getParamValue(var, ""), inQuote);
    }
}

size_t TemplateCGI::getWord(FILE* in, char* buf, size_t buflen) {
    char c;
    size_t len=0;
    for (;len<(buflen-1);) {
        c = getc(in);
        if ((c >= '0' && c <= '9') ||
            (c >= 'a' && c <= 'z') ||
            (c >= 'A' && c <= 'Z') ||
            (c == '_')) {
            buf[len++] = c;
        } else {
            break;
        }
    }
    buf[len] = 0;
    ungetc(c, in); // ungetc ignores EOF
    return len;
}

//+ Here is some untested code for implementing a #include facility,
//+ with this syntax:
//+
//+  $:include:filename_to_include.txt:
//+
//+ This would provide an alternative to using the Javascript SRC directive
//+ and MIGHT let us run on non Javascript-savvy http servers.  Currently
//+ we only run on Apache (on Unix or Win32) so we don't need this.

//+size_t TemplateCGI::getUntil(FILE* in, char delim, char* buf, size_t buflen) {
//+    char c;
//+    size_t len=0;
//+    for (;len<(buflen-1);) {
//+        c = getc(in);
//+        if (c != delim) {
//+            buf[len++] = c;
//+        } else {
//+            break;
//+        }
//+    }
//+    buf[len] = 0;
//+    return len;
//+}
//+
//+void TemplateCGI::copyFile(FILE* out, const char* filename) {
//+    FILE *file = fopen(filename, "r");
//+    if (file != NULL) {
//+        char c;
//+        while ((c = getc(file)) != EOF) {
//+            putc(out, c);
//+        }
//+        fclose(file);
//+    }
//+}

void TemplateCGI::processTemplate(FILE* in, FILE* out) {
    char last = 0;
    char buf[256];
    for (;;) {
        int c = getc(in);
        if (c == EOF) {
            break;
        } else if (c == '$') {
//+            c = getc(in);
//+            if (c == ':') {
//+                getUntil(in, ':', buf, sizeof(buf));
//+                if (strcmp(buf, "include") == 0) {
//+                    getUntil(in, ':', buf, sizeof(buf));
//+                    copyFile(out, buf);
//+                }
//+            }
            size_t len = getWord(in, buf, sizeof(buf));
            if (len == 0) {
                // Not a variable.  Convert $$ -> $ or
                // isolated $ -> $.
                putc('$', out);
                c = getc(in);
                if (c != EOF && c != '$') {
                    ungetc(c, in);
                }
            } else {
                handleTemplateVariable(out, buf, last=='"');
            }
        } else {
            fprintf(out, "%c", c);
            last = c;
        }
    }
}

void TemplateCGI::die(const char* msg) {
    printf("<h1>Fatal Error</h1><p>");
    puts(msg);
    exit(0);
}

//eof
