/*
* Copyright (c) {1999}, International Business Machines Corporation and others. All Rights Reserved.
**********************************************************************
*   Date        Name        Description
*   1999-dec-06 srl         Creation.
**********************************************************************
*/


#include "unicode/utypes.h"
#include "unicode/utrnslit.h"
#include "unicode/translit.h"
#include "unicode/rbt.h"
/* 
#include "cpputils.h"
*/


/***************
 *
 *  This is what I get for trying to put a C wrapper in some backwater 
 *  demo program. Heh
 */

/* ripped off from CPPUTILS.H */
U_CAPI void U_EXPORT2 T_fillOutputParams(const UnicodeString* temp,
				       UChar* result, 
				       const int32_t resultLength,
				       int32_t* resultLengthOut, 
				       UErrorCode* status);

/********** end ripoff */

#include <stdio.h>
#include <unicode/ustring.h>

UTransliterator * 
utrns_open(const char *id)
{
    int c = Transliterator::countAvailableIDs();
    int i;
    
      for(i=0;i<c;i++)
      {
          UnicodeString n = Transliterator::getAvailableID(i);
          char ch[200];
      
          u_austrcpy(ch, n.getUChars());
          fprintf(stderr, "%d: %s\n", i, ch);
          fflush(stderr);
      }
    
    return (UTransliterator*) Transliterator::createInstance(UnicodeString(id, ""));
}

int32_t utrns_transliterate(UTransliterator *_this, 
                            const UChar *source,
                            int32_t srcLength,
                            UChar *result, 
                            int32_t resultLength,
                            UErrorCode *status)
{
    if(U_FAILURE(*status)) return -1;

    int32_t actLen;

    UnicodeString res(result, 0, resultLength);
    res.replace(0, srcLength, source, 0, srcLength);    

//    UnicodeString res = UnicodeString(FALSE, (UChar*)source, srcLength);


    ((Transliterator*)_this)->transliterate(res,0,srcLength);
/*
    ((Transliterator*)_this)->transliterate(UnicodeString(FALSE, (UChar*)source, srcLength),
                                            0, srcLength,
                                            res);
*/
    T_fillOutputParams(&res, result, resultLength, &actLen, status);

    return actLen;
}


/**
 * Close a transliterator object
 */
U_CAPI
 void 
utrns_close(UTransliterator *t)
{
    if(t) 
       delete (Transliterator*)t;
}
