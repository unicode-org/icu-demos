/**********************************************************************
*   Copyright (C) 1999, International Business Machines
*   Corporation and others.  All Rights Reserved.
***********************************************************************/
/* 7/15/1999 srl */

/*
    Code for converting from Unicode [preliminary Syriac mapping] to
    a font named 'Gabrial Ator' [from www.sil.org] for HTML-based Syriac
    [Assyrian] text output.
 */

#include "unicode/syrtables.h"
#include <stdio.h>
#include <string.h>

#define kBufLen 99

/* look up a char in the Assyrian table */
char syrLookupChar(UChar uch)
{
  for(int i=0;AssyrianBasic[i].uch;i++)
    {
      if(AssyrianBasic[i].uch == uch)
	return AssyrianBasic[i].ch;
    }
  
  return 0xAE; /* substitute char */
}

/* print out the 8 bit string in reverse form. Should probably do the FONT emitting here. */
void syrRevWrite(const char *c)
{
  if(*c == 0)
    return;

  const char *q;
	  printf("<FONT FACE=\"Gabrial Ator\">");
  for(q= (c + strlen(c) - 1); q >= c; q--)
    putchar(*q);

	printf("</FONT>");
}


/* we must update the parentLoc to point to the first non-syriac text once we find it. */
/* accumulate 8bits into 'tmp', then reverse it and output it. 
   NOTE: Diacriticals must PRECEDE what they modify, due to the font. This is OK,
   they will FOLLOW them in tmp, but it will be OK when tmp is reversed.
*/
void doSyriac(const UChar **parentLoc)
{
  bool_t inFont = FALSE;
  
  const UChar* c = *parentLoc;
  const UChar* p;
  int32_t n = 0;
  bool_t  sawLiveConsonant = FALSE;
  char tmp[300];
  
  tmp[0] = 0;
  c = *parentLoc;
  inFont = FALSE;

  char *out = tmp;
  
  for(p = c;*p;p++)
    {
      if((*p == 0x0020)) // always..
	{
	  *(out++) = ' ';
	  continue;
	}

	/* nota bene: U+0330 and other diacritics aren't a problem here. If we see
	   them from this following line, it means that the proper processing code
 	   didn't process them, and thus it's OK to flush.. */
      if(((*p)&0xFF00) != 0x0700)
	{
	  if(inFont == TRUE)
	    {
	      *out = 0;
	      syrRevWrite(tmp);
	      inFont = FALSE;
	      *parentLoc = p;
	      return; /**************** BREAK OUT ON NON SYRIAC TEXT **********/
	    }
	}
      
      if(inFont == FALSE)
	{
	  inFont = TRUE;
	}

      *(out) = syrLookupChar(*p);

      /* now do some simple diacrit cases */
      switch(p[1])
	{
	case 0x0330: /* tilde below */
	  {
	    switch(p[0])
	      {
		case 0x0713: *out = 0x47; p++; break; /* gamal -> jamal */
	      }
	  }
	  break;

	case 0x073f: /* dot above */
	  {
	    switch(p[0])
	      {
	      case 0x0718: *out = 0x4d; p++; break; /* waw */
	      }
	  }
	  break;

	case 0x0742: /* rukkakha  - dot below*/
	  {
	    switch(p[0])
	      {
	      case 0x0718: *out = 0x4e; p++; break; /* waw */
	      case 0x071D: *out = 0x53; p++; break; /* Yodh */
	      default: out++; *out = 0x77; p++; break; /* teth */
	      }
	  }
	  break;
	}


      if((p[0] == 0x0710) && ((p[1] & 0xFF00) != 0x0700))
	*out = 0x43; /* terminal alaph */

      out++;
    }
  
  if(out != tmp)
    {
      *out = 0;
      syrRevWrite(tmp);

      if(inFont == TRUE)
	{
	  /* printf("</FONT>"); */
	}

      inFont = FALSE;
      *parentLoc = p;
      return; /**************** BREAK OUT ON NON SYRIAC TEXT **********/
    }
}

