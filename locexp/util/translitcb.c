/**********************************************************************
*   Copyright (C) 1999, International Business Machines
*   Corporation and others.  All Rights Reserved.
***********************************************************************/
#include <stdio.h>

/*

  'transliterating' callback. render using the translit framework..

  Steven R. Loomis <srl@monkey.sbay.org>

*/

#include "translitcb.h"
#include "utrnslit.h"
#include "uchar.h"
#include "ustring.h"

UConverterFromUCallback TRANSLITERATED_lastResortCallback = UCNV_FROM_U_CALLBACK_SUBSTITUTE;

UTransliterator  **gTR;

/* clone the converter, reset it, and then try to transcode the source into the
   target. If it fails, then transcode into the error buffer. 

   source isn't modified because this fcn is expected to deal with all of it.
   
   This would be a very useful function for other callbacks. Needs some more
   design,
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
    if(!gTR[n])
    {
        gTR[n] = utrns_open(id);
    }
    
    return gTR[n];
}

#define _beginMark "<FONT COLOR=green>"   /* 18 */
#define _endMark   "</FONT>"              /* 7  */

U_STRING_DECL(beginMark, _beginMark, 18 );
U_STRING_DECL(  endMark, _endMark,   7);


UTransliterator *getTransliteratorForScript(UCharScript script)
{
    int i;

    if(gTR == NULL)
    {
        U_STRING_INIT(beginMark, _beginMark, 18);
        U_STRING_INIT(  endMark, _endMark, 7);

        gTR = malloc(sizeof(UTransliterator*)*U_CHAR_SCRIPT_COUNT);
        for(i=0;i<U_CHAR_SCRIPT_COUNT;i++)
            gTR[i] = 0;
    }

    switch(script)
    {
    case U_DEVANAGARI:
        return loadTranslitFromCache((int)script, "Devanagari-Latin");
        
    case U_GREEK:
        return loadTranslitFromCache((int)script, "Greek-Latin");

    case U_ARABIC:
        return loadTranslitFromCache((int)script, "Arabic-Latin");

    case U_CYRILLIC:
        return loadTranslitFromCache((int)script, "Russian-Latin"); /* Cyrillic != Russian, but.. */

    case U_HEBREW:
        return loadTranslitFromCache((int)script, "Hebrew-Latin"); 

    case U_KATAKANA:
    case U_HIRAGANA:
        return loadTranslitFromCache((int)script, "Kana-Latin"); 

    case U_CJK_UNIFIED_IDEOGRAPHS:
        return loadTranslitFromCache((int)script, "SChinese-Latin"); 

    case U_HALFWIDTH_AND_FULLWIDTH_FORMS:
        return loadTranslitFromCache((int)script, "Halfwidth-Latin"); 


    default:
        return loadTranslitFromCache((int)0, "Unicode-Hex");
        
    }
}


U_CAPI void 
  UCNV_FROM_U_CALLBACK_TRANSLITERATED (UConverter * _this,
					    char **target,
					    const char *targetLimit,
					    const UChar ** source,
					    const UChar * sourceLimit,
					    int32_t *offsets,
					    bool_t flush,
					    UErrorCode * err)
{
    int32_t len;
    UErrorCode status2 = U_ZERO_ERROR;
    int32_t i;
    UTransliterator *myTrans;
    UCharScript script;
    int srclen;

    UChar tmpbuf[300];
    

    if (CONVERSION_U_SUCCESS (*err))
        return;

    *err = U_ZERO_ERROR; /* so that we get called in a loop */

    script =  u_charScript(_this->invalidUCharBuffer[0]);
    myTrans = getTransliteratorForScript(script);

    convertIntoTargetOrErrChars(_this, target, targetLimit, beginMark, beginMark+u_strlen(beginMark), err); 

    len = utrns_transliterate(myTrans, _this->invalidUCharBuffer, _this->invalidUCharLength, tmpbuf, 300, &status2);
    
    convertIntoTargetOrErrChars(_this, target, targetLimit, tmpbuf, tmpbuf+len, err);

    /* Todo: check script of the rest of the invaliducharbuffer */

    /* Look for any more chars with the same script.*/
    
    while((*source) < sourceLimit)
    {
        for(srclen=0; ((*source)+srclen)<sourceLimit && u_charScript( (*source)[srclen] ) == script  ; srclen++);
        
        /* If we found any, xliterate them */
        if(srclen > 0)
        {
            len = utrns_transliterate(myTrans, *source, srclen, tmpbuf, 300, &status2);
            convertIntoTargetOrErrChars(_this, target, targetLimit, tmpbuf, tmpbuf+len, err);
            (*source) += srclen;
        }
        script = u_charScript((*source)[srclen]);

        break;
    }

    convertIntoTargetOrErrChars(_this, target, targetLimit, endMark, endMark+u_strlen(endMark), err); 

}





