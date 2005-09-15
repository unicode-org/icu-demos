/*
*******************************************************************************
* Copyright (C) 1996-2005, International Business Machines Corporation and    *
* others. All Rights Reserved.                                                *
*******************************************************************************
* HTML Design by Josh Mast <josh@hivehaus.org>                                *
*******************************************************************************
*
* File ubrowse.c
*
* Modification History:
*
*   Date        Name        Description
*   06/11/99    stephen     Creation.
*   06/16/99    stephen     Modified to use uprint.
*   08/02/1999  srl         Unibrowse
*   12/02/1999  srl         Integrated design changes from Josh Mast
*   05/12/2001  srl         Revamping design from the beautiful St. Paul's Bay, Malta
*   06/16/2001  srl         More design updates.. added incremental search by name.
*******************************************************************************
*/

/*
   Short list of todo's:
     - a new 'character' view that drills down into a single char,  with all the u_isXXX properties put back
     - add new searches by BiDi properties and such, 
     - better default/fallback handling
     - encodings popup
     - fastcgi (will allow some caching [?- if needed] and make l10n more reasonable)
     - fix the main page, add help
     - make block/type list jump to blocks [search->block]
     - put radical list back
     - ui to select encodings
     - use a table for the block list
     - use mime type! (mapping?)
     - localize
     - general search - i.e., show list of 'chars matching this general prop'. Make
        restartable.
*/

/*
    Query formats which come in:

  *****************
        go=XXXXXX           (from the 'go' panel- key doesn't tell
                             you the action)
          k.x=                (-> copy go= to k, continue - column format)
         or
          n.x=                (-> copy go= to n, continue- block format)
 *****************
       k=XXXXXXX            column  16- XXXXX?  ECOLUMN
 *****************
       n=XXXXXXXX           block  256- XXXXnn  EBLOCK
*****************
       s=_______            search by text      ENAME|EEXACT
          &sx=              .. +exact
          &cs=XXXX          .. continue search from XXXX

*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "unicode/utypes.h"
#include "unicode/ustring.h"

#include "unicode/ucnv.h"
#include "unicode/udat.h"
#include "unicode/ucal.h"
#include "unicode/uchar.h"
#include <unicode/ustdio.h>
#include "unicode/uscript.h"
#include "unicode/decompcb.h" /* from locexp/util */
#include "unicode/lx_utils.h"
#include "unicode/uset.h"
#include "icons.h"

#include "demo_settings.h"
#include "demoutil.h"

int validate_sanity();

#define HAVE_KANGXI
/* #define RADICAL_LIST */
#include <kangxi.c> /* Kang-Xi Radical mapping table */

typedef enum { ECHAR, ETOP, EBLOCK, ECOLUMN, ERADLST, ERADICAL, ENAME, EEXACTNAME, ESET, ESETCHUNK } ESearchMode;

#define kSetChunkWidth 10  /* chars per row in unicode set chunk */
#define kSetChunkHeight 10 /* chars per column in unicode set chunk */
#define kSetChunkSize (kSetChunkWidth*kSetChunkHeight)
#define kSetListSize 10

/* Protos */
int main(int argc, char **argv);

UChar32 doSearchBlock(int32_t, UChar32 startFrom);
UChar32 doSearchType(int8_t, UChar32 startFrom);
void showSearchMenu(UChar32 startFrom);
void printCharName(UChar32 ch);
void printModeCHAR(UChar32 ch);
void printModeSET(const char *qs, ESearchMode mode);
void printElsewhereBox(UChar32 block, ESearchMode mode);

/**
 * Print a row (like in "Column" view)
 * @param theChar Char to show
 * @param showBlock Whether to include the 'block' column (type)
 * @param hilite  Hilite for the items (ex: "" or " COLOR=#00B1FF ")
 * @param link Type of link to make under char name (ex 'k' for column)
 */
void printRow(UChar32 theChar, UBool showBlock, const char *hilite, const char *link);
void printRowHeader(UBool showBlock);


/* globals, current search.. */
int32_t gSearchType = -1;
int32_t gSearchBlock = -1;
char    gSearchName[512];
char    gSearchHTML[512];
UChar usf[1024]; /* unicode string set */
UChar32 gSearchChar = 0xFFFF;
UBool gSearchCharValid = FALSE;

UBool anyDecompose = FALSE;

UFILE      *gOut = NULL;

int enumHits = 0, foundCount = 0; /* # of enumerations, # of hits */

static const char htmlHeader[]=
    "Content-Type: text/html; charset=utf-8\n"
    "\n"
    "<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Transitional//EN\" \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd\">\n"
    "<html xmlns=\"http://www.w3.org/1999/xhtml\" xml:lang=\"en\" lang=\"en\">\n"
    "<head>\n";

static const char defaultHeader[]=
    "<meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\">\n"
    "<title>IBM: ICU Demonstration - Unicode Browser</title>\n";

static const char endHeaderBeginBody[] =
    "</head>\n"
    "<body>\n"
    
    "<style type=text/css>\n" 
                    "<!-- \n" 
    ".elsewhere { margin-top: 1em; margin-right: 3em; border: 1px solid gray; background-color:#FFFEFC; " 
         " padding: 1em 1em 1em 1em; font-size: smaller; } \n" 
    "--></style>";

static const char breadCrumbMainHeader[]=
    "<a class=\"bctl\" href=\"//www.ibm.com/software/\">Software</a><span class=\"bct\">&nbsp;&nbsp;&gt;&nbsp;</span>\n"
    "<a class=\"bctl\" href=\"//www.ibm.com/software/globalization/index.jsp\">Globalization</a><span class=\"bct\">&nbsp;&nbsp;&gt;&nbsp;</span>\n"
    "<a class=\"bctl\" href=\"//www.ibm.com/software/globalization/icu/index.jsp\">ICU</a><span class=\"bct\">&nbsp;&nbsp;&gt;&nbsp;</span>\n"
    "<a class=\"bctl\" href=\"//www.ibm.com/software/globalization/icu/chartsdemostools.jsp\">Demo</a><span class=\"bct\">&nbsp;&nbsp;&gt;&nbsp;</span>\n"
    "<h1>Unicode Browser</h1>\n";

static const char *htmlFooter=
    "</body>\n"
    "</html>";

void printCharName(UChar32 c)
{
  char junkbuf[512];
  UErrorCode status = U_ZERO_ERROR;
  u_charName(c, U_UNICODE_CHAR_NAME, junkbuf, 512, &status);

  /* once this used to lowercase the unicode names.....*/
/*    for(i=0;junkbuf[i];i++) */
/*    { */
/*      junkbuf[i] = tolower(junkbuf[i]); */
/*    } */

  if(U_SUCCESS(status))
    u_fprintf(gOut, "%s", junkbuf);
  else
    u_fprintf(gOut, "{U+%04X}", c);
}

/******************************************************** derived from ucnv_err.c */
#define ToOffset(a) a<=9?(0x0030+a):(0x0030+a+7)

#ifdef USE_SUBWITHVALUEHTML
/*Takes a int32_t and fills in  a UChar* string with that number "radix"-based
 * and padded with "pad" zeroes
 */
static void 
itou (UChar * buffer, int32_t i, int32_t radix, int32_t pad)
{
  int32_t length = 0;
  int32_t num = 0;
  int8_t digit;
  int32_t j;
  UChar temp;

  while (i > radix)
    {
      num = i / radix;
      digit = (int8_t) (i - num * radix);
      buffer[length++] = (UChar) (ToOffset (digit));
      i = num;
    }

  buffer[length] = (UChar) (ToOffset (i));

  while (length < pad)
    buffer[++length] = (UChar) 0x0030;        /*zero padding */
  buffer[length--] = (UChar) 0x0000;

  /*Reverses the string */
  for (j = 0; j < (pad / 2); j++)
    {
      temp = buffer[length - j];
      buffer[length - j] = buffer[j];
      buffer[j] = temp;
    }

  return;
}

