/**********************************************************************
*   Copyright (C) 1999, International Business Machines
*   Corporation and others.  All Rights Reserved.
***********************************************************************/

/* Largest decomposition sequence is 18 chars at U+FDFA */
#define COLLECT_DEFAULT_SIZE 16

#include "unicode/utypes.h"
#include "unicode/ucnv.h"
#include "unicode/uchar.h"
#include "unicode/collectcb.h"

UConverterFromUCallback COLLECT_lastResortCallback = UCNV_FROM_U_CALLBACK_SUBSTITUTE;
UBool                  COLLECT_alnum = FALSE;

static UChar *COLLECT_BAD_CHARS = NULL;
static int32_t COLLECT_BAD_CHARS_SIZE = 0;


/* set the # of bad chars to be stored, also clear the list. */
U_CAPI void COLLECT_setSize(int32_t sz)
{
  int32_t i;

  free(COLLECT_BAD_CHARS);
  COLLECT_BAD_CHARS_SIZE = sz;

  COLLECT_BAD_CHARS = malloc(sizeof(UChar) * (sz+1));

  for(i=0;i<=sz;i++)
    COLLECT_BAD_CHARS[i] = 0;
}

/* Return a null terminated list of bad UChars (up to the size set in setSize) */
U_CAPI const UChar* COLLECT_getChars()
{
  if(!COLLECT_BAD_CHARS)
    COLLECT_setSize(COLLECT_DEFAULT_SIZE);
    
  return COLLECT_BAD_CHARS;
}


static void COLLECT_badChar(UChar c)
{
  int32_t i;
  
  if(c == 0x0000)
    return;

  /* restrict the set */
  if((COLLECT_alnum == TRUE) && (u_isalnum(c) == FALSE))
    return;

  if(COLLECT_BAD_CHARS == NULL)
    COLLECT_setSize(COLLECT_DEFAULT_SIZE);

  for(i=0;i<COLLECT_BAD_CHARS_SIZE;i++)
    {
      if(COLLECT_BAD_CHARS[i] == c)
	return;
      
      if(COLLECT_BAD_CHARS[i] == 0) /* got one! */
	{
	  COLLECT_BAD_CHARS[i] = c;
	  return;
	}
    }

  return; /* overflowed. */
}

U_CAPI void 
  UCNV_FROM_U_CALLBACK_COLLECT (UConverter * _this,
				char **target,
				const char *targetLimit,
				const UChar ** source,
				const UChar * sourceLimit,
				int32_t *offsets,
				UBool flush,
				UErrorCode * err)
{
  int32_t i;

#ifdef WIN32
  if (!((*err == U_INVALID_CHAR_FOUND) || (*err == U_ILLEGAL_CHAR_FOUND)))    return;
#else
  if (CONVERSION_U_SUCCESS(*err))
    return;
#endif

  /* Store all of 'em in the bad char buffer */
  for(i=0;i<_this->invalidUCharLength;i++)
    {
      COLLECT_badChar(_this->invalidUCharBuffer[i]);
    }

  /* call the default */
  (*COLLECT_lastResortCallback)(_this,
				  target,
				  targetLimit,
				  source, /* source shouldn't be needed */
				  sourceLimit, /* sourcelimit doesn't matter */
				  offsets, /* offsets, */
				  flush, /* flush, */
				  err);

  return;
}






