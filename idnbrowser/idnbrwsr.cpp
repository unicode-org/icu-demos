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
*   created on: 2003Mar11
*   created by: Ram Viswanadha
*
*   This is a fairly crude, un-internationalized cgi for a IDNA.
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
#include "unicode/uidna.h"
#include "unicode/uscript.h"

#ifdef WIN32
#   define _WIN32_WINNT 0x0400 
#   include "windows.h"
#endif

#include "parseqs.h"

#define LENGTHOF(array) (sizeof(array)/sizeof((array)[0]))

static const char *htmlHeader=
    "Content-Type: text/html; charset=utf-8\n"
    "\n"
    "<html lang=\"en-US\">\n"
    "<head>\n"
    "<meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\">\n"
    "<title>IDNA Demo</title>\n"
    "</head>\n"
    "<body bgcolor=\"#FFFFFF\">\n"
    "<style>\n"
    "span {background-color: white; color: red; border-left:"
    " 1px solid  blue; border-right: 1px solid blue}.word {background-color:#DDFFFF}.num"
    " {background-color:#FFDDFF} body  {font-size: 12pt}\n"
    "</style>\n"
    "<a href=\"http://oss.software.ibm.com/icu/\">ICU</a> &gt;\n"
    "<a href=\"http://oss.software.ibm.com/icu/demo/\">Demo</a> &gt;<br><hr>\n"
    "<h1>IDNA Demo</h1>\n";

static const char *htmlFooter=
    "</body>";

static const char *helpText=
    "<a name=\"help\"></a><h2>About this demo</h2>\n"
    " This CGI program demostrates the <a href=\"http://www.ietf.org/rfc/rfc3490.txt\">IDNA</a> implementation."
    " The RFC defines 2 operations: ToASCII and ToUnicode. Domain labels"
    " containing non-ASCII code points are required to be processed by"
    " ToASCII operation before passing it to resolver libraries. Domain names"
    " that are obtained from resolver libraries are required to be processed by"
    " ToUnicode operation before displaying the domain name to the user."
    " IDNA requires that implementations process input strings with"
    " <a href=\"http://www.ietf.org/rfc/rfc3491.txt\">Nameprep</a>,"
    " which is a profile of <a href=\"http://www.ietf.org/rfc/rfc3454.txt\"> Stringprep </a>,"
    " and then with <a href=\"http://www.ietf.org/rfc/rfc3492.txt\">Punycode</a>"
    " <p>This demo also provides a simple illustration of how a GUI can visually indicated"
    " boundaries between different scripts, to help avoid spoofing. The code is rough,"
    " and only meant for illustration.</p>"
    "<p>One could certainly refine this to call out more characters that are visually"
    " confusable. For example, many CJK Radicals are identical in appearance to CJK"
    " Ideographs.</p>"
    "<hr>";

static const char *inputError="<p>Error parsing the input string: %s</p>\n";

static const char *emptyString="(empty)";

static const char *startString="%04x";

static const char *midString=" %04x";

static const char *endString="";

static const char *startForm=
    "<form method=\"GET\" action=\"%s\">\n"
    "<p>Enter the domain name to be converted in UTF-8 or escaped Unicode text:<br>"
    "<input type=\"text\" name=\"t\" maxlength=\"500\" size=\"164\" value=\"%s\"> </p>\n";

static const char *endForm=
            "<input type=\"submit\" value=\"Perform IDNA\" size=\"100\">\n"
            "</form>\n";

static const char *startTable=
    "<table border=\"1\">\n"
    "<tr><th>Mode</th><th>Text</th><th>Code Points</th></tr>\n"
    "<tr><th>Input</th>";

static const char *endTable="</table>";
static const char *STD3Fail = "<b> The input does not satisfy STD3 ASCII rules</b>\n";
static const char *STD3Pass = "The input satisfies STD3 ASCII rules\n";
static const char *unassignedFail = "<b> The input contains unassigned code points, can be used for query operations only.</b>\n";
static const char *unassignedPass = "The input does not contain any unassigned code points"; 



static const char *modeNames[]={ "(null)", "(None)", "ToASCII(input)", "ToUnicode(input)", "ToUnicode(ToASCII(input))"};

