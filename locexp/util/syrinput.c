/**********************************************************************
*   Copyright (C) 1999, International Business Machines
*   Corporation and others.  All Rights Reserved.
***********************************************************************/

/*
7/15/1999
Syriac input
modelled after gabrial.sdf  [www.sil.org]
steven r. loomis

normal letters supported:
 `bgGdhwzxTyklmns'pSqrct
 we DO NOT support: 
  K Chap
  P We
  C Zhin
what are these??

Diacritics:    AaEeoui

Don't support: / ++ =   WHAT ARE THESE


*/

/**
 * Convert a Unicode string in-place 
 */
void doSyriacInput(UChar *src);

struct
{
	char in;
	UChar out[4];
} syrInputTable[] =
{
 { '`', { 0x0710, 0x0000 } },
 { 'b', { 0x0712, 0x0000 } },
 { 'g', { 0x0713, 0x0000 } },
 { 'G', { 0x0713, 0x0330, 0x0000 } },
 { 'd', { 0x0715, 0x0000 } },
 { 'h', { 0x0717, 0x0000 } },
 { 'w', { 0x0718, 0x0000 } },
 { 'z', { 0x0719, 0x0000 } },
 { 'x', { 0x071a, 0x0000 } },
 { 'T', { 0x071b, 0x0000 } },
 { 'y', { 0x071d, 0x0000 } },
 { 'k', { 0x071f, 0x0000 } },
 /* { 'K', { 0x0710, 0x0000 } },  Chap?? */
 { 'l', { 0x0720, 0x0000 } },
 { 'm', { 0x0721, 0x0000 } },
 { 'n', { 0x0722, 0x0000 } },
 { 's', { 0x0723, 0x0000 } },
 { '\'', { 0x0725, 0x0000 } },
 { 'p', { 0x0726, 0x0000 } },
 /* { 'P', { 0x07, 0x0000 } }, We?? */
 { 'S', { 0x0728, 0x0000 } },
 { 'q', { 0x0729, 0x0000 } },
 { 'r', { 0x072a, 0x0000 } },
 { 'c', { 0x072b, 0x0000 } },
 /*{ 'C', { 0x07??, 0x0000 } },  Zhin?? */
 { 't', { 0x072c, 0x0000 } },

/* now, diacritics */

 { 'A', { 0x0735, 0x0000 } },
 { 'a', { 0x0732, 0x0000 } },
 { 'E', { 0x0739, 0x0000 } },
 { 'e', { 0x0738, 0x0000 } },
 { 'o', { 0x073f, 0x0000 } },
 { 'u', { 0x0742, 0x0000 } },
 { 'i', { 0x071d, 0x0742, 0x0000 } },

/* srl addition */
 { 'v', { 0x0712, 0x0742, 0x0000 } },

 { 0, {0x0000} }

};

void doSyriacInput(UChar *src)
{
	UChar temp[999];
	UChar *p, *q, *s;
	int  i;

	u_strcpy(temp,src);
	q = src;

	for(p=temp;*p;p++)
	{
		for(i=0;syrInputTable[i].in != 0;i++)
		{
			if(syrInputTable[i].in == *p) /* ASCIISM */
			{
				s = syrInputTable[i].out; /* copy the out to the end of q */
				while(*s)
				{
					*(q++) = *(s++);	
				}
				break;
			}
		}

		if(syrInputTable[i].in == 0)
			*(q++) = *p;
								
	}

	*q = 0; /* terminate at end */
}
