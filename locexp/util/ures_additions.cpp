/**********************************************************************
*   Copyright (C) 1999, International Business Machines
*   Corporation and others.  All Rights Reserved.
***********************************************************************/
#include "unicode/ures_additions.h"
#include "unicode/resbund.h"
#include "unicode/ustring.h"
#include "unicode/decimfmt.h"

U_CAPI void ures_count2dArrayItems(const UResourceBundle *resourceBundle,
				 const char * resourceTag,
				 int32_t *rowCount,
				 int32_t *columnCount,
				 UErrorCode* status)

{
  if ( U_FAILURE(*status)) return;
  if (!resourceBundle || !resourceTag || (rowCount == 0) || (columnCount == 0) )
    {
      *status = U_ILLEGAL_ARGUMENT_ERROR;
      return;
    }
  
  /* ignore result */
  ((ResourceBundle*)resourceBundle)-> get2dArray(resourceTag,
			      *rowCount,
			      *columnCount,
			      *status);
}

U_CAPI const char* ures_getTaggedArrayTag(const UResourceBundle *resourceBundle,
						       const char *resourceTag, 
						       int32_t index,
						       UErrorCode* status)
{
  UnicodeString *itemTags;
  UnicodeString *items;
  int32_t numItems;

  if (U_FAILURE(*status)) return NULL;
  if (!resourceBundle || !resourceTag || (index < 0) )
    {
      *status = U_ILLEGAL_ARGUMENT_ERROR;
      return NULL;
    }

   ((ResourceBundle*)resourceBundle)
    ->getTaggedArray(resourceTag,
		     itemTags,
		     items,
		     numItems,
		     *status);
  if (U_SUCCESS(*status))
    {
      delete [] items; 

      if(index >= numItems)
	{
	  *status = U_INDEX_OUTOFBOUNDS_ERROR;
	  delete [] itemTags;
	  return NULL;
	}
      else
	{
	  char *str = (char*)malloc(itemTags[index].size() + 3);
	  u_austrcpy(str,itemTags[index].getUChars()); /* should be extract? */
	  delete [] itemTags;
	  return str; /* LEAK */
	}
    }
  else return NULL;
  
}


/* ripped off from udat_applyPattern */
U_CAPI void
unum_applyPattern(            UNumberFormat     *format,
			      bool_t          localized,
			      const   UChar           *pattern,
			      int32_t         patternLength)
{
  int32_t len = (patternLength == -1 ? u_strlen(pattern) : patternLength);
  const UnicodeString pat((UChar*)pattern, len, len);
  UErrorCode status = U_ZERO_ERROR;
  
  if(localized)
    ((DecimalFormat*)format)->applyLocalizedPattern(pat, status);
  else
    ((DecimalFormat*)format)->applyPattern(pat, status);
}