static const char *versions=
    "<p>Unicode version used by IDNA %s &mdash; "
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
printString(const char *s, int32_t length) {
    UChar32 c;
    int32_t i;

    if(length<0) {
        length= strlen(s);
    }
    if(length==0) {
        printf(emptyString);
    } else {
        i=0;
        c = s[i++];

        printf(startString, c);

        while(i<length) {
            c = s[i++];
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

static void breakScripts(const UChar* src, int32_t srcLength, UnicodeString& result) {

  UScriptCode lastScript = USCRIPT_INVALID_CODE;
  UBool inSpan = FALSE;
  int32_t spanCount = 0;
  UErrorCode status = U_ZERO_ERROR;
  UnicodeString str( src, srcLength );
  for (int32_t i = 0; i < str.length(); ++i) {
    UChar32 c = str.char32At(i);
    
    UScriptCode script = uscript_getScript(c, &status);

    // log += c + ", " + script + ", " + lastScript + "; ";
        
    // Certain characters assume the script of certain adjacent characters
    if(c == 0x002E || c == 0x002D){
        script = USCRIPT_INVALID_CODE;
    }
    if (script == USCRIPT_INVALID_CODE){
        script = lastScript;
    }else if (lastScript == USCRIPT_INVALID_CODE){
        lastScript = script;
    }
    /*
    if (script == HAN_N && (lastScript == HAN_T || lastScript == HAN_S)) script = lastScript;
    else if (lastScript == HAN_N && (script == HAN_T || script == HAN_S)) lastScript = script;
    */
    if(u_isdigit(c)){
        script = USCRIPT_CODE_LIMIT;
    }
   
    if (script != lastScript) {
        if (inSpan){
            result += "</span>";
        }else{
            result += "<span>";
        }
        lastScript = script;
        inSpan = !inSpan;
        ++spanCount;
    }
    // add the character afterwards
    result += c;
  }
  if (inSpan) result += "</span>";
  // alert(log);
  // return result;
}

static void
printTableText(const UChar *s, int32_t length, UBool useBreakScripts) {
    char buffer[6000];
    int32_t utf8Length;
    UErrorCode errorCode;

    errorCode=U_ZERO_ERROR;
    UnicodeString result;
    if(useBreakScripts){
        breakScripts(s,length, result);
    }else{
        result.append(s,length);
    }
    u_strToUTF8(buffer, sizeof(buffer), &utf8Length, result.getBuffer(), result.length(), &errorCode);
    result.releaseBuffer();
    if(U_FAILURE(errorCode) || errorCode==U_STRING_NOT_TERMINATED_WARNING) {
        printf("<td>%s</td>", u_errorName(errorCode));
    } else {
        printf("<td>%s</td>", buffer);
    }
}
#define DAGGER 0x2020
#define ASTERIX 0x002A
#define SPACE 0x0020
static void
printToUnicode (const UChar *s, int32_t length, UBool &printUnassignedError, UBool &printSTD3Error, UErrorCode& errorCode) {
    UChar output[1500];
    UChar tempOut[1500];
    int32_t outputLength;
    UParseError parseError;
    printUnassignedError = FALSE;
    printSTD3Error = FALSE;

    printf("<tr><th>%s</th>", modeNames[3]);

    errorCode=U_ZERO_ERROR;
    outputLength= uidna_IDNToUnicode(s, length, output, 1500,UIDNA_ALLOW_UNASSIGNED, &parseError, &errorCode);

    if(U_FAILURE(errorCode)) {
        printf("<td>%s</td><td>&nbsp;</td>", u_errorName(errorCode));
    } else {
        UnicodeString result;
        breakScripts(output,outputLength, result);
        
        uidna_IDNToUnicode(s, length, tempOut,  1500, UIDNA_DEFAULT, &parseError, &errorCode);
        if(errorCode == U_IDNA_UNASSIGNED_CODEPOINT_FOUND_ERROR){
            printUnassignedError  = TRUE;
        }
        errorCode = U_ZERO_ERROR;
        uidna_IDNToUnicode(s, length, tempOut, 1500, UIDNA_USE_STD3_RULES, &parseError, &errorCode);
        if(errorCode == U_IDNA_STD3_ASCII_RULES_ERROR){
            printSTD3Error = TRUE;
        }
        
        printTableText(result.getBuffer(), result.length(),FALSE);
        result.releaseBuffer();
        printTableString(output,outputLength);
        result.releaseBuffer();

    }

    puts("</tr>");

}

static void
printToUnicode (const UChar *s, int32_t length) {
    UChar output[1500];
    UChar tempOut[1500];
    int32_t outputLength,tempOutLen;
    UErrorCode errorCode;
    UParseError parseError;

    printf("<tr><th>%s</th>", modeNames[4]);

    errorCode=U_ZERO_ERROR;
    tempOutLen =  uidna_IDNToASCII(s,length,tempOut, 1500, UIDNA_ALLOW_UNASSIGNED, &parseError, &errorCode);

    outputLength= uidna_IDNToUnicode(tempOut, tempOutLen, output, 1500,UIDNA_ALLOW_UNASSIGNED, &parseError, &errorCode);

    if(U_FAILURE(errorCode)) {
        printf("<td>%s</td><td>&nbsp;</td>", u_errorName(errorCode));
    } else {
        UnicodeString result;
        breakScripts(output,outputLength, result);
        
        printTableText(result.getBuffer(), result.length(),FALSE);
        result.releaseBuffer();
        printTableString(output,outputLength);
        result.releaseBuffer();

    }

    puts("</tr>");

}

static void
printToASCII(const UChar *s, int32_t length,UBool &printUnassignedError, UBool &printSTD3Error, UErrorCode& errorCode) {

    UChar output[1500];
    UChar tempOut[1500];
    int32_t outputLength;
    UParseError parseError;
    printUnassignedError = FALSE;
    printSTD3Error = FALSE;
    printf("<tr><th>%s</th>", modeNames[2]);

    errorCode=U_ZERO_ERROR;
    outputLength= uidna_IDNToASCII(s, length, output, 1500, UIDNA_ALLOW_UNASSIGNED, &parseError, &errorCode);

    if(U_FAILURE(errorCode)) {
        printf("<td>%s</td><td>&nbsp;</td>", u_errorName(errorCode));
    } else {
        uidna_IDNToASCII(s, length, tempOut,  1500, UIDNA_DEFAULT, &parseError, &errorCode);
        UnicodeString result (output, outputLength);

        if(errorCode == U_IDNA_UNASSIGNED_CODEPOINT_FOUND_ERROR){
           printUnassignedError  = TRUE;
        }
        errorCode = U_ZERO_ERROR;
        uidna_IDNToASCII(s, length, tempOut, 1500, UIDNA_USE_STD3_RULES, &parseError, &errorCode);
        if(errorCode == U_IDNA_STD3_ASCII_RULES_ERROR){
            printSTD3Error = TRUE;
        }

        printTableText(result.getBuffer(), result.length(),FALSE);
        result.releaseBuffer();
        printTableString(output, outputLength);
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
parseEscaped(const char *s, int32_t srclen, char *dest, int32_t destCapacity, UErrorCode &errorCode) {
    int32_t i, hi, lo;
    char c;
    const char* srcLimit = s + srclen;
    i=0;
    while( (s < srcLimit) && (c=*s++)!=0 && c!='&') {
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
parseString(const char *s, int32_t srcLen,
            UChar *dest, int32_t destCapacity,
            UErrorCode &errorCode) {
    char *end;
    UChar32 value;
    int32_t length;
    UBool isError;
    const char* srcLimit = s + srcLen;
    length=0;
    isError=FALSE;
    for(;;) {
        s=skipEscapedWhitespace(s);
        if((s >= srcLimit) || *s=='&' || *s==0) {
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
    return length;
}

enum QueryOptionsEnum {
    INPUT,
    INPUT_TYPE,
    OPERATION,
};

QueryOption options[] = {
    QUERY_OPTION_DEF("t", NULL, NULL),
    QUERY_OPTION_DEF("e", NULL, NULL),
    QUERY_OPTION_DEF("o", NULL, NULL)
};


extern int
main(int argc, const char *argv[]) {
    UChar input[1500], buffer16[600]; // buffer16 should be 6 times longer than input for \\uhhhh
    char input8[1500];
    char buffer[1000];
    int32_t inputLength;
    UErrorCode errorCode;

    const char *cgi, *script;
    UBool inputIsUTF8;

    script=getenv("SCRIPT_NAME"); 
    puts(htmlHeader);
    input8[0] =0 ;
    inputLength=0;
    inputIsUTF8=FALSE;
    errorCode=U_ZERO_ERROR;
/* on win32 systems to debug uncomment the block below
 * Invoke the CGI application. 
 * Attach a debugger (such as Visual C) to the CGI process while a message box is on the screen. 
 * When the debugger is attached, open the source file and set a break point. 
 * Click OK to dismiss the message box. When the message box is dismissed, 
 * the CGI execution will resume and the break point will be hit. 
 */
#if 0
#   ifdef WIN32
  
    char szMessage [256];
    
    wsprintf (szMessage, "Please attach a debbuger to the process 0x%X (%s) and click OK",
              GetCurrentProcessId(),"idnbrowser");
    MessageBox(NULL, szMessage, "CGI Debug Time!",
               MB_OK|MB_SERVICE_NOTIFICATION);
    
#   endif
#endif
    if((cgi=getenv("QUERY_STRING"))!=NULL) {
        parseQueryString(cgi,strlen(cgi),sizeof(options)/sizeof(options[0]),options);
//        if(options[INPUT_TYPE].doesOccur){
            const char* inputType = options[INPUT_TYPE].value;
            int32_t len = options[INPUT_TYPE].valueLen;
    //        if(strncmp(inputType, "utf8", len )==0 || strncmp(inputType,"escaped", len) == 0){
                inputIsUTF8=TRUE;
                inputLength=parseEscaped(options[INPUT].value,options[INPUT].valueLen, buffer, sizeof(buffer), errorCode);
                u_strFromUTF8(buffer16, LENGTHOF(buffer16), &inputLength,
                              buffer, inputLength,
                              &errorCode);
                UnicodeString us(FALSE, (const UChar *)buffer16, inputLength); // readonly alias
                inputLength=us.unescape().extract(input, LENGTHOF(input), errorCode);

                if(errorCode==U_STRING_NOT_TERMINATED_WARNING) {
                    errorCode=U_BUFFER_OVERFLOW_ERROR;
                }

      //      }else if(strncmp(inputType, "codepoints", len) == 0){
      //        inputLength=parseString(options[INPUT].value, options[INPUT].valueLen, input, LENGTHOF(input), errorCode);
      //      }

            u_strToUTF8(input8, sizeof(input8), NULL,
                        input, inputLength,
                        &errorCode);
            if(errorCode==U_STRING_NOT_TERMINATED_WARNING) {
                errorCode=U_BUFFER_OVERFLOW_ERROR;
            }
  //      }
    }

    if(U_FAILURE(errorCode)) {
        printf(inputError, u_errorName(errorCode));
        inputLength=0;
    }

    printf(startForm, script ? script : "",  input8 );

//    if(!inputIsUTF8 && inputLength>0) {
//       printString(options[INPUT].value, options[INPUT].valueLen);
//    }
    printf(endForm, "", "", "",
         "checked" );


    printf(startTable);
    
    printTableText(input, inputLength,TRUE);
    
    printf("<td>");
    
    printString(input, inputLength);
    
    printf("</td>");
    
    puts("</tr>");
    /*
    if(options[OPERATION].doesOccur){
        if(strncmp(options[OPERATION].value,"ToASCII", options[OPERATION].valueLen) ==0){
            printToASCII(input,inputLength, 2, UIDNA_DEFAULT);
            printToASCII(input,inputLength, 3, UIDNA_USE_STD3_RULES);
            printToASCII(input,inputLength, 4, UIDNA_ALLOW_UNASSIGNED);
            printToASCII(input,inputLength, 5, UIDNA_USE_STD3_RULES+UIDNA_ALLOW_UNASSIGNED);
        }else{
            printToUnicode(input,inputLength, 2, UIDNA_DEFAULT);
            printToUnicode(input,inputLength, 3, UIDNA_USE_STD3_RULES);
            printToUnicode(input,inputLength, 4, UIDNA_ALLOW_UNASSIGNED);
            printToUnicode(input,inputLength, 5, UIDNA_USE_STD3_RULES+UIDNA_ALLOW_UNASSIGNED);
        }
    }
    */
    UBool unass1,unass2,std31,std32;
    UErrorCode status; 
    printToASCII  (input, inputLength,unass1,std31,status);
    printToUnicode (input,inputLength);
    printToUnicode (input, inputLength,unass2,std32,status);

    puts(endTable);
    if(inputLength >0  && U_SUCCESS(status)){
        printf("<font size=\"4\"><b>Information </b></font><br>\n");
        if(std31==TRUE ||std32 ==TRUE){
            printf(STD3Fail);
        }else{
            printf(STD3Pass);
        }
        printf("<br>\n");
        if(unass1==TRUE || unass2 ==TRUE){
            printf(unassignedFail);
        }else{
            printf(unassignedPass);
        }
    }
    char uvString[16], ivString[16];
    UVersionInfo uv, iv;

    puts(helpText);

    //u_getUnicodeVersion(uv);
    u_getVersion(iv);
    u_versionToString(uv, uvString);
    u_versionToString(iv, ivString);
    printf(versions, "3.2", ivString);

    puts(htmlFooter);
    return 0;
}
