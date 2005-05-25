/*
*******************************************************************************
*
*   Copyright (C) 2003-2005, International Business Machines
*   Corporation and others.  All Rights Reserved.
*
*******************************************************************************
*   file name:  nbrowser.cpp
*   encoding:   US-ASCII
*   tab size:   8 (not used)
*   indentation:4
*
*   created on: 2003feb16
*   created by: Markus W. Scherer
*
*   This is a fairly crude, un-internationalized cgi for a "Normalization Browser".
*   It demonstrates various normalization forms and options and their
*   effects on user-provided input strings.
*
*   This code works only if compiled and run with an ASCII-based charset!
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "unicode/utypes.h"
#include "unicode/uchar.h"
#include "unicode/ustring.h"
#include "unicode/unistr.h"
#include "unicode/unorm.h"
#include "unormimp.h"           // ### TODO internal file, for normalization prototype
#include "demo_settings.h"

#define LENGTHOF(array) (sizeof(array)/sizeof((array)[0]))

static const char *htmlHeader=
    "Content-Type: text/html; charset=utf-8\n"
    "\n"
    "<html lang=\"en-US\">\n"
    "<head>\n";

static const char endHeaderBeginBody[] =
    "</head>\n"
    "<body>\n";

static const char breadCrumbMainHeader[]=
    DEMO_BREAD_CRUMB_BAR
    "<h1>Normalization Browser</h1>\n";

static const char defaultHeader[]=
    "<meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\">\n"
    "<title>Normalization Browser</title>\n";

static const char *htmlFooter=
    "</body>";

static const char *helpText=
    "<a name=\"help\"></a><h2>About this demo</h2>\n"
    "<p>The options flags are for a prototype "
    "to demonstrate tailored normalization as mentioned as\n"
    "<a href=\"http://www.unicode.org/review/\">Unicode public review</a> issue 7.\n"
    "Uncheck all of these options for regular <a href=\"http://www.unicode.org/reports/tr15/\">Unicode Normalization</a>.</p>\n"
    "<p>Hangul excludes AC00..D7A3. CJK Compat. excludes CJK Compatibility Ideographs (those with a canonical decomposition).</p>\n"
    "\n"
    "<p>The Unicode 3.2 option performs normalization according to Unicode 3.2 (except for NormalizationCorrections) "
    "even if ICU otherwise supports a higher version.\n</p>"
    "\n"
    "<p>FCD is not a normalization form but a test for whether\n"
    "text is canonically ordered. \"Normalizing to FCD\" does not generate\n"
    "a unique form but only one of potentially many that are canonically ordered.\n"
    "See <a href=\"http://www.unicode.org/notes/tn5/\">UTN #5 Canonical Equivalence in Applications</a>.</p>"
    "<hr>";

static const char *inputError="<p>Error parsing the input string: %s</p>\n";

static const char *emptyString="(empty)";

static const char *startString="%04x";

static const char *midString=" %04x";

static const char *endString="";

static const char *startForm=
    "<form method=\"GET\" action=\"%s\">\n"
    "<p>Input string: Enter a string with \\uhhhh and \\Uhhhhhhhh escapes<br>\n"
    "<input size=\"80\" name=\"t\" value=\"%s\"><br>\n"
    "or enter code points (e.g. 0061 0308 0304 ac01 f900 50000)<br>\n"
    "<input size=\"80\" name=\"s\" value=\"";

static const char *endForm=
    "\"></p>\n"
    "<p>Decomposition exclusions:"
    " <input type=\"checkbox\" name=\"op0\" %s> Hangul"
    " <input type=\"checkbox\" name=\"op1\" %s> CJK Compat."
    "<br>\n"
    "Unicode version:"
    " <input type=\"radio\" name=\"uv\" value=\"0\" %s>current"
    " <input type=\"radio\" name=\"uv\" value=\"1\" %s>Unicode 3.2"
    "<br>\n"
    "<input type=\"image\" src=\"//www.ibm.com/i/v14/buttons/us/en/submit.gif\" alt=\"Submit\" value=\"Submit\" />\n"
    " (<a href=\"#help\">Help</a>)"
    "</p>\n"
    "</form>\n";

static const char *startTable=
    "<table border=\"0\" cellspacing=\"0\" cellpadding=\"0\" class=\"data-table-2\">\n"
    "<caption><em>Normalization Results</em></caption>\n"
    "<tr><th>Mode</th><th>Quick Check</th><th>Normalized</th><th>Text</th></tr>\n"
    "<tr><td>Input</td><td>&nbsp;</td><td>";

static const char *endTable="</table>";

static const char *modeNames[]={ "(null)", "(None)", "NFD", "NFKD", "NFC", "NFKC", "FCD", "FCC" };

static const char *qcNames[]={ "NO", "YES", "MAYBE" };

static const char *versions=
    "<p>Unicode version %s &mdash; "
    "<a href=\"" ICU_URL "\">ICU</a> %s</p>\n";

static void
printString(const UChar *s, int32_t length) {
    UChar32 c;
    int32_t i;

    if(length<0) {
        length=u_strlen(s);
    }
    if(length==0) {
        printf(emptyString);
    } else {
        i=0;
        U16_NEXT(s, i, length, c);
        printf(startString, c);

        while(i<length) {
            U16_NEXT(s, i, length, c);
            printf(midString, c);
        }
        printf(endString);
    }
}

static void
printTableString(const UChar *s, int32_t length) {
    printf("<td>");
    printString(s, length);
    puts("</td>");
}

static void
printTableText(const UChar *s, int32_t length) {
    char buffer[1000];
    int32_t utf8Length;
    UErrorCode errorCode;

    errorCode=U_ZERO_ERROR;
    u_strToUTF8(buffer, sizeof(buffer), &utf8Length, s, length, &errorCode);
    if(U_FAILURE(errorCode) || errorCode==U_STRING_NOT_TERMINATED_WARNING) {
        printf("<td>%s</td>", u_errorName(errorCode));
    } else {
        printf("<td>%s</td>", buffer);
    }
}

static void
printNormalized(const UChar *s, int32_t length,
                UNormalizationMode mode, int32_t options) {
    UChar output[500];
    int32_t outputLength;
    UNormalizationCheckResult qc;
    UErrorCode errorCode;

    printf("<tr><td>%s</td>", modeNames[mode]);

    errorCode=U_ZERO_ERROR;
    qc=unorm_quickCheckWithOptions(s, length, mode, options, &errorCode);
    if(U_FAILURE(errorCode)) {
        printf("<td>%s</td>", u_errorName(errorCode));
    } else {
        printf("<td>%s</td>", qcNames[qc]);
    }

    errorCode=U_ZERO_ERROR;
    outputLength=unorm_normalize(s, length,
                                 mode, options,
                                 output, LENGTHOF(output),
                                 &errorCode);
    if(U_FAILURE(errorCode)) {
        printf("<td>%s</td><td>&nbsp;</td>", u_errorName(errorCode));
    } else {
        printTableString(output, outputLength);
        printTableText(output, outputLength);
    }

    puts("</tr>");
}

static inline int32_t
getHexValue(char c) {
    if('0'<=c && c<='9') {
        return (int32_t)(c-'0');
    } else if('a'<=c && c<='f') {
        return (int32_t)(c-'a'+10);
    } else if('A'<=c && c<='F') {
        return (int32_t)(c-'A'+10);
    } else {
        return -1;
    }
}

static const char *
skipEscapedWhitespace(const char *s) {
    while(*s==' ' || *s=='\t' || *s=='+') {
        ++s;
    }
    return s;
}

/* parse percent-escaped input into a byte array; does not NUL-terminate */
static int32_t
parseEscaped(const char *s, char *dest, int32_t destCapacity, UErrorCode &errorCode) {
    int32_t i, hi, lo;
    char c;

    i=0;
    while((c=*s++)!=0 && c!='&') {
        if(i==destCapacity) {
            errorCode=U_BUFFER_OVERFLOW_ERROR;
            return 0;
        }

        if(c=='%' && (hi=getHexValue(*s))>=0 && (lo=getHexValue(s[1]))>=0) {
            dest[i++]=(char)((hi<<4)|lo);
            s+=2;
        } else if(c=='+') {
            dest[i++]=' ';
        } else {
            dest[i++]=c;
        }
    }

    return i;
}

