/*
*******************************************************************************
* Copyright (C) 1996-2000, International Business Machines Corporation and    *
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

#define HAVE_KANGXI
/* #define RADICAL_LIST */
#include <kangxi.c> /* Kang-Xi Radical mapping table */

typedef enum { ETOP, EBLOCK, ECOLUMN, ERADLST, ERADICAL, ENAME, EEXACTNAME } ESearchMode;

/* Protos */
int main(int argc, char **argv);
void doDecodeQueryField(const char *in, char *out);
 const char * up_u2c(char *buf, const UChar *src, int32_t len, UErrorCode *status);  
void printType(int8_t type);
void printBlock(UBlockCode block);
UChar32 doSearchBlock(int32_t, UChar32 startFrom);
UChar32 doSearchType(int8_t, UChar32 startFrom);
void showSearchMenu(UChar32 startFrom);
void printCharName(UChar32 ch);

/**
 * Print a row (like in "Column" view)
 * @param theChar Char to show
 * @param showBlock Whether to include the 'block' column (type)
 * @param hilite  Hilite for the items (ex: "" or " COLOR=#00B1FF ")
 * @param link Type of link to make under char name (ex 'k' for column)
 */
void printRow(UChar32 theChar, UBool showBlock, const char *hilite, const char *link);

/* globals, current search.. */
int32_t gSearchType = -1;
int32_t gSearchBlock = -1;
char    gSearchName[512];
char    gSearchHTML[512];

UChar32 gSearchChar = 0xFFFF;
UBool gSearchCharValid = FALSE;

UBool anyDecompose = FALSE;

const char *gOurEncoding = NULL;
UFILE      *gOut = NULL;

int enumHits = 0, foundCount = 0; /* # of enumerations, # of hits */

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
    buffer[++length] = (UChar) 0x0030;	/*zero padding */
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
#if 0
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


void printBasicBlockInfo(UChar32 theChar)
{
  char  theString[100];
  UChar chars[5];
  UErrorCode status = U_ZERO_ERROR;
  int offset = 0;

  /* UTF_APPEND_CHAR_UNSAFE(chars, offset, theChar); */
/*  chars[offset] = 0; */
  u_fprintf(gOut, "%K", theChar);
/*  up_u2c(theString, chars, offset, &status); */

  if(status == U_USING_FALLBACK_ERROR)
    {
      u_fprintf(gOut, "<FONT COLOR=\"#00DD00\"><I>");
      anyDecompose = TRUE;
    }
  /*  fprintf(stderr, "%d", status); */

  u_fprintf(gOut, "%s", "<FONT SIZE=+1>");
  u_fprintf(gOut, "%s", theString);

  if(status == U_USING_FALLBACK_ERROR)
      u_fprintf(gOut, "</I></FONT>");
  u_fprintf(gOut, "%s", "</FONT>");
}


UBool myEnumCharNamesFn(void *context,
                        UChar32 code,
                        UCharNameChoice nameChoice,
                        const char *name,
                       int32_t length)
{
  enumHits++;
  if(strstr(name, gSearchName))
  {
    foundCount ++;

    printRow(code, TRUE, "",  "k1");

    if((foundCount % 15) ==14) /* ------------ Search by name: too many hits --- */
    {
      u_fprintf(gOut, "</table> <P><UL><H4><A HREF=\"?s=%s&cs=%04X\">Continue search for '%s' from U+%04X...</A></H4></UL><P>",
             gSearchHTML, code+1, gSearchName, code+1);
      return FALSE;
    }

/*     u_fprintf(gOut, "<LI><A HREF=\"?k1=%04X#here\"><TT>%04X</TT> - %s\r\n", */
/*            code, code, name); */
  
/*     printBasicBlockInfo( code ); */
/*     u_fprintf(gOut, "</A>\r\n"); */
  }
  return TRUE;
}

void printIconMenu(const char *alt, const char *name, ESearchMode target, ESearchMode current)
{
  u_fprintf(gOut, "<INPUT TYPE=image "
         "align=center border=0 width=16 height=16 "
         "alt=\"%s%s%s\" value=\"%s%s%s\" "
         "name=\"%s\" "
         "src=\"/icu/demo/ubrowse.d/%s%s.gif\" "
         ">",

          (current==target)?"[":"", alt,  (current==target)?"]":"",
          (current==target)?"[":"", alt,  (current==target)?"]":"",
         name, 
                                  alt, (current==target)?"-x":"");
} 

