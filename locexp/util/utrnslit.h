/*
* Copyright (c) {1999}, International Business Machines Corporation and others. All Rights Reserved.
**********************************************************************
*   Date         Name        Description
*   1999-dec-06  srl         Creation.
**********************************************************************
*/

/**

A quick C interface to the transliterator.

TODO's:

0. DOCUMENT

[from Helena]
1. utrns_open should also take an error code.
2. Like Alan suggested, utrns_openRules(const char* ID, const char* rules,
UErrorCode *status);
3. utrns_getDisplayName().
4. getAvailable() and countAvailable().  Alan, is there an easy way to do
this in the current implementation?
5. getRules().


*/

#ifndef UTRNSLIT_H
#define UTRNSLIT_H

#include "utypes.h"

typedef void* UTransliterator;

/**
 * open a named transliterator
 * @param id name of this transliterator
 * @returns The new transliterator, or NULL if it didnt work. 
 */
U_CAPI 
UTransliterator * utrns_open(const char *id);

/**
 * the main transliteration fcn
 */

U_CAPI
int32_t utrns_transliterate(UTransliterator *t, const UChar *source, int32_t limit, UChar *result, int32_t resultLength, UErrorCode *status);


/**
 * Close a transliterator object
 */
U_CAPI
 void 
utrns_close(UTransliterator *t);

#endif