/* parse a list of hex code point values into the input[] string */
U_CAPI int32_t U_EXPORT2
parseString(const char *s,
            UChar *dest, int32_t destCapacity,
            UErrorCode &errorCode) {
    char *end;
    UChar32 value;
    int32_t length;
    UBool isError;

    length=0;
    isError=FALSE;
    for(;;) {
        s=skipEscapedWhitespace(s);
        if(*s=='&' || *s==0) {
            return length;
        }

        /* read one code point */
        value=(UChar32)strtol(s, &end, 16);
        if(end<=s || (*end!=' ' && *end!='\t' && *end!='+' && *end!='&' && *end!=0) || (uint32_t)value>=0x110000) {
            errorCode=U_PARSE_ERROR;
            return 0;
        }

        /* append it to the destination array */
        if(length>=destCapacity) {
            errorCode=U_BUFFER_OVERFLOW_ERROR;
            return 0;
        }
        U16_APPEND(dest, length, destCapacity, value, isError);
        if(isError) {
            errorCode=U_BUFFER_OVERFLOW_ERROR;
            return 0;
        }

        /* go to the following characters */
        s=end;
    }
}

/**
 * @returns 1 if open failed
 */
static int printTemplateFile(char *templateFileName) {
    size_t size = 0;
    size_t savedPos;
    char *buffer;
    FILE *templateFile = fopen(templateFileName, "r");
    
    if (templateFile == NULL) {
        printf("<!-- ERROR: %s cannot be opened -->\n", templateFileName);
        return 1;
    }   
    
    /* Go to the end, find the size, and go back to the beginning. */
    savedPos = ftell(templateFile);
    fseek(templateFile, 0, SEEK_END);
    size = ftell(templateFile);
    fseek(templateFile, savedPos, SEEK_SET);
    
    /* Read in the whole file and print it out */
    buffer = (char *)malloc(size+1);
    memset(buffer, 0, size+1);  // Make sure the whole buffer is NULL terminated
    fread(buffer, size, 1, templateFile);
    printf("%s", buffer);
    
    free(buffer);
    fclose(templateFile);
    return 0;
}

