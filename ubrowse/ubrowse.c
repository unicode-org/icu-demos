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
void printBlock(UCharBlock block);
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
bool_t gSearchCharValid = FALSE;

bool_t anyDecompose = FALSE;

const char *gOurEncoding = NULL;

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
    puts(junkbuf);
  else
    printf("{U+%04X}", c);
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
					    bool_t flush,
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

  UTF_APPEND_CHAR_UNSAFE(chars, offset, theChar);
/*  chars[offset] = 0; */

  up_u2c(theString, chars, offset, &status);

  if(status == U_USING_FALLBACK_ERROR)
    {
      printf("<FONT COLOR=\"#00DD00\"><I>");
      anyDecompose = TRUE;
    }
  /*  fprintf(stderr, "%d", status); */

  puts("<FONT SIZE=+1>");
  puts(theString);

  if(status == U_USING_FALLBACK_ERROR)
      printf("</I></FONT>");
  puts("</FONT>");
}


UBool myEnumCharNamesFn(void *context,
                        UChar32 code,
                        UCharNameChoice nameChoice,
                        const char *name,
                        UTextOffset length)
{
  enumHits++;
  if(strstr(name, gSearchName))
  {
    foundCount ++;

    printRow(code, TRUE, "",  "k1");

    if((foundCount % 15) ==14) /* ------------ Search by name: too many hits --- */
    {
      printf("</table> <P><UL><H4><A HREF=\"?s=%s&cs=%04X\">Continue search for '%s' from U+%04X...</A></H4></UL><P>",
             gSearchHTML, code+1, gSearchName, code+1);
      return FALSE;
    }

//    printf("<LI><A HREF=\"?k1=%04X#here\"><TT>%04X</TT> - %s\r\n",
//           code, code, name);
    
//    printBasicBlockInfo( code );
//    printf("</A>\r\n");
  }
  return TRUE;
}