void printRow(UChar32 theChar, UBool showBlock, const char *hilite, const char *link)
{
  UBool searchedFor;

	  u_fprintf(gOut, "<TR %s >", hilite);

          /** 0 Row # (un needed????) **/ 
/*           u_fprintf(gOut, "<TD><B>%X</B></TD>", r); */

          /** 1 The Char **/
	  u_fprintf(gOut, "<TD ");

          /* TODO: 'if matches(theChar)' */

	  u_fprintf(gOut, "%s ALIGN=CENTER>\r\n", hilite);
	  
	  if((theChar == gSearchChar) && gSearchCharValid)
	    u_fprintf(gOut, "<A NAME=here></A>");
	  
	  /* print the simple data (i.e. the actual char ) */
	  printBasicBlockInfo(theChar);
	  
	  u_fprintf(gOut, "</TD>");


          /** 2. print the char val and name  */
          u_fprintf(gOut, "<TD>");
          if(link != NULL)
          {
            u_fprintf(gOut, "<A HREF=\"?%s=%04X#here\">", link, theChar);
          }
          u_fprintf(gOut, "<FONT SIZE=-1>U+%04X ", theChar);
          printCharName(theChar);
          u_fprintf(gOut, "</FONT>");
          if(link != NULL)
          {
            u_fprintf(gOut, "</A>");
          }
          u_fprintf(gOut, "</TD>");

          /** 3. char type */
	  searchedFor = (u_charType(theChar) == gSearchType);
          u_fprintf(gOut, "<td>");
	  if(searchedFor)
	    u_fprintf(gOut, "<B>");
	  printType(u_charType(theChar));
	  if(searchedFor)
	    u_fprintf(gOut, "</B>");
          u_fprintf(gOut, "</td>");

          /** 3 1/2. optional - block */
          if(showBlock)
          {  
            u_fprintf(gOut, "<TD ");
            searchedFor = (u_charScript(theChar) == gSearchBlock);
            
            if(searchedFor)
              u_fprintf(gOut, " BGCOLOR=\"#EE0000\" "); /* mark the one we were searching for */
            
            u_fprintf(gOut, " >");
            
            if(searchedFor)
              u_fprintf(gOut, "<B>");
            
            u_fprintf(gOut, "<A HREF=\"?scr=%d&b=%04X\">", (u_charScript(theChar)+1)%UBLOCK_COUNT, theChar);
            printBlock(u_charScript(theChar));
            u_fprintf(gOut, "</A>");
            
            if(searchedFor)
              u_fprintf(gOut, "</B>");
            
            u_fprintf(gOut, "</TD>");
          }

          /** 3 3/4: script **/
          {
            UErrorCode scriptErr = U_ZERO_ERROR;
            UScriptCode scriptCode;
            
            scriptCode = uscript_getScript(theChar, &scriptErr);
            if(U_FAILURE(scriptErr))
            {
              u_fprintf(gOut, "<TD BGCOLOR=\"#888888\">&nbsp;</TD>\r\n");
            }
            else
            {
              u_fprintf(gOut, "<TD>");
              
              u_fprintf(gOut, "%s", uscript_getShortName(scriptCode));

              u_fprintf(gOut, "</TD>");
            }
          }

          /** 4: digit (optional) **/
	  u_fprintf(gOut, "<TD>");
	  if(u_isdigit(theChar))
          {
            if(u_charDigitValue(theChar) == -1)
            {
              u_fprintf(gOut, "-");
            }
            else
            {
              u_fprintf(gOut, "%d", u_charDigitValue(theChar));
            }
          }
	  u_fprintf(gOut, "</TD>");

          /** 5: cell width **/
          u_fprintf(gOut, "<TD>");
	  switch(u_charCellWidth(theChar))
	  {
	    case U_ZERO_WIDTH: u_fprintf(gOut, "0"); break; 
	    case U_HALF_WIDTH: u_fprintf(gOut, "1/2"); break; 
	    case U_FULL_WIDTH: u_fprintf(gOut, "1"); break; 
	    case U_NEUTRAL_WIDTH: u_fprintf(gOut, "-"); break; 
	  }

          u_fprintf(gOut, "</TD>");

          /** 6. direction **/
          u_fprintf(gOut, "<TD>");
	  switch(u_charDirection(theChar))
	    {
	    case U_LEFT_TO_RIGHT: u_fprintf(gOut, "LTR"); break; 
	    case U_RIGHT_TO_LEFT: u_fprintf(gOut, "RTL"); break; 
	    case U_EUROPEAN_NUMBER: u_fprintf(gOut, "European Number"); break; 
	    case U_EUROPEAN_NUMBER_SEPARATOR: u_fprintf(gOut, "Eur. Num. Sep"); break; 
	    case U_EUROPEAN_NUMBER_TERMINATOR: u_fprintf(gOut, "Eur. num. Term"); break; 
	    case U_ARABIC_NUMBER: u_fprintf(gOut, "Arabic Num."); break; 
	    case U_COMMON_NUMBER_SEPARATOR: u_fprintf(gOut, "Cmn Number Sep"); break; 
	    case U_BLOCK_SEPARATOR: u_fprintf(gOut, "Block Sep"); break; 
	    case U_SEGMENT_SEPARATOR: u_fprintf(gOut, "Segment Sep"); break; 
	    case U_WHITE_SPACE_NEUTRAL: u_fprintf(gOut, "White Space Neutral"); break; 
	    case U_OTHER_NEUTRAL: u_fprintf(gOut, "Other Neutral"); break; 
	    case U_LEFT_TO_RIGHT_EMBEDDING: u_fprintf(gOut, "LRE"); break; 
	    case U_LEFT_TO_RIGHT_OVERRIDE: u_fprintf(gOut, "LRO"); break; 
	    case U_RIGHT_TO_LEFT_ARABIC: u_fprintf(gOut, "RTL-Arabic"); break; 
	    case U_RIGHT_TO_LEFT_EMBEDDING: u_fprintf(gOut, "RLE"); break; 
	    case U_RIGHT_TO_LEFT_OVERRIDE: u_fprintf(gOut, "RLO"); break; 
	    case U_POP_DIRECTIONAL_FORMAT: u_fprintf(gOut, "PDF"); break; 
	    case U_DIR_NON_SPACING_MARK: u_fprintf(gOut, "Combining Class %d", u_getCombiningClass(theChar)); break; 
	    case U_BOUNDARY_NEUTRAL: u_fprintf(gOut, "BN"); break; 
	    default: u_fprintf(gOut, "Unknown Dir\n");
	    }
          /*  put the mirrored thing here. Not so common, keeps it out of the way. */
          if(u_isMirrored(theChar))
          {
            u_fprintf(gOut, " (Mirrored)");
          }
	  u_fprintf(gOut, "</TD>");


	  /** image - removed **/
/*  	  u_fprintf(gOut, "<TD>"); */

/*                -- this works but it's kind of RUDE. -- [to link in w/o asking] */
/*                u_fprintf(gOut, "<IMG WIDTH=32 HEIGHT=32 SRC=\"http://charts.unicode.org/Unicode.charts/Small.Glyphs/%02X/U%04X.gif\">\r\n", */
/*                   ((theChar&0x1FFF00)>>8), */
/*                   theChar); */
/*  #error you're rude */
/*            u_fprintf(gOut, "</TD>"); */

          u_fprintf(gOut, "</TR>\r\n");

}

