/**********************************************************************
*   Copyright (C) 1999, International Business Machines
*   Corporation and others.  All Rights Reserved.
***********************************************************************/
#include <stdio.h>

/*

  'transliterating' callback. render using the translit framework..

  Steven R. Loomis <srl@monkey.sbay.org>

*/

#include "unicode/translitcb.h"
#include "unicode/utrans.h"
#include "unicode/uchar.h"
#include "unicode/ustring.h"

UBool TRANSLITERATED_tagWithHTML = TRUE;

UConverterFromUCallback TRANSLITERATED_lastResortCallback = UCNV_FROM_U_CALLBACK_SUBSTITUTE;

UTransliterator  **gTR;


/*    if(gTR_utf8 == NULL) */
/*    { */
/*        gTR_utf8 = ucnv_open("utf-8", &subErr); */

/*
1: Arabic-Latin
3: Devanagari-Latin
5: Greek-Latin
7: Hebrew-Latin
9: Russian-Latin
11: Halfwidth-Latin
13: Kana-Latin

*/

UTransliterator loadTranslitFromCache(int n, const char *id)
{
  UErrorCode status = U_ZERO_ERROR;
  if(!gTR[n])
    {
      gTR[n] = utrans_open(id, UTRANS_FORWARD, NULL, 0, NULL, &status);
      /* fprintf(stderr, "TR[%d:%s]=%p\n", n, id, gTR[n]);  */
    }
  
  if(!gTR[n] || U_FAILURE(status))
    {
        gTR[n] = utrans_open("Null", UTRANS_FORWARD, NULL, 0, NULL, &status);
    }
    

  return gTR[n];
}


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

/* TODO: Use the real scriptRun APIs instead of UCharBlock */
UTransliterator *getTransliteratorForScript(UBlockCode script)
{
    int i;

    if(gTR == NULL)
    {
        U_STRING_INIT(beginMark, _beginMark, 18);
        U_STRING_INIT(  endMark, _endMark, 7);

        gTR = (UTransliterator**) malloc(sizeof(UTransliterator*)*UBLOCK_COUNT);
        for(i=0;i<UBLOCK_COUNT;i++)
            gTR[i] = 0;
    }

    switch(script)
    {
    case UBLOCK_LATIN_EXTENDED_A:
        return loadTranslitFromCache((int)script, "Maltese-Latin");

    case UBLOCK_DEVANAGARI:
        return loadTranslitFromCache((int)script, "Devanagari-Latin");

    case UBLOCK_TAMIL:
        return loadTranslitFromCache((int)script, "Tamil-Latin");
        
    case UBLOCK_GREEK:
        return loadTranslitFromCache((int)script, "Greek-Latin");

    case UBLOCK_ARABIC:
        return loadTranslitFromCache((int)script, "Arabic-Latin");

    case UBLOCK_CYRILLIC:
        return loadTranslitFromCache((int)script, "Cyrillic-Latin"); /* Cyrillic != Russian, but.. */

    case UBLOCK_HEBREW:
        return loadTranslitFromCache((int)script, "Hebrew-Latin"); 

    case UBLOCK_KATAKANA:
    case UBLOCK_HIRAGANA:
        return loadTranslitFromCache((int)script, "Kana-Latin"); 

    case UBLOCK_CJK_UNIFIED_IDEOGRAPHS:
        return loadTranslitFromCache((int)script, "Kanji-English"); 

    case UBLOCK_HALFWIDTH_AND_FULLWIDTH_FORMS:
        return loadTranslitFromCache((int)script, "Halfwidth-Latin"); 


    case UBLOCK_HANGUL_JAMO:
    case UBLOCK_HANGUL_SYLLABLES:
        return loadTranslitFromCache((int)script, "Hangul-Jamo;Jamo-Latin"); 


    default:
        return loadTranslitFromCache((int)0, "Null");
        
    }
}


U_CAPI void 
  UCNV_FROM_U_CALLBACK_TRANSLITERATED  (void *context,
                                UConverterFromUnicodeArgs *fromUArgs,
                                const UChar* codeUnits,
                                int32_t length,
                                UChar32 codePoint,
                                UConverterCallbackReason reason,
                                UErrorCode *err)
{
    int32_t len;
    UErrorCode status2 = U_ZERO_ERROR;
    UTransliterator *myTrans;
    UBlockCode scriptBlock;
    int srclen;
    FromUTransliteratorContext *ctx;
    UConverter *oC = NULL;

    int n = 0;

    UChar totrans[300];
    
    ctx = (FromUTransliteratorContext*) context;
   
    if(reason > UCNV_IRREGULAR)
    {
      return;
    }
 
    *err = U_ZERO_ERROR; /* so that we get called in a loop */

    scriptBlock =  u_charScript(codePoint);
    myTrans = getTransliteratorForScript(scriptBlock);

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

    /* the <FONT> thing */
    if(ctx->html == TRUE)
      {
        const UChar *mySource;
        mySource = beginMark;
        *err = U_ZERO_ERROR;
        ucnv_cbFromUWriteUChars(fromUArgs, &mySource, mySource+u_strlen(beginMark), 0, err);
      }

    /* len = utrns_transliterate(myTrans, _this->invalidUCharBuffer, _this->invalidUCharLength, tmpbuf, 300, &status2);*/
    
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

          utrans_transUChars(myTrans, totrans,  &n, 300, 0, &len, &status2);
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

      utrans_transUChars(myTrans, totrans, &n, 300, 0, &len, &status2);
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






