/**********************************************************************
*   Copyright (C) 1998-1999, International Business Machines
*   Corporation and others.  All Rights Reserved.
***********************************************************************/


/*
    "Fonted Callback" plugin module for devanagari.

    Very simple support - r's and stuff aren't supported.

    See devtables.h for more information on the data.
 */

#include "unicode/devtables.h"
#include <stdio.h>
#include "unicode/utypes.h"
#include "unicode/ucnv.h"
#include "unicode/unistr.h"
#ifdef WIN32
#include <string.h>
#endif

#define kBufLen 99

#if 0
void doSyriac(const UChar **p);
#endif

extern "C"
{
#include "unicode/fonted_imp.h"
}

/* clone the converter, reset it, and then try to transcode the source into the
   target. If it fails, then transcode into the error buffer. 

   source isn't modified because this fcn is expected to deal with all of it.
   
   This would be a very useful function for other callbacks.
*/
static void convertIntoTargetOrErrChars(UConverter *_this,
					unsigned char **target,
					const char *targetLimit,
					const UChar **source,
					const UChar *sourceLimit,
					UErrorCode *err)
{
  const UChar      *sourceAlias = *source;
  UErrorCode subErr = U_ZERO_ERROR;   
  unsigned char       *myTarget;
  UConverter myConverter = *_this; /* bitwise copy */

  ucnv_reset(&myConverter); /* necessary???? */

  /*  ucnv_setFromUCallBack (&myConverter,               <-- unneeded
			 (UConverterFromUCallback)  UCNV_FROM_U_CALLBACK_DECOMPOSE,
			 &err2);*/
  
  ucnv_fromUnicode (&myConverter,
		    (char **)target,
		    targetLimit,
		    &sourceAlias,
		    sourceLimit,
		    NULL,
		    TRUE,
		    &subErr); /* pass them the real error. */
  
  if(subErr == U_INDEX_OUTOFBOUNDS_ERROR)
    {
      /* it didn't fit. */
      subErr = U_ZERO_ERROR;

      myTarget = _this->charErrorBuffer + _this->charErrorBufferLength;

      /* OK hit it */
#ifdef WIN32
      ucnv_fromUnicode(&myConverter,
		       (char**)&myTarget,
		       (const char *)_this->charErrorBuffer + UCNV_ERROR_BUFFER_LENGTH,
		       &sourceAlias,
		       sourceLimit,
		       NULL,
		       TRUE,
		       &subErr);
#else
      ucnv_fromUnicode(&myConverter,
		       (char**)&myTarget,
		       _this->charErrorBuffer + UCNV_ERROR_BUFFER_LENGTH,
		       &sourceAlias,
		       sourceLimit,
		       NULL,
		       TRUE,
		       &subErr);
#endif
      /* fix the charBufferLength */

      /* **todo: check err here! */
      _this->charErrorBufferLength = ((unsigned char *)myTarget - _this->charErrorBuffer);

      *err = U_INDEX_OUTOFBOUNDS_ERROR;
    }
}


static void copyIntoTargetOrErrChars(UConverter *_this,
					unsigned char **target,
					const unsigned char *targetLimit,
					const char *source,
				     int32_t len,
					UErrorCode *err)
{
  UErrorCode subErr = U_ZERO_ERROR;   
  unsigned char       *myTarget;
  int32_t      copied = 0;

  if(len == -1)
    len = strlen(source);

  copied = targetLimit - (*target);
  if(copied > len)
    copied = len;

  memcpy(*target, source, copied);

  source += copied;
  *target += copied;
  len -= copied;

  if(len)
    {
      memcpy(_this->charErrorBuffer + _this->charErrorBufferLength,
	     source,
	     len);
      source += len;
      _this->charErrorBufferLength += len;
      *err = U_INDEX_OUTOFBOUNDS_ERROR;
    }
}


void flushDevanagari(const UChar *buf, const int32_t *typ,
	int32_t n, unsigned char **target, const unsigned char *targetLimit, UErrorCode *err)
{
  int32_t i;
  unsigned char *str;
  for(i=0;i<n;i++)
    {
      if(*target == targetLimit)
	{
	  *err = U_INDEX_OUTOFBOUNDS_ERROR;
	  return; 
	}
      
      if(typ[i] == dv_CD)
	{
          str = convTableHalf[buf[i] & 0x7f];
	}
      else
	{
	  str = convTbl[buf[i] & 0x7f];
	}
      
      while(*str)
	{
	  if(*target == targetLimit)
	    {
	      *err = U_INDEX_OUTOFBOUNDS_ERROR;
	      return; 
	    }
	  *(*target) = *str;
	  str++;
          // fprintf(stderr, "t=%08x, ", *target);fflush(stderr);
	  *target = *target + 1;
	  //fprintf(stderr, "t=%08x\n", *target);fflush(stderr);
	  //	  (*target)++;

	}
    }
}

