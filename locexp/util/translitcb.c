/**********************************************************************
*   Copyright (C) 1999-2001, International Business Machines
*   Corporation and others.  All Rights Reserved.
***********************************************************************/
/*

  'transliterating' callback. render using the translit framework..

  New scheme:   Translate using "xx_YY-Latin" where xx_YY is a locale.
  Not caching transliterators, anymore.

  Steven R. Loomis <srl@jtcsv.com>

//        gTR[n] = utrans_open("Null", UTRANS_FORWARD, NULL, 0, NULL, &status);

*/

#include <stdio.h>
#include "unicode/translitcb.h"
#include "unicode/utrans.h"
#include "unicode/uchar.h"
#include "unicode/ustring.h"
#include "unicode/ucnv_cb.h"
#include <stdlib.h>

#define _beginMark "<FONT COLOR=green>"   /* 18 */
#define _endMark   "</FONT>"              /* 7  */

#if defined(WIN32)
#define L_beginMark "<FONT COLOR=green>"   /* 18 */
#define L_endMark   "</FONT>"              /* 7  */
#endif

#if defined(AIX)
#define L_beginMark L"<FONT COLOR=green>"   /* 18 */
#define L_endMark   L"</FONT>"              /* 7  */
#endif


U_STRING_DECL(beginMark, _beginMark, 18 );
U_STRING_DECL(  endMark, _endMark,   7);

static void TRANSLITERATED_LoadTransliteratorIfNotLoaded(FromUTransliteratorContext *ctx)
{
  UErrorCode status = U_ZERO_ERROR;
  char id[200];

  sprintf(id,"%s", ctx->locale);

  if(ctx->trans == 0)
  {
    ctx->transerr = U_ZERO_ERROR;
    ctx->trans = utrans_open(id, UTRANS_FORWARD, NULL, -1, NULL, &ctx->transerr);
    /*     fprintf(stderr, "TR[%d:%s]=%p [%s]\n", 9133, id, ctx->trans, u_errorName(status));  */
  }

  if(!ctx->trans || U_FAILURE(status))
  {
    ctx->trans = utrans_open("Null", UTRANS_FORWARD, NULL, 0, NULL, &status);
  }
}


U_CAPI void 
  UCNV_FROM_U_CALLBACK_TRANSLITERATED  (const void *context,
                                UConverterFromUnicodeArgs *fromUArgs,
                                const UChar* codeUnits,
                                int32_t length,
                                UChar32 codePoint,
                                UConverterCallbackReason reason,
                                UErrorCode *err)
{
    int32_t len;
    UErrorCode status2 = U_ZERO_ERROR;
    UBlockCode scriptBlock;
    int srclen;
    FromUTransliteratorContext *ctx;
    UConverter *oC = NULL;

    int n = 0;

    UChar totrans[300];
    
    ctx = (FromUTransliteratorContext*) context;
  
    if(reason == UCNV_RESET) 
    {
        if(ctx->utf8)
        {
            ucnv_reset(ctx->utf8);
        }
        return;
    }
    else if(reason == UCNV_CLOSE)
    {
      if(ctx->trans)
      {
        utrans_close(ctx->trans);
      }
      if(ctx->utf8)
      {
        ucnv_close(ctx->utf8);
      }
      return;
    }
    else if(reason > UCNV_IRREGULAR)
    {
      return; /* ? */
    }

    *err = U_ZERO_ERROR; /* so that we get called in a loop */

    if(ctx->locale == NULL)
    {
      /* I guess they don't want anything, yet. */
      return;
    }

    TRANSLITERATED_LoadTransliteratorIfNotLoaded(ctx);


    u_strncpy(totrans,codeUnits, length);

    n = length;

    if(fromUArgs->converter == ctx->utf8)
    {
      /* use callbacks here! */
      return;
    }

    if(ctx->utf8 == NULL)
    {
      UErrorCode u8err = U_ZERO_ERROR;

      ctx->utf8 = ucnv_open("utf-8", &u8err);
    }

    /* FATAL ERR .......... */
    if( (ctx->utf8==NULL) || (ctx->trans==NULL) )
    {
      *err = U_INTERNAL_PROGRAM_ERROR;
      return;
    }

    /* the <FONT> thing */
    if(ctx->html == TRUE)
    {
        const UChar *mySource;
        mySource = beginMark;
        *err = U_ZERO_ERROR;
        ucnv_cbFromUWriteUChars(fromUArgs, &mySource, mySource+u_strlen(beginMark), 0, err);
    }

    /* len = utrns_transliterate(ctx->trans, _this->invalidUCharBuffer, _this->invalidUCharLength, tmpbuf, 300, &status2);*/
    
    /* convertIntoTargetOrErrChars(_this, target, targetLimit, tmpbuf, tmpbuf+len, err); */

    /* Todo: check script of the rest of the invaliducharbuffer */

    /* Look for any more chars with the same script.*/
    
    while(fromUArgs->source < fromUArgs->sourceLimit)
    {
      /* TODO: UTF-16 support */
        for(srclen=0; (fromUArgs->source+srclen)<fromUArgs->sourceLimit && u_charScript( fromUArgs->source[srclen] ) == scriptBlock  ; srclen++);
       
        if(srclen > 0)
        {
            u_strncpy(totrans+n, fromUArgs->source, srclen);
            n += srclen;
        }
 
        /* If we found any, xliterate them */
        if(n > 0)
        {
          const UChar *mySource; 
          len = n;

          utrans_transUChars(ctx->trans, totrans,  &n, 300, 0, &len, &status2);
          mySource = totrans;

          oC = fromUArgs->converter;
          fromUArgs->converter = ctx->utf8;
          
          ucnv_cbFromUWriteUChars(fromUArgs,
                                  &mySource,
                                  mySource+len,
                                  0,
                                  err);

          fromUArgs->converter = oC;

          fromUArgs->source += srclen; /* if any of the actual source was found */

          n = 0; /* reset */
        }
        scriptBlock = u_charScript(fromUArgs->source[srclen]);

        break;
    }

    /* handle single char case */
    if(n > 0)
    {
      const UChar *mySource;

      len = n;
      srclen = 0;

      utrans_transUChars(ctx->trans, totrans, &n, 300, 0, &len, &status2);
      mySource = totrans;
  
      oC = fromUArgs->converter;
      fromUArgs->converter = ctx->utf8;

      ucnv_cbFromUWriteUChars(fromUArgs,
                              &mySource,
                              mySource+len,
                              0,
                              err);

      fromUArgs->converter = oC;

      fromUArgs->source += srclen; /* if any of the actual source was found */
      n = 0; /* reset */
    }
    
    if(ctx->html == TRUE)
    {
        const UChar *mySource;
        mySource = endMark;
        ucnv_cbFromUWriteUChars(fromUArgs, &mySource, mySource+u_strlen(beginMark), 0, err);
    }
}