void 
SubstituteWithValueHTML (UConverter * _this,
                         char **target,
                         const char *targetLimit,
                         const UChar ** source,
                         const UChar * sourceLimit,
                         int32_t *offsets,
                         UBool flush,
                         UErrorCode * err)
{
  UChar valueString[100];
  int32_t valueStringLength = 0;
  const UChar *mySource = *source;
  UChar codepoint[100];
  int32_t i = 0;
  /*Makes a bitwise copy of the converter passwd in */
  UConverter myConverter = *_this;
  char myTarget[100];
  char *myTargetAlias = myTarget;
  const UChar *myValueSource = NULL;
  UErrorCode err2 = U_ZERO_ERROR;


#ifdef WIN32
  if (!((*err == U_INVALID_CHAR_FOUND) || (*err == U_ILLEGAL_CHAR_FOUND)))    return;
#else
  if (CONVERSION_U_SUCCESS (*err))
    return;
#endif

  ucnv_reset (&myConverter);
  ucnv_setFromUCallBack (&myConverter,
                         (UConverterFromUCallback)   UCNV_FROM_U_CALLBACK_STOP, 
                         &err2);
  if (U_FAILURE (err2))
    {
      *err = err2;
      return;
    }
  u_uastrcpy(codepoint,"<I>\\u");

  while (i < _this->invalidUCharLength)
    {
      itou (codepoint + 5, _this->invalidUCharBuffer[i++], 16, 4);
      u_uastrcpy(codepoint+9, "</I>");
      memcpy (valueString + valueStringLength, codepoint, sizeof (UChar) * (5+4+4) );
      valueStringLength += (5+4+4) ;
    }

  myValueSource = valueString;

  /*converts unicode escape sequence */
  ucnv_fromUnicode (&myConverter,
                    &myTargetAlias,
                    myTargetAlias + 100,
                    &myValueSource,
                    myValueSource + (5+4+4),
                    NULL,
                    TRUE,
                    &err2);

  if (U_FAILURE (err2))
    {
      UCNV_FROM_U_CALLBACK_SUBSTITUTE (_this,
                                       target,
                                       targetLimit,
                                       source,
                                       sourceLimit,
                                       offsets,
                                       flush,
                                       err);
      return;
    }

  valueStringLength = myTargetAlias - myTarget;

  /*if we have enough space on the output buffer we just copy
   * the subchar there and update the pointer
   */
  if ((targetLimit - *target) >= valueStringLength)
    {
      memcpy (*target, myTarget, valueStringLength);
      *target += valueStringLength;
      *err = U_ZERO_ERROR;
    }
  else
    {
      /*if we don't have enough space on the output buffer
       *we copy as much as we can to it, update that pointer.
       *copy the rest in the internal buffer, and increase the
       *length marker
       */
      memcpy (*target, valueString, (targetLimit - *target));
      memcpy (_this->charErrorBuffer + _this->charErrorBufferLength,
                  valueString + (targetLimit - *target),
                  valueStringLength - (targetLimit - *target));
      _this->charErrorBufferLength += valueStringLength - (targetLimit - *target);
      *target += (targetLimit - *target);
      *err = U_INDEX_OUTOFBOUNDS_ERROR;
    }

  return;
}
#endif
/*******************************************************end of borrowed code from ucnv_err.c **/


void printOneUChar32(UChar32 theChar)
{
  UChar chars[20];
  int offset = 0;

  chars[0]=0;
  UTF_APPEND_CHAR_UNSAFE(chars, offset, theChar); 
  chars[offset] = 0; 
  if (chars[0] == '>')
    u_fprintf(gOut, "&gt;");
  else if (chars[0] == '<')
    u_fprintf(gOut, "&lt;");
  else
    u_fprintf(gOut, "%S", chars);
  u_fflush(gOut);
}


void explainOneUChar32(UChar32 theChar)
{
  UChar chars[20];
  int offset = 0;

  chars[0]=0;
  UTF_APPEND_CHAR_UNSAFE(chars, offset, theChar); 
  chars[offset] = 0; 
  u_fprintf(gOut, "<a STYLE=\"text-decoration:  none\" href=\"?ch=%04X\" ", theChar); 
  u_fprintf(gOut, " title=\"%04X: ", theChar);
  printCharName(theChar);
  u_fprintf(gOut, "\">%S", chars);
  u_fprintf(gOut, "</a>");
  u_fflush(gOut);
}

int words_count = 0;
char words[16][50];

void myEnumCharNamesFn_setup()
{
    const char *p, *q;
    int32_t len;
    p = gSearchName;
    
    while(isspace(*p)) p++;
    words_count = 0;
    while(p && *p)
    {
#if defined (UB_DEBUG)
      fprintf(stderr, "[P=%s count=%d\n", p, words_count);
#endif
        q = strchr(p, ' ');
        if(q == NULL)  /* last one */
        {
          strncpy(&(words[words_count][0]), p, 49);
        }
        else
        {
            len = q - p;
            if(len>48) 
            {
                len = 48;
            }
            strncpy(words[words_count], p, len);
            q++;
        }
        words[words_count][49]=0;
#if defined (UB_DEBUG)
        fprintf(stderr, " w=%s\n", words[words_count]);
#endif
        words_count++;
        p = q;
        if(p != NULL) {
            while(isspace(*p)) p++;
        }
        if(words_count >= 16) {
            return;
        }
    }
    
}

UBool myEnumCharNamesFn(void *context,
                        UChar32 code,
                        UCharNameChoice nameChoice,
                        const char *name,
                       int32_t length)
{
  UBool isFound = FALSE;
  enumHits++;
  if(strstr(name, gSearchName)) {
    isFound = TRUE;
  } else if(words_count>1) {
    const char *k = name;
    int n;
    for(n=0;k&&(n<words_count);n++) {
      if((k=strstr(k,words[n]))) {
        const char *nextk = k+strlen(words[n]);
#if defined (UB_DEBUG)
        fprintf(stderr, "[%d] considering %s -> %s\n", n, k, nextk);
#endif
        if((n>0)&&(k>name)&&(k[-1]!=' ')) { /* subsequent words must be on the beginning of a word boundary */
          n--; /* retry n */
        }
        k = nextk;
      }
    }
    if(k != NULL) {
      isFound = TRUE;
    }
  }
  if(isFound == TRUE) {
    if(foundCount == 0) {
      printRowHeader(TRUE);
    }

    foundCount ++;


    printRow(code, TRUE, "",  "ch");

    if((foundCount % 15) ==14) /* ------------ Search by name: too many hits --- */
    {
        u_fprintf(gOut, "</table> <blockquote><H4><a href=\"?s=%s&amp;cs=%04X\">Continue search for '%s' from U+%04X...</a></H4></blockquote><p>",
             gSearchHTML, code+1, gSearchName, code+1);
      return FALSE;
    }

/*     u_fprintf(gOut, "<LI><a href=\"?k1=%04X#here\"><tt>%04X</tt> - %s\n", */
/*            code, code, name); */
  
/*     printOneUChar32( code ); */
/*     u_fprintf(gOut, "</a>\n"); */
  }
  return TRUE;
}

void printIconMenu(const char *alt, const char *name, ESearchMode target, ESearchMode current)
{
  u_fprintf(gOut, "<input type=\"image\" "
	    " border=\"0\" width=\"32\" height=\"32\" "
	    "alt=\"%s%s%s\" value=\"%s%s%s\" "
	    "name=\"%s\" "
	    "src=\"%s/_/%s%s.gif\" "
	    " />\n",
	    (current==target)?"[":"", alt,  (current==target)?"]":"",
	    (current==target)?"[":"", alt,  (current==target)?"]":"",
	    name, 
	    getenv("SCRIPT_NAME"),
	    alt, (current==target)?"-x":"");
} 


