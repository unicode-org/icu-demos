/**********************************************************************
*   Copyright (C) 1999, International Business Machines
*   Corporation and others.  All Rights Reserved.
***********************************************************************
*
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

#include "unicode/ustdio.h"
#include "unicode/ucnv.h"
#include "unicode/ustring.h"
#include "unicode/udat.h"
#include "unicode/uloc.h"
#include "unicode/ures.h"
#include "unicode/ucol.h"
#include "string.h"
#include "unicode/ucal.h"
#include "unicode/uchar.h"
#include "ctype.h"


/* Some misc utils that were used in listrb but aren't specific 
   to listrb. */

#define my_min(a,b) ((a<b)?(a):(b))

#define SORTKEYSIZE 100



typedef struct mysortable
{
  char key[SORTKEYSIZE];
  uint32_t keySize;
  UChar *ustr;
  char *str;
  
  struct mysortable *subLocs;
  struct mysortable *parent;
  int32_t        nSubLocs;
  int32_t        subLocsSize;
} MySortable;


/* Sort */
U_CAPI void mySort(MySortable *root, UErrorCode *err, UBool recurse);


/**
 * NON recursive exact match search
 * @param toSort  the list of locales to search
 * @param locale  locale to search for
 * @return index into toSort.subLocs[], or -1 if not found
 */

U_CAPI
int32_t findLocaleNonRecursive(MySortable *toSort, const char *locale);

/** 
 * Initialize a MySortable with the specified locale. Will reset all fields.
 * TODO: limit displayname to just a part (root -> English -> United States -> California, instead of   .... -> English (United States) -> ... 
 * @param s The MySortable struct to initialize
 * @param locid The Locale that the MySortable should refer to
 * @param inLocale The Locale which the MySortable's display name should be displayed 
 * @param parent parent locale of the node
 */

U_CAPI
void initSortable(MySortable *s, const char *locid, const char *inLocale, MySortable *parent);

/**
 * create a MySortable tree of locales. 
 * root -> en, ja, de, ... -> en_US, en_CA, .. -> en_US_CALIFORNIA
 * @param inLocale the Locale the tree should be created in
 * @param localeCount [return] total # of items in the tree
 * @return a new MySortable tree, owned by the caller.  Not sorted.
 */

U_CAPI MySortable *createLocaleTree(const char *inLocale, int32_t *localeCount);

U_CAPI void destroyLocaleTree(MySortable *d);

/**
 * Recursive search for the specified locale.
 * Returns the parent node as well if requested.
 * @param root The root of the tree to search
 * @param locale The locale to search for 
 * @return Pointer to a node matching the requested locale, or NULL
 */
U_CAPI MySortable *findLocale(MySortable *root, const char *locale);

/* Pick a better charset name */
U_CAPI const char *MIMECharsetName(const char *origName);

/* Convert unichars to an escaped query (one containing \uXXXX, etc */
void ucharsToEscapedUrlQuery(char *urlQuery, const UChar *src);

/* Decode escaped query field.  returns: # chars.  dstlen is a max. src can point to the beginning of the field (after the '='), it will terminate at & or \0 */
U_CAPI int32_t unescapeAndDecodeQueryField(UChar *dst, int32_t dstLen, const char *src);
U_CAPI int32_t unescapeAndDecodeQueryField_enc(UChar *dst, int32_t dstLen, const char *src, const char *enc);

/* copy UChars around, WITHOUT termination, and convert \uXXXX back to the right chars */
U_CAPI int32_t copyWithUnescaping( UChar* chars, const UChar* src, int32_t origLen);

/* Decode a URL query field  [%XX, +, etc.] */
U_CAPI void doDecodeQueryField(const char *in, char *out, uint32_t length);

/**
 * Format the current date in the given style
 * @param tz The timezone to format in (NULL for default)
 * @param style Style to format in (try UDAT_FULL)
 * @param status [returned] status of formatting, etc.
 * @return pointer to the formatted chars. Caller must dispose of them.
 */
U_CAPI UChar *date(const UChar *tz, UDateFormatStyle style, const char *locale, UErrorCode *status);

/**
 * Format a date in the given style
 * @param date date to format
 * @param tz The timezone to format in (NULL for default)
 * @param style Style to format in (try UDAT_FULL)
 * @param status [returned] status of formatting, etc.
 * @return pointer to the formatted chars. Caller must dispose of them.
 */
UChar *dateAt(UDate date, const UChar *tz, UDateFormatStyle style, const char *locale, UErrorCode *status);

