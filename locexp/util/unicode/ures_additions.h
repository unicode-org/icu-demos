/**********************************************************************
*   Copyright (C) 1999, International Business Machines
*   Corporation and others.  All Rights Reserved.
***********************************************************************/

/*
  srl's Pet Peeves.  Do not feed the animals. :)

  A collection of functions that wish they were in the ICU's api,
  but aren't.

  (Just because they want to is no reason to bloat the ICU.)

*/

#ifndef URES_ADDITIONS_H
#define URES_ADDITIONS_H

#include "unicode/utypes.h"
#include "unicode/uloc.h"
#include "unicode/ures.h"
#include "unicode/unum.h"

/**
* Counts the numbers of rows and columns in the specified 2d array
*@param resourceBundle: resourceBundle containing the desired string
*@param resourceTag: key tagging the desired array
*@param rowCount: receives the number of rows at return
*@param colCount: receives the number of columns at return
*@param status: fills in the outgoing error code
*                could be <TT>MISSING_RESOURCE_ERROR</T> if the key is not found
*                could be a non-failing error 
*                e.g.: <TT>USING_FALLBACK_ERROR</TT>,<TT>USING_DEFAULT_ERROR </TT>
*@see ures_get2dArrayItem
*/

U_CAPI void ures_count2dArrayItems(const UResourceBundle *resourceBundle,
				 const char * resourceTag,
				 int32_t *rowCount,
				 int32_t *colCount,
				 UErrorCode* status);

/**
 * Access the nth tag of a tagged array item
 *@param resourceBundle: resource bundle containing the desired strings
 *@param resourceKey: key tagging the resource
 *@param err: fills in the outgoing error code
 *                could be <TT>MISSING_RESOURCE_ERROR</T> if the key is not found
 *                could be a non-failing error 
 *                e.g.: <TT>USING_FALLBACK_ERROR</TT>,<TT>USING_DEFAULT_ERROR </TT>
 *@return pointer to the tag if found, 0 if not found. Client owns the chars.
 *@see ures_get
 *@see ures_getArrayItem
 *@see ures_getTaggedArrayItem
 *@see ures_get2dArrayItem
 */

U_CAPI const char* U_EXPORT2 ures_getTaggedArrayTag(const UResourceBundle *resourceBundle,
						       const char *resourceTag, 
						       int32_t index,
						       UErrorCode* status);


#endif