void printCanonEquivs(UChar32 block)
{
  UErrorCode canErr = U_ZERO_ERROR;
  UChar ch[1024];
  UChar buf[1024];
  UCanonicalIterator iter = NULL;
  int offset=0;
  int bannered = 0;
  
  ch[0]=0;
  UTF_APPEND_CHAR_UNSAFE(ch, offset, block);
  ch[offset]=0;
  
  iter = ucanit_open(ch, &canErr);
  if(!iter || U_FAILURE(canErr)) {
    u_fprintf(gOut, "Fail opening canit: %p -> %s<p>\n", iter, u_errorName(canErr));
  } else {
    while(ucanit_next(iter, buf, 1024, &canErr) > 0) {
      int i;
      UChar32 c;
      int shownComma = 0;
      
      if(!u_strcmp(buf,ch)) continue;
      
      if(!bannered) {
        u_fprintf(gOut, "<p><b>Canonical Equivalencies:</b></p>\n");
        u_fprintf(gOut, "<ol>\n");
      }
      
      u_fprintf(gOut, "<li>%S<br />\n", buf);
      
      /* now show it as codepoints */
      for(i=0;buf[i];) {
        UTF_NEXT_CHAR_UNSAFE(buf,i, c);
        u_fprintf(gOut, "%s <a href=\"?ch=%04X\">U+%04X</a> ", (shownComma++)?",":"", c, c);
        printCharName(c);
      }
      u_fprintf(gOut, "<br />\n");
      u_fprintf(gOut, "<br /></li>\n");
      bannered =1;
    }
    if(bannered) {
      u_fprintf(gOut, "</ol>\n");
    }
    ucanit_close(iter);
  }
}


void printRowHeader(UBool showBlock)
{
  u_fprintf(gOut, "<tr>"); /* 0: column index */
#if 0
  u_fprintf(gOut, "<td></td>"); /* 0.5 zoom in */
#endif
  u_fprintf(gOut, "<td>C</td>"); /* 1: char */
  u_fprintf(gOut, "<td>N</td>"); /* 2: char #, name */
  u_fprintf(gOut, "<td><b>General Category</b></td>"); /* 3: type */ 
  if(showBlock == TRUE) 
    {
      u_fprintf(gOut, "<td><b>Block</b></td>"); /* 3 1/2 */
    }
  u_fprintf(gOut, "<td><b>Script</b></td>"); /* 3 3/4 Script */
  u_fprintf(gOut, "<td><b>#</b></td>"); /* 4 digit */
  u_fprintf(gOut, "<td><b>BiDi</b></td>"); /* 6 Direction (BiDi?) */
  u_fprintf(gOut, "</tr>\n");
}

void printRow(UChar32 theChar, UBool showBlock, const char *hilite, const char *link)
{
  UBool searchedFor;

  u_fprintf(gOut, "<tr %s >", hilite);


  /** 1 The Char **/
  u_fprintf(gOut, "<td ");

  /* TODO: 'if matches(theChar)' */

  u_fprintf(gOut, "%s align=\"center\">\n", hilite);
          
  if((theChar == gSearchChar) && gSearchCharValid)
    u_fprintf(gOut, "<a name=here></a>");
          
          /* print the simple data (i.e. the actual char ) */
  printOneUChar32(theChar);
          
  u_fprintf(gOut, "</td>");


          /** 2. print the char val and name  */
  u_fprintf(gOut, "<td>");
  if(link != NULL)
    {
      u_fprintf(gOut, "<a href=\"?%s=%04X#here\">", link, theChar);
    }
  u_fprintf(gOut, "<small>U+%04X ", theChar);
  printCharName(theChar);
  u_fprintf(gOut, "</small>");
  if(link != NULL)
    {
      u_fprintf(gOut, "</a>");
    }
  u_fprintf(gOut, "</td>");

          /** 3. char type */
  searchedFor = (u_charType(theChar) == gSearchType);
  u_fprintf(gOut, "<td>");
  if(searchedFor) {
    u_fprintf(gOut, "<b>");
  }
  u_fprintf(gOut, "%s",
            getUCharCategoryName(u_charType(theChar)));
  if(searchedFor)
    u_fprintf(gOut, "</b>");
  u_fprintf(gOut, "</td>");

  /** 3 1/2. optional - block */
  if(showBlock)
    {  
      u_fprintf(gOut, "<td ");
      searchedFor = (ublock_getCode(theChar) == gSearchBlock);
            
      if(searchedFor)
        u_fprintf(gOut, " bgcolor=\"#EE0000\" "); /* mark the one we were searching for */
            
      u_fprintf(gOut, " >");
            
      if(searchedFor)
        u_fprintf(gOut, "<b>");

           u_fprintf(gOut, "<a href=\"?scr=%d&amp;b=%04X\">", (ublock_getCode(theChar)) % UBLOCK_COUNT, theChar);
      u_fprintf(gOut, "%s", getUBlockCodeName(ublock_getCode(theChar)));
      u_fprintf(gOut, "</a>");
            
      if(searchedFor)
        u_fprintf(gOut, "</b>");
            
      u_fprintf(gOut, "</td>");
    }

  /** 3 3/4: script **/
  {
    UErrorCode scriptErr = U_ZERO_ERROR;
    UScriptCode scriptCode;
            
    scriptCode = uscript_getScript(theChar, &scriptErr);
    if(U_FAILURE(scriptErr))
      {
        u_fprintf(gOut, "<td bgcolor=\"#888888\">&nbsp;</td>\n");
      }
    else
      {
        u_fprintf(gOut, "<td>");
              
        u_fprintf(gOut, "%s", uscript_getShortName(scriptCode));

        u_fprintf(gOut, "</td>");
      }
  }

  /** 4: digit (optional) **/
  u_fprintf(gOut, "<td>");
  if(u_getNumericValue(theChar) != U_NO_NUMERIC_VALUE) {
    u_fprintf(gOut, "%g", u_getNumericValue(theChar));
  } else {
    u_fprintf(gOut, "-");
  }
  u_fprintf(gOut, "</td>");

          /** 6. direction **/
u_fprintf(gOut, "<td>%s", u_getPropertyValueName(UCHAR_BIDI_CLASS,
                                                      u_charDirection(theChar),
                                                      U_SHORT_PROPERTY_NAME));
  /*  put the mirrored thing here. Not so common, keeps it out of the way. */
  if(u_isMirrored(theChar))
    {
      u_fprintf(gOut, " (Mirrored)");
    }
  u_fprintf(gOut, "</td>");


          /** image - removed **/
/*            u_fprintf(gOut, "<td>"); */

/*                -- this works but it's kind of RUDE. -- [to link in w/o asking] */
/*                u_fprintf(gOut, "<IMG WIDTH=32 HEIGHT=32 SRC=\"http://charts.unicode.org/Unicode.charts/Small.Glyphs/%02X/U%04X.gif\">\n", */
/*                   ((theChar&0x1FFF00)>>8), */
/*                   theChar); */
/*  #error you're rude */
/*            u_fprintf(gOut, "</td>"); */

          
#if 0  /* 8+ - space */
          u_fprintf(gOut, "<td>");
          u_fprintf(gOut, "%c%c%c",
                    !u_isspace(theChar)      ?'-':'S',
                    !u_isWhitespace(theChar) ?'-':'W',
                    !u_isUWhiteSpace(theChar)?'-':'U');
          u_fprintf(gOut, "</td>");
#endif

          u_fprintf(gOut, "</tr>\n");

}

