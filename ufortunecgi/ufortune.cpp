/******************************************************************************
*
*   Copyright (C) 2005-2006, International Business Machines
*   Corporation and others.  All Rights Reserved.
*
*******************************************************************************
*/
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef WIN32
#   define _WIN32_WINNT 0x0400 
#   include "windows.h"
#endif

#include "unicode/udata.h"     /* ICU API for data handling.                 */
#include "unicode/ures.h"      /* ICU API for resource loading               */
#include "unicode/uchar.h"
#include "unicode/ustring.h"
#include "parseqs.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#ifndef UFORTUNE_NOSETAPPDATA
/*
 *  Resource Data Reference.  The data is packaged as a dll (or .so or
 *           whatever, depending on the platform) that exports a data
 *           symbol.  The application (that's us) references that symbol,
 *           here, and will pass the data address to ICU, which will then
 *           be able to fetch resources from the data.
 */
extern "C" {
    extern  const void U_IMPORT *ufortune_dat;
}
#endif

#define LENGTHOF(array) (sizeof(array)/sizeof((array)[0]))

static const char *htmlHeader=
    "Content-Type: text/html; charset=utf-8\n"
    "\n"
    "<html lang=\"en-US\">\n"
    "<head>\n"
    "<meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\">\n"
    "<title>ufortune</title>\n"
    "<SCRIPT TYPE=\"text/javascript\">\n"
    "    //<!--\n"
    "    function dropdown(mySel)\n"
    "    {\n"
    "       var myWin, myVal;\n"
    "       myVal=\"../cgi-bin/ufortunecgi.exe?locale=\";\n"
   // "       alert(\"wazzup\");"
   // "       alert(document.localeSelect.locale.options[document.localeSelect.locale.selectedIndex].value);"
    "       myVal += document.localeSelect.locale.options[document.localeSelect.locale.selectedIndex].value;\n"
    "       if(myVal)\n"
    "       {\n"
    "           if(mySel.form.target){\n"
    "               myWin = parent[mySel.form.target];\n"
    "           }else{ \n"
    "               myWin = window;\n"
    "           }\n"
    "           if (! myWin) \n"
    "               return true;\n"
    "           myWin.location = myVal;\n"
    "       }\n"
    "       return false;\n"
    "    }\n"
    "    //-->\n"
    "</SCRIPT>\n"
    "</head>\n"
    "<body bgcolor=\"#FFFFFF\">\n"
    "<style>\n"
    "span {background-color: white; color: red; border-left:"
    " 1px solid  blue; border-right: 1px solid blue}.word {background-color:#DDFFFF}.num"
    " {background-color:#FFDDFF} body  {font-size: 12pt}\n"
    "</style>\n"
    "<a href=\"http://www.icu-project.org/\">ICU</a> &gt;\n"
    "<a href=\"http://demo.icu-project.org/icu-bin/icudemos\">Demo</a> &gt;<br><hr>\n"
    "<h1>ufortune</h1>\n";

static const char *htmlFooter=
    "</body>";

static const char *startForm=
    "<FORM ACTION=\"../cgi-bin/ufortunecgi.exe\" name=\"localeSelect\"\n" 
    "    METHOD=\"POST\">\n"
    "Please chose the locale in which you would like the fortune message displayed\n<br>"
    "<SELECT NAME=\"locale\"  onChange=\"return dropdown(this)\">\n"
    "<OPTION VALUE=\"root\">(locale)\n"
    "<OPTION VALUE=\"en\">English\n"
    "<OPTION VALUE=\"es\">Spanish\n"
    "<OPTION VALUE=\"de\">German\n"
    "<OPTION VALUE=\"te\">Telugu\n";
static const char *endForm=
    "</SELECT>"
    "</FORM>\n";

static const char *http = "http://";
#define HTTP_LEN strlen(http);

static const char *emptyString="(empty)";

static const char *startString="%04X";

static const char *midString=" %04X";

static const char *endString="";

