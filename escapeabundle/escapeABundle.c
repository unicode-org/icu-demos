/**********************************************************************
*   Copyright (C) 1999-2000, International Business Machines
*   Corporation and others.  All Rights Reserved.
***********************************************************************/
/*
   This little program takes utf16 input and outputs 
   escaped text suitable for inclusion in a resource bundle.

   Use it like this:

    uconv -f <srccodepage> -t utf16_be | escapeForBundle >> myResource.txt
*/

#include <stdio.h>
#include <ctype.h>

#ifdef WIN32
#include <stdlib.h>
#include <fcntl.h>
#include <io.h>
#endif

typedef unsigned short UChar;


int main()
{
  UChar myChar;
  unsigned char  cHi, cLo;
  unsigned char *cH, *cL;

#ifdef WIN32
  if( setmode( fileno ( stdin ), O_BINARY ) == -1 ) {
          perror ( "Cannot set stdin to binary mode" );
          exit(-1);
  }
  if( setmode( fileno ( stdout ), O_BINARY ) == -1 ) {
          perror ( "Cannot set stdout to binary mode" );
          exit(-1);
  }

#endif

  cHi = getchar();
  
  if(feof(stdin)) return 0; /* half-char */

  cLo = getchar();

  myChar = (cHi<<8) | cLo;

  if (myChar == 0xFEFF) {
    cH = &cHi;
    cL = &cLo;
  } else if (myChar == 0xFFFE) {
    cH = &cLo;
    cL = &cHi;
  }

  while(!feof(stdin))
    {
      *cH = getchar();
      
      if(feof(stdin)) return 0; /* half-char */

      *cL = getchar();

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
      else if (myChar != 0xFEFF)
        printf("\\u%04X", (int) myChar);
    }
    }
	return 0;
}