int
main(int argc,
     char **argv)
{
  char *qs;
  char *pi;
  char *tmp = NULL;
  UChar chars[800];
  UChar32 theChar;
  int n,i,r,c,uc;
  UBool searchedFor;
  UChar32 block = 0xFFFF;
  ESearchMode mode;
  UVersionInfo uvi;
  UErrorCode status = U_ZERO_ERROR;

  chars[1] = 0;
  pi = getenv("PATH_INFO");

  if(pi && !strncmp(pi, "/_/", 3)) {
    pi += 3;
    if(icons_init()) {
      printf("Content-type: text/plain\n\n");
      printf("fatal: couldn't open ubrowse data. %s\n", u_errorName(status));
    }
    icons_write(pi);
    return 0;
  }


  gOut = u_finit(stdout, "en_US", "UTF-8");

  if(gOut == NULL)
  {
      printf("Couldnt' open output file.\n");
      return 0;
  }

  u_fprintf(gOut, "%s", htmlHeader);
  if (!printHTMLFragment(gOut, NULL, DEMO_COMMON_DIR "ubrowse-header.html")) {
      u_fprintf(gOut, "%s", defaultHeader);
  }
  u_fprintf(gOut, "%s", endHeaderBeginBody);
  if (printHTMLFragment(gOut, NULL, DEMO_COMMON_MASTHEAD)) {
      u_fprintf(gOut, "%s", DEMO_BEGIN_LEFT_NAV);
      printHTMLFragment(gOut, NULL, DEMO_COMMON_LEFTNAV);
      u_fprintf(gOut, "%s", DEMO_END_LEFT_NAV);
      u_fprintf(gOut, "%s", DEMO_BEGIN_CONTENT);
  }
  u_fprintf(gOut, "%s", breadCrumbMainHeader);

  if(lxu_validate_property_sanity()) {
    u_fprintf(gOut, "<H1>Err, uchar.h changed without update to lx_utils.c</H1>\n");
  }

  /* set up the converter */
  {
     UConverter *cnv;
     cnv = u_fgetConverter(gOut);
     if(cnv)
          ucnv_setSubstChars(cnv,"_",1,&status);  /* DECOMPOSE calls SUBSTITUTE on failure. */
  }
#if 0 
/* until it is implemented for 1.6 (!!!) */
  ucnv_setFromUCallBack(gConverter, &UCNV_FROM_U_CALLBACK_DECOMPOSE, &status);
#endif
  
  /* Now, see what we're supposed to do */
  qs = getenv("QUERY_STRING");
  if (qs==NULL)
      qs = "";
  mode = ETOP;

  if(qs)
    { 
      if(strstr(qs,"&gosetn")) {
        mode = ESETCHUNK;
      } else if(strstr(qs,"&gosetk")) {
        mode = ESET;
      } else if (sscanf(qs,"go=%x", &block)== 1)
        {
          if(strstr(qs,"ch.x="))
          {
            mode = ECHAR;
          }
          else if(strstr(qs,"k.x="))
          {
            block &= 0x1FFFF0;
            mode = ECOLUMN;
          }
          else if(strstr(qs, "n.x="))
          {
            block &= 0x1FFF00;
            mode = EBLOCK;
          }
          else
          {
            mode = ETOP;
            /* Title comes lower */
          }
        }
      else if(sscanf(qs,"ch=%x", &block)== 1)
        {
          mode = ECHAR;
        }
      else if(sscanf(qs,"n=%x", &block)== 1)
        {
          block &= 0x1FFF00;
          mode = EBLOCK;
        }
      else if(sscanf(qs, "k1=%x", &block) == 1)
        {
          gSearchChar = block;
          gSearchCharValid = TRUE;
          block &= 0x1FFFF0;
          mode = ECOLUMN;
        }
      else if(sscanf(qs, "k=%x", &block) == 1)
        {
          block &= 0x1FFFF0;
          mode = ECOLUMN;
        }
      else if(sscanf(qs, "scr=%d&b=%x", &gSearchBlock, &block) == 2)
        {
          mode = ECOLUMN;
          block = doSearchBlock(gSearchBlock, block);
          block &= 0x1FFFF0;
        }
      else if(sscanf(qs, "typ=%d&b=%x", &gSearchType, &block) == 2)
        {
          mode = ECOLUMN;
          block = doSearchType(gSearchType, block);
          block &= 0x1FFFF0;
        }
/* For: search -> block (not column) 
      else if(sscanf(qs, "scr=%d&n=%x", &gSearchBlock, &block) == 2)
        {
          block = doSearchBlock(gSearchBlock, block);
          block &= 0x1FFF00;
          mode = EBLOCK;
        }
      else if(sscanf(qs, "typ=%d&n=%x", &gSearchType, &block) == 2)
        {
          block = doSearchType(gSearchType, block);
          block &= 0x1FFF00;
          mode = EBLOCK;
        }
*/
      else if(sscanf(qs, "radlst=%d", &block) == 1)
        {
          mode = ERADLST;
        }
      else if(sscanf(qs, "radical=%d", &block) == 1)
        {
          mode = ERADICAL;
        }
      else if(sscanf(qs, "s=%200s", gSearchHTML) == 1)
      {
        char *ss = NULL;
        char *cs = NULL;
        char *p;

        gSearchHTML[199]=0;
        
        block = 0;
        /* look for 'continue' tag */
        if((cs=strstr(qs,"cs=")))
        {
          if(sscanf(cs,"cs=%X",&block) != 1)
          {
            block = 0; /* error */
          }
        }

        /* look for 'exact' tag */
        ss = strstr(gSearchHTML,"&sx=");
        
        mode = ENAME;
        
        if(ss != NULL)
        {
          *ss=0;
          mode=EEXACTNAME;
        }

        if((ss=strchr(gSearchHTML, '&')))
        {
          *ss = 0;
        }

        /* copy - safely */
        strncpy(gSearchName,gSearchHTML,200);
        gSearchName[200] = 0;

        /* de HTML ize. Note we don't care about %s'. */
        for(p=gSearchName;*p;p++)
        {
          *p = toupper(*p);
          if(*p == '+')
            *p = ' ';
        }
        
      }
    }

  u_fprintf(gOut, "<form method=\"get\" action=\"%s\"><table summary=\"Navigation Control\" border=\"1\" cellpadding=\"1\" cellspacing=\"1\"><tr>", getenv("SCRIPT_NAME"));

  u_fprintf(gOut, "<td >");

  u_fprintf(gOut, "<label for=\"go\">Go: </label><input id=\"go\" type=\"text\" size=\"6\" name=\"go\" value=\"%04X\" />\n", block);

  /* show which item we're on */
  printIconMenu("ch", "ch", ECHAR, mode);
  printIconMenu("column", "k", ECOLUMN, mode);
  printIconMenu("block",  "n", EBLOCK, mode);
  u_fprintf(gOut, " | ");
  printIconMenu("all",    "all", ETOP, mode);

  /* forward and back buttons */
  switch(mode)
  {
  case EBLOCK:
    u_fprintf(gOut, " | <a href=\"?n=%04X\">prev</a> <a href=\"?n=%04X\">next</a> ",
           (block & 0x1FFF00)-0x100, (block & 0x1FFF00)+0x100);
    break;

  case ECOLUMN:
    u_fprintf(gOut, " | <a href=\"?k=%04X\">prev</a> <a href=\"?k=%04X\">next</a> ",
             (block & 0x1FFFF0)-0x10, (block & 0x1FFFF0)+0x10);
    break;

  case ECHAR:
    u_fprintf(gOut, " | <a href=\"?ch=%04X\">prev</a> <a href=\"?ch=%04X\">next</a> ",
             (block & 0x1FFFFF)-0x1, (block & 0x1FFFFF)+0x1);
    break;
  default:
    u_fprintf(gOut, "<!-- Unknown mode! -->\r\n");
  }

  u_fprintf(gOut, "");
  u_fprintf(gOut, "</td>\n");

  u_fprintf(gOut, "<td>");

  {
    const char *q;
    q = strstr(qs,"&us=");
    if(!q) { q = ""; } else { q += 4; }
    usf[0]=0;
    unescapeAndDecodeQueryField_enc(usf, 1023,
                                    q, "UTF-8");
  }
  u_fprintf(gOut, "<label for=\"set\">Set:</label><input id=\"set\" type=\"text\" name=\"us\" size=\"50\" value=\"%S\" />", usf);

  printIconMenu("column", "gosetk", ESET, mode);
  printIconMenu("block", "gosetn", ESETCHUNK, mode);
  u_fprintf(gOut, "</td>\n");

  if(mode == ETOP) /* top level list of blocks ******************************** ETOP ********** */
  {
      u_fprintf(gOut, "</tr></table></form>"); /* closer of menu */

      u_fprintf(gOut, "<br /><table summary=\"UnicodeBrowser\" border=\"0\" cellpadding=\"1\" cellspacing=\"1\"><tr><td bgcolor=\"#cccccc\">\n");

      u_fprintf(gOut, "<b>Unicode Browser</b> - Click on a type of character view it in more detail<br />\n");
      u_fprintf(gOut, "</td></tr><tr><td bgcolor=\"#eeeeee\">\n"
             );

      u_fprintf(gOut, "<b>Blocks:</b> ");
      for(i=UBLOCK_BASIC_LATIN;i<UBLOCK_COUNT;i++)
      {
        if (UBLOCK_BASIC_LATIN != i) {
          u_fprintf(gOut, " | &nbsp;");
        }
        u_fprintf(gOut, "<a href=\"?scr=%d&amp;b=0\">%s</a>", i, getUBlockCodeName(i));
      }
      u_fprintf(gOut, "<br />\n<br />\n");
      

      u_fprintf(gOut, "<b>General Categories:</b> ");
      for(i=U_UNASSIGNED;i<U_CHAR_CATEGORY_COUNT;i++)
      {
        if (U_UNASSIGNED != i) {
          u_fprintf(gOut, " | &nbsp;");
        }
        u_fprintf(gOut, "<a href=\"?typ=%d&amp;b=0\">%s</a>", i, getUCharCategoryName(i));
      }
      u_fprintf(gOut, "<br />");


      u_fprintf(gOut, "</td></tr></table>\n");

/*       u_fprintf(gOut, "\n</td></tr><tr><td align=right>\n"); */
      
      showSearchMenu(0x0000);
    }      
  else if (mode == EBLOCK) /* *************** BLOCK *******************************************/
    {
    u_fprintf(gOut, "</tr></table></form>"); /* closer of menu */

      if(block <= 0xFFFF)
        {
            printElsewhereBox(block,mode);
        }

      /* Unicode table  at block 'block' */

      n = 0;
      /*      for(n=0; n<0x100; n += 0x080) */
        {
          u_fprintf(gOut, "<table summary=\"Block View\" border=\"1\">");
          u_fprintf(gOut, "<tr><td></td>");
          for(c = n;c<(n + 0x100);c+= 0x10)
            {
              u_fprintf(gOut, "<td><b><a href=\"?k=%04X\"><tt>%03X</tt></a></b></td>", (block|c),  (block | c) >> 4   );
            }
          u_fprintf(gOut, "</tr>\n");
          for(r = 0; r < 0x10; r++)
            {
              u_fprintf(gOut, "<tr><td><b><tt>%X</tt></b></td>", r);
              for(c = n;c<(n + 0x100);c+= 0x10)
                {
                  theChar = (block | r | c );

                  u_fprintf(gOut, "<td ");
                  
                  if(u_charType(theChar) == U_UNASSIGNED)
                    {
                      u_fprintf(gOut, " bgcolor=\"#888888\" ");
                    }
                  
                  u_fprintf(gOut, " align=center>\n");
                  
                  
                  /* print the simple data */
                  explainOneUChar32(theChar);
                  
                  u_fprintf(gOut, "</td>");
                }
              u_fprintf(gOut, "<td><b><tt>%X</tt></b></td>", r);
            }
        }
        u_fprintf(gOut, "</tr>");
        u_fprintf(gOut, "<tr><td colspan=\"18\" align=\"center\"><i>Click on a column number to zoom in.</i></td></tr>\n");
        u_fprintf(gOut, "</table>");
      u_fprintf(gOut, "<hr />\n");
      showSearchMenu( block + 0x0100);
    }
  else if(mode == ECOLUMN ) /****************************** COLUMN **************************/
    {
      const char *hilite;
      UBool showBlock = FALSE;
      if(  ((block&~0xF)==0xFFF0) ||    /* FFFD/FFFF and */
           ((block&~0xF)==0xFEF0) ) {   /* FEFF are exceptional columns */
        showBlock = TRUE;
      }
      /* Unicode table  at column 'block' */

      if(showBlock == FALSE) /* Explain what the block is ONCE: here */
      {  
        theChar = block;
        u_fprintf(gOut, "</td></tr></table></form>");  /* closer */
        u_fprintf(gOut, "<br />"); 
        u_fprintf(gOut, "<p><b>Block:</b>  ");
        searchedFor = (ublock_getCode(theChar) == gSearchBlock);
        
        
        if(searchedFor)
          u_fprintf(gOut, "<b>");
        
        u_fprintf(gOut, "%s", getUBlockCodeName(ublock_getCode(theChar)));
        if(searchedFor)
          u_fprintf(gOut, "</b>");
        u_fprintf(gOut, "</p>");

        /*  "| Next " on block list . removed. 
         * u_fprintf(gOut, " | <a href=\"?scr=%d&b=%04X\">", (ublock_getCode(theChar)+1)%U_CHAR_SCRIPT_COUNT, theChar);
         * u_fprintf(gOut, "next</a>");
         */
            
      }
      else
      {
        u_fprintf(gOut, "</tr></table></form>"); /* closer of menu */
      }



      u_fprintf(gOut, "<table summary=\"Unicode Column\" border=\"1\">");


      printRowHeader(showBlock);

      for(r = 0; r < 0x10; r++)  /***** rows ******/
        {
          theChar = (block | r );

          /* Do we have a match? for cell highlighting */ 
          if( (ublock_getCode(theChar) == gSearchBlock) || (u_charType(theChar) == gSearchType) || ((theChar == gSearchChar) && gSearchCharValid))
          {
            hilite = " bgcolor=\"#ffdddd\" ";
          }
          else if(u_charType(theChar) == U_UNASSIGNED)
          {
            hilite = " bgcolor=\"#666666\" ";
          }
          else 
          {
            hilite = "";
          }

          printRow(theChar, showBlock, hilite, "ch");

        }
      

      u_fprintf(gOut, "</table><hr />");

      showSearchMenu( block + 0x0010);

    }
  else if(mode == ECHAR) /************************* CHAR *****************************/
    {
      u_fprintf(gOut, "</tr></table></form>"); /* closer of menu */

      if(block <= 0xFFFF) {
        printElsewhereBox(block, mode);
      }

      printModeCHAR(block);

      u_fprintf(gOut, "<br /><hr />\n");

      showSearchMenu( block + 1);
    }
#ifdef RADICAL_LIST 
  else if(mode == ERADLST) /************************ RADICAL LIST ********************/
    {
      u_fprintf(gOut, "</table></form>");
      u_fprintf(gOut, "<table  summary=\"radical list\" border=\"1\">");
      u_fprintf(gOut, "<tr>");
      for(i=0;gKangXiRadicalTable[i];i+=2)
        {
          if((i%16) == 0)
            u_fprintf(gOut, "</tr><tr>\n");
          u_fprintf(gOut, "<td>");
          u_fprintf(gOut, "<a href=\"?radical=%d\">", (i/2)+1);
          printOneUChar32(gKangXiRadicalTable[i]);
          u_fprintf(gOut, "</a>");
          u_fprintf(gOut, "</td>");

        }
      u_fprintf(gOut, "</tr></table>");
    }
  else if(mode == ERADICAL) /************************* RADICAL ************************/
    {
      FILE *f;
      int u,stroke;
      char s[200];

      u_fprintf(gOut, "</td></tr></table></form>"); /* closer */

      u_fprintf(gOut, "<b>");
      printOneUChar32(gKangXiRadicalTable[(block-1)*2]);
      u_fprintf(gOut, " (radical %d)</b> \n",block);
      printCharName(gKangXiRadicalTable[(block-1)*2]);
      sprintf(s,"/home/srl/icu/data/PSI/rad%03d.lst", block);
      u_fprintf(gOut, "<table border=\"1\">");
      f = fopen(s,"r");
      i=0;
      u_fprintf(gOut, "<tr>");
      while(fscanf(f,"%04X\t%d", &u, &stroke) == 2)
        {
          if(!( (i++)%10))
            u_fprintf(gOut, "</tr><tr>");
          u_fprintf(gOut, "<td>");
          u_fprintf(gOut, "%d ", stroke);
          u_fprintf(gOut, "<a href=\"?k1=%04X#here\">", u);
          printOneUChar32(u);
          u_fprintf(gOut, "</a></td>");
        }
      fclose(f);
      u_fprintf(gOut, "</tr></table>");
    }
#endif /* RADICAL_LIST */
  else if(mode == ENAME) /************************ NAME ***************************/
    {
      UErrorCode status = U_ZERO_ERROR;

      u_fprintf(gOut, "</tr></table></form>"); /* closer */

      /* "Be careful what you search for, you just might find it"
         (and 0x10FFFE of it's close friends!)
      */

      if(block == 0)
      {
        u_fprintf(gOut, "<H1>Searching for '%s'...</H1>\n", gSearchName);
      }
      else
      {
        u_fprintf(gOut, "<H1>Searching for '%s' from U+%04X...</H1>\n", gSearchName, block);
      }
      
      u_fprintf(gOut, "<table summary=\"Search Results\" border=\"1\">");
      
      myEnumCharNamesFn_setup(); /* break apart words */

      u_enumCharNames(block, UCHAR_MAX_VALUE,
                      myEnumCharNamesFn,
                      NULL,
                      U_UNICODE_CHAR_NAME,
                      &status);

      if(foundCount == 0)
        {
          u_fprintf(gOut, "<tr><td><i>Not found.</i></td></tr>\n");
        }
       if((foundCount % 15) !=14) {
           u_fprintf(gOut, "</table>");
       }
        u_fprintf(gOut, "<!-- %d tested --><hr />\n",enumHits);
      showSearchMenu( 0x0000 ); 
    }
  else if(mode == EEXACTNAME) /************** EXACT NAME ********************************/
    {
      UChar32 c;
      UErrorCode status = U_ZERO_ERROR;

      u_fprintf(gOut, "</td></tr></table></form>"); /* closer */


      u_fprintf(gOut, "<H1>Searching for '%s'...</H1>\n", gSearchName);
      
      
      u_fprintf(gOut, "<hr /><ul>\n");

      c = u_charFromName(U_UNICODE_CHAR_NAME,
                         gSearchName,
                         &status);

      if(U_FAILURE(status))
      {
          u_fprintf(gOut, "Error - %s %d<p>\n", u_errorName(status), status);
      }
      else if(c == 0x00FFFF )
      {
        u_fprintf(gOut, "<i>Not found.</i><p>Please note that this search requires an exact unicode name.\n");
      }
      else
      {
        u_fprintf(gOut, "<li><a href=\"?k1=%04X#here\"><tt>%04X</tt> - %s\n",
               c,c,gSearchName);
        printOneUChar32(c);
        
      }

      u_fprintf(gOut, "</ul><hr />\n");
      showSearchMenu( 0x0000 ); 
    } else if((mode == ESET)||(mode=ESETCHUNK)) {  /************** UnicodeSet****************/
    printModeSET(qs, mode);
    u_fprintf(gOut, "<P><P><P><P><P>\n");
  }  else /************************************* ????????????????????????? ****************/
    {
      u_fprintf(gOut, "</td></tr></table></form>"); /* closer */

      u_fprintf(gOut, "Unknown search mode ! <hr />\n");
    }
  

  u_fprintf(gOut, "</td></tr></table>\n\n"); /* ? */



  if(anyDecompose)
    {
      u_fprintf(gOut, "Note: text in <FONT COLOR=\"#00DD00\">");
      u_fprintf(gOut, "<I>Italic Green</I></font> is the best-try using decomposition. Underscore (_) denotes missing characters in the decomposition.<p>");

    }

  uvi[0]=0xFF;
  uvi[1]=0xFF;
  uvi[2]=0xFF;
  uvi[3]=0xFF;
  u_getUnicodeVersion(uvi);
  u_fprintf(gOut, "<br /><a href=\"http://www.unicode.org/\">Based on Unicode ");
  for(uc=0;uc<U_MAX_VERSION_LENGTH;uc++) {
    if((uc < 3) || (uvi[uc] != 0)) /* don't show trailing zero in 4th place. */
      u_fprintf(gOut, "%d.",uvi[uc]);
  }
  u_fprintf(gOut, "</a>\n");

  u_fprintf(gOut, "<br />Powered by <a href=\"" ICU_URL "\">ICU %s</a>\n", U_ICU_VERSION);
  
  u_fprintf(gOut, "%s", DEMO_END_CONTENT);
  printHTMLFragment(gOut, NULL, DEMO_COMMON_FOOTER);
  u_fprintf(gOut, "%s", htmlFooter);

  u_fclose(gOut);

  return 0;
}

