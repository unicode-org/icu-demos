/**********************************************************************
*   Copyright (C) 1999, International Business Machines
*   Corporation and others.  All Rights Reserved.
***********************************************************************/

#include "stdio.h"
#include "stddef.h"

/* Largest decomposition sequence is 18 chars at U+FDFA */
#define DECOMP_MAX 32

#include "unicode/utypes.h"
#include "unicode/ustring.h"

#include "unicode/ucnv.h"
#include "unicode/udat.h"
#include "unicode/ucal.h"
#include "unicode/uchar.h"

#include "unicode/ucol.h"

#include "unicode/decompcb.h"
#include "unicode/ucnv_cb.h"

#define HAVE_KANGXI
#include "unicode/kangxi.h"

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

/* called for each char */
static void DECOMPOSE_uchar (void *context,
                             UConverterFromUnicodeArgs *fromUArgs,
                             const UChar* codeUnits,
                             int32_t length,
                             UErrorCode *err)
{
  UChar      decomposedSequence[DECOMP_MAX];
  const UChar    *output;
  int32_t    decomposedLen;
  UErrorCode err2 = U_ZERO_ERROR;

  FromUDecomposeContext *ctx = (FromUDecomposeContext*)context;

  bool_t     changedSomething = FALSE;  /* have we had *any* effect here? 
					   Used to exit when this fcn isn't doing
					   any good. */

  const UChar *mySource;

  UConverterFromUCallback oldCallback = NULL;
  void *oldContext                    = NULL;
  UConverterFromUCallback junkCallback = NULL;
  void *junkContext                    = NULL;

  
  /* First, attempt a decompose */
  decomposedLen = u_normalize(codeUnits,
			      length,
			      UCOL_DECOMP_COMPAT,
			      0,
			      decomposedSequence,
			      DECOMP_MAX,
			      &err2);

  if( (decomposedLen != length)  ||
      u_strncmp(codeUnits, decomposedSequence, length) )
  {
    /* Since we changed something, we'll try doing another conversion.
       Guess what callback we use! */
    
    /* Q:: In fact, do we even need a new converter here? would it mess things up here to
       simply use _this as the converter?? the state would be kinda-ok then. 
       Need opinions! 
    */
    const UChar *mySource;

    *err = U_USING_FALLBACK_ERROR;
    mySource = decomposedSequence;
    /* Nota Bene! we're going to recurse. Hang on tight. :) */

    ucnv_cbFromUWriteUChars(fromUArgs, 
                            &mySource,
                            mySource+decomposedLen,
                            0,
                            err);

    /* Put it back */
    if(oldCallback != NULL)
    {
      ucnv_setFromUCallBack(fromUArgs->converter,
                            oldCallback,
                            oldContext,
                            &junkCallback,
                            &junkContext,
                            &err2);
    }

    return;
  }


  /* Try other substitutions */
  u_strncpy(decomposedSequence, codeUnits, length);
  decomposedSequence[length] = 0;
  output = decomposedSequence;

  /* the following switch statement can either:  
     1). change 'output' to a totally different string,
     2). modify the string in 'decomposedSequence', OR
     3). do nothing, in which case the SUBSTITUTE callback will be called.
  */
  if(length == 1) /* we don't handle non BMP chars yet... */
  {
    UChar theChar = decomposedSequence[0];
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
	  UErrorCode setStatus = U_ZERO_ERROR; /* must use a different error - the INVALID_CHAR will prevent setFromUCallBack from running. */
	  decomposedSequence[0] = block0300Subs[theChar & 0x00FF];
	  
	  /* Prevent a loop from [for example] U+0308 -> U+00A8 -> U+0308...*/
          
          ucnv_setFromUCallBack(fromUArgs->converter,
                                ctx->subCallback,
                                ctx->subContext,
                                &oldCallback,
                                &oldContext,
                                &setStatus);
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
      
#ifdef HAVE_KANGXI
    case 0x2F00: /* radical forms */
      if(theChar < 0x2FD6 )
	{
	  decomposedSequence[0] = 0xFE43;
	  decomposedSequence[1] = gKangXiRadicalTable[((theChar & 0x00FF)*2) + 1];
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
        
        mySource = output;
        /* Nota Bene! we're likely to recurse. Hang on tight. :) */
        ucnv_cbFromUWriteUChars(fromUArgs, 
                                &mySource,
                                mySource+decomposedLen,
                                0,
                                err);
        
        if(oldCallback != NULL)
          {
            ucnv_setFromUCallBack(fromUArgs->converter,
                                  oldCallback,
                                  oldContext,
                                  &junkCallback,
                                  &junkContext,
                                  &err2);
          }
        
        return;
      }
  }
  else
  {
    /* non bmp .... handle here! */
  }

  
  /* nothing WE can do ship it out.... */
  if(oldCallback == NULL)
  {
    ucnv_setFromUCallBack(fromUArgs->converter,
                          ctx->subCallback,
                          ctx->subContext,
                          &oldCallback,
                          &oldContext,
                          &err2);
  }

  /* write it out . */
  {
    const UChar *mySource;
    mySource = output;
    *err = U_ZERO_ERROR;
    /* Nota Bene! we're likely to recurse. Hang on tight. :) */
    ucnv_cbFromUWriteUChars(fromUArgs, 
                            &mySource,
                            mySource+u_strlen(output),
                            0,
                            err);
  }
  
  /* clean up */

  ucnv_setFromUCallBack(fromUArgs->converter,
                        oldCallback,
                        oldContext,
                        &junkCallback,
                        &junkContext,
                        &err2);
}

U_CAPI void 
UCNV_FROM_U_CALLBACK_DECOMPOSE  (void *context,
                                 UConverterFromUnicodeArgs *fromUArgs,
                                 const UChar* codeUnits,
                                 int32_t length,
                                 UChar32 codePoint,
                                 UConverterCallbackReason reason,
                                 UErrorCode *err)
{

  if((reason > UCNV_IRREGULAR) || (length <= 0))
  {
    return;
  }

  
  DECOMPOSE_uchar(context,
                  fromUArgs,
                  codeUnits,
                  length,
                  err);

  return;
}
/*******************************************************end of borrowed code from ucnv_err.c **/






