/**********************************************************************
*   Copyright (C) 1999, International Business Machines
*   Corporation and others.  All Rights Reserved.
***********************************************************************/
/*
   This little program takes escaped text and converts it to utf16.
   Endianess depends on platform!
*/

#include <stdio.h>
#include <ctype.h>

#ifdef WIN32
#include <stdlib.h>
#include <fcntl.h>
#include <io.h>
#endif

typedef unsigned short UChar;
static void uni_out(unsigned char cHi, unsigned char cLo);


int main(int argc, char *argv[])
{
  UChar myChar;
  unsigned char  cHi, cLo, c, esc ;
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
  cH = &cLo;
  cL = &cHi;
  uni_out(0xFF, 0xFE);
#else
  cH = &cHi;
  cL = &cLo;
  uni_out(0xFE, 0xFF);
#endif

  while(!feof(stdin)) {
      c = getchar();
      if(c=='\\') { /* escaped stuff */
          esc = getchar();
          if(esc=='u') { /*actual unicode char*/
            scanf("%04X", &myChar);
            *cH = (myChar>>8)&0xFF;
            *cL = myChar&0xFF;
            uni_out(cHi,cLo);
          } else { /*something else*/
              *cH = 0;
              *cL = '\\';
              uni_out(cHi, cLo);
              *cH = 0;
              *cL = esc;
              uni_out(cHi,cLo);
          }
      } else {
          if(c!=0xFF) {
              *cH = 0;
              *cL = c;
              uni_out(cHi,cLo);
          }
      }
  }
}

static void uni_out(unsigned char cHi, unsigned char cLo) {
    putchar(cHi);
    putchar(cLo);
}