UnicodeString getFortune(UnicodeString& locale)
{
    
    UErrorCode         err           = U_ZERO_ERROR;   /* Error return, used for most ICU     */
                                                       /*   functions.                        */

    UResourceBundle   *myResources;              /* ICU Resource "handles"                    */
    UResourceBundle   *fortunes_r;

    int32_t            numFortunes;              /* Number of fortune strings available.      */
    int                i;

    const UChar       *resString;                /* Points to strings fetched from Resources. */
    int32_t            len;

    UnicodeString szMessage;
    const char* programName ="ufortune";

#ifndef UFORTUNE_NOSETAPPDATA
    /* Tell ICU where our resource data is located in memory.
     *   The data lives in the Fortune_Resources dll, and we just
     *   pass the address of an exported symbol from that library
     *   to ICU.
     */
    udata_setAppData("ufortune", &ufortune_dat, &err);
    if (U_FAILURE(err)) {
        //fprintf(stderr, "%s: ures_open failed with error \"%s\"\n", programName, u_errorName(err));
        //exit(-1);
    }
#endif
    
    char loc[20] = {"\0"};
    u_UCharsToChars(locale.getBuffer(10), (char*)loc, locale.length());
    locale.releaseBuffer();

    /* Open our resources.
    */
    myResources = ures_open("ufortune",loc, &err);
    if (U_FAILURE(err)) {
        fprintf(stdout, "%s: ures_open failed with error \"%s\"\n", programName, u_errorName(err)); 
        return "";
    }



    /*
     * Open the "fortunes" resources from within the already open resources
     */
    fortunes_r = ures_getByKey(myResources, "fortunes", NULL, &err);
    if (U_FAILURE(err)) {
        fprintf(stdout, "%s: ures_getByKey(\"fortunes\") failed, %s\n", programName, u_errorName(err));
        return "";
    }


    /*
     * Pick up and display a random fortune
     *
     */
    numFortunes = ures_countArrayItems(myResources, "fortunes", &err);
    if (U_FAILURE(err)) {
        fprintf(stdout,"%s: ures_countArrayItems(\"fortunes\") failed, %s\n", programName, u_errorName(err));
        return "";
    }
    if (numFortunes <= 0) {
        fprintf(stdout,"%s: no fortunes found.\n");
        return "";
    }

    i = time(NULL) % numFortunes;    /*  Use time to pick a somewhat-random fortune.  */
    resString = ures_getStringByIndex(fortunes_r, i, &len, &err);
    if (U_FAILURE(err)) {
        fprintf(stdout,"%s: ures_getStringByIndex(%d) failed, %s\n", programName, i, u_errorName(err));
        return "";
    }

    return UnicodeString(resString);
}

static void
printTableText(const UChar *s, int32_t length, UBool useBreakScripts, UBool makeLink) {
    char buffer[6000];
    int32_t utf8Length;
    UErrorCode errorCode;

    errorCode=U_ZERO_ERROR;
    UnicodeString result;
    UnicodeString str(s,length);
    if(makeLink == true){
    	result.append("<a href=\"");
   	if(str.indexOf(http)==-1){
	   result.append(http);
    	}
    	result.append(s,length);
    	result.append("\">");
    }

    result.append(s,length);

    if(makeLink == true){
    	result.append("</a>\n");
    }
    u_strToUTF8(buffer, sizeof(buffer), &utf8Length, result.getBuffer(), result.length(), &errorCode);
    result.releaseBuffer();
    if(U_FAILURE(errorCode) || errorCode==U_STRING_NOT_TERMINATED_WARNING) {
        printf("<td>%s</td>", u_errorName(errorCode));
    } else {
        printf("<td>%s</td>", buffer);
    }
}
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
		printf("<br>%s<br>\n", u_errorName(status));
	}else{
		printf("<br>%s<br>\n",c);
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

enum QueryOptionsEnum {
    LOCALE,
  //  INPUT_TYPE,
  //  OPERATION,
};

QueryOption options[] = {
    QUERY_OPTION_DEF("locale", NULL, NULL),
   // QUERY_OPTION_DEF("help", NULL, NULL),
   // QUERY_OPTION_DEF("", NULL, NULL)
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

    inputLength=0;
    inputIsUTF8=false;
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
              GetCurrentProcessId(),"ufortunecgi");
    MessageBox(NULL, szMessage, "CGI Debug Time!",
               MB_OK|MB_SERVICE_NOTIFICATION);
    
#   endif
#endif

    printf(startForm,"", "", "",
         "checked" );

    printf(endForm, "", "", "",
         "checked" );
    if((cgi=getenv("QUERY_STRING"))!=NULL) {
        parseQueryString(cgi,strlen(cgi),LENGTHOF(options),options);
        const char* loc = options[LOCALE].value;    
        UnicodeString locale(loc, options[LOCALE].valueLen);
        UnicodeString fortune = getFortune(locale);
        printf("<br><font color=\"#FF0000\" size=\"18\">");
        printStringUTF8(fortune.getBuffer(), fortune.length());
        printf("</font>");
    }

    if(inputLength > 0){
        printf("<font size=\"4\"><b>Information </b></font><br>\n");

        printf("<br>\n");

    }
    char uvString[16], ivString[16];
    UVersionInfo uv, iv;

//    puts(helpText);

    //u_getUnicodeVersion(uv);
    u_getVersion(iv);
    u_versionToString(uv, uvString);
    u_versionToString(iv, ivString);
//    printf(versions, "3.2", ivString);
    
    us.releaseBuffer();
    puts(htmlFooter);
    free(buffer);
    free(buffer16);
    free(input8);
    return 0;
}