int
main(int argc,
     char **argv)
{
  char *qs;
  char *pi;
  char *tmp;
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
  if(!pi)
    pi = "/utf-8";


  pi++;

  tmp = strchr(pi, '/');
  if(tmp)
    *tmp = 0; /* terminate */

  if(*pi==0)
  {
    gOurEncoding = "utf-8";
  }
  else 
  {
    gOurEncoding = pi;
  }

  printf("content-type: text/html;charset=%s\r\n\r\n",gOurEncoding);

  gOut = u_finit(stdout, "en_US", gOurEncoding);

  if(gOut == NULL)
  {
      /**/printf("Couldnt' open output file in encoding %s.\n", gOurEncoding);
      return 0;
  }

  /* set up the converter */
  ucnv_setSubstChars(u_fgetConverter(gOut),"_",1,&status);  /* DECOMPOSE calls SUBSTITUTE on failure. */
#if 0 
/* until it is implemented for 1.6 (!!!) */
  ucnv_setFromUCallBack(gConverter, &UCNV_FROM_U_CALLBACK_DECOMPOSE, &status);
#endif
  

/*  ucnv_setDefaultName(gOurEncoding); */

  u_fprintf(gOut, "<HTML><HEAD>\r\n");
  if(!tmp) /* if there was no trailing '/' ... */
  {
    if(pi!=NULL) {
      u_fprintf(gOut, "<BASE HREF=\"http://%s%s/%s/\">\r\n", getenv("SERVER_NAME"),
             getenv("SCRIPT_NAME"),
             pi);
    } else {
      u_fprintf(gOut, "<BASE HREF=\"http://%s%s/\">\r\n", getenv("SERVER_NAME"),
             getenv("SCRIPT_NAME"));
    }
    
  }

  /* Now, see what we're supposed to do */
  qs = getenv("QUERY_STRING");
  
  mode = ETOP;

  if(qs)
    { 
      if (sscanf(qs,"go=%x", &block)== 1)
        {
          if(strstr(qs,"k.x="))
          {
            block &= 0x1FFFF0;
            u_fprintf(gOut, "<TITLE>ICU UnicodeBrowser: column U+%04X</TITLE>\r\n", block);
            mode = ECOLUMN;
          }
          else if(strstr(qs, "n.x="))
          {
            block &= 0x1FFF00;
            u_fprintf(gOut, "<TITLE>ICU UnicodeBrowser: block U+%04X</TITLE>\r\n", block);
            mode = EBLOCK;
          }
          else
          {
            mode = ETOP;
            /* Title comes lower */
          }
        }
      else if(sscanf(qs,"n=%x", &block)== 1)
	{
	  block &= 0x1FFF00;
	  u_fprintf(gOut, "<TITLE>ICU UnicodeBrowser: block U+%04X</TITLE>\r\n", block);
	  mode = EBLOCK;
	}
      else if(sscanf(qs, "k1=%x", &block) == 1)
	{
	  gSearchChar = block;
	  gSearchCharValid = TRUE;
	  block &= 0x1FFFF0;
	  u_fprintf(gOut, "<TITLE>ICU UnicodeBrowser: column U+%04X</TITLE>\r\n", block);
	  mode = ECOLUMN;
	}
      else if(sscanf(qs, "k=%x", &block) == 1)
	{
	  block &= 0x1FFFF0;
	  u_fprintf(gOut, "<TITLE>ICU UnicodeBrowser: column U+%04X</TITLE>\r\n", block);
	  mode = ECOLUMN;
	}
      else if(sscanf(qs, "scr=%d&b=%x", &gSearchBlock, &block) == 2)
	{
	  block = doSearchBlock(gSearchBlock, block);
	  block &= 0x1FFFF0;
	  mode = ECOLUMN;
	}
      else if(sscanf(qs, "typ=%d&b=%x", &gSearchType, &block) == 2)
	{
	  block = doSearchType(gSearchType, block);
	  block &= 0x1FFFF0;
	  mode = ECOLUMN;
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
      else if(sscanf(qs, "s=%200s", &gSearchHTML) == 1)
      {
        char *ss = NULL;
        char *cs = NULL;
        char *p;
        
        block = 0;
        /* look for 'continue' tag */
        if(cs=strstr(qs,"cs="))
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

        if(ss=strchr(gSearchHTML, '&'))
        {
          *ss = 0;
        }

        /* copy - safely */
        strncpy(gSearchName,gSearchHTML,200);
        gSearchName[200] = 0;


        /* de HTML ize. Note we dont' care about %s'. */
        for(p=gSearchName;*p;p++)
        {
          *p = toupper(*p);
          if(*p == '+')
            *p = ' ';
        }
        
      }
    }

  if(mode == ETOP)
    {
      u_fprintf(gOut, "<TITLE>ICU UnicodeBrowser</TITLE>\r\n");
    }


  u_fprintf(gOut, "</HEAD>\r\n");
  
  u_fprintf(gOut, "<BODY BGCOLOR=\"#FFFFFF\" link=\"green\" vlink=\"brown\">\r\n");

  u_fprintf(gOut, "<A HREF=\"http://oss.software.ibm.com/icu\">ICU</A> &gt;\r\n"
         "<A HREF=\"http://oss.software.ibm.com/icu/demo\">Demo</A> &gt;\r\n"
         "<B>Unicode Browser</B><BR>\r\n");


  u_fprintf(gOut, "<form><table border=1 cellpadding=1 cellspacing=1><tr>");

  u_fprintf(gOut, "<td >");

  u_fprintf(gOut, "Go: <INPUT size=7 NAME=go VALUE=\"%04X\">", block);

  /* show which item we're on */
  printIconMenu("column", "k", ECOLUMN, mode);
  printIconMenu("block",  "n", EBLOCK, mode);
  u_fprintf(gOut, " | ");
  printIconMenu("all",    "all", ETOP, mode);

  /* forward and back buttons */
  switch(mode)
  {
  case EBLOCK:
    u_fprintf(gOut, " | <A HREF=\"?n=%04X\">prev</A> <A HREF=\"?n=%04X\">next</A> ",
           (block & 0x1FFF00)-0x100, (block & 0x1FFF00)+0x100);
    break;

  case ECOLUMN:
    u_fprintf(gOut, " | <A HREF=\"?k=%04X\">prev</A> <A HREF=\"?k=%04X\">next</A> ",
	     (block & 0x1FFFF0)-0x10, (block & 0x1FFFF0)+0x10);
    break;
  }

  u_fprintf(gOut, "");
  u_fprintf(gOut, "</TD>");

  u_fprintf(gOut, "<td>\r\n"); 

  u_fprintf(gOut, "</td>");  


  if(mode == ETOP) /* top level list of blocks ******************************** ETOP ********** */
    {

      u_fprintf(gOut, "</tr></table></form>"); /* closer of menu */

        u_fprintf(gOut, "<table border=0 cellpadding=1 cellspacing=1><tr><td bgcolor=\"#cccccc\">\r\n");

      u_fprintf(gOut, "<b>Unicode Browser</b> - Click on a type of character view it in more detail<br>\r\n");
      u_fprintf(gOut, "</td></tr><tr><td bgcolor=\"#eeeeee\">\r\n"
             );

      u_fprintf(gOut, "<B>Blocks:</B>");
      for(i=UBLOCK_BASIC_LATIN;i<UBLOCK_COUNT;i++)
      {
        u_fprintf(gOut, "<A HREF=\"?scr=%d&b=0\">", i);
        printBlock(i);
        u_fprintf(gOut, "</A> | &nbsp;");
      }
      u_fprintf(gOut, "<P>");
      

      u_fprintf(gOut, "<B>General Categories:</B>");
      for(i=U_UNASSIGNED;i<U_CHAR_CATEGORY_COUNT;i++)
      {
        u_fprintf(gOut, "<A HREF=\"?typ=%d&b=0\">", i);
        printType(i);
        u_fprintf(gOut, "</A> | &nbsp;");
      }
      u_fprintf(gOut, "<P>");
      


      u_fprintf(gOut, 
             "</td></tr></table>\r\n"
             );

/*       u_fprintf(gOut, "\r\n</td></tr><tr><td align=right>\r\n"); */
      
      showSearchMenu(0x0000);
    }      
  else if (mode == EBLOCK) /* *************** BLOCK *******************************************/
    {

    u_fprintf(gOut, "</tr></table></form>"); /* closer of menu */


      /* Unicode table  at block 'block' */

      n = 0;
      /*      for(n=0; n<0x100; n += 0x080) */
	{
	  u_fprintf(gOut, "<TT><TABLE BORDER=1>");
	  u_fprintf(gOut, "<TR><TD></TD>");
	  for(c = n;c<(n + 0x100);c+= 0x10)
	    {
	      u_fprintf(gOut, "<TD><B><A HREF=\"?k=%04X\"><TT>%03X</TT></A></B></TD>", (block|c),  (block | c) >> 4   );
	    }
	  u_fprintf(gOut, "</TR>\r\n");
	  for(r = 0; r < 0x10; r++)
	    {
	      u_fprintf(gOut, "<TR><TD><B><TT>%X</TT></B></TD>", r);
	      for(c = n;c<(n + 0x100);c+= 0x10)
		{
		  theChar = (block | r | c );

		  u_fprintf(gOut, "<TD ");
		  
		  if(u_charType(theChar) == U_UNASSIGNED)
		    {
		      u_fprintf(gOut, " BGCOLOR=\"#888888\" ");
		    }
		  
		  u_fprintf(gOut, " ALIGN=CENTER>\r\n");
		  
		  
		  /* print the simple data */
		  printBasicBlockInfo(theChar);
		  
		  u_fprintf(gOut, "</TD>");
		}
	      u_fprintf(gOut, "<TD><B><TT>%X</TT></B></TD>", r);
	    }
	}
	printf("</TR>");
	printf("<TR><TD COLSPAN=18 ALIGN=CENTER><I>Click on a column number to zoom in.</I></TD></TR>\r\n");
	printf("</TABLE></TT>");
      u_fprintf(gOut, "<HR>\r\n");
      if(block <= 0xFFFF)
        {
          u_fprintf(gOut, "<A HREF=\"http://charts.unicode.org/Unicode.charts/normal/U%04X.html#Glyphs\">this block on charts.unicode.org</A>\r\n", block);
        }
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
        u_fprintf(gOut, "<TD>"); 
        u_fprintf(gOut, "<B>Block:</B>  ");
        searchedFor = (u_charScript(theChar) == gSearchBlock);
        
        
        if(searchedFor)
          u_fprintf(gOut, "<B>");
        
        printBlock(u_charScript(theChar));
        if(searchedFor)
          u_fprintf(gOut, "</B>");

        /*  "| Next " on block list . removed. 
         * u_fprintf(gOut, " | <A HREF=\"?scr=%d&b=%04X\">", (u_charScript(theChar)+1)%U_CHAR_SCRIPT_COUNT, theChar);
         * u_fprintf(gOut, "next</A>");
         */
            
        u_fprintf(gOut, "</TD></TR></TABLE></form>");  /* closer */
      }
      else
      {
        u_fprintf(gOut, "</tr></table></form>"); /* closer of menu */
      }



      u_fprintf(gOut, "<TABLE BORDER=1>");


      u_fprintf(gOut, "<TR>"); /* 0: column index */
      u_fprintf(gOut, "<TD>C</TD>"); /* 1: char */
      u_fprintf(gOut, "<TD>N</TD>"); /* 2: char #, name */
      u_fprintf(gOut, "<TD><B>General Category</B></TD>"); /* 3: type */ 
      if(showBlock == TRUE) 
      {
        u_fprintf(gOut, "<TD><B>Block</B></TD>"); /* 3 1/2 */
      }
      u_fprintf(gOut, "<TD><B>Script</B></TD>"); /* 3 3/4 Script */
      u_fprintf(gOut, "<TD><B></B></TD>"); /* 4 digit */
      u_fprintf(gOut, "<TD><B>Wid</B></TD>"); /* 5 width */
      u_fprintf(gOut, "<TD><B>BiDi</B></TD>"); /* 6 Direction (BiDi?) */
      u_fprintf(gOut, "</TR>\r\n");

      for(r = 0; r < 0x10; r++)  /***** rows ******/
	{
	  theChar = (block | r );

          /* Do we have a match? for cell highlighting */ 
	  if( (u_charScript(theChar) == gSearchBlock) || (u_charType(theChar) == gSearchType) || ((theChar == gSearchChar) && gSearchCharValid))
          {
	    hilite = " BGCOLOR=\"#FFdddd\" ";
          }
	  else if(u_charType(theChar) == U_UNASSIGNED)
	  {
            hilite = " BGCOLOR=\"#666666\" ";
	  }
          else 
          {
            hilite = "";
          }

          printRow(theChar, showBlock, hilite, NULL); /* TODO: change to CHAR view */

	}
      

      u_fprintf(gOut, "</TABLE></TT><HR>");

      showSearchMenu( block + 0x0010);

    }

#ifdef RADICAL_LIST 
  else if(mode == ERADLST) /************************ RADICAL LIST ********************/
    {
      u_fprintf(gOut, "</table></form>");
      u_fprintf(gOut, "<TABLE BORDER=1>");
      u_fprintf(gOut, "<TR>");
      for(i=0;gKangXiRadicalTable[i];i+=2)
	{
	  if((i%16) == 0)
	    u_fprintf(gOut, "</TR><TR>\r\n");
	  u_fprintf(gOut, "<TD>");
	  u_fprintf(gOut, "<A HREF=\"?radical=%d\">", (i/2)+1);
	  printBasicBlockInfo(gKangXiRadicalTable[i]);
	  u_fprintf(gOut, "</A>");
	  u_fprintf(gOut, "</TD>");

	}
      u_fprintf(gOut, "</TR></TABLE>");
    }
  else if(mode == ERADICAL) /************************* RADICAL ************************/
    {
      FILE *f;
      int u,stroke;
      char s[200];

      u_fprintf(gOut, "</td></tr></table></form>"); /* closer */

      u_fprintf(gOut, "<B>");
      printBasicBlockInfo(gKangXiRadicalTable[(block-1)*2]);
      u_fprintf(gOut, " (radical %d)</B> \r\n",block);
      printCharName(gKangXiRadicalTable[(block-1)*2]);
      sprintf(s,"/home/srl/icu/data/PSI/rad%03d.lst", block);
      u_fprintf(gOut, "<TABLE BORDER=1>");
      f = fopen(s,"r");
      i=0;
      u_fprintf(gOut, "<TR>");
      while(fscanf(f,"%04X\t%d", &u, &stroke) == 2)
	{
	  if(!( (i++)%10))
	    u_fprintf(gOut, "</TR><TR>");
	  u_fprintf(gOut, "<TD>");
	  u_fprintf(gOut, "%d ", stroke);
	  u_fprintf(gOut, "<A HREF=\"?k1=%04X#here\">", u);
	  printBasicBlockInfo(u);
	  u_fprintf(gOut, "</A></TD>");
	}
      fclose(f);
      u_fprintf(gOut, "</TR></TABLE>");
    }
#endif /* RADICAL_LIST */
  else if(mode == ENAME) /************************ NAME ***************************/
    {
      UErrorCode status = U_ZERO_ERROR;

      u_fprintf(gOut, "</td></tr></table></form>"); /* closer */

      /* "Be careful what you search for, you just might find it"
	 (and 0x10FFFF of it's close friends!)
      */

      if(block == 0)
      {
        u_fprintf(gOut, "<H1>Searching for '%s'...</H1>\r\n", gSearchName);
      }
      else
      {
        u_fprintf(gOut, "<H1>Searching for '%s' from U+%04X...</H1>\r\n", gSearchName, block);
      }
      
      u_fprintf(gOut, "<table border=1>");

      u_enumCharNames(block, UCHAR_MAX_VALUE,
                      myEnumCharNamesFn,
                      NULL,
                      U_UNICODE_CHAR_NAME,
                      &status);

      if(foundCount == 0)
        {
          u_fprintf(gOut, "<tr><td><i>Not found.</i></td></tr>\n");
        }

      u_fprintf(gOut, "</table><!-- %d tested --><HR>\r\n",enumHits);
      showSearchMenu( 0x0000 ); 
    }
  else if(mode == EEXACTNAME) /************** EXACT NAME ********************************/
    {
      UChar32 c;
      UErrorCode status = U_ZERO_ERROR;

      u_fprintf(gOut, "</td></tr></table></form>"); /* closer */


      u_fprintf(gOut, "<H1>Searching for '%s'...</H1>\r\n", gSearchName);
      
      
      u_fprintf(gOut, "<HR><UL>\r\n");

      c = u_charFromName(U_UNICODE_CHAR_NAME,
                         gSearchName,
                         &status);

      if(U_FAILURE(status))
      {
          u_fprintf(gOut, "Error - %s %d<P>\n", u_errorName(status), status);
      }
      else if(c == 0x00FFFF )
      {
        u_fprintf(gOut, "<i>Not found.</i><p>Please note that this search requires an exact unicode name.\n");
      }
      else
      {
        u_fprintf(gOut, "<LI><A HREF=\"?k1=%04X#here\"><TT>%04X</TT> - %s\r\n",
               c,c,gSearchName);
        printBasicBlockInfo(c);
        
      }

      u_fprintf(gOut, "</UL><HR>\r\n");
      showSearchMenu( 0x0000 ); 
    }
  else /************************************* ????????????????????????? ****************/
    {
      u_fprintf(gOut, "</td></tr></table></form>"); /* closer */

      u_fprintf(gOut, "Unknown search mode ! <HR>\r\n");
    }
  

  u_fprintf(gOut, "</td></tr></table>\r\n\r\n"); /* ? */



  if(anyDecompose)
    {
      u_fprintf(gOut, "Note: text in <FONT COLOR=\"#00DD00\">");
      u_fprintf(gOut, "<I>Italic Green</I></FONT> is the best-try using decomposition. Underscore (_) denotes missing characters in the decomposition.<P>");

    }

  uvi[0]=0xFF;
  uvi[1]=0xFF;
  uvi[2]=0xFF;
  uvi[3]=0xFF;
  u_getUnicodeVersion(uvi);
  u_fprintf(gOut, "<A HREF=\"http://www.unicode.org/\">Based on Unicode ");
  for(uc=0;uc<U_MAX_VERSION_LENGTH;uc++) {
    if((uc < 3) || (uvi[uc] != 0)) /* don't show trailing zero in 4th place. */
      u_fprintf(gOut, "%d.",uvi[uc]);
  }
  u_fprintf(gOut, "</A>\r\n");

  u_fprintf(gOut, "<BR>Powered by <A HREF=\"http://oss.software.ibm.com/icu/\">ICU %s</A>\r\n", U_ICU_VERSION);
  
  u_fprintf(gOut, "</td></tr></table>\r\n");

  u_fprintf(gOut, "</BODY><!-- SFCBugibba design 12 ta' Mejju 2001 --></HTML>\r\n");

  u_fclose(gOut);

  return 0;
}

