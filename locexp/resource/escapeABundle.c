/**********************************************************************
*   Copyright (C) 1999, International Business Machines
*   Corporation and others.  All Rights Reserved.
***********************************************************************/
/*
   This little program takes utf16_be input and outputs 
   escaped text suitable for inclusion in a resource bundle.

   Use it like this:

    uconv -f <srccodepage> -t utf16_be | escapeForBundle >> myResource.txt
*/

#include <stdio.h>
#include <ctype.h>

typedef unsigned short UChar;


int main()
{
  UChar myChar;
  unsigned char  cHi, cLo;

  while(!feof(stdin))
    {
      cHi = getchar();
      
      if(feof(stdin)) return 0; /* half-char */

      cLo = getchar();

      myChar = (cHi<<8) | cLo;

      if( (cHi == 0) &&  /* high byte UNset */
	  (isprint(cLo) ||isspace(cLo)) )              /* considered printable */
	{
	  putchar(cLo);
	}
      else
	{
        if(myChar == 0xFFFF)
	    printf("// FFFF\n");
	  else
	    printf("\\u%04X", (int) myChar);
	}
    }
	return 0;
}
