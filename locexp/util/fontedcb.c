/**********************************************************************
*   Copyright (C) 1998-1999, International Business Machines
*   Corporation and others.  All Rights Reserved.
***********************************************************************/
/*
  'fonted' callback. Uses (special) fonts to render different scripts
  in HTML..

  Steven R. Loomis <srl@monkey.sbay.org>

*/
#include <stdio.h>

#include "unicode/fontedcb.h"
#include "unicode/fonted_imp.h"

#include "unicode/uchar.h"

UConverterFromUCallback FONTED_lastResortCallback = UCNV_FROM_U_CALLBACK_SUBSTITUTE;

/*

  At present, invalidUCharLength will always be 1. 

  When a char is identified as one we can handle with a font, we then
  process: 
    #1 that char in the invalidUCharbuf
    #2 any remaining chars in that buf
    #3 as much of source as we can

  breaking when we reach an unhandleable char.


The following fcn basically just dispatches..

Note: WE ASSUME INVALID UCHAR LEN is 1
otherwise, callback chaining works strangely

Also, the underlying encoding should be latin-1

*/

U_CAPI void 
  UCNV_FROM_U_CALLBACK_FONTED (UConverter * _this,
					    char **target,
					    const char *targetLimit,
					    const UChar ** source,
					    const UChar * sourceLimit,
					    int32_t *offsets,
					    UBool flush,
					    UErrorCode * err)
{
  char *oldTarget;
  UChar u;
  UBool handled = FALSE;

#ifdef WIN32
  if (!((*err == U_INVALID_CHAR_FOUND) || (*err == U_ILLEGAL_CHAR_FOUND)))    return;
#else
  if (CONVERSION_U_SUCCESS (*err))
    return;
#endif
  /* ERROR */
  *err = U_ZERO_ERROR; /* clean slate */

  if(_this->invalidUCharLength != 1)
    {
      *target = '#';
      target++;
      return;
    }

  u  = _this->invalidUCharBuffer[0];

  /* Todo: register handlers here */
  fprintf(stderr, "_p=%04X\n", u);
  if((u >= U_FONTED_DEVANAGARI_START) && (u <= U_FONTED_DEVANAGARI_END))
    {
      fprintf(stderr, "-->D\n");
      handled = fonted_devanagari(_this, target, targetLimit, source, sourceLimit, offsets, flush, err);
    }

  else if((u >= U_FONTED_KANNADA_START) && (u <= U_FONTED_KANNADA_END))
    {
      fprintf(stderr, "-->K\n");
      handled = fonted_kannada(_this, target, targetLimit, source, sourceLimit, offsets, flush, err);
    }
      
  if(!handled)
    {
      (*FONTED_lastResortCallback)(_this,
				      target,
				      targetLimit,
				      source, /* source shouldn't be needed */
				      sourceLimit, /* sourcelimit doesn't matter */
				      offsets, /* offsets, */
				      flush, /* flush, */
				      err);
    }
}


#if 0


/* clone the converter, reset it, and then try to transcode the source into the
   target. If it fails, then transcode into the error buffer. 

   source isn't modified because this fcn is expected to deal with all of it.
   
   This would be a very useful function for other callbacks.
*/
static void convertIntoTargetOrErrChars(UConverter *_this,
					char **target,
					const char *targetLimit,
					const UChar *source,
					const UChar *sourceLimit,
					UErrorCode *err)
{
  const UChar      *sourceAlias = source;
  UErrorCode subErr = U_ZERO_ERROR;   
  char       *myTarget;
  UConverter myConverter = *_this; /* bitwise copy */

  ucnv_reset(&myConverter); /* necessary???? */

  ucnv_setFromUCallBack (&myConverter,
			 (UConverterFromUCallback)  UCNV_FROM_U_CALLBACK_SUBSTITUTE,
			 &subErr);
  
  ucnv_fromUnicode (&myConverter,
		    target,
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
      ucnv_fromUnicode(&myConverter,
		       &myTarget,
		       _this->charErrorBuffer + UCNV_ERROR_BUFFER_LENGTH,
		       &sourceAlias,
		       sourceLimit,
		       NULL,
		       TRUE,
		       &subErr);
      /* fix the charBufferLength */

      /* **todo: check err here! */
      _this->charErrorBufferLength = ((unsigned char *)myTarget - _this->charErrorBuffer);

      *err = U_INDEX_OUTOFBOUNDS_ERROR;
    }
}




U_CAPI void 
  UCNV_FROM_U_CALLBACK_NAMED (UConverter * _this,
			      char **target,
			      const char *targetLimit,
			      const UChar ** source,
			      const UChar * sourceLimit,
			      int32_t *offsets,
			      UBool flush,
			      UErrorCode * err)
{
  char *oldTarget;
  UChar u;
  UBool handled = FALSE;
  int32_t l;

  if (CONVERSION_U_SUCCESS (*err))
    return;

  for(l=0;l<_this->invalidUCharLength;l++)
    {
      char buf[100];
      UChar ubuf[100];
      UChar *p, *plimit;
      int32_t plen;
      UErrorCode status = U_ZERO_ERROR;
      
      u  = _this->invalidUCharBuffer[l];

      buf[0] = '[';

      plen = u_charName(u, U_UNICODE_CHAR_NAME, buf+1,100,&status);
      buf[plen+1] = ']';
      buf[plen+2] = 0;
      
      plen += 2;

      if(U_FAILURE(status))
	{
	  strcpy(buf, "[????]");
	  plen = 6;
	}

      u_uastrcpy(ubuf,buf);


      p = ubuf;
      plimit = p + u_strlen(p);

      convertIntoTargetOrErrChars(_this,
				  target,
				  targetLimit,
				  p,
				  plimit,
				  err);
      
      /*      fprintf(stderr, "ch->[%s - %s]\n", buf, _errorName(status)); */

    }
}
#endif