/* still lazy evaluated. but it'll give us an alternate strcpy */


UConverter *gConverter = NULL;

const char *
up_u2c(char *buf, const UChar *src, int32_t len, UErrorCode *cnvStatus)
{
  /* borrowed from udate */
  int32_t sourceLen;
  const UChar *mySource;
  const UChar *mySourceEnd;
  char *myTarget;
  int32_t arraySize;
  UErrorCode  status = U_ZERO_ERROR;

  /*  return u_austrcpy(buf,src);*/

  status = U_ZERO_ERROR;

  if(gConverter == NULL) /* open just one of these */
    {
      /* open a default converter */
      gConverter = ucnv_open(NULL, &status);
      ucnv_setSubstChars(gConverter,"_",1,&status);  /* DECOMPOSE calls SUBSTITUTE on failure. */
      if(U_FAILURE(status))
	{
	  u_fprintf(gOut, "%s", "Could not open the default converter.");
	}
    }

  /* set up the conversion parameters */
  sourceLen    = len;
  mySource     = src;
  mySourceEnd  = mySource + sourceLen;
  myTarget     = buf;
  arraySize    = 0x000FFFFF; /* should be a param */

  /* put our special error handler in */
#if 0 
/* until it is implemented for 1.6 */
  ucnv_setFromUCallBack(gConverter, &UCNV_FROM_U_CALLBACK_DECOMPOSE, &status);
#endif


  /*  ucnv_setFromUCallBack(gConverter, &SubstituteWithValueHTML, &status); */

  /* perform the conversion */

  /* perform the conversion */
  ucnv_fromUnicode(gConverter,
		   &myTarget,
		   myTarget + arraySize,
		   &mySource, 
		   mySourceEnd, 
		   NULL,
		   TRUE,
		   cnvStatus);

    /* Write the converted data to the FILE* */

  *myTarget = 0; /* tie it off */

  return buf;
}


