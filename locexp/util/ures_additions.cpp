/**********************************************************************
*   Copyright (C) 1999, International Business Machines
*   Corporation and others.  All Rights Reserved.
***********************************************************************/
#include <stdlib.h>
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
  
  /* ignore result
  ((ResourceBundle*)resourceBundle)-> get2dArray(resourceTag,
                  *rowCount,
                  *columnCount,
                  *status); */
}

U_CAPI const char* ures_getTaggedArrayTag(const UResourceBundle *resourceBundle,
                               const char *resourceTag, 
                               int32_t index,
                               UErrorCode* status)
{

  if (U_FAILURE(*status)) {
      return NULL;
  }
  if (!resourceBundle || !resourceTag || (index < 0) )
    {
      *status = U_ILLEGAL_ARGUMENT_ERROR;
      return NULL;
    }

  UnicodeString uStr = ((ResourceBundle*)resourceBundle)->getStringEx(index,*status);
  if (U_SUCCESS(*status))
    {
       char *str = (char*)malloc(uStr.length() + 3);
       u_austrcpy(str,(const UChar*)(uStr.getBuffer())); /* should be extract? */
       return str;
     }
  else return NULL;
  
}



