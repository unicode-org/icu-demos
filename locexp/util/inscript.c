/**********************************************************************
*   Copyright (C) 1999, International Business Machines
*   Corporation and others.  All Rights Reserved.
***********************************************************************/

/* 
   A rough approximation of an 'INSCRIPT' input table for Hindi input

   Again, the table is NOT complete.
*/

#include "unicode/utypes.h"
#include "unicode/ustring.h"

extern UChar ktbl[];

/*
  Given Unicode characters in INSCRIPT in, convert to Unicode devanagari
*/

void doInscript(UChar *src)
{
  while(*src)
  {
	if((*src) < 0x100)
	{
		if(ktbl[(*src)] != 0xFFFD)
			*src = ktbl[*src];
	}
	src++;
  }
}

UChar ktbl[]
= {

	0xFFFD,  /*    00 */
	0xFFFD,  /*    01 */
	0xFFFD,  /*    02 */
	0xFFFD,  /*    03 */
	0xFFFD,  /*    04 */
	0xFFFD,  /*    05 */
	0xFFFD,  /*    06 */
	0xFFFD,  /*    07 */
	0xFFFD,  /*    08 */
	0xFFFD,  /*    09 */
	0xFFFD,  /*    0a */
	0xFFFD,  /*    0b */
	0xFFFD,  /*    0c */
	0xFFFD,  /*    0d */
	0xFFFD,  /*    0e */
	0xFFFD,  /*    0f */
	0xFFFD,  /*    10 */
	0xFFFD,  /*    11 */
	0xFFFD,  /*    12 */
	0xFFFD,  /*    13 */
	0xFFFD,  /*    14 */
	0xFFFD,  /*    15 */
	0xFFFD,  /*    16 */
	0xFFFD,  /*    17 */
	0xFFFD,  /*    18 */
	0xFFFD,  /*    19 */
	0xFFFD,  /*    1a */
	0xFFFD,  /*    1b */
	0xFFFD,  /*    1c */
	0xFFFD,  /*    1d */
	0xFFFD,  /*    1e */
	0xFFFD,  /*    1f */
	0xFFFD,  /*   20 */
	0xFFFD,  /* ! 21 */
	0xFFFD,  /* " 22 */
	0xFFFD,  /* # 23 */
	0xFFFD,  /* $ 24 */
	0xFFFD,  /* % 25 */
	0xFFFD,  /* & 26 */
	0x091F,  /* ' 27 */
	0xFFFD,  /* ( 28 */
	0xFFFD,  /* ) 29 */
	0xFFFD,  /* * 2a */
	0xFFFD,  /* + 2b */
	0xFFFD,  /* , 2c */
	0xFFFD,  /* - 2d */
	0xFFFD,  /* . 2e */
	0xFFFD,  /* / 2f */
	0xFFFD,  /* 0 30 */
	0xFFFD,  /* 1 31 */
	0xFFFD,  /* 2 32 */
	0xFFFD,  /* 3 33 */
	0xFFFD,  /* 4 34 */
	0xFFFD,  /* 5 35 */
	0xFFFD,  /* 6 36 */
	0xFFFD,  /* 7 37 */
	0xFFFD,  /* 8 38 */
	0xFFFD,  /* 9 39 */
	0xFFFD,  /* : 3a */
	0xFFFD,  /* ; 3b */
	0xFFFD,  /* < 3c */
	0xFFFD,  /* = 3d */
	0x0964,  /* > 3e */
	0xFFFD,  /* ? 3f */
	0xFFFD,  /* @ 40 */
	0xFFFD,  /* A 41 */
	0xFFFD,  /* B 42 */
	0xFFFD,  /* C 43 */
	0x0905,  /* D 44 */
	0xFFFD,  /* E 45 */
	0xFFFD,  /* F 46 */
	0xFFFD,  /* G 47 */
	0xFFFD,  /* H 48 */
	0xFFFD,  /* I 49 */
	0xFFFD,  /* J 4a */
	0xFFFD,  /* K 4b */
	0xFFFD,  /* L 4c */
	0xFFFD,  /* M 4d */
	0xFFFD,  /* N 4e */
	0xFFFD,  /* O 4f */
	0xFFFD,  /* P 50 */
	0xFFFD,  /* Q 51 */
	0xFFFD,  /* R 52 */
	0xFFFD,  /* S 53 */
	0xFFFD,  /* T 54 */
	0xFFFD,  /* U 55 */
	0xFFFD,  /* V 56 */
	0xFFFD,  /* W 57 */
	0xFFFD,  /* X 58 */
	0xFFFD,  /* Y 59 */
	0xFFFD,  /* Z 5a */
	0xFFFD,  /* [ 5b */
	0xFFFD,  /* \ 5c */
	0xFFFD,  /* ] 5d */
	0xFFFD,  /* ^ 5e */
	0xFFFD,  /* _ 5f */
	0xFFFD,  /* ` 60 */
	0x094b,  /* a 61 */
	0x0935,  /* b 62 */
	0x092e,  /* c 63 */
	0x094D,  /* d 64 */
	0xFFFD,  /* e 65 */
	0x093F,  /* f 66 */
	0x0941,  /* g 67 */
	0x092A,  /* h 68 */
	0xFFFD,  /* i 69 */
	0x0930,  /* j 6a */
	0x0915,  /* k 6b */
	0x0924,  /* l 6c */
	0x0938,  /* m 6d */
	0xFFFD,  /* n 6e */
	0x0926,  /* o 6f */
	0xFFFD,  /* p 70 */
	0xFFFD,  /* q 71 */
	0x0940,  /* r 72 */
	0x0947,  /* s 73 */
	0xFFFD,  /* t 74 */
	0x0939,  /* u 75 */
	0x0928,  /* v 76 */
	0x0948,  /* w 77 */
	0xFFFD,  /* x 78 */
	0xFFFD,  /* y 79 */
	0xFFFD,  /* z 7a */
	0xFFFD,  /* { 7b */
	0xFFFD,  /* | 7c */
	0xFFFD,  /* } 7d */
	0xFFFD,  /* ~ 7e */
};