/* See printTypeFull also */
void printType(int8_t type)
{
  switch(type)
    {
    case U_UNASSIGNED: u_fprintf(gOut, "Unassigned"); break; 
    case U_UPPERCASE_LETTER: u_fprintf(gOut, "Uppercase Letter");  break;  
    case U_LOWERCASE_LETTER:  u_fprintf(gOut, "Lowercase Letter");  break; 
    case U_TITLECASE_LETTER: u_fprintf(gOut, "Titlecase Letter"); break;  
    case U_MODIFIER_LETTER: u_fprintf(gOut, "Modifier Letter"); break; 
    case U_OTHER_LETTER: u_fprintf(gOut, "Other Letter"); break; 
    case U_NON_SPACING_MARK: u_fprintf(gOut, "Non-Spacing Mark"); break; 
    case U_ENCLOSING_MARK: u_fprintf(gOut, "Enclosing Mark"); break; 
    case U_COMBINING_SPACING_MARK: u_fprintf(gOut, "Combining Spacing Mark"); break; 
    case U_DECIMAL_DIGIT_NUMBER: u_fprintf(gOut, "Decimal Digit Number"); break; 
    case U_LETTER_NUMBER: u_fprintf(gOut, "Letter Number"); break; 
    case U_OTHER_NUMBER: u_fprintf(gOut, "Other Number"); break; 
    case U_SPACE_SEPARATOR: u_fprintf(gOut, "Space Separator"); break; 
    case U_LINE_SEPARATOR: u_fprintf(gOut, "Line Separator"); break; 
    case U_PARAGRAPH_SEPARATOR: u_fprintf(gOut, "Paragraph Separator"); break; 
    case U_CONTROL_CHAR: u_fprintf(gOut, "Control"); break; 
    case U_FORMAT_CHAR: u_fprintf(gOut, "Format"); break; 
    case U_PRIVATE_USE_CHAR: u_fprintf(gOut, "Private Use"); break; 
    case U_SURROGATE: u_fprintf(gOut, "Surrogate"); break; 
    case U_DASH_PUNCTUATION: u_fprintf(gOut, "Dash Punctuation"); break; 
    case U_START_PUNCTUATION: u_fprintf(gOut, "Start Punctuation"); break; 
    case U_END_PUNCTUATION: u_fprintf(gOut, "End Punctuation"); break; 
    case U_CONNECTOR_PUNCTUATION: u_fprintf(gOut, "Connector Punctuation"); break; 
    case U_OTHER_PUNCTUATION: u_fprintf(gOut, "Other Punctuation"); break; 
    case U_MATH_SYMBOL: u_fprintf(gOut, "Math Symbol"); break; 
    case U_CURRENCY_SYMBOL: u_fprintf(gOut, "Currency Symbol"); break; 
    case U_MODIFIER_SYMBOL: u_fprintf(gOut, "Modifier Symbol"); break; 
    case U_OTHER_SYMBOL: u_fprintf(gOut, "Other Symbol"); break; 
    case U_INITIAL_PUNCTUATION: u_fprintf(gOut, "Initial Punctuation"); break; 
    case U_FINAL_PUNCTUATION: u_fprintf(gOut, "Final Punctuation"); break; 
      /* case U_GENERAL_OTHER_TYPES: u_fprintf(gOut, "General Other Types"); break;      */

    default: u_fprintf(gOut, "Unknown type %d", type); break;
    }
}


