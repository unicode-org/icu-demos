/*
*******************************************************************************
*                                                                             *
* COPYRIGHT:                                                                  *
*   (C) Copyright International Business Machines Corporation, 1998, 1999     *
*   Licensed Material - Program-Property of IBM - All Rights Reserved.        *
*   US Government Users Restricted Rights - Use, duplication, or disclosure   *
*   restricted by GSA ADP Schedule Contract with IBM Corp.                    *
*                                                                             *
*******************************************************************************
*
* File LX_utils.h
*
* Modification History:
*
*   Date        Name        Description
*  8/25/1999     srl         moved a bunch of stuff here
*******************************************************************************
*/

#ifndef _LXUTILS
#define _LXUTILS

#include "ustdio.h"
#include "ucnv.h"
#include "ustring.h"
#include "udat.h"
#include "uloc.h"
#include "ures.h"
#include "ucol.h"
#include "string.h"
#include "ucal.h"
#include "ctype.h"


/* Some misc utils that were used in listrb but aren't specific 
   to listrb. */

#define my_min(a,b) ((a<b)?(a):(b))

#define SORTKEYSIZE 100



typedef struct mysortable
{
  char key[SORTKEYSIZE];
  uint32_t keySize;
  const UChar *ustr;
  const char *str;
  
  struct mysortable *subLocs;
  struct mysortable *parent;
  int32_t        nSubLocs;
  int32_t        subLocsSize;
} MySortable;


/* Sort */
void mySort(MySortable *root, UErrorCode *err, bool_t recurse);


/**
 * NON recursive exact match search
 * @param toSort  the list of locales to search
 * @param locale  locale to search for
 * @return index into toSort.subLocs[], or -1 if not found
 */

int32_t findLocaleNonRecursive(MySortable *toSort, const char *locale);

/** 
 * Initialize a MySortable with the specified locale. Will reset all fields.
 * TODO: limit displayname to just a part (default -> English -> United States -> California, instead of   .... -> English (United States) -> ... 
 * @param s The MySortable struct to initialize
 * @param locid The Locale that the MySortable should refer to
 * @param inLocale The Locale which the MySortable's display name should be displayed 
 * @param parent parent locale of the node
 */

void initSortable(MySortable *s, const char *locid, const char *inLocale, MySortable *parent);

/**
 * create a MySortable tree of locales. 
 * default -> en, ja, de, ... -> en_US, en_CA, .. -> en_US_CALIFORNIA
 * @param inLocale the Locale the tree should be created in
 * @param localeCount [return] total # of items in the tree
 * @return a new MySortable tree, owned by the caller.  Not sorted.
 */

MySortable *createLocaleTree(const char *inLocale, int32_t *localeCount);


/**
 * Recursive search for the specified locale.
 * Returns the parent node as well if requested.
 * @param root The root of the tree to search
 * @param locale The locale to search for 
 * @return Pointer to a node matching the requested locale, or NULL
 */
MySortable *findLocale(MySortable *root, const char *locale);

/* Pick a better charset name */
const char *MIMECharsetName(const char *origName);

/* Convert unichars to an escaped query (one containing \uXXXX, etc */
void ucharsToEscapedUrlQuery(char *urlQuery, const UChar *src);

/* Decode escaped query field.  returns: # chars.  dstlen is a max. src can point to the beginning of the field (after the '='), it will terminate at & or \0 */
int32_t unescapeAndDecodeQueryField(UChar *dst, int32_t dstLen, const char *src);

/* copy UChars around, WITHOUT termination, and convert \uXXXX back to the right chars */
int32_t copyWithUnescaping( UChar* chars, const UChar* src, int32_t origLen);

/* Decode a URL query field  [%XX, +, etc.] */
void doDecodeQueryField(const char *in, char *out, int32_t length);

/**
 * Format the current date in the given style
 * @param tz The timezone to format in (NULL for default)
 * @param style Style to format in (try UDAT_FULL)
 * @param status [returned] status of formatting, etc.
 * @return pointer to the formatted chars. Caller must dispose of them.
 */
UChar *date(const UChar *tz, UDateFormatStyle style, UErrorCode *status);


/* substitute with value, of the form: <B>\uXXXX</B>  */
void
UCNV_FROM_U_CALLBACK_BACKSLASH_ESCAPE_HTML (UConverter * _this,
			 char **target,
			 const char *targetLimit,
			 const UChar ** source,
			 const UChar * sourceLimit,
			 int32_t *offsets,
			 bool_t flush,
			 UErrorCode * err);

/* substitute with value, of the form: \uXXXX  */
void
UCNV_FROM_U_CALLBACK_BACKSLASH_ESCAPE (UConverter * _this,
			 char **target,
			 const char *targetLimit,
			 const UChar ** source,
			 const UChar * sourceLimit,
			 int32_t *offsets,
			 bool_t flush,
			 UErrorCode * err);

/**
 * Replace all instances of 'from' with 'to'.
 * @param str the string to modify. Null terminated
 * @param from the char to look for
 * @param to the char to change it to.
 */

void u_replaceChar(UChar *str, UChar from, UChar to);

#endif