extern int
main(int argc, const char *argv[]) {
    UChar input[100], buffer16[600]; // buffer16 should be 6 times longer than input for \\uhhhh
    char input8[400];
    char buffer[1000];
    int32_t inputLength, options;
    UErrorCode errorCode;

    const char *cgi, *script;
    UBool inputIsUTF8;

    script=getenv("SCRIPT_NAME"); //"/cgi-bin/nbrowser"
    puts(htmlHeader);
    if (printTemplateFile(DEMO_COMMON_DIR "normalization-header.html")) {
        puts(defaultHeader);
    }
    puts(endHeaderBeginBody);
    printTemplateFile(DEMO_COMMON_MASTHEAD);
    puts(DEMO_BEGIN_LEFT_NAV);
    printTemplateFile(DEMO_COMMON_LEFTNAV);
    puts(DEMO_END_LEFT_NAV);
    puts(DEMO_BEGIN_CONTENT);
    puts(breadCrumbMainHeader);

    inputLength=options=0;
    inputIsUTF8=FALSE;
    errorCode=U_ZERO_ERROR;

    if((cgi=getenv("QUERY_STRING"))!=NULL && cgi[0]!=0) {
        // get input from cgi variable, e.g. t=a\\u0308%EA%B0%81&s=0061+0308&op1=on
        const char *in;

        if((in=strstr(cgi, "t="))!=NULL && in[2]!='&' && in[2]!=0) {
            inputIsUTF8=TRUE;
            in+=2; // skip "t="
            inputLength=parseEscaped(in, buffer, sizeof(buffer), errorCode);
            u_strFromUTF8(buffer16, LENGTHOF(buffer16), &inputLength,
                          buffer, inputLength,
                          &errorCode);
            UnicodeString us(FALSE, (const UChar *)buffer16, inputLength); // readonly alias
            inputLength=us.unescape().extract(input, LENGTHOF(input), errorCode);
            u_strToUTF8(input8, sizeof(input8), NULL,
                        input, inputLength,
                        &errorCode);
            if(errorCode==U_STRING_NOT_TERMINATED_WARNING) {
                errorCode=U_BUFFER_OVERFLOW_ERROR;
            }
        } else if((in=strstr(cgi, "s="))!=NULL && in[2]!='&' && in[2]!=0) {
            in+=2; // skip "s="
            inputLength=parseString(in, input, LENGTHOF(input), errorCode);
        } else {
            inputLength=0;
        }

        options=0;
        if(strstr(cgi, "op0=")!=NULL) {
            options|=1;
        }
        if(strstr(cgi, "op1=")!=NULL) {
            options|=2;
        }
        if(strstr(cgi, "op2=")!=NULL) {
            options|=4;
        }
        if(strstr(cgi, "uv=1")!=NULL) {
            options|=UNORM_UNICODE_3_2;
        }
    }

    if(U_FAILURE(errorCode)) {
        printf(inputError, u_errorName(errorCode));
        inputLength=0;
    }

    printf(startForm, script ? script : "", inputIsUTF8 ? input8 : "");
    if(!inputIsUTF8 && inputLength>0) {
        printString(input, inputLength);
    }
    printf(endForm,
        options&1 ? "checked" : "",
        options&2 ? "checked" : "",
        (options&0xe0)==0 ? "checked" : "",
        (options&0xe0)==UNORM_UNICODE_3_2 ? "checked" : "");

    printf(startTable);
    printString(input, inputLength);
    printf("</td>");
    printTableText(input, inputLength);
    puts("</tr>");

    printNormalized(input, inputLength, UNORM_NFD, options);
    printNormalized(input, inputLength, UNORM_NFC, options);
    printNormalized(input, inputLength, UNORM_NFKD, options);
    printNormalized(input, inputLength, UNORM_NFKC, options);
    printNormalized(input, inputLength, UNORM_FCD, options);

    puts(endTable);

    char uvString[16], ivString[16];
    UVersionInfo uv, iv;

    puts(helpText);

    u_getUnicodeVersion(uv);
    u_getVersion(iv);
    u_versionToString(uv, uvString);
    u_versionToString(iv, ivString);
    printf(versions, uvString, ivString);

    puts(DEMO_END_CONTENT);
    printTemplateFile(DEMO_COMMON_FOOTER);
    puts(htmlFooter);

    return 0;
}
