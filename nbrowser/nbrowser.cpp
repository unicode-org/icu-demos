/*
*******************************************************************************
*
*   Copyright (C) 2003, International Business Machines
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
#include "uparse.h"             // toolutil library

#define LENGTHOF(array) (sizeof(array)/sizeof((array)[0]))

static const char *htmlHeader=
    "Content-Type: text/html; charset=utf-8\n"
    "\n"
    "<html lang=\"en-US\">\n"
    "<head>\n"
    "<meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\">\n"
    "<title>Normalization Browser</title>\n"
    "</head>\n"
    "<body>\n"
    "<a href=\"http://oss.software.ibm.com/icu/\">ICU</a> &gt;\n"
    "<a href=\"http://oss.software.ibm.com/icu/demo/\">Demo</a> &gt;<br>\n"
    "<h1>Normalization Browser</h1>\n";

static const char *htmlFooter=
    "</body>";

static const char *helpText=
    "<a name=\"help\"></a><h2>About this demo</h2>\n"
    "<p>The options flags are for a prototype "
    "to demonstrate tailored normalization as mentioned as\n"
    "<a href=\"http://www.unicode.org/review/\">Unicode public review</a> issue 7.\n"
    "Uncheck all of these options for regular <a href=\"http://www.unicode.org/reports/tr15/\">Unicode Normalization</a>.</p>\n"
    "<p>Hangul excludes AC00..D7A3. CJK Compat. excludes CJK Compatibility Ideographs (those with a canonical decomposition).\n"
    "A-umlaut excludes just U+00E4, which does not fulfill the closure condition for exclusions (try e.g. U+01DF).</p>\n"
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
    " <input type=\"checkbox\" name=\"op2\" %s> A-umlaut"
    " <input type=\"submit\" value=\"Go\">"
    " (<a href=\"#help\">Help</a>)"
    "</p>\n"
    "</form>\n";

static const char *startTable=
    "<table border=\"1\">\n"
    "<tr><th>Mode</th><th>quick check</th><th>normalized</th><th>text</th></tr>\n"
    "<tr><th>Input</th><td>&nbsp;</td><td>";

static const char *endTable="</table>";

static const char *modeNames[]={ "(null)", "(None)", "NFD", "NFKD", "NFC", "NFKC", "FCD", "FCC" };

static const char *qcNames[]={ "NO", "YES", "MAYBE" };

static const char *versions=
    "<p>Unicode version %s &mdash; "
    "<a href=\"http://oss.software.ibm.com/icu/\">ICU</a> %s</p>\n";

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

    printf("<tr><th>%s</th>", modeNames[mode]);

    errorCode=U_ZERO_ERROR;
    qc=unorm_quickCheckTailored(s, length, mode, options, &errorCode);
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
        } else {
            dest[i++]=c;
        }
    }

    return i;
}

/*
 * Handle various command-line input:
 * - one code point per argv[]
 * - multiple code points separated by '+'
 * - end of input: NUL or ';' or '&'
 */
static int32_t
parseInput(const char *argv[], int argc,
           int32_t &countArgs, const char *&endInLastArg,
           char *buffer, int32_t bufferCapacity,
           UChar *input, int32_t inputCapacity,
           UErrorCode &errorCode) {
    char *p, *q;
    int32_t length, piece, pieceLength;

    length=0;
    p=NULL; // !=NULL when an end-character was found
    for(piece=0; piece<argc; ++piece) {
        /* find the end - look for ';' or '&' */
        p=strchr(argv[piece], ';');
        q=strchr(argv[piece], '&');
        if(p!=NULL) {
            if(q!=NULL) {
                if(p<q) {
                    pieceLength=(int32_t)(p-argv[piece]);
                } else {
                    pieceLength=(int32_t)(q-argv[piece]);
                    p=q;
                }
            } else {
                pieceLength=(int32_t)(p-argv[piece]);
            }
        } else if(q!=NULL) {
            pieceLength=(int32_t)(q-argv[piece]);
            p=q;
        } else {
            pieceLength=strlen(argv[piece]);
        }

        // add a separating space
        if(0<length && length<bufferCapacity) {
            buffer[length++]=' ';
        }

        // copy the current piece
        if((length+pieceLength+1)>=bufferCapacity) {
            errorCode=U_BUFFER_OVERFLOW_ERROR;
            return 0;
        }
        memcpy(buffer+length, argv[piece], pieceLength);
        length+=pieceLength;

        if(p!=NULL) {
            // finish without ++piece
            break;
        }
    }

    // NUL-terminate
    buffer[length]=0;

    // replace plusses by spaces
    q=buffer;
    while((q=strchr(q, '+'))!=NULL) {
        *q++=' ';
    }

    countArgs=piece;
    endInLastArg=p;
    return u_parseString(buffer, input, inputCapacity, NULL, &errorCode);
}

extern int
main(int argc, const char *argv[]) {
    UChar input[100], buffer16[600]; // buffer16 should be 6 times longer than input for \\uhhhh
    char input8[400];
    char buffer[1000];
    const char *endInLastArg;
    int32_t inputLength, countArgs, options;
    UErrorCode errorCode;

    const char *cgi, *script;
    UBool inputIsUTF8;

    script=getenv("SCRIPT_NAME"); //"/cgi-bin/nbrowser"
    puts(htmlHeader);

    inputLength=options=0;
    inputIsUTF8=FALSE;
    errorCode=U_ZERO_ERROR;
    if(argc>1) {
        /* get input from command line */
        inputLength=parseInput(argv+1, argc-1,
                               countArgs, endInLastArg,
                               buffer, sizeof(buffer),
                               input, LENGTHOF(input),
                               errorCode);
        if(endInLastArg!=NULL && *(++endInLastArg)==0) {
            endInLastArg=NULL; // nothing in the last parsed argument after ';' or '&'
            ++countArgs;
        }
        if(endInLastArg==NULL && (1+countArgs)<argc) {
            endInLastArg=argv[1+countArgs];
        }
        if(endInLastArg!=NULL) {
            options=(int32_t)strtol(endInLastArg, NULL, 16);
        } else {
            options=0;
        }
    } else if((cgi=getenv("QUERY_STRING"))!=NULL) {
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
            inputLength=parseInput(&in, 1,
                                   countArgs, endInLastArg,
                                   buffer, sizeof(buffer),
                                   input, LENGTHOF(input),
                                   errorCode);
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
        options&4 ? "checked" : "");

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

    puts(htmlFooter);
    return 0;
}
