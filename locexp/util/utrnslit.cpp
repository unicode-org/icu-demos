/*
* Copyright (c) {1999}, International Business Machines Corporation and others. All Rights Reserved.
**********************************************************************
*   Date        Name        Description
*   1999-dec-06 srl         Creation.
**********************************************************************
*/


#include <stdlib.h>
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

int32_t utrns_countAvailableIDs()
{
  return Transliterator::countAvailableIDs();
}

const char *utrns_getAvailableID(int32_t i)
{
  char *ret;
  UnicodeString n = Transliterator::getAvailableID(i);

  if(!n.length())
    {
      return NULL;
    }

  ret = (char*)malloc(n.length()+1);
  u_austrcpy(ret, n.getUChars()); /* should be extract? */
  return ret;
}

UTransliterator * 
utrns_open(const char *id)
{
  UnicodeString uid(id, "");
  UTransliterator* u = (UTransliterator*) Transliterator::createInstance(uid); /* "" */
  //   fprintf(stderr, "utrns_open(%s) => %p\n", id, u); 
  return u;
}

int32_t utrns_transliterate(UTransliterator *_this, 
                            const UChar *source,
                            int32_t srcLength,
                            UChar *result, 
                            int32_t resultLength,
                            UErrorCode *status)
{
    if(U_FAILURE(*status)) return -1;

    /*    fprintf(stderr, "utrns_transliterate(%p)\n", _this);
          fflush(stderr);*/

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
    /*    fprintf(stderr, "utrns_close(%p)\n"); fflush(stderr); */
}