void printIconMenu(const char *alt, const char *name, ESearchMode target, ESearchMode current)
{
  printf("<INPUT TYPE=image "
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

	  printf("<TR %s >", hilite);

          /** 0 Row # (un needed????) **/ 
//          printf("<TD><B>%X</B></TD>", r);

          /** 1 The Char **/
	  printf("<TD ");

          /* TODO: 'if matches(theChar)' */

	  printf("%s ALIGN=CENTER>\r\n", hilite);
	  
	  if((theChar == gSearchChar) && gSearchCharValid)
	    printf("<A NAME=here></A>");
	  
	  /* print the simple data (i.e. the actual char ) */
	  printBasicBlockInfo(theChar);
	  
	  printf("</TD>");


          /** 2. print the char val and name  */
          printf("<TD>");
          if(link != NULL)
          {
            printf("<A HREF=\"?%s=%04X#here\">", link, theChar);
          }
          printf("<FONT SIZE=-1>U+%04X ", theChar);
          printCharName(theChar);
          printf("</FONT>");
          if(link != NULL)
          {
            printf("</A>");
          }
          printf("</TD>");

          /** 3. char type */
	  searchedFor = (u_charType(theChar) == gSearchType);
          printf("<td>");
	  if(searchedFor)
	    printf("<B>");
	  printType(u_charType(theChar));
	  if(searchedFor)
	    printf("</B>");
          printf("</td>");

          /** 3 1/2. optional - block */
          if(showBlock)
          {  
            printf("<TD ");
            searchedFor = (u_charScript(theChar) == gSearchBlock);
            
            if(searchedFor)
              printf(" BGCOLOR=\"#EE0000\" "); /* mark the one we were searching for */
            
            printf(" >");
            
            if(searchedFor)
              printf("<B>");
            
            printf("<A HREF=\"?scr=%d&b=%04X\">", (u_charScript(theChar)+1)%U_SCRIPT_BLOCK_COUNT, theChar);
            printBlock(u_charScript(theChar));
            printf("</A>");
            
            if(searchedFor)
              printf("</B>");
            
            printf("</TD>");
          }

          /** 4: digit (optional) **/
	  printf("<TD>");
	  if(u_isdigit(theChar))
          {
            if(u_charDigitValue(theChar) == -1)
            {
              printf("-");
            }
            else
            {
              printf("%d", u_charDigitValue(theChar));
            }
          }
	  printf("</TD>");

          /** 5: cell width **/
          printf("<TD>");
	  switch(u_charCellWidth(theChar))
	  {
	    case U_ZERO_WIDTH: printf("0"); break; 
	    case U_HALF_WIDTH: printf("1/2"); break; 
	    case U_FULL_WIDTH: printf("1"); break; 
	    case U_NEUTRAL_WIDTH: printf("-"); break; 
	  }

          printf("</TD>");

          /** 6. direction **/
          printf("<TD>");
	  switch(u_charDirection(theChar))
	    {
	    case U_LEFT_TO_RIGHT: printf("LTR"); break; 
	    case U_RIGHT_TO_LEFT: printf("RTL"); break; 
	    case U_EUROPEAN_NUMBER: printf("European Number"); break; 
	    case U_EUROPEAN_NUMBER_SEPARATOR: printf("Eur. Num. Sep"); break; 
	    case U_EUROPEAN_NUMBER_TERMINATOR: printf("Eur. num. Term"); break; 
	    case U_ARABIC_NUMBER: printf("Arabic Num."); break; 
	    case U_COMMON_NUMBER_SEPARATOR: printf("Cmn Number Sep"); break; 
	    case U_BLOCK_SEPARATOR: printf("Block Sep"); break; 
	    case U_SEGMENT_SEPARATOR: printf("Segment Sep"); break; 
	    case U_WHITE_SPACE_NEUTRAL: printf("White Space Neutral"); break; 
	    case U_OTHER_NEUTRAL: printf("Other Neutral"); break; 
	    case U_LEFT_TO_RIGHT_EMBEDDING: printf("LRE"); break; 
	    case U_LEFT_TO_RIGHT_OVERRIDE: printf("LRO"); break; 
	    case U_RIGHT_TO_LEFT_ARABIC: printf("RTL-Arabic"); break; 
	    case U_RIGHT_TO_LEFT_EMBEDDING: printf("RLE"); break; 
	    case U_RIGHT_TO_LEFT_OVERRIDE: printf("RLO"); break; 
	    case U_POP_DIRECTIONAL_FORMAT: printf("PDF"); break; 
	    case U_DIR_NON_SPACING_MARK: printf("Combining Class %d", u_getCombiningClass(theChar)); break; 
	    case U_BOUNDARY_NEUTRAL: printf("BN"); break; 
	    default: printf("Unknown Dir\n");
	    }
          /*  put the mirrored thing here. Not so common, keeps it out of the way. */
          if(u_isMirrored(theChar))
          {
            printf(" (Mirrored)");
          }
	  printf("</TD>");


	  /** image - removed **/
/*  	  printf("<TD>"); */
/*            /* */
/*                -- this works but it's kind of RUDE. -- [to link in w/o asking] */
/*                printf("<IMG WIDTH=32 HEIGHT=32 SRC=\"http://charts.unicode.org/Unicode.charts/Small.Glyphs/%02X/U%04X.gif\">\r\n", */
/*                   ((theChar&0x1FFF00)>>8), */
/*                   theChar); */
/*  #error you're rude */
/*            printf("</TD>"); */

          printf("</TR>\r\n");

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
  bool_t searchedFor;
  UChar32 block = 0xFFFF;
  ESearchMode mode;
  UVersionInfo uvi;

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

  ucnv_setDefaultName(gOurEncoding);
  printf("content-type: text/html;charset=%s\r\n\r\n",gOurEncoding);

  printf("<HTML><HEAD>\r\n");
  if(!tmp) /* if there was no trailing '/' ... */
  {
    if(pi!=NULL) {
      printf("<BASE HREF=\"http://%s%s/%s/\">\r\n", getenv("SERVER_NAME"),
             getenv("SCRIPT_NAME"),
             pi);
    } else {
      printf("<BASE HREF=\"http://%s%s/\">\r\n", getenv("SERVER_NAME"),
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
            printf("<TITLE>ICU UnicodeBrowser: column U+%04X</TITLE>\r\n", block);
            mode = ECOLUMN;
          }
          else if(strstr(qs, "n.x="))
          {
            block &= 0x1FFF00;
            printf("<TITLE>ICU UnicodeBrowser: block U+%04X</TITLE>\r\n", block);
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
	  printf("<TITLE>ICU UnicodeBrowser: block U+%04X</TITLE>\r\n", block);
	  mode = EBLOCK;
	}
      else if(sscanf(qs, "k1=%x", &block) == 1)
	{
	  gSearchChar = block;
	  gSearchCharValid = TRUE;
	  block &= 0x1FFFF0;
	  printf("<TITLE>ICU UnicodeBrowser: column U+%04X</TITLE>\r\n", block);
	  mode = ECOLUMN;
	}
      else if(sscanf(qs, "k=%x", &block) == 1)
	{
	  block &= 0x1FFFF0;
	  printf("<TITLE>ICU UnicodeBrowser: column U+%04X</TITLE>\r\n", block);
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
      printf("<TITLE>ICU UnicodeBrowser</TITLE>\r\n");
    }


  printf("</HEAD>\r\n");
  
  printf("<BODY BGCOLOR=\"#FFFFFF\" link=\"green\" vlink=\"brown\">\r\n");

  printf("<A HREF=\"http://oss.software.ibm.com/icu\">ICU</A> &gt;\r\n"
         "<A HREF=\"http://oss.software.ibm.com/icu/demo\">Demo</A> &gt;\r\n"
         "<B>Unicode Browser</B><BR>\r\n");


  printf("<form><table border=1 cellpadding=1 cellspacing=1><tr>");

  printf("<td >");

  printf("Go: <INPUT size=7 NAME=go VALUE=\"%04X\">", block);

  /* show which item we're on */
  printIconMenu("column", "k", ECOLUMN, mode);
  printIconMenu("block",  "n", EBLOCK, mode);
  printf(" | ");
  printIconMenu("all",    "all", ETOP, mode);

  /* forward and back buttons */
  switch(mode)
  {
  case EBLOCK:
    printf(" | <A HREF=\"?n=%04X\">prev</A> <A HREF=\"?n=%04X\">next</A> ",
           (block & 0x1FFF00)-0x100, (block & 0x1FFF00)+0x100);
    break;

  case ECOLUMN:
    printf(" | <A HREF=\"?k=%04X\">prev</A> <A HREF=\"?k=%04X\">next</A> ",
	     (block & 0x1FFFF0)-0x10, (block & 0x1FFFF0)+0x10);
    break;
  }

  printf("");
  printf("</TD>");

  printf("<td>\r\n"); 

  printf("</td>");  


  if(mode == ETOP) /* top level list of blocks ******************************** ETOP ********** */
    {

      printf("</tr></table></form>"); /* closer of menu */

        printf("<table border=0 cellpadding=1 cellspacing=1><tr><td bgcolor=\"#cccccc\">\r\n");

      printf("<b>Unicode Browser</b> - Click on a type of character view it in more detail<br>\r\n");
      printf("</td></tr><tr><td bgcolor=\"#eeeeee\">\r\n"
             );

      printf("<B>Blocks:</B>");
      for(i=U_BASIC_LATIN_BLOCK;i<U_SCRIPT_BLOCK_COUNT;i++)
      {
        printf("<A HREF=\"?scr=%d&b=0\">", i);
        printBlock(i);
        printf("</A> | &nbsp;");
      }
      printf("<P>");
      

      printf("<B>General Categories:</B>");
      for(i=U_UNASSIGNED;i<=U_GENERAL_OTHER_TYPES;i++)
      {
        printf("<A HREF=\"?typ=%d&b=0\">", i);
        printType(i);
        printf("</A> | &nbsp;");
      }
      printf("<P>");
      


      printf(
             "</td></tr></table>\r\n"
             );

//      printf("\r\n</td></tr><tr><td align=right>\r\n");
      
      showSearchMenu(0x0000);
    }      
  else if (mode == EBLOCK) /* *************** BLOCK *******************************************/
    {

    printf("</tr></table></form>"); /* closer of menu */


      /* Unicode table  at block 'block' */

      n = 0;
      /*      for(n=0; n<0x100; n += 0x080) */
	{
	  printf("<TT><TABLE BORDER=1>");
	  printf("<TR><TD></TD>");
	  for(c = n;c<(n + 0x100);c+= 0x10)
	    {
	      printf("<TD><B><A HREF=\"?k=%04X\"><TT>%03X</TT></A></B></TD>", (block|c),  (block | c) >> 4   );
	    }
	  printf("</TR>\r\n");
	  for(r = 0; r < 0x10; r++)
	    {
	      printf("<TR><TD><B><TT>%X</TT></B></TD>", r);
	      for(c = n;c<(n + 0x100);c+= 0x10)
		{
		  theChar = (block | r | c );

		  printf("<TD ");
		  
		  if(u_charType(theChar) == U_UNASSIGNED)
		    {
		      printf(" BGCOLOR=\"#888888\" ");
		    }
		  
		  printf(" ALIGN=CENTER>\r\n");
		  
		  
		  /* print the simple data */
		  printBasicBlockInfo(theChar);
		  
		  printf("</TD>");
		}
	      printf("<TD><B><TT>%X</TT></B></TD>", r);
	    }
	}
	printf("</TR>");
	printf("<TR><TD COLSPAN=18 ALIGN=CENTER><I>Click on a column number to zoom in.</I></TD></TR>\r\n");
	printf("</TABLE></TT>");
      printf("<HR>\r\n");
      if(block <= 0xFFFF)
        {
          printf("<A HREF=\"http://charts.unicode.org/Unicode.charts/normal/U%04X.html#Glyphs\">this block on charts.unicode.org</A>\r\n", block);
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
        printf("<TD>"); 
        printf("<B>Block:</B>  ");
        searchedFor = (u_charScript(theChar) == gSearchBlock);
        
        
        if(searchedFor)
          printf("<B>");
        
        printBlock(u_charScript(theChar));
        if(searchedFor)
          printf("</B>");

        /*  "| Next " on block list . removed. 
         * printf(" | <A HREF=\"?scr=%d&b=%04X\">", (u_charScript(theChar)+1)%U_CHAR_SCRIPT_COUNT, theChar);
         * printf("next</A>");
         */
            
        printf("</TD></TR></TABLE></form>");  /* closer */
      }
      else
      {
        printf("</tr></table></form>"); /* closer of menu */
      }



      printf("<TABLE BORDER=1>");


      printf("<TR>"); /* 0: column index */
      printf("<TD>C</TD>"); /* 1: char */
      printf("<TD>N</TD>"); /* 2: char #, name */
      printf("<TD><B>General Category</B></TD>"); /* 3: type */ 
      if(showBlock == TRUE) 
      {
        printf("<TD><B>Block</B></TD>"); /* 3 1/2 */
      }
      printf("<TD><B></B></TD>"); /* 4 digit */
      printf("<TD><B>Wid</B></TD>"); /* 5 width */
      printf("<TD><B>BiDi</B></TD>"); /* 6 Direction (BiDi?) */
      printf("</TR>\r\n");

#if 0 /* Don't identify the columns (for now) */
      printf("<TR><TD></TD>");
      printf("<TD><B>%03X</B></TD>",  (block ) >> 4   );


      printf("<TD><!-- <B>Lower<BR>Upper<BR>Title</B>--></TD>");
      printf("<TD><B>#</B></TD>");
      printf("<TD><!-- <B>alph<BR>spc<BR>ctrl<BR>print</B>--></TD>");
      printf("<TD><B>base<BR>direction<BR>width<BR>type</B></TD>");
      if(showBlock == TRUE) 
      {
        printf("<TD><B>Block</B></TD>");
      }

      printf("</TR>\r\n");
#endif
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
      

      printf("</TABLE></TT><HR>");

      showSearchMenu( block + 0x0010);

    }

#ifdef RADICAL_LIST 
  else if(mode == ERADLST) /************************ RADICAL LIST ********************/
    {
      printf("</table></form>");
      printf("<TABLE BORDER=1>");
      printf("<TR>");
      for(i=0;gKangXiRadicalTable[i];i+=2)
	{
	  if((i%16) == 0)
	    printf("</TR><TR>\r\n");
	  printf("<TD>");
	  printf("<A HREF=\"?radical=%d\">", (i/2)+1);
	  printBasicBlockInfo(gKangXiRadicalTable[i]);
	  printf("</A>");
	  printf("</TD>");

	}
      printf("</TR></TABLE>");
    }
  else if(mode == ERADICAL) /************************* RADICAL ************************/
    {
      FILE *f;
      int u,stroke;
      char s[200];

      printf("</td></tr></table></form>"); // closer

      printf("<B>");
      printBasicBlockInfo(gKangXiRadicalTable[(block-1)*2]);
      printf(" (radical %d)</B> \r\n",block);
      printCharName(gKangXiRadicalTable[(block-1)*2]);
      sprintf(s,"/home/srl/icu/data/PSI/rad%03d.lst", block);
      printf("<TABLE BORDER=1>");
      f = fopen(s,"r");
      i=0;
      printf("<TR>");
      while(fscanf(f,"%04X\t%d", &u, &stroke) == 2)
	{
	  if(!( (i++)%10))
	    printf("</TR><TR>");
	  printf("<TD>");
	  printf("%d ", stroke);
	  printf("<A HREF=\"?k1=%04X#here\">", u);
	  printBasicBlockInfo(u);
	  printf("</A></TD>");
	}
      fclose(f);
      printf("</TR></TABLE>");
    }
#endif /* RADICAL_LIST */
  else if(mode == ENAME) /************************ NAME ***************************/
    {
      UErrorCode status = U_ZERO_ERROR;

      printf("</td></tr></table></form>"); /* closer */

      /* "Be careful what you search for, you just might find it"
	 (and 0x10FFFF of it's close friends!)
      */

      if(block == 0)
      {
        printf("<H1>Searching for '%s'...</H1>\r\n", gSearchName);
      }
      else
      {
        printf("<H1>Searching for '%s' from U+%04X...</H1>\r\n", gSearchName, block);
      }
      
      printf("<table border=1>");

      u_enumCharNames(block, UCHAR_MAX_VALUE,
                      myEnumCharNamesFn,
                      NULL,
                      U_UNICODE_CHAR_NAME,
                      &status);

      if(foundCount == 0)
        {
          printf("<tr><td><i>Not found.</i></td></tr>\n");
        }

      printf("</table><!-- %d tested --><HR>\r\n",enumHits);
      showSearchMenu( 0x0000 ); 
    }
  else if(mode == EEXACTNAME) /************** EXACT NAME ********************************/
    {
      UChar32 c;
      UErrorCode status = U_ZERO_ERROR;

      printf("</td></tr></table></form>"); // closer


      printf("<H1>Searching for '%s'...</H1>\r\n", gSearchName);
      
      
      printf("<HR><UL>\r\n");

      c = u_charFromName(U_UNICODE_CHAR_NAME,
                         gSearchName,
                         &status);

      if(U_FAILURE(status))
      {
          printf("Error - %s %d<P>\n", u_errorName(status), status);
      }
      else if(c == 0x00FFFF )
      {
        printf("<i>Not found.</i><p>Please note that this search requires an exact unicode name.\n");
      }
      else
      {
        printf("<LI><A HREF=\"?k1=%04X#here\"><TT>%04X</TT> - %s\r\n",
               c,c,gSearchName);
        printBasicBlockInfo(c);
        
      }

      printf("</UL><HR>\r\n");
      showSearchMenu( 0x0000 ); 
    }
  else /************************************* ????????????????????????? ****************/
    {
      printf("</td></tr></table></form>"); /* closer */

      printf("Unknown search mode ! <HR>\r\n");
    }
  

  printf("</td></tr></table>\r\n\r\n"); /* ? */



  if(anyDecompose)
    {
      printf("Note: text in <FONT COLOR=\"#00DD00\">");
      printf("<I>Italic Green</I></FONT> is the best-try using decomposition. Underscore (_) denotes missing characters in the decomposition.<P>");

    }

  u_getUnicodeVersion(uvi);
  printf("<A HREF=\"http://www.unicode.org/\">Based on Unicode ");
  for(uc=0;uc<U_MAX_VERSION_LENGTH;uc++) {
    if((uc < 3) || (uvi[uc] != 0)) /* don't show trailing zero in 4th place. */
      printf("%d.",uvi[uc]);
  }
  printf("</A>\r\n");

  printf("<BR>Powered by <A HREF=\"http://oss.software.ibm.com/icu/\">ICU %s</A>\r\n", U_ICU_VERSION);
  
  printf("</td></tr></table>\r\n");

  printf("</BODY><!-- SFCBugibba design 12 ta' Mejju 2001 --></HTML>\r\n");
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
	  puts("Could not open the default converter.");
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
    case U_UNASSIGNED: printf("Unassigned"); break; 
    case U_UPPERCASE_LETTER: printf("Uppercase Letter");  break;  
    case U_LOWERCASE_LETTER:  printf("Lowercase Letter");  break; 
    case U_TITLECASE_LETTER: printf("Titlecase Letter"); break;  
    case U_MODIFIER_LETTER: printf("Modifier Letter"); break; 
    case U_OTHER_LETTER: printf("Other Letter"); break; 
    case U_NON_SPACING_MARK: printf("Non-Spacing Mark"); break; 
    case U_ENCLOSING_MARK: printf("Enclosing Mark"); break; 
    case U_COMBINING_SPACING_MARK: printf("Combining Spacing Mark"); break; 
    case U_DECIMAL_DIGIT_NUMBER: printf("Decimal Digit Number"); break; 
    case U_LETTER_NUMBER: printf("Letter Number"); break; 
    case U_OTHER_NUMBER: printf("Other Number"); break; 
    case U_SPACE_SEPARATOR: printf("Space Separator"); break; 
    case U_LINE_SEPARATOR: printf("Line Separator"); break; 
    case U_PARAGRAPH_SEPARATOR: printf("Paragraph Separator"); break; 
    case U_CONTROL_CHAR: printf("Control"); break; 
    case U_FORMAT_CHAR: printf("Format"); break; 
    case U_PRIVATE_USE_CHAR: printf("Private Use"); break; 
    case U_SURROGATE: printf("Surrogate"); break; 
    case U_DASH_PUNCTUATION: printf("Dash Punctuation"); break; 
    case U_START_PUNCTUATION: printf("Start Punctuation"); break; 
    case U_END_PUNCTUATION: printf("End Punctuation"); break; 
    case U_CONNECTOR_PUNCTUATION: printf("Connector Punctuation"); break; 
    case U_OTHER_PUNCTUATION: printf("Other Punctuation"); break; 
    case U_MATH_SYMBOL: printf("Math Symbol"); break; 
    case U_CURRENCY_SYMBOL: printf("Currency Symbol"); break; 
    case U_MODIFIER_SYMBOL: printf("Modifier Symbol"); break; 
    case U_OTHER_SYMBOL: printf("Other Symbol"); break; 
    case U_INITIAL_PUNCTUATION: printf("Initial Punctuation"); break; 
    case U_FINAL_PUNCTUATION: printf("Final Punctuation"); break; 
    case U_GENERAL_OTHER_TYPES: printf("General Other Types"); break;  /* sic */
    default: printf("Unknown type %d", type); break;
    }
}


void printBlock(UCharBlock block)
{
  switch(block)
    {
case U_BASIC_LATIN_BLOCK: printf("Basic Latin"); return;
case U_LATIN_1_SUPPLEMENT_BLOCK: printf("Latin-1 Supplement"); return;
case U_LATIN_EXTENDED_A_BLOCK: printf("Latin Extended A"); return;
case U_LATIN_EXTENDED_B_BLOCK: printf("Latin Extended B"); return;
case U_IPA_EXTENSIONS_BLOCK: printf("IPA Extensions"); return;
case U_SPACING_MODIFIER_LETTERS_BLOCK: printf("Spacing Modifier Letters"); return;
case U_COMBINING_DIACRITICAL_MARKS_BLOCK: printf("Combining Diacritical Marks"); return;
case U_GREEK_BLOCK: printf("Greek"); return;
case U_CYRILLIC_BLOCK: printf("Cyrillic"); return;
case U_ARMENIAN_BLOCK: printf("Armenian"); return;
case U_HEBREW_BLOCK: printf("Hebrew"); return;
case U_ARABIC_BLOCK: printf("Arabic"); return;
case U_SYRIAC_BLOCK: printf("Syriac"); return;
case U_THAANA_BLOCK: printf("Thaana"); return;
case U_DEVANAGARI_BLOCK: printf("Devanagari"); return;
case U_BENGALI_BLOCK: printf("Bengali"); return;
case U_GURMUKHI_BLOCK: printf("Gurmukhi"); return;
case U_GUJARATI_BLOCK: printf("Gujarati"); return;
case U_ORIYA_BLOCK: printf("Oriya"); return;
case U_TAMIL_BLOCK: printf("Tamil"); return;
case U_TELUGU_BLOCK: printf("Telugu"); return;
case U_KANNADA_BLOCK: printf("Kannada"); return;
case U_MALAYALAM_BLOCK: printf("Malayalam"); return;
case U_SINHALA_BLOCK: printf("Sinhala"); return;
case U_THAI_BLOCK: printf("Thai"); return;
case U_LAO_BLOCK: printf("Lao"); return;
case U_TIBETAN_BLOCK: printf("Tibetan"); return;
case U_MYANMAR_BLOCK: printf("Myanmar"); return;
case U_GEORGIAN_BLOCK: printf("Georgian"); return;
case U_HANGUL_JAMO_BLOCK: printf("Hangul-Jamo"); return;
case U_ETHIOPIC_BLOCK: printf("Ethiopic"); return;
case U_CHEROKEE_BLOCK: printf("Cherokee"); return;
case U_UNIFIED_CANADIAN_ABORIGINAL_SYLLABICS_BLOCK: printf("Unified Canadian Aboriginal Syllabics"); return;
case U_OGHAM_BLOCK: printf("Ogham"); return;
case U_RUNIC_BLOCK: printf("Runic"); return;
case U_KHMER_BLOCK: printf("Khmer"); return;
case U_MONGOLIAN_BLOCK: printf("Mongolian"); return;
case U_LATIN_EXTENDED_ADDITIONAL_BLOCK: printf("Latin Extended Additional"); return;
case U_GREEK_EXTENDED_BLOCK: printf("Greek Extended"); return;
case U_GENERAL_PUNCTUATION_BLOCK: printf("General Punctuation"); return;
case U_SUPERSCRIPTS_AND_SUBSCRIPTS_BLOCK: printf("Superscripts and Subscripts"); return;
case U_CURRENCY_SYMBOLS_BLOCK: printf("Currency Symbols"); return;
case U_COMBINING_MARKS_FOR_SYMBOLS_BLOCK: printf("Combining Marks for Symbols"); return;
case U_LETTERLIKE_SYMBOLS_BLOCK: printf("Letterlike Symbols"); return;
case U_NUMBER_FORMS_BLOCK: printf("Number Forms"); return;
case U_ARROWS_BLOCK: printf("Arrows"); return;
case U_MATHEMATICAL_OPERATORS_BLOCK: printf("Mathematical Operators"); return;
case U_MISCELLANEOUS_TECHNICAL_BLOCK: printf("Miscellaneous Technical"); return;
case U_CONTROL_PICTURES_BLOCK: printf("Control Pictures"); return;
case U_OPTICAL_CHARACTER_RECOGNITION_BLOCK: printf("Optical Character Recognition"); return;
case U_ENCLOSED_ALPHANUMERICS_BLOCK: printf("Enclosed Alphanumerics"); return;
case U_BOX_DRAWING_BLOCK: printf("Box Drawing"); return;
case U_BLOCK_ELEMENTS_BLOCK: printf("Block Elements"); return;
case U_GEOMETRIC_SHAPES_BLOCK: printf("Geometric Shapes"); return;
case U_MISCELLANEOUS_SYMBOLS_BLOCK: printf("Miscellaneous Symbols"); return;
case U_DINGBATS_BLOCK: printf("Dingbats"); return;
case U_BRAILLE_PATTERNS_BLOCK: printf("Braille Patterns"); return;
case U_CJK_RADICALS_SUPPLEMENT_BLOCK: printf("CJK Radicals Supplement"); return;
case U_KANGXI_RADICALS_BLOCK: printf("KangXi Radicals"); return;
case U_IDEOGRAPHIC_DESCRIPTION_CHARACTERS_BLOCK: printf("Ideographic Description Characters"); return;
case U_CJK_SYMBOLS_AND_PUNCTUATION_BLOCK: printf("CJK Symbols and Punctuation"); return;
case U_HIRAGANA_BLOCK: printf("Hiragana"); return;
case U_KATAKANA_BLOCK: printf("Katakana"); return;
case U_BOPOMOFO_BLOCK: printf("Bopomofo"); return;
case U_HANGUL_COMPATIBILITY_JAMO_BLOCK: printf("Hangul Compatibility Jamo"); return;
case U_KANBUN_BLOCK: printf("Kanbun"); return;
case U_BOPOMOFO_EXTENDED_BLOCK: printf("Bopomofo Extended"); return;
case U_ENCLOSED_CJK_LETTERS_AND_MONTHS_BLOCK: printf("Enclosed CJK Letters and Months"); return;
case U_CJK_COMPATIBILITY_BLOCK: printf("CJK Compatibility"); return;
case U_CJK_UNIFIED_IDEOGRAPHS_EXTENSION_A_BLOCK: printf("CJK Unified Ideographs Extension A"); return;
case U_CJK_UNIFIED_IDEOGRAPHS_BLOCK: printf("CJK Unified Ideographs"); return;
case U_YI_SYLLABLES_BLOCK: printf("Yi Syllables"); return;
case U_YI_RADICALS_BLOCK: printf("Yi Radicals"); return;
case U_HANGUL_SYLLABLES_BLOCK: printf("Hangul Syllables"); return;
case U_HIGH_SURROGATES_BLOCK: printf("High Surrogates"); return;
case U_HIGH_PRIVATE_USE_SURROGATES_BLOCK: printf("High Private-Use Surrogates"); return;
case U_LOW_SURROGATES_BLOCK: printf("Low Surrogates"); return;
case U_PRIVATE_USE_AREA_BLOCK /* PRIVATE_USE */: printf("Private Use Area"); return;
case U_CJK_COMPATIBILITY_IDEOGRAPHS_BLOCK: printf("CJK Compatibility Ideographs"); return;
case U_ALPHABETIC_PRESENTATION_FORMS_BLOCK: printf("Alphabetic Presentation Forms"); return;
case U_ARABIC_PRESENTATION_FORMS_A_BLOCK: printf("Arabic Presentation Forms A"); return;
case U_COMBINING_HALF_MARKS_BLOCK: printf("Combining Half Marks"); return;
case U_CJK_COMPATIBILITY_FORMS_BLOCK: printf("CJK Compatibility Forms"); return;
case U_SMALL_FORM_VARIANTS_BLOCK: printf("Small Form Variants"); return;
case U_ARABIC_PRESENTATION_FORMS_B_BLOCK: printf("Arabic Presentation Forms B"); return;
case U_SPECIALS_BLOCK: printf("Specials"); return;
case U_HALFWIDTH_AND_FULLWIDTH_FORMS_BLOCK: printf("Halfwidth and Fullwidth Forms"); return;
  /*case U_CHAR_SCRIPT_COUNT: printf("Script Count"); return; */
case U_NO_SCRIPT: printf("No Script"); return;

    default: printf("Unknown block %d",block); return;
    }
}

UChar32 doSearchBlock(int32_t block, UChar32 startFrom)
{
  UChar32 end = (startFrom-1);
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
  for(;startFrom != end; startFrom++)
  {
    if(startFrom > UCHAR_MAX_VALUE)
      startFrom = UCHAR_MIN_VALUE;

    if(u_charType(startFrom) == type)
      return startFrom;
  }

  return startFrom;
}

void showSearchMenu(UChar32 startFrom)
{
  int32_t i;

  printf("<table border=0 cellpadding=0 cellspacing=0 qwidth=100% ><tr><td bgcolor=\"#000000\">\r\n"
         "<table border=0 cellpadding=1 cellspacing=1 qwidth=100%><tr><td bgcolor=\"#cccccc\">\r\n"
//         "<table border=2 cellpadding=2 cellspacing=2 qwidth=100%><tr><td bgcolor=\"#cccccc\">\r\n"
         "\r\n");

  printf("<b>Find</b><br>\r\n"
         "\r\n"
         "</td></tr><tr><td bgcolor=\"#eeeeee\">\r\n"
         "<table cellspacing=8><tr><td align=left>Block:</td><FORM METHOD=GET><td align=right>\r\n"
         "\r\n");

  printf("<SELECT NAME=scr>");
  for(i=U_BASIC_LATIN_BLOCK;i<=U_SCRIPT_BLOCK_COUNT;i++)
    {
      printf("  <OPTION ");
      if(i == gSearchBlock)
	printf(" SELECTED ");
      printf(" VALUE=\"%d\">", i);
      printBlock(i);
      printf("\r\n");
    }
  printf("</SELECT>\r\n");
  printf("<INPUT TYPE=hidden NAME=b VALUE=%04X>\r\n", startFrom);
  printf("<INPUT TYPE=SUBMIT VALUE=\"Find%s\">",
         (startFrom != 0)?" Next":"");
  printf("</td></FORM>");

  printf("</tr><tr><FORM METHOD=GET><td align=left>General Category: </td><td align=right>");
  printf("<SELECT NAME=typ>\r\n");
  for(i=U_UNASSIGNED;i<=U_GENERAL_OTHER_TYPES;i++)
    {
      printf("  <OPTION ");
      if(i == gSearchType)
	printf(" SELECTED ");
      printf(" VALUE=\"%d\">", i);
      printType(i);
      printf("\r\n");
    }
  printf("</SELECT>\r\n");
  printf("<INPUT TYPE=hidden NAME=b VALUE=%04X>\r\n", startFrom);
  printf("<INPUT TYPE=SUBMIT VALUE=\"Find%s\">", 
         (startFrom != 0)?" Next":"");
  printf("");

  printf("</td></FORM></tr>");
  printf("<tr><FORM METHOD=GET><td align=left>Name</td><td align=right><INPUT size=40 NAME=s VALUE=\"%s\"> &nbsp; <input type=checkbox name=sx>Exact?<INPUT TYPE=SUBMIT VALUE=\"Find\">",
         gSearchName);
  printf("</TD></FORM>\r\n");

#ifdef RADICAL_LIST
  printf("<td><A HREF=\"?radlst=1\">Radicals</A></td>");
#endif
  
  printf("</tr><tr><td>\r\n"
         "\r\n");
  printf("<B>Encoding: %s.</B><BR>\r\n", gOurEncoding);
  printf("\r\n"
         "</td></tr></table>\r\n"
         "</td></tr></table>\r\n");

}