U_CAPI UBool
  fonted_devanagari (UConverter * _this,
					    unsigned char **target,
					    const unsigned char *targetLimit,
					    const UChar ** source,
					    const UChar * sourceLimit,
					    int32_t *offsets,
					    UBool flush,
					    UErrorCode * err)
{
  UBool inFont = FALSE;


  UChar buf[kBufLen];
  int32_t typ[kBufLen];
  int32_t n = 0;
  UBool  sawLiveConsonant = FALSE;
  unsigned char tmp[99];

  const UChar *mysrc = *source;
  UChar  p, nextp;
  UBool shouldAdvanceSource = FALSE;

  for(mysrc = (*source - 1);mysrc < sourceLimit;mysrc++)
  {
    if(mysrc == (*source - 1))
      {
        p = _this->invalidUCharBuffer[0];
        nextp = *mysrc;
      }
    else
      {
        shouldAdvanceSource = TRUE;
        p = *mysrc;
        nextp = mysrc[1];
      }
  
#if 0
    /** BROKEN _ wants to optimize spaces **/
   if((p == 0x0020)) // always..
   {
     flushDevanagari(buf,typ,n,target,targetLimit,err);
     sawLiveConsonant = FALSE;
     n = 0;
     
     WriteByte((*p)&0xFF); /* BUG - should use the right codepage */

     continue;
   }
#endif

   if((((p)&0xFF00) != 0x0900) || ((p & 0xFF80) == 0x0980) ) // non hindi
	if(p != 0x0981)
          {
            break;
          }

#if 0
	if(inFont == TRUE)
	{
		flushDevanagari(buf,typ,n);
		sawLiveConsonant = FALSE;
		n = 0;

		printf("</FONT>");
		inFont = FALSE;
	}

	if(((p)&0xFF00) == 0x0700)
	{
		doSyriac(&p); /* will advance the ptr, etc. */
		p--; /* since the loop will advance it... */
	}
	else
	{
		UChar buf[2];
		UErrorCode dontcare;
	int osrl_mode;
	
	dontcare = ZERO_ERROR;
	buf[0] = *p;
	buf[1] = 0;
	osrl_mode = srl_mode;
	srl_mode = 0;
	/* nnngggaa! horrid sphagetti code. And i'm hungry as i write this too. mmm */
	uprint(buf,stdout,&dontcare);
	srl_mode = osrl_mode;
		/* WriteByte((*p)&0xFF); */ 
	}
	continue;
   }
#endif

  if(shouldAdvanceSource == TRUE)
      (*source)++;

     // hindi.
   if(inFont == FALSE)
   {
     UnicodeString str  = "<FONT FACE=\"Xdvng,xdvng\">";
     int len = str.length();
     const UChar *chars = str.getBuffer();
#ifdef WIN32
     convertIntoTargetOrErrChars(_this,target,(const char *)targetLimit,&chars,chars+len,err);
#else
     convertIntoTargetOrErrChars(_this,target,(unsigned char*)targetLimit,&chars,chars+len,err);
#endif
     inFont = TRUE;
   }

  //sprintf(tmp,"<font face=arial>n=%d %02x</font>",n,(*p) & 0x7f); *this << tmp;

   if(p == 0x0981)
     {
       buf[n] = 0xFA;
       typ[n] = dv_C;
       n++; // one more out
     }
   else
     if(typeTable[(p) & 0x7f] &  dv_C)
       {
	 if(sawLiveConsonant) 
   	{
	  flushDevanagari(buf,typ,n,target,targetLimit,err);
	  sawLiveConsonant = FALSE;
	  n = 0;
	}
	 
	 if(nextp == dv_VIRAMA)
	   {
	     buf[n] = p;
	     typ[n] = dv_CD;
	     n++; // one more out
             (*source)++;

 	     mysrc ++; // skip the virama
	   }
	 else
	   {
	     buf[n] = p;
	     typ[n] = dv_C;
	     n++;
	     sawLiveConsonant = TRUE;
	   }
       }
     else if(p == dv_VS_LEFT)
       {
	 int32_t ii;
	 for(ii=n;ii>0;ii--)
	   {
	     buf[ii]=buf[ii-1];
	     typ[ii]=typ[ii-1];
	   }
	 buf[0] = p;
	 typ[0] =typeTable[(p) & 0x7f]; 	
	 n++;
       }
     else
       {
	 buf[n] = p;
	 typ[n] =typeTable[(p) & 0x7f]; 	
	 n++;
       }
  }   

// final flush
  flushDevanagari(buf,typ,n,target,targetLimit,err);

  if(inFont == TRUE)
  {
     UnicodeString str  = "</FONT>";
     int len = str.length();
     const UChar *chars = str.getBuffer();

#ifdef WIN32
     convertIntoTargetOrErrChars(_this,target,(const char *)targetLimit,&chars,chars+len,err);
#else
     convertIntoTargetOrErrChars(_this,target,targetLimit,&chars,chars+len,err);
#endif

     inFont = TRUE;
  }
#ifdef WIN32
  return TRUE;
#endif
}

//#include "syriac.cpp"