/* still lazy evaluated. but it'll give us an alternate strcpy */


UChar32 doSearchBlock(int32_t block, UChar32 startFrom)
{
  UChar32 end = (startFrom-1);
  int wrapped = 0;
  if(end > UCHAR_MAX_VALUE)
  {
    end = UCHAR_MAX_VALUE;
  }

  for(;startFrom != end; startFrom++)
  {
    if(startFrom > UCHAR_MAX_VALUE)
   {
      startFrom = UCHAR_MIN_VALUE;
      wrapped++;
	if(wrapped > 2) return startFrom; /* bug */ 
   }
   if(ublock_getCode(startFrom) == block)
      return startFrom;
  }

  return startFrom;
}

UChar32 doSearchType(int8_t type, UChar32 startFrom)
{
  UChar32 end = (startFrom-1);
#if defined(UB_DEBUG)
  int tick  =0;
#endif
  if(end > UCHAR_MAX_VALUE) {
    end = UCHAR_MAX_VALUE;
  }

  for(;startFrom != end; startFrom++)
  {
    if(startFrom > UCHAR_MAX_VALUE)
    {
      startFrom = UCHAR_MIN_VALUE;
    }

    if(u_charType(startFrom) == type)
      return startFrom;


#if defined (UB_DEBUG)
    tick++;

    if((tick%1024) ==0) {
      fprintf(stderr, "%d: Searching, %04X... to %04X\n", getpid(), startFrom, end);
      fflush(stderr);
    }
#endif
  }

  return startFrom;
}