typedef struct _FromUBackslashContext
{
  UConverterFromUCallback  subCallback;
  const void              *subContext;

  UBool                    html;
} FromUBackslashContext;

/* substitute with value, of the form: <B>\uXXXX</B>  */
U_CAPI void
UCNV_FROM_U_CALLBACK_BACKSLASH (const void *context,
                                UConverterFromUnicodeArgs *fromUArgs,
                                const UChar* codeUnits,
                                int32_t length,
                                UChar32 codePoint,
                                UConverterCallbackReason reason,
                                UErrorCode *err);

/**
 * Replace all instances of 'from' with 'to'.
 * @param str the string to modify. Null terminated
 * @param from the char to look for
 * @param to the char to change it to.
 */

U_CAPI void u_replaceChar(UChar *str, UChar from, UChar to);

/**
 * Duplicate a string from the host encoding to Unicode 
 * caller owns storage
 * @param hostchars chars in host encoding
 * @return ptr to new unichars
 */

#ifdef WIN32
U_CAPI UChar *uastrdup(const char *hostchars);
#else
U_CFUNC UChar *uastrdup(const char *hostchars);
#endif


/**
 * Test whether a particular converter handles the sample text
 * @param converter Which converter to test
 * @param sample Sample uchars to test
 * @param sampleLen length of sample uchars
 * @param buffer output buffer for the test
 * @param bufLen length of the output buffer
 * @return TRUE if the converter handles the test, FALSE otherwise.
 */


U_CAPI UBool testConverter(const char *converter, 
                     const UChar *sample,
		     int32_t sampleLen, 
		     int8_t *buffer,
		     int32_t bufLen);


/** FSWF. All functions are NON multithread safe! */

/* fetch string with fallback */
U_CAPI const UChar *FSWF(const char *key, const char *fallback);

/* Return a ptr to FSWF's bundle */
U_CAPI UResourceBundle *FSWF_getBundle();

U_CAPI UErrorCode FSWF_bundleError();

/* close FSWF's bundle */
U_CAPI void FSWF_close();

/* set bundle locale */
U_CAPI void FSWF_setLocale( const char* loc);

/* returns a path to load FSWF's bundle from..  */
U_CAPI const char *FSWF_bundlePath();

/* Set the path for FSWF. Default: icudata/FSWF/ */
U_CAPI void FSWF_setBundlePath(char *newPath);

U_CAPI
void lx_setHTMLFilterOnTransliterator(UTransliterator *xlit, UBool doFilter);

/** UUSet, a mutant C api to UnicodeSet **/

typedef void *UUSet;

U_CAPI UUSet* U_EXPORT2 
uuset_openPattern(const UChar* pattern, UErrorCode* status);

U_CAPI int32_t U_EXPORT2 
uuset_size(UUSet *_this);

U_CAPI UChar32 U_EXPORT2 
uuset_charAt(UUSet *_this, int32_t index);

U_CAPI UBool U_EXPORT2 
uuset_contains(UUSet *_this, UChar32 c);

U_CAPI void U_EXPORT2
uuset_close(UUSet *_this);

/** Blocks & Property names **/
U_CAPI const char *getUCharCategoryName(int8_t type);
U_CAPI const char *getUBlockCodeName(UBlockCode block);
U_CAPI const char *getUPropertyName(UProperty u);

U_CAPI const char *getUCharCategorySortedName(int32_t n);
U_CAPI const char *getUBlockCodeSortedName(int32_t n);
U_CAPI const char *getUPropertySortedName(int32_t n);

U_CAPI UCharCategory getUCharCategorySorted(int32_t n);
U_CAPI UBlockCode    getUBlockCodeSorted(int32_t n);
U_CAPI UProperty     getUPropertySorted(int32_t n);

U_CAPI int lxu_validate_property_sanity();

/** CanonIter **/

typedef void *UCanonicalIterator;
U_CAPI UCanonicalIterator ucanit_open(const UChar *str, UErrorCode *status);

/**
 * see UCanonicalIterator::next
 * @param iter the iterator
 * @param buffer the output buffer. Alwyas null ternimated.
 * @param len Length of buffer
 * @param status Error code.  U_BUFFER_OVERFLOW_ERROR if buffer is full.
 * @return actual size of string.  0 if done. 
 */
U_CAPI int32_t ucanit_next(UCanonicalIterator *iter, UChar *buffer, int32_t len, UErrorCode *status);

U_CAPI void ucanit_close(UCanonicalIterator *iter);

#endif