void printBlock(UBlockCode block)
{
  switch(block)
    {
case UBLOCK_BASIC_LATIN: u_fprintf(gOut, "Basic Latin"); return;
case UBLOCK_LATIN_1_SUPPLEMENT: u_fprintf(gOut, "Latin-1 Supplement"); return;
case UBLOCK_LATIN_EXTENDED_A: u_fprintf(gOut, "Latin Extended A"); return;
case UBLOCK_LATIN_EXTENDED_B: u_fprintf(gOut, "Latin Extended B"); return;
case UBLOCK_IPA_EXTENSIONS: u_fprintf(gOut, "IPA Extensions"); return;
case UBLOCK_SPACING_MODIFIER_LETTERS: u_fprintf(gOut, "Spacing Modifier Letters"); return;
case UBLOCK_COMBINING_DIACRITICAL_MARKS: u_fprintf(gOut, "Combining Diacritical Marks"); return;
case UBLOCK_GREEK: u_fprintf(gOut, "Greek"); return;
case UBLOCK_CYRILLIC: u_fprintf(gOut, "Cyrillic"); return;
case UBLOCK_ARMENIAN: u_fprintf(gOut, "Armenian"); return;
case UBLOCK_HEBREW: u_fprintf(gOut, "Hebrew"); return;
case UBLOCK_ARABIC: u_fprintf(gOut, "Arabic"); return;
case UBLOCK_SYRIAC: u_fprintf(gOut, "Syriac"); return;
case UBLOCK_THAANA: u_fprintf(gOut, "Thaana"); return;
case UBLOCK_DEVANAGARI: u_fprintf(gOut, "Devanagari"); return;
case UBLOCK_BENGALI: u_fprintf(gOut, "Bengali"); return;
case UBLOCK_GURMUKHI: u_fprintf(gOut, "Gurmukhi"); return;
case UBLOCK_GUJARATI: u_fprintf(gOut, "Gujarati"); return;
case UBLOCK_ORIYA: u_fprintf(gOut, "Oriya"); return;
case UBLOCK_TAMIL: u_fprintf(gOut, "Tamil"); return;
case UBLOCK_TELUGU: u_fprintf(gOut, "Telugu"); return;
case UBLOCK_KANNADA: u_fprintf(gOut, "Kannada"); return;
case UBLOCK_MALAYALAM: u_fprintf(gOut, "Malayalam"); return;
case UBLOCK_SINHALA: u_fprintf(gOut, "Sinhala"); return;
case UBLOCK_THAI: u_fprintf(gOut, "Thai"); return;
case UBLOCK_LAO: u_fprintf(gOut, "Lao"); return;
case UBLOCK_TIBETAN: u_fprintf(gOut, "Tibetan"); return;
case UBLOCK_MYANMAR: u_fprintf(gOut, "Myanmar"); return;
case UBLOCK_GEORGIAN: u_fprintf(gOut, "Georgian"); return;
case UBLOCK_HANGUL_JAMO: u_fprintf(gOut, "Hangul-Jamo"); return;
case UBLOCK_ETHIOPIC: u_fprintf(gOut, "Ethiopic"); return;
case UBLOCK_CHEROKEE: u_fprintf(gOut, "Cherokee"); return;
case UBLOCK_UNIFIED_CANADIAN_ABORIGINAL_SYLLABICS: u_fprintf(gOut, "Unified Canadian Aboriginal Syllabics"); return;
case UBLOCK_OGHAM: u_fprintf(gOut, "Ogham"); return;
case UBLOCK_RUNIC: u_fprintf(gOut, "Runic"); return;
case UBLOCK_KHMER: u_fprintf(gOut, "Khmer"); return;
case UBLOCK_MONGOLIAN: u_fprintf(gOut, "Mongolian"); return;
case UBLOCK_LATIN_EXTENDED_ADDITIONAL: u_fprintf(gOut, "Latin Extended Additional"); return;
case UBLOCK_GREEK_EXTENDED: u_fprintf(gOut, "Greek Extended"); return;
case UBLOCK_GENERAL_PUNCTUATION: u_fprintf(gOut, "General Punctuation"); return;
case UBLOCK_SUPERSCRIPTS_AND_SUBSCRIPTS: u_fprintf(gOut, "Superscripts and Subscripts"); return;
case UBLOCK_CURRENCY_SYMBOLS: u_fprintf(gOut, "Currency Symbols"); return;
case UBLOCK_COMBINING_MARKS_FOR_SYMBOLS: u_fprintf(gOut, "Combining Marks for Symbols"); return;
case UBLOCK_LETTERLIKE_SYMBOLS: u_fprintf(gOut, "Letterlike Symbols"); return;
case UBLOCK_NUMBER_FORMS: u_fprintf(gOut, "Number Forms"); return;
case UBLOCK_ARROWS: u_fprintf(gOut, "Arrows"); return;
case UBLOCK_MATHEMATICAL_OPERATORS: u_fprintf(gOut, "Mathematical Operators"); return;
case UBLOCK_MISCELLANEOUS_TECHNICAL: u_fprintf(gOut, "Miscellaneous Technical"); return;
case UBLOCK_CONTROL_PICTURES: u_fprintf(gOut, "Control Pictures"); return;
case UBLOCK_OPTICAL_CHARACTER_RECOGNITION: u_fprintf(gOut, "Optical Character Recognition"); return;
case UBLOCK_ENCLOSED_ALPHANUMERICS: u_fprintf(gOut, "Enclosed Alphanumerics"); return;
case UBLOCK_BOX_DRAWING: u_fprintf(gOut, "Box Drawing"); return;
case UBLOCK_BLOCK_ELEMENTS: u_fprintf(gOut, "Block Elements"); return;
case UBLOCK_GEOMETRIC_SHAPES: u_fprintf(gOut, "Geometric Shapes"); return;
case UBLOCK_MISCELLANEOUS_SYMBOLS: u_fprintf(gOut, "Miscellaneous Symbols"); return;
case UBLOCK_DINGBATS: u_fprintf(gOut, "Dingbats"); return;
case UBLOCK_BRAILLE_PATTERNS: u_fprintf(gOut, "Braille Patterns"); return;
case UBLOCK_CJK_RADICALS_SUPPLEMENT: u_fprintf(gOut, "CJK Radicals Supplement"); return;
case UBLOCK_KANGXI_RADICALS: u_fprintf(gOut, "KangXi Radicals"); return;
case UBLOCK_IDEOGRAPHIC_DESCRIPTION_CHARACTERS: u_fprintf(gOut, "Ideographic Description Characters"); return;
case UBLOCK_CJK_SYMBOLS_AND_PUNCTUATION: u_fprintf(gOut, "CJK Symbols and Punctuation"); return;
case UBLOCK_HIRAGANA: u_fprintf(gOut, "Hiragana"); return;
case UBLOCK_KATAKANA: u_fprintf(gOut, "Katakana"); return;
case UBLOCK_BOPOMOFO: u_fprintf(gOut, "Bopomofo"); return;
case UBLOCK_HANGUL_COMPATIBILITY_JAMO: u_fprintf(gOut, "Hangul Compatibility Jamo"); return;
case UBLOCK_KANBUN: u_fprintf(gOut, "Kanbun"); return;
case UBLOCK_BOPOMOFO_EXTENDED: u_fprintf(gOut, "Bopomofo Extended"); return;
case UBLOCK_ENCLOSED_CJK_LETTERS_AND_MONTHS: u_fprintf(gOut, "Enclosed CJK Letters and Months"); return;
case UBLOCK_CJK_COMPATIBILITY: u_fprintf(gOut, "CJK Compatibility"); return;
case UBLOCK_CJK_UNIFIED_IDEOGRAPHS_EXTENSION_A: u_fprintf(gOut, "CJK Unified Ideographs Extension A"); return;
case UBLOCK_CJK_UNIFIED_IDEOGRAPHS: u_fprintf(gOut, "CJK Unified Ideographs"); return;
case UBLOCK_YI_SYLLABLES: u_fprintf(gOut, "Yi Syllables"); return;
case UBLOCK_YI_RADICALS: u_fprintf(gOut, "Yi Radicals"); return;
case UBLOCK_HANGUL_SYLLABLES: u_fprintf(gOut, "Hangul Syllables"); return;
case UBLOCK_HIGH_SURROGATES: u_fprintf(gOut, "High Surrogates"); return;
case UBLOCK_HIGH_PRIVATE_USE_SURROGATES: u_fprintf(gOut, "High Private-Use Surrogates"); return;
case UBLOCK_LOW_SURROGATES: u_fprintf(gOut, "Low Surrogates"); return;
case UBLOCK_PRIVATE_USE_AREA /* PRIVATE_USE */: u_fprintf(gOut, "Private Use Area"); return;
case UBLOCK_CJK_COMPATIBILITY_IDEOGRAPHS: u_fprintf(gOut, "CJK Compatibility Ideographs"); return;
case UBLOCK_ALPHABETIC_PRESENTATION_FORMS: u_fprintf(gOut, "Alphabetic Presentation Forms"); return;
case UBLOCK_ARABIC_PRESENTATION_FORMS_A: u_fprintf(gOut, "Arabic Presentation Forms A"); return;
case UBLOCK_COMBINING_HALF_MARKS: u_fprintf(gOut, "Combining Half Marks"); return;
case UBLOCK_CJK_COMPATIBILITY_FORMS: u_fprintf(gOut, "CJK Compatibility Forms"); return;
case UBLOCK_SMALL_FORM_VARIANTS: u_fprintf(gOut, "Small Form Variants"); return;
case UBLOCK_ARABIC_PRESENTATION_FORMS_B: u_fprintf(gOut, "Arabic Presentation Forms B"); return;
case UBLOCK_SPECIALS: u_fprintf(gOut, "Specials"); return;
case UBLOCK_HALFWIDTH_AND_FULLWIDTH_FORMS: u_fprintf(gOut, "Halfwidth and Fullwidth Forms"); return;
    case UBLOCK_OLD_ITALIC: u_fprintf(gOut, "Old Italic"); return;
    case UBLOCK_GOTHIC: u_fprintf(gOut, "Gothic"); return;
    case UBLOCK_DESERET: u_fprintf(gOut, "Deseret"); return;
    case UBLOCK_BYZANTINE_MUSICAL_SYMBOLS: u_fprintf(gOut, "Byzantine Musical Symbols"); return;
    case UBLOCK_MUSICAL_SYMBOLS: u_fprintf(gOut, "Musical Symbols"); return;
    case UBLOCK_MATHEMATICAL_ALPHANUMERIC_SYMBOLS: u_fprintf(gOut, "Mathematical Alphanumeric Symbols"); return;
    case UBLOCK_CJK_UNIFIED_IDEOGRAPHS_EXTENSION_B: u_fprintf(gOut, "CJK Unified Ideographs, Extension B"); return;
    case UBLOCK_CJK_COMPATIBILITY_IDEOGRAPHS_SUPPLEMENT: u_fprintf(gOut, "CJK Compatibility Ideographs, Supplement"); return;
    case UBLOCK_TAGS: u_fprintf(gOut, "Tags"); return;
      /* ICU 2.2: (Unicode 3.2) */
    case UBLOCK_CYRILLIC_SUPPLEMENTARY: u_fprintf(gOut, "Cyrillic Supplementary"); return;
    case UBLOCK_TAGALOG: u_fprintf(gOut, "Tagalog"); return;
    case UBLOCK_HANUNOO: u_fprintf(gOut, "Hanunoo"); return;
    case UBLOCK_BUHID: u_fprintf(gOut, "Buhid"); return;
    case UBLOCK_TAGBANWA: u_fprintf(gOut, "Tagbanwa"); return;
    case UBLOCK_MISCELLANEOUS_MATHEMATICAL_SYMBOLS_A: u_fprintf(gOut, "Miscellaneous Mathematical Symbols A"); return;
    case UBLOCK_SUPPLEMENTAL_ARROWS_A: u_fprintf(gOut, "Supplemental Arrows A"); return;
    case UBLOCK_SUPPLEMENTAL_ARROWS_B: u_fprintf(gOut, "Supplemental Arrows B"); return;
    case UBLOCK_MISCELLANEOUS_MATHEMATICAL_SYMBOLS_B: u_fprintf(gOut, "Miscellaneous Mathematical Symbols B"); return;
    case UBLOCK_SUPPLEMENTAL_MATHEMATICAL_OPERATORS: u_fprintf(gOut, "Supplemental Mathematical Operators"); return;
    case UBLOCK_KATAKANA_PHONETIC_EXTENSIONS: u_fprintf(gOut, "Katakana Phonetic Extensions"); return;
    case UBLOCK_VARIATION_SELECTORS: u_fprintf(gOut, "Variation Selectors"); return;
    case UBLOCK_SUPPLEMENTARY_PRIVATE_USE_AREA_A: u_fprintf(gOut, "Supplementary Private Use Area A"); return;
    case UBLOCK_SUPPLEMENTARY_PRIVATE_USE_AREA_B: u_fprintf(gOut, "Supplementary Private Use Area B"); return;
      

    default: u_fprintf(gOut, "Unknown block %d",block); return;
    }
}