void showSearchMenu(UChar32 startFrom)
{
  int32_t i;
    /* was: width=100% */
  u_fprintf(gOut, "<br /><table summary=\"Search Menu\" border=\"0\" cellpadding=\"0\" cellspacing=\"0\" ><tr><td bgcolor=\"#000000\">\n"
            "<table summary=\"\" border=\"0\" cellpadding=\"1\" cellspacing=\"1\"><tr><td bgcolor=\"#cccccc\">\n"
            "\n");

  u_fprintf(gOut, "<b>Find</b><br />\n"
         "\n"
         "</td></tr><tr><td bgcolor=\"#eeeeee\">\n"
         "<form action=\"%s\" method=\"get\"><table width=\"453\" border=\"0\" summary=\"\" cellspacing=\"8\">\n<tr>\n"
         "<td align=\"left\"><label for=\"block\">Block:</label></td>\n", getenv("SCRIPT_NAME"));

  u_fprintf(gOut, "<td align=\"right\"><select id=\"block\" name=\"scr\">\n");
  for(i=0;i<(UBLOCK_COUNT-1);i++)
  {
      u_fprintf(gOut, "<option ");
      if(getUBlockCodeSorted(i) == gSearchBlock)
        u_fprintf(gOut, " selected=\"selected\" ");
      u_fprintf(gOut, " value=\"%d\">%s", getUBlockCodeSorted(i), getUBlockCodeSortedName(i));
      u_fprintf(gOut, "</option>\n");
  }
  u_fprintf(gOut, "</select>\n");
  u_fprintf(gOut, "<input type=\"hidden\" name=\"b\" value=\"%04X\" />\n", startFrom);
  u_fprintf(gOut, "<input type=\"submit\" value=\"Find%s\" />\n",
         (startFrom != 0)?" Next":"");
  u_fprintf(gOut, "</td>\n</tr></table></form>\n");

  u_fprintf(gOut, "<form action=\"%s\" method=\"get\">\n<table width=\"453\" border=\"0\" summary=\"\" cellspacing=\"8\">\n<tr>\n<td align=\"left\"><label for=\"generalcategory\">General Category: </label></td>\n<td align=\"right\">", getenv("SCRIPT_NAME"));
  u_fprintf(gOut, "<select id=\"generalcategory\" name=\"typ\">\n");
  for(i=0;i<U_CHAR_CATEGORY_COUNT;i++) {
    u_fprintf(gOut, "<option ");
    if(getUCharCategorySorted(i) == gSearchType)
      u_fprintf(gOut, " selected=\"selected\" ");
    u_fprintf(gOut, " value=\"%d\">%s", getUCharCategorySorted(i), getUCharCategorySortedName(i));
    u_fprintf(gOut, "</option>\n");
  }
  u_fprintf(gOut, "</select>\n");
  u_fprintf(gOut, "<input type=\"hidden\" name=\"b\" value=\"%04X\" />\n", startFrom);
  u_fprintf(gOut, "<input type=\"submit\" value=\"Find%s\" />", 
         (startFrom != 0)?" Next":"");
  u_fprintf(gOut, "");

  u_fprintf(gOut, "\n</td>\n</tr>\n</table></form>\n");
  u_fprintf(gOut, "<form action=\"%s\" method=\"get\">\n<table width=\"453\" border=\"0\" summary=\"\" cellspacing=\"8\">\n<tr>\n<td align=\"left\"><label for=\"name\">Name:</label></td>\n<td align=\"right\"><input id=\"name\" type=\"text\" size=\"40\" name=\"s\" value=\"%s\" />\n\n<input id=\"exact\" type=\"checkbox\" name=\"sx\" />\n<label for=\"exact\">Exact?&nbsp;</label><input type=\"submit\" value=\"Find\" />\n",
            getenv("SCRIPT_NAME"), gSearchName);
  u_fprintf(gOut, "</td></tr></table>\n");

#ifdef RADICAL_LIST
  u_fprintf(gOut, "<td><a href=\"?radlst=1\">Radicals</a></td>");
#endif
  
  u_fprintf(gOut, "</form>\n<table border=\"0\" summary=\"\" cellspacing=\"8\">\n<tr>\n<td>\n"
         "\n");
  u_fprintf(gOut, "<br />\n");
  u_fprintf(gOut, "\n"
         "</td></tr></table>\n"
         "</td></tr></table>\n");

}

