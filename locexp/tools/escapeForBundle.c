/* 
   Copyright (C) 1999-2012, International Business Machines
   Corporation and others.  All Rights Reserved.

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
  char  cHi, cLo;

  while(!feof(stdin))
    {
      cHi = getchar();
      
      if(feof(stdin)) return 1; /* half-char */

      cLo = getchar();

      myChar = (cHi<<8) | cLo;

      if( (cHi == 0) &&  /* high byte UNset */
	  isprint(cLo) &&              /* considered printable */
	  (cLo != '"') &&              /* Not our two special chars: " and */
	  (cLo != '\\') )              /*  \ */
	{
	  putchar(cLo);
	}
      else
	{
	  printf("\\u%04X", (int) myChar);
	}
    }
    return 0;
}