UChar32 doSearchBlock(int32_t block, UChar32 startFrom)
{
  UChar32 end = (startFrom-1);
  if(end > UCHAR_MAX_VALUE)
  {
    end = UCHAR_MAX_VALUE;
  }

  for(;startFrom != end; startFrom++)
  {
    if(startFrom > UCHAR_MAX_VALUE)
      startFrom = UCHAR_MIN_VALUE;
    if(u_charScript(startFrom) == block)
      return startFrom;
  }

  return startFrom;
}

UChar32 doSearchType(int8_t type, UChar32 startFrom)
{
  UChar32 end = (startFrom-1);
#ifdef DEBUG_UBROWSE
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


#ifdef DEBUG_UBROWSE
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

  u_fprintf(gOut, "<table border=0 cellpadding=0 cellspacing=0 qwidth=100% ><tr><td bgcolor=\"#000000\">\r\n"
         "<table border=0 cellpadding=1 cellspacing=1 qwidth=100%><tr><td bgcolor=\"#cccccc\">\r\n"
/*          "<table border=2 cellpadding=2 cellspacing=2 qwidth=100%><tr><td bgcolor=\"#cccccc\">\r\n" */
         "\r\n");

  u_fprintf(gOut, "<b>Find</b><br>\r\n"
         "\r\n"
         "</td></tr><tr><td bgcolor=\"#eeeeee\">\r\n"
         "<table cellspacing=8><tr><td align=left>Block:</td><FORM METHOD=GET><td align=right>\r\n"
         "\r\n");

  u_fprintf(gOut, "<SELECT NAME=scr>");
  for(i=UBLOCK_BASIC_LATIN;i<UBLOCK_COUNT;i++)
    {
      u_fprintf(gOut, "  <OPTION ");
      if(i == gSearchBlock)
	printf(" SELECTED ");
      u_fprintf(gOut, " VALUE=\"%d\">", i);
      printBlock(i);
      u_fprintf(gOut, "\r\n");
    }
  u_fprintf(gOut, "</SELECT>\r\n");
  u_fprintf(gOut, "<INPUT TYPE=hidden NAME=b VALUE=%04X>\r\n", startFrom);
  u_fprintf(gOut, "<INPUT TYPE=SUBMIT VALUE=\"Find%s\">",
         (startFrom != 0)?" Next":"");
  u_fprintf(gOut, "</td></FORM>");

  u_fprintf(gOut, "</tr><tr><FORM METHOD=GET><td align=left>General Category: </td><td align=right>");
  u_fprintf(gOut, "<SELECT NAME=typ>\r\n");
  for(i=U_UNASSIGNED;i<U_CHAR_CATEGORY_COUNT;i++)
    {
      u_fprintf(gOut, "  <OPTION ");
      if(i == gSearchType)
	printf(" SELECTED ");
      u_fprintf(gOut, " VALUE=\"%d\">", i);
      printType(i);
      u_fprintf(gOut, "\r\n");
    }
  u_fprintf(gOut, "</SELECT>\r\n");
  u_fprintf(gOut, "<INPUT TYPE=hidden NAME=b VALUE=%04X>\r\n", startFrom);
  u_fprintf(gOut, "<INPUT TYPE=SUBMIT VALUE=\"Find%s\">", 
         (startFrom != 0)?" Next":"");
  u_fprintf(gOut, "");

  u_fprintf(gOut, "</td></FORM></tr>");
  u_fprintf(gOut, "<tr><FORM METHOD=GET><td align=left>Name</td><td align=right><INPUT size=40 NAME=s VALUE=\"%s\"> &nbsp; <input type=checkbox name=sx>Exact?<INPUT TYPE=SUBMIT VALUE=\"Find\">",
         gSearchName);
  u_fprintf(gOut, "</TD></FORM>\r\n");

#ifdef RADICAL_LIST
  u_fprintf(gOut, "<td><A HREF=\"?radlst=1\">Radicals</A></td>");
#endif
  
  u_fprintf(gOut, "</tr><tr><td>\r\n"
         "\r\n");
  u_fprintf(gOut, "<B>Encoding: %s.</B><BR>\r\n", gOurEncoding);
  u_fprintf(gOut, "\r\n"
         "</td></tr></table>\r\n"
         "</td></tr></table>\r\n");

}

