/**********************************************************************
*   Copyright (C) 1998-2000, International Business Machines
*   Corporation and others.  All Rights Reserved.
***********************************************************************/


/*
    "Fonted Callback" plugin module for Kannada

    Very simple support - r's and stuff aren't supported.

    See kantables.h for more information on the data.

    Apr 29, 2000 - copied from devanagari to kannada
 */

#include "unicode/kantables.h"
#include <stdio.h>
#include "unicode/utypes.h"
#include "unicode/ucnv.h"
#include "unicode/unistr.h"
#ifdef WIN32
#include <string.h>
#endif

#define kBufLen 99

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


static void writeStr(const char *str, unsigned char **target, const unsigned char *targetLimit, UErrorCode *err)
{
  int32_t i;
 
  while(*str)
    {
      if(*target == targetLimit)
        {
          *err = U_INDEX_OUTOFBOUNDS_ERROR;
          return; 
        }
 
      *(*target) = *str;
      str++;
      (*target)++;
    }
}

U_CAPI UBool
fonted_kannada (UConverter * _this,unsigned char **target,const unsigned char *targetLimit,const UChar ** source,const UChar * sourceLimit,int32_t *offsets,UBool flush,UErrorCode * err)
{
  UBool inFont = FALSE;
 
 
  UChar buf[kBufLen];
  int32_t typ[kBufLen];
  int32_t n = 0;
  UBool  sawLiveConsonant = FALSE;
  unsigned char tmp[99];
 
  const UChar *mysrc = *source;
  UChar  p;
  UBool shouldAdvanceSource = FALSE;
 
  for(mysrc = (*source - 1);mysrc < sourceLimit;mysrc++)
    {
      if(mysrc == (*source - 1))
        {
          fprintf(stderr, "snagging from IUCB\n");
          p = _this->invalidUCharBuffer[0];
        }
      else
        {
          shouldAdvanceSource = TRUE;
          p = *mysrc;
        }
      
      fprintf(stderr, "p=%04X\n", p);
      
      if( (p<U_FONTED_KANNADA_START) ||
          (p>U_FONTED_KANNADA_END)) // non kannada
        {
          if(inFont == TRUE)
            {
              UnicodeString str  = "</FONT>";
              int len = str.length();
              const UChar *chars = str.getBuffer();
#ifdef WIN32
              convertIntoTargetOrErrChars(_this,target,(const char *)targetLimit,&chars,chars+len,err);
#else
              convertIntoTargetOrErrChars(_this,target,(unsigned char*)targetLimit,&chars,chars+len,err);
#endif
              inFont = FALSE;
            }
          
          return TRUE;
        }
      

      if(shouldAdvanceSource == TRUE)
        (*source)++;
      
      if(inFont == FALSE)
        {
          UnicodeString str  = "<FONT FACE=\"Kpnews,kpnews\">";
          int len = str.length();
          const UChar *chars = str.getBuffer();
#ifdef WIN32
          convertIntoTargetOrErrChars(_this,target,(const char *)targetLimit,&chars,chars+len,err);
#else
          convertIntoTargetOrErrChars(_this,target,(unsigned char*)targetLimit,&chars,chars+len,err);
#endif
          inFont = TRUE;
        }
      
      if(kanTable[(p-0x0C80)][0])
        {
          writeStr(kanTable[(p-0x0C80)], target, targetLimit, err);
        }
      else
        {
          UnicodeString str  = "!";
          int len = str.length();
          const UChar *chars = str.getBuffer();
#ifdef WIN32
          convertIntoTargetOrErrChars(_this,target,(const char *)targetLimit,&chars,chars+len,err);
#else
          convertIntoTargetOrErrChars(_this,target,(unsigned char*)targetLimit,&chars,chars+len,err);
#endif
        }
      
      if(inFont == TRUE)
        {
          UnicodeString str  = "</FONT>";
          int len = str.length();
          const UChar *chars = str.getBuffer();
#ifdef WIN32
          convertIntoTargetOrErrChars(_this,target,(const char *)targetLimit,&chars,chars+len,err);
#else
          convertIntoTargetOrErrChars(_this,target,(unsigned char*)targetLimit,&chars,chars+len,err);
#endif
          inFont = FALSE;
        }
    } 
  return TRUE;
}