void printModeCHAR(UChar32 ch) 
{
  int i;
  u_fprintf(gOut, "<table summary=\"Canonical Equivalents\" border=\"5\" cellpadding=\"3\" cellspacing=\"6\"><tr><td style=\"font-size: 300%%;\">&nbsp;");
  printOneUChar32(ch);
  u_fprintf(gOut, "&nbsp;</td><td>");
  
  printCanonEquivs(ch);
  
  u_fprintf(gOut, "</td></tr></table><br /><br />\n");
  
  u_fprintf(gOut, "<table summary=\"Character Basic Row\" border=\"2\">");
  printRowHeader(TRUE);
  printRow(ch, TRUE, "", "k1");
  u_fprintf(gOut, "</table>\n<br /><br />\n");
  
  u_fprintf(gOut, "<table summary=\"Detailed Character Properties\" border=\"2\">\n");
  u_fprintf(gOut, "<tr><td><b>PN</b></td><td><b>Type</b></td><td><b>Name</b></td><td><b>PVN</b></td><td><b>Value</b></td></tr>\n");
  
  {
    UVersionInfo va;
    char age[U_MAX_VERSION_STRING_LENGTH];
    u_charAge(ch, va);
    u_versionToString(va, age);
    u_fprintf(gOut, "<tr><td>-</td><td>%s</td><td>%s</td><td></td><td>%s</td></tr>\n",
              "Vers", "Derived Age", age);
  }
  
  for(i=UCHAR_BINARY_START;i<UCHAR_BINARY_LIMIT;i++) {
#if 0
    UBool has;
    has = u_hasBinaryProperty(ch, i);
    
    u_fprintf(gOut, "<tr><td>%d</td><td>%s</td><td>%s%s%s</td><td>%s</td><td>%s</td></tr>\n",
              i, "bin", has?"<b>":"", getUPropertyName(i), has?"</b>":"",  has?"T":"f", u_getPropertyValueName(i,u_getIntPropertyValue(ch,i),U_LONG_PROPERTY_NAME));
#endif
    int32_t has;
    has = u_getIntPropertyValue(ch, i);
    
    u_fprintf(gOut, "<tr><td>%d</td><td>%s</td><td>%s</td><td>%ld (%ld..%ld)</td><td>%s</td></tr>\n",
              i, "bin", getUPropertyName(i), has,
              u_getIntPropertyMinValue(i),
              u_getIntPropertyMaxValue(i) ,u_getPropertyValueName(i,u_getIntPropertyValue(ch,i),U_LONG_PROPERTY_NAME));
    
  }
  
  for(i=UCHAR_INT_START;i<UCHAR_INT_LIMIT;i++) {
    int32_t has;
    has = u_getIntPropertyValue(ch, i);
    
    u_fprintf(gOut, "<tr><td>%d</td><td>%s</td><td>%s</td><td>%ld (%ld..%ld)</td><td>%s</td></tr>\n",
              i, "int", getUPropertyName(i), has,
              u_getIntPropertyMinValue(i),
              u_getIntPropertyMaxValue(i) ,u_getPropertyValueName(i,u_getIntPropertyValue(ch,i),U_LONG_PROPERTY_NAME));
    
    
  }
  
  for(i=UCHAR_MASK_START;i<UCHAR_MASK_LIMIT;i++) {
    int32_t has;
    has = u_getIntPropertyValue(ch, i);
    
    u_fprintf(gOut, "<tr><td>%d</td><td>%s</td><td>%s</td><td>%ld (%ld..%ld)</td><td>%s</td></tr>\n",
              i, "int", getUPropertyName(i), has,
              u_getIntPropertyMinValue(i),
              u_getIntPropertyMaxValue(i) ,u_getPropertyValueName(i,u_getIntPropertyValue(ch,i),U_LONG_PROPERTY_NAME));
    
  }
  
#if 0
  for(i=UCHAR_DOUBLE_START;i<UCHAR_DOUBLE_LIMIT;i++) {
    double has;
    has = u_getDoublePropertyValue(ch, i);
    
    u_fprintf(gOut, "<tr><td>%d</td><td>%s</td><td>%s</td><td>%g (%ld..%ld)</td><td>%s</td></tr>\n",
              i, "int", getUPropertyName(i), has,
              u_getIntPropertyMinValue(i),
              u_getIntPropertyMaxValue(i) ,u_getPropertyValueName(i,u_getIntPropertyValue(ch,i),U_LONG_PROPERTY_NAME));
    
  }
  
  for(i=UCHAR_STRING_START;i<UCHAR_STRING_LIMIT;i++) {
    int32_t has;
    has = u_getIntPropertyValue(ch, i);
    
    u_fprintf(gOut, "<tr><td>%d</td><td>%s</td><td>%s</td><td>%ld (%ld..%ld)</td><td>%s</td></tr>\n",
              i, "int", getUPropertyName(i), has,
              u_getIntPropertyMinValue(i),
              u_getIntPropertyMaxValue(i) ,u_getPropertyValueName(i,u_getIntPropertyValue(ch,i),U_LONG_PROPERTY_NAME));
    
  }
#endif
  
  u_fprintf(gOut, "</table>\n");
  
  u_fprintf(gOut, "<br /> The PN column and the PVN column refer to the internal property number and property value number. Except in the case of the Canonical_Combining_Class, these numbers have no relevance outside of ICU. ");
  
}

