#include "stdio.h"
#include "stddef.h"

/* Largest decomposition sequence is 18 chars at U+FDFA */
#define DECOMP_MAX 32

#include "utypes.h"
#include "ustring.h"

#include "ucnv.h"
#include "ucnv_bld.h"
#include "udat.h"
#include "ucal.h"
#include "uchar.h"

#include "ucol.h"

#include "decompcb.h"

UConverterFromUCallback DECOMPOSE_lastResortCallback = UCNV_FROM_U_CALLBACK_SUBSTITUTE;

static UChar block0300Subs[] =
    { 0x0060, 0x00b4, 0x005e, 0x007e, 0x007e, 0x00af, 0x0306, 0x0307, 0x00a8, 0x0309, 0x00b0, 0x0022, 0x030c, 0x0022 }; 


static UChar block0390Subs[] = 
{ 
  0x0390,
  0x0041,
  0x0042,
  0x0047,
  0x0044,
  0x0045,
  0x005A,
  0x0048,
  0x0398,
  0x0049
};

#ifdef KXI
#include "../dev/kxitbl.c"
#endif

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

  /*  ucnv_setFromUCallBack (&myConverter,               <-- unneeded
			 (UConverterFromUCallback)  UCNV_FROM_U_CALLBACK_DECOMPOSE,
			 &err2);*/
  
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





/* called for each char */
static void DECOMPOSE_uchar(UConverter * _this,
			    const UChar **source,
			    const UChar *sourceLimit,
		  char **target,
		  const char *targetLimit,
		  UChar theChar,
		  UErrorCode * err)
{
  UChar      decomposedSequence[DECOMP_MAX];
  const UChar     *tempSource, *output;
  int32_t    decomposedLen;
  UErrorCode err2 = U_ZERO_ERROR;

  bool_t     changedSomething = FALSE;  /* have we had *any* effect here? 
					   Used to exit when this fcn isn't doing
					   any good. */

  
  tempSource = &theChar;

  /* First, attempt a decompose */
  decomposedLen = u_normalize(&theChar,
			      1,
			      UCOL_DECOMP_COMPAT,
			      0,
			      decomposedSequence,
			      DECOMP_MAX,
			      &err2);

  if( (decomposedLen != 1)  ||
      u_strncmp(&theChar, decomposedSequence, 1) )
    {
      *err = U_USING_FALLBACK_ERROR;

      /* Since we changed something, we'll try doing another conversion.
         Guess what callback we use! */

      /* Q:: In fact, do we even need a new converter here? would it mess things up here to
	 simply use _this as the converter?? the state would be kinda-ok then. 
	 Need opinions! 
      */
      
      convertIntoTargetOrErrChars(_this, 
				  target,
				  targetLimit,
				  decomposedSequence,
				  decomposedSequence+decomposedLen,
				  err);

      return;

    }


  /* Try other substitutions */
  decomposedSequence[0] = theChar;
  decomposedSequence[1] = 0;
  output = decomposedSequence;


  /* the following switch statement can either:  
     1). change 'output' to a totally different string,
     2). modify the string in 'decomposedSequence', OR
     3). do nothing, in which case the SUBSTITUTE callback will be called.
  */
  
  switch(theChar & 0xFF00)
    {
    case 0x0000:
      switch(theChar)
	{
	case 0x00E6:
	  decomposedSequence[0] = 'a'; /* ASCIISM */
	  decomposedSequence[1] = 'e'; /* ASCIISM */
	  decomposedSequence[2] = 0;
	  break;
	}
      break;

    case 0x0300: /* combining diacriticals ------------------ 0300 */
      if(theChar < (0x0300 + (sizeof(block0300Subs)/sizeof(UChar))))
	{
	  decomposedSequence[0] = block0300Subs[theChar & 0x00FF];
	}
#if 0
      else if( (theChar >= 0x0391) && (theChar <= (0x0390 - 1 + (sizeof(block0390Subs)/sizeof(UChar)))))
	{ /* greek xliteration */
	  decomposedSequence[0] = block0390Subs[theChar - 0x0390];
	}
#endif
      break;
      
    case 0x2000:
      switch(theChar)
	{
	case 0x2044:
	  decomposedSequence[0] = '/'; /* ASCIISM */
	  break;
	}
      break;
      
#ifdef KXI
    case 0x2F00: /* radical forms */
      if(theChar < 0x2FD6 )
	{
	  decomposedSequence[0] = 0xFE43;
	  decomposedSequence[1] = kxitbl[((theChar & 0x00FF)*2) + 1];
	  decomposedSequence[2] = 0xFE44;
	  decomposedSequence[3] = 0x0000;
	}
      break;
#endif
    }

  if((output != decomposedSequence) ||       /* if it's a different ptr */
     ( (decomposedSequence[0] != theChar) || /* if the decomposedSeq changed*/
       (decomposedSequence[1] != 0) ) )
    {
      /* Yes! We have something different. Put it out.. */
      *err = U_USING_FALLBACK_ERROR;
      
      convertIntoTargetOrErrChars(_this, 
				  target,
				  targetLimit,
				  output,
				  output+u_strlen(output),
				  err);
      return;
    }


  /* nothing WE can do .. */
  
  (*DECOMPOSE_lastResortCallback)(_this,
				  target,
				  targetLimit,
				  source, /* source shouldn't be needed */
				  sourceLimit, /* sourcelimit doesn't matter */
				  0, /* offsets, */
				  TRUE, /* flush, */
				  err);

}

U_CAPI void 
  UCNV_FROM_U_CALLBACK_DECOMPOSE (UConverter * _this,
					    char **target,
					    const char *targetLimit,
					    const UChar ** source,
					    const UChar * sourceLimit,
					    int32_t *offsets,
					    bool_t flush,
					    UErrorCode * err)
{
  int32_t i;
  char   *oldTarget;

  if (CONVERSION_U_SUCCESS (*err))
    return;

  for(i=0;i<_this->invalidUCharLength;i++)
    {
      oldTarget = *target;

      /* 
	 TODO: [optimization]
	 if(target == targetLimit)
	    theirTarget = _this->charErrorBuffer;
	 etc.

	 Then, after calling the fcn, update the charErrorLen.

	 This way, the subfunc won't have to copy back into the charErrorLen
      */

      DECOMPOSE_uchar(_this,
		      source,
		      sourceLimit,
		      target,
		      targetLimit,
		      _this->invalidUCharBuffer[i],
		      err);

      /* TODO:
	 o  update offsets[0..(target-oldTarget)]
	 o  offsets += [target-oldTarget]

	 thank you. 
	    --american P
      */
    }


  return;
}
/*******************************************************end of borrowed code from ucnv_err.c **/






