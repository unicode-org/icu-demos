/*
*******************************************************************************
*
*   Copyright (C) 2003-2006, International Business Machines
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

#include "unicode/utypes.h"
#include "unicode/uchar.h"
#include "unicode/ustring.h"
#include "unicode/unistr.h"
#include "unicode/unorm.h"
#include "unicode/uidna.h"
#include "unicode/uscript.h"
#include "unicode/uniset.h"

#include "demo_settings.h"
#include "demoutil.h"

#include "idnbrwsr.h"
#include "parseqs.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
//#ifdef WIN32
//#   define _WIN32_WINNT 0x0400 
//#   include "windows.h"
//#endif

#define LENGTHOF(array) (sizeof(array)/sizeof((array)[0]))

static const char htmlHeader[]=
    "Content-Type: text/html; charset=utf-8\n"
    "\n"
    "<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Transitional//EN\" \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd\">\n"
    "<html xmlns=\"http://www.w3.org/1999/xhtml\" xml:lang=\"en\" lang=\"en\">\n"
    "<head>\n";

static const char defaultHeader[]=
    "<meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\">\n"
    "<title>IDNA Demo</title>\n";

static const char endHeaderBeginBody[] =
    "<style type=\"text/css\">\n"
    "/*<![CDATA[*/\n"
    ".highlight {background-color: white; color: red; border-left: 1px solid  blue; border-right: 1px solid blue}\n"
    ".word {background-color:#DDFFFF}\n"
    ".num {background-color:#FFDDFF}\n"
    "/*]]>*/\n"
    "</style>\n"
    "</head>\n"
    "<body>\n";

static const char breadCrumbMainHeader[]=
    "<a class=\"bctl\" href=\"//www.icu-project.org/\">ICU</a><span class=\"bct\">&nbsp;&nbsp;&gt;&nbsp;</span>\n"
    "<a class=\"bctl\" href=\"icudemos\">Demo</a><span class=\"bct\">&nbsp;&nbsp;&gt;&nbsp;</span>\n"
    "<h1>IDNA Demo</h1>\n";

static const char *htmlFooter=
    "</body>\n"
    "</html>";

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
    " and then with <a href=\"http://www.ietf.org/rfc/rfc3492.txt\">Punycode</a>."
    " <p>In the above demo, different combinations of ToASCII and ToUnicode are applied to the input."
    " It also provides a simple illustration of how a GUI can visually indicate"
    " boundaries between different scripts, to help avoid spoofing. The code is rough,"
    " and only meant for illustration."
    " One could certainly refine this to call out more characters that are visually"
    " confusable. For example, many CJK Radicals are identical in appearance to CJK"
    " Ideographs. Mixtures of simplified and traditional characters can also be visually"
    " highlighted, to help signal possible user errors.</p>\n"
    " <br /> <i>Examples</i>\n"
    " <br /> You can either paste in Unicode text into the above box, or you can use Unicode escapes.\n"
    " For example, you can either use \"&#x00E4;\" or \"\\u00E4\", or could use the decomposition \"a\\u0308\".\n"
    " You can also copy some interesting Unicode text samples from the following pages:\n"
    " <ul>\n"
    " 	<li><a href=\"http://www.unicode.org/standard/WhatIsUnicode.html\">What is Unicode? </a> </li>\n"
    "	<li><a href=\"ubrowse\">Unicode Browser </a></li>\n"
    "<li><a href=\"locexp\">Locale Explorer </a></li>\n"
    " </ul>\n" 
    "<hr />";

static const char *inputError="<p>Error parsing the input string: %s</p>\n";

static const char *emptyString="(empty)";

static const char *startString="%04X";

static const char *midString=" %04X";

static const char *endString="";

static const char *startForm=
    "<form method=\"get\" action=\"%s\">\n"
    "<label for=\"domainname\">Enter the domain name to be converted in UTF-8 or escaped Unicode text (\\uXXXX or \\UXXXXXXXX) :</label><br />\n"
    "<input id=\"domainname\" type=\"text\" name=\"t\" maxlength=\"500\" size=\"80\" value=\"%s\" /> \n";

static const char *endForm=
            "<input type=\"submit\" value=\"Display Results\" />\n"
            "</form>\n";

static const char *startTable=
    "<table border=\"0\" cellspacing=\"0\" cellpadding=\"0\" class=\"data-table-2\" style=\"margin-right: 5px;\">\n"
    "<caption><em>Results of Operation</em></caption>\n"
    "<tr><th>Mode</th><th style=\"width: 8em;\">Text</th><th>Code Points</th></tr>\n"
    "<tr><td>Input</td>";