void printModeSET(const char *qs, ESearchMode mode)
{
    const char *q;
    UErrorCode status = U_ZERO_ERROR;
    USet  *aSet = NULL;
    UConverter *u7 = NULL;
    int32_t itemN = 0;
    int32_t charN = 0;
    int32_t len;
    int32_t countDown;
    UChar32 start = 0;
    UChar32 end = 0;
    UChar32 cur = 0xFFFF; /* cur>end = invalid */
    int32_t iCount; /* item count */
    int32_t charCount; /* chars count */
    UChar   littleBuffer[412];
    UBool charNSet = FALSE;
    int bCount =0;

    u_fprintf(gOut, "</td></tr></table></form>"); /* closer */
    
    if((q = strstr(qs,"itemN="))) {
      q += 6;    itemN=atoi(q);
    }
    if((q = strstr(qs,"charN="))) {
      q += 6;    charN=atoi(q);
      charNSet = TRUE;
    }

#if defined (UB_DEBUG)
    u_fprintf(gOut, "Usf: |%S|<br />\n", usf);
#endif
    aSet = uset_openPattern(usf, -1, &status);

    if(U_FAILURE(status)) {
      goto uFailIt;
    }

    iCount = uset_getItemCount(aSet);
    charCount = uset_size(aSet);

    if(charCount == 1) {
      mode = ECHAR; 
    } else if (mode==ESETCHUNK) {
      if(charCount <= kSetListSize) {
        countDown = kSetListSize;
        mode = ESET;
      } else {
        countDown = kSetChunkSize;
      }
    } else { /* ESET */
      countDown = kSetListSize;
    }
    
#if defined (UB_DEBUG)
    u_fprintf(gOut, "chars %d, items %d<p>\n", charCount, iCount);
#endif
    
    if(mode == ECHAR) {
      UErrorCode iStatus = U_ZERO_ERROR;
      len = uset_getItem(aSet, 0, &start, &end, littleBuffer, 
                         (sizeof(littleBuffer)/sizeof(littleBuffer[0]))-1,&iStatus);
      if(U_FAILURE(iStatus)) {
        u_fprintf(gOut, "<b>err %s</b><p>\n", u_errorName(iStatus));
      } else if(len == -1) {
        u_fprintf(gOut, "<b>err - out of range</b><p>\n");
      } else if(len == 0) {
        printModeCHAR(start);
      } else {
        u_fprintf(gOut, "<B>string:</B> '<tt>%S</tt>'<p>\n", littleBuffer);
      }
      itemN++;
    } else { /* MODE=ESET... */
      u_fprintf(gOut, "<table summary=\"Search Results\" border=\"1\">");
      if(mode == ESETCHUNK) {
        u_fprintf(gOut, "<tr>");
      }
    
      while((countDown--) && /* haven't gone too far and */
            (itemN<iCount ||  /* more items OR */
             cur<=end)) {    /* more to go on this item */
        UErrorCode iStatus = U_ZERO_ERROR;
        if(cur>end) { /* fetch another item */
          len = uset_getItem(aSet, itemN, &start, &end, littleBuffer, 
                             (sizeof(littleBuffer)/sizeof(littleBuffer[0]))-1,&iStatus);
          if(U_FAILURE(iStatus) || (len==-1)) {
            cur=1;
            start=end=0;
            if(U_FAILURE(iStatus)) {
              u_fprintf(gOut, "<tr><td colspan=3><b>err %s</b></td></tr>\n", u_errorName(iStatus));
            } else {
              u_fprintf(gOut, "<tr><td colspan=3><b>err - out of range</b></td></tr>\n");
            }
          } else {
            if(len == 0) { /* range */
              if(charNSet == FALSE) {
                cur=start;
              } else {
                cur=start+charN;
              }
            } else { /* string */
              u_fprintf(gOut, "<tr><td colspan=3><B>string:</B> '<tt>%S</tt>'</td></tr>\n", 
                        littleBuffer);
              cur=1; /* no range. */
              end=0;
              start=0;
            }
            itemN++;
          }
          
          /* make sure charN is always 0 apart from first load */
          if(charNSet) {
            charNSet = FALSE;
          } else {
            charN = 0;
          }
        } /* end fetch-next-item */
        
        if(cur<=end) { /* valid char within range */
          if(mode == ESET) {
            /**
             * print char 
             */
            printRow(cur, TRUE, "",  "ch");
          } else {
            if(mode==ESETCHUNK && ((bCount++)%10)==0) {
              u_fprintf(gOut, "</tr><tr>");
            }
            u_fprintf(gOut, "<td>");
            explainOneUChar32(cur);
            u_fprintf(gOut, "</td>");
          }
          
          cur++;
          charN++;
        }
      }
      if(mode == ESETCHUNK) {
        u_fprintf(gOut, "</tr>");
      }
      u_fprintf(gOut, "</table>");
    } /* mode != ESETCHAR */
#if defined (UB_DEBUG)
    u_fprintf(gOut, "on item %d, char %d (of %d)<br />\n", 
              itemN, charN, (end-start));
#endif
    
    if((end-start) && (cur<=end)) {
      itemN--; /* reread 'current' item (because we already incremented above) */
#if defined (UB_DEBUG)
      u_fprintf(gOut, "need to resume on %d:%d<br />\n", itemN, charN);
#endif
    } else if(itemN<iCount) {
      charN = 0; /* reset to beginning */
#if defined (UB_DEBUG)
      u_fprintf(gOut, "need to resume on %d:<br />\n", itemN);
    } else {
      u_fprintf(gOut, "finished.<br />\n");
#endif
    }

    if(itemN < iCount) { /* continuation? */
      u_fprintf(gOut, "<form method=\"get\" action=\"%s\">", getenv("SCRIPT_NAME"));
      u_fprintf(gOut, "<input type=\"hidden\" name=\"itemN\" value=\"%d\"><input type=\"hidden\" name=\"charN\" value=\"%d\" />\n", itemN, charN);
      u_fprintf(gOut, "<input type=\"hidden\" name=\"us\" value=\"%S\" />\n", usf);
      u_fprintf(gOut, "<input type=\"submit\" name=\"goset%c\" value=\"Next\" />",
                (mode==ESET)?'k':'n');
      u_fprintf(gOut, "</form>\n");
    }

    uset_close(aSet);
  uFailIt:
#if !defined(UB_DEBUG)
    if(U_FAILURE(status)) {
#endif
      u_fprintf(gOut, "<hr width=\"22%%\" />\nstatus is %s<p>\n", u_errorName(status));
#if !defined(UB_DEBUG)
    }
#endif
}

void printElsewhereBox(UChar32 block, ESearchMode mode)
{
  char dunum[10];
  sprintf(dunum, "%d", block+102); /* 102?? */
  
  u_fprintf(gOut, "<div class='elsewhere' style='float: right;'>");
  u_fprintf(gOut, "<b>see also:</b><br/>");
  switch(mode) { 
    case ECHAR:
        u_fprintf(gOut, "<a href='http://www.fileformat.info/info/unicode/char/%04X/'>fileformat.info</a> <br/> \n", block);
        u_fprintf(gOut, "<a href='http://decodeunicode.org/w3.php?nodeId=%s&page=1&lang=2&zoom=&prop='>decodeunicode.org</a> \n", dunum);
        break;
    case EBLOCK:
        u_fprintf(gOut, "<a href=\"http://www.unicode.org/charts/PDF/U%04X.pdf\">PDF@unicode.org</a>\n", block, block);
        u_fprintf(gOut, "<a href='http://decodeunicode.org/w3.php?viewMode=block&ucHex=%04x'>decodeunicode.org</a> \n", block);
        break;
  }
  u_fprintf(gOut, "</div>");
}
