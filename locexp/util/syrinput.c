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

#include "unicode/syrinput.h"

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