static const char *endTable="</table>";
static const char *STD3Fail = "<b> The input does not satisfy STD3 ASCII rules</b>\n";
static const char *STD3Pass = "The input satisfies STD3 ASCII rules\n";
static const char *unassignedFail = "<b> The input contains unassigned code points, can be used for query operations only.</b>\n";
static const char *unassignedPass = "The input does not contain any unassigned code points"; 
static const char *http = "//";
#define HTTP_LEN strlen(http);

static const char *modeNames[]={ "(null)", "(None)", "ToASCII(input)", "ToUnicode(input)", "ToUnicode(ToASCII(input))", "ToASCII(ToUnicode(input))"};

static const char *versions=
    "<p>Unicode version used by IDNA %s &mdash; Powered by "
    "<a href=\"//www.icu-project.org/\">ICU</a> %s</p>\n";

static const char *samples[] = { "www.&#x65E5;&#x672C;&#x5E73;.jp","www.&#x30CF;&#x30F3;&#x30C9;&#x30DC;&#x30FC;&#x30EB;&#x30B5;&#x30E0;&#x30BA;.com","www.f&#x00E4;rgbolaget.nu","www.b&#x00FC;cher.de","www.br&#x00E6;ndendek&#x00E6;rlighed.com","www.r&#x00E4;ksm&#x00F6;rg&#x00E5;s.se",  "www.&#xC608;&#xBE44;&#xAD50;&#xC0AC;.com", "&#x7406;&#x5BB9;&#x30CA;&#x30AB;&#x30E0;&#x30E9;.com", "&#x3042;&#x30FC;&#x308B;&#x3044;&#x3093;.com", "www.f&#xE4;rjestadsbk.net", "www.m&#xE4;kitorppa.com", NULL };

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
printStringUTF8(const UChar *s, int32_t length) {
    char *c = NULL;

    if(length<0) {
        length=u_strlen(s);
    }
    if(length==0) {
        printf(emptyString);
    } else {
    	c = (char*)malloc(length * 8);
	UErrorCode status = U_ZERO_ERROR;
        int32_t destLen = 0;	
	u_strToUTF8(c,length*8,&destLen,s,length,&status);
	if(U_FAILURE(status)){
		printf("<br />%s<br />\n", u_errorName(status));
	}else{
		printf("<br />%s<br />\n",c);
	}
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
static UnicodeSet *han_t_set = NULL;
static UnicodeSet *han_s_set = NULL;
UErrorCode setCreationError = U_ZERO_ERROR;

static UErrorCode initSets(){
	if((han_t_set == NULL || han_s_set == NULL) && U_SUCCESS(setCreationError)){
		UnicodeString han_t_str("[");
		UnicodeString han_s_str("[");
		han_t_str.append(han_t_contents);
        	han_s_str.append(han_s_contents);
		han_t_str.append("]");
		han_s_str.append("]");
		UErrorCode status = U_ZERO_ERROR;
		han_t_set = new UnicodeSet(han_t_str,status);
		han_s_set = new UnicodeSet(han_s_str,status);
		setCreationError = status;
		if(U_FAILURE(status)){
			printStringUTF8(han_t_str.getBuffer(), han_t_str.length());
			han_t_str.releaseBuffer();
		}
	}
	return setCreationError;
}
#define USCRIPT_HAN_S ((UScriptCode)(USCRIPT_CODE_LIMIT+1))
#define USCRIPT_HAN_T ((UScriptCode)(USCRIPT_CODE_LIMIT+2))
#define USCRIPT_HAN_N ((UScriptCode)(USCRIPT_CODE_LIMIT+3))

static void breakScripts(const UChar* src, int32_t srcLength, UnicodeString& result) {

  UScriptCode lastScript = USCRIPT_INVALID_CODE;
  UBool inSpan = FALSE;
  int32_t spanCount = 0;
  UErrorCode status = U_ZERO_ERROR;
  UnicodeString str( src, srcLength );

  status = initSets();
  if(U_FAILURE(status)){
	result.append(u_errorName(status));
	return;
  }
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
    if(script == USCRIPT_INHERITED || script == USCRIPT_COMMON){
	    script = lastScript;
    }
    if ((c >= 0x4E00 && c <= 0x9FFF) && (han_t_set !=NULL && han_s_set!=NULL)) {
        if(han_t_set->contains(c)){
		    script = USCRIPT_HAN_T;
	    }else if(han_s_set->contains(c)){
		    script = USCRIPT_HAN_S;
	    }else{
		    script = USCRIPT_HAN_N;
	    }
    }
    if (script == USCRIPT_HAN_N && (lastScript == USCRIPT_HAN_T || lastScript == USCRIPT_HAN_S)){
	 script = lastScript;
    }else if (lastScript == USCRIPT_HAN_N && (script == USCRIPT_HAN_T || script == USCRIPT_HAN_S)){
	 lastScript = script;
    }
    
    if(u_isdigit(c)){
        script = USCRIPT_CODE_LIMIT;
    }
   
    if (script != lastScript) {
        if (inSpan){
            result += "</span>";
        }else{
            result += "<span class=\"highlight\">";
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
printTableText(const UChar *s, int32_t length, UBool useBreakScripts, UBool makeLink) {
    char buffer[6000];
    int32_t utf8Length;
    UErrorCode errorCode;

    errorCode=U_ZERO_ERROR;
    UnicodeString result;
    UnicodeString str(s,length);
    if(makeLink == TRUE){
        result.append("<a href=\"");
        if(str.indexOf(http)==-1){
            result.append(http);
        }
        result.append(s,length);
        result.append("\">");
    }
    if(useBreakScripts){
        breakScripts(s,length, result);
    }else{
        result.append(s,length);
    }
    if(makeLink == TRUE){
        result.append("</a>\n");
    }
    u_strToUTF8(buffer, sizeof(buffer), &utf8Length, result.getBuffer(), result.length(), &errorCode);
    result.releaseBuffer();
    if(length==0) {
        printf("<td>&nbsp;</td>");
    }
    else if(U_FAILURE(errorCode) || errorCode==U_STRING_NOT_TERMINATED_WARNING) {
        printf("<td>%s</td>", u_errorName(errorCode));
    }
    else {
        printf("<td>%s</td>", buffer);
    }
}
#define DAGGER 0x2020
#define ASTERIX 0x002A
#define SPACE 0x0020
static void
printToUnicode (const UChar *s, int32_t length, UBool &printUnassignedError, UBool &printSTD3Error, UErrorCode& errorCode) {
    UChar* output = (UChar*) malloc(U_SIZEOF_UCHAR * length);
    UChar* tempOut = (UChar*) malloc(U_SIZEOF_UCHAR * length);
    int32_t outputLength;
    UParseError parseError;
    printUnassignedError = FALSE;
    printSTD3Error = FALSE;

    printf("<tr><td>%s</td>", modeNames[3]);
/*
    UnicodeString str(s,length);
    if(str.indexOf(http)!=-1){
        s = s+HTTP_LEN;
        length -= HTTP_LEN;
    }
*/
    errorCode=U_ZERO_ERROR;
    outputLength= uidna_IDNToUnicode(s, length, output, length,UIDNA_ALLOW_UNASSIGNED, &parseError, &errorCode);

    if(length==0) {
        printf("<td>&nbsp;</td><td>&nbsp;</td>");
    }
    else if(U_FAILURE(errorCode)) {
        printf("<td>%s</td><td>&nbsp;</td>", u_errorName(errorCode));
    } else {
        
        uidna_IDNToUnicode(s, length, tempOut,  length, UIDNA_DEFAULT, &parseError, &errorCode);
        if(errorCode == U_IDNA_UNASSIGNED_ERROR){
            printUnassignedError  = TRUE;
        }
        errorCode = U_ZERO_ERROR;
        uidna_IDNToUnicode(s, length, tempOut, length, UIDNA_USE_STD3_RULES, &parseError, &errorCode);
        if(errorCode == U_IDNA_STD3_ASCII_RULES_ERROR){
            printSTD3Error = TRUE;
        }
        
        printTableText(output, outputLength,TRUE,FALSE);
        printTableString(output,outputLength);

    }

    puts("</tr>");
    free(output);
    free(tempOut);

}

static void
printToUnicode (const UChar *s, int32_t length) {
    UChar *output = NULL;
    int32_t tempOutLen = length * 9;
    UChar *tempOut = (UChar*) malloc(U_SIZEOF_UCHAR * tempOutLen) ;
    int32_t outputLength;
    UErrorCode errorCode;
    UParseError parseError;

    printf("<tr><td>%s</td>", modeNames[4]);
/*
    UnicodeString str(s,length);
    if(str.indexOf(http)!=-1){
        s = s+HTTP_LEN;
        length -= HTTP_LEN;
    }
*/
    errorCode=U_ZERO_ERROR;
    tempOutLen =  uidna_IDNToASCII(s,length,tempOut, tempOutLen, UIDNA_ALLOW_UNASSIGNED, &parseError, &errorCode);

    length = tempOutLen * 9;
    output = (UChar*) malloc(U_SIZEOF_UCHAR * length);

    outputLength= uidna_IDNToUnicode(tempOut, tempOutLen, output, length,UIDNA_ALLOW_UNASSIGNED, &parseError, &errorCode);

    if(length==0) {
        printf("<td>&nbsp;</td><td>&nbsp;</td>");
    }
    else if(U_FAILURE(errorCode)) {
        printf("<td>%s</td><td>&nbsp;</td>", u_errorName(errorCode));
    } else {
        printTableText(output, outputLength, TRUE,FALSE);
        printTableString(output,outputLength);
    }

    puts("</tr>");
    free(tempOut);
    free(output);

}

static void
printToASCII(const UChar *s, int32_t length,UBool &printUnassignedError, UBool &printSTD3Error, UErrorCode& errorCode) {
    int32_t capacity = length * 9;
    UChar *output = (UChar*)  malloc(U_SIZEOF_UCHAR * capacity);
    UChar *tempOut = (UChar*) malloc(U_SIZEOF_UCHAR * capacity);
    int32_t outputLength;
    UParseError parseError;
    printUnassignedError = FALSE;
    printSTD3Error = FALSE;
    printf("<tr><td>%s</td>", modeNames[2]);
/*    
    UnicodeString str(s,length);
    if(str.indexOf(http)!=-1){
        s = s+HTTP_LEN;
        length -= HTTP_LEN;
    }
*/
    errorCode=U_ZERO_ERROR;
    outputLength= uidna_IDNToASCII(s, length, output, capacity, UIDNA_ALLOW_UNASSIGNED, &parseError, &errorCode);

    if(length==0) {
        printf("<td>&nbsp;</td><td>&nbsp;</td>");
    }
    else if(U_FAILURE(errorCode)) {
        printf("<td>%s</td><td>&nbsp;</td>", u_errorName(errorCode));
    } else {
        uidna_IDNToASCII(s, length, tempOut,  capacity, UIDNA_DEFAULT, &parseError, &errorCode);

        if(errorCode == U_IDNA_UNASSIGNED_ERROR){
            printUnassignedError  = TRUE;
        }
        errorCode = U_ZERO_ERROR;
        uidna_IDNToASCII(s, length, tempOut, capacity, UIDNA_USE_STD3_RULES, &parseError, &errorCode);
        if(errorCode == U_IDNA_STD3_ASCII_RULES_ERROR){
            printSTD3Error = TRUE;
        }

        printTableText(output, outputLength,FALSE,TRUE);
        printTableString(output, outputLength);
    }


    puts("</tr>");
    
    free(tempOut);
    free(output);

}

static void
printToASCII (const UChar *s, int32_t length) {
    int32_t capacity  = length * 9;
    UChar *output = (UChar*) malloc(U_SIZEOF_UCHAR * capacity);
    UChar *tempOut = (UChar*) malloc(U_SIZEOF_UCHAR * capacity);
    int32_t outputLength,tempOutLen;
    UErrorCode errorCode;
    UParseError parseError;

    printf("<tr><td>%s</td>", modeNames[5]);
/* 
    UnicodeString str(s,length);
    if(str.indexOf(http)!=-1){
	s = s+HTTP_LEN;
	length -= HTTP_LEN;
    }
*/    
    errorCode=U_ZERO_ERROR;
    tempOutLen =  uidna_IDNToUnicode(s,length,tempOut, capacity, UIDNA_ALLOW_UNASSIGNED, &parseError, &errorCode);

    outputLength= uidna_IDNToASCII(tempOut, tempOutLen, output, capacity,UIDNA_ALLOW_UNASSIGNED, &parseError, &errorCode);

    if(length==0) {
        printf("<td>&nbsp;</td><td>&nbsp;</td>");
    }
    else if(U_FAILURE(errorCode)) {
        printf("<td>%s</td><td>&nbsp;</td>", u_errorName(errorCode));
    } else {
        printTableText(output, outputLength,FALSE,TRUE);
        printTableString(output,outputLength);
    }

    puts("</tr>");
    free(tempOut);
    free(output);
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
    UChar *input = NULL, *buffer16 = NULL; 
    char  *input8 = NULL;
    char  *buffer = NULL;
    int32_t inputLength;
    UErrorCode errorCode;
    UnicodeString us;
    const char *cgi, *script;
    UBool inputIsUTF8;

    script=getenv("SCRIPT_NAME"); 
    puts(htmlHeader);
    if (!printHTMLFragment(NULL, NULL, DEMO_COMMON_DIR "idna-header.html")) {
        puts(defaultHeader);
    }
    puts(endHeaderBeginBody);
    if (printHTMLFragment(NULL, NULL, DEMO_COMMON_MASTHEAD)) {
        puts(DEMO_BEGIN_LEFT_NAV);
        printHTMLFragment(NULL, NULL, DEMO_COMMON_LEFTNAV);
        puts(DEMO_END_LEFT_NAV);
        puts(DEMO_BEGIN_CONTENT);
    }
    puts(breadCrumbMainHeader);

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
    cgi=getenv("QUERY_STRING");
    if(cgi != NULL && cgi[0] != 0) {
        parseQueryString(cgi,strlen(cgi),LENGTHOF(options),options);
        const char* inputType = options[INPUT_TYPE].value;
        int32_t len = (options[INPUT].valueLen * 2);
        if ( len != 0 ) {
            inputIsUTF8=TRUE;
            buffer = (char*) malloc( len );
            inputLength=parseEscaped(options[INPUT].value,options[INPUT].valueLen, buffer, len, errorCode);
            if(U_FAILURE(errorCode)){
                printf("#### len = %i valueLen = %i %s\n", len, options[INPUT].valueLen,buffer);
            }
            buffer16 = (UChar*) malloc ( U_SIZEOF_UCHAR * (inputLength+10));
            u_strFromUTF8(buffer16, inputLength+10, &inputLength,
                buffer, inputLength,
                &errorCode);
            us.append((const UChar *)buffer16, inputLength); 
            us = us.unescape();

            if(errorCode==U_STRING_NOT_TERMINATED_WARNING) {
                errorCode=U_BUFFER_OVERFLOW_ERROR;
            }
            if(U_FAILURE(errorCode)){
                printf("#### inputLength = %i \n", inputLength);
            }
            input = (UChar*) us.getBuffer();
            inputLength = us.length();
            input8  = (char*) malloc( inputLength * 9); 
            int32_t reqLength =0;
            u_strToUTF8(input8,inputLength*8 , &reqLength,
                input, inputLength,
                &errorCode);
            if(inputLength !=0 && errorCode==U_STRING_NOT_TERMINATED_WARNING) {
                errorCode=U_BUFFER_OVERFLOW_ERROR;
            }
            if(U_FAILURE(errorCode)){
                printf("#### inputLength = %i capacity = %i reqLength = %i \n", inputLength, inputLength * 8, reqLength);
            }
        }
    }

    if(U_FAILURE(errorCode)) {
        printf(inputError, u_errorName(errorCode));
        inputLength=0;
    }

    printf(startForm, script ? script : "", input8 ? input8 : "");

    printf(endForm, "", "", "",
         "checked=\"checked\"" );


    printf(startTable);
    
    printTableText(input, inputLength,TRUE,FALSE);
    
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
    printToASCII   (input, inputLength);
    puts(endTable);
    if(inputLength > 0  && (status == U_IDNA_UNASSIGNED_ERROR || status == U_IDNA_STD3_ASCII_RULES_ERROR)){
        printf("<font size=\"4\"><b>Information </b></font><br />\n");
        if((std31==TRUE ||std32 ==TRUE)){
            printf(STD3Fail);
        }else if(status !=  U_IDNA_UNASSIGNED_ERROR){
            printf(STD3Pass);
        }
        printf("<br />\n");
        if(unass1==TRUE || unass2 ==TRUE){
            printf(unassignedFail);
        }else{
            printf(unassignedPass);
        }
    }
    char uvString[16], ivString[16];
    UVersionInfo uv, iv;
    printf("<form method=\"get\" action=\"%s\"><table border=\"0\"><tr><td><label for=\"samples\">Or choose a sample from this list:</label></td><td><select id=\"samples\" name='t'>", "?");
    printf("<option selected=\"selected\" value=\"\">(samples...)</option>\n");
    for(int j=0;samples[j];j++) {
      printf("<option value=\"%s\">%s</option>", samples[j], samples[j]);
    }
    puts("</select>\n<input type=\"submit\" value=\"Load\" /></td></tr></table></form>\n");

    puts(helpText);

    //u_getUnicodeVersion(uv);
    u_getVersion(iv);
    u_versionToString(uv, uvString);
    u_versionToString(iv, ivString);
    printf(versions, "3.2", ivString);
    
    puts(DEMO_END_CONTENT);
    printHTMLFragment(NULL, NULL, DEMO_COMMON_FOOTER);
    puts(htmlFooter);

    us.releaseBuffer();
    free(buffer);
    free(buffer16);
    free(input8);
    return 0;
}
