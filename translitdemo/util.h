#ifndef UTIL_H
#define UTIL_H

#include "unicode/utypes.h"
#include <stdio.h>
#include "unicode/unistr.h"


// Encoding we use
#define ENCODING "UTF8"

/**
 * Given a UnicodeString, extract its contents to a char* buffer using
 * ENCODING and return it.  Caller must delete returned pointer.
 * Return value will be 0 if out of memory.
 */
char* util_createChars(const UnicodeString& str);

/**
 * Write the given bytes out to the given file.  The
 * inverse of this operation is util_readFrom().
 */
UBool util_writeTo(FILE* file, const char* data, int32_t length);

/**
 * Return NULL on failure.
 */
char* util_readFrom(FILE* file, int32_t& length);

/**
 * Write the given string out to the given file using ENCODING.  The
 * inverse of this operation is util_readFrom().  Unlike
 * util_printf(), this function writes a length long.
 */
UBool util_writeTo(FILE* file, const UnicodeString& str);

/**
 * Return TRUE on success.
 */
UBool util_readFrom(FILE* file, UnicodeString& key);

/**
 * Escape a string for JavaScript, such that JavaScript's unescape()
 * function recovers the original string.  Modifies the string in place.
 */
void util_escapeJavaScriptString(UnicodeString& str);

/**
 * Write the given string to the given FILE*, escaping certain
 * characters assuming we are within a JavaScript double quoted string
 * if inQuote is true.
 */
void util_fprintf(FILE* out, const char* str, UBool inQuote);

/**
 * Write the given string to the given FILE*, using ENCODING, escaping
 * certain characters assuming we are within a JavaScript double
 * quoted string if inQuote is true.
 */
UBool util_fprintf(FILE* out, const UnicodeString& str, UBool inQuote=FALSE);

/**
 * Send the given UnicodeString to 'out' using ENCODING and
 * JavaScript escaping for double quotes ONLY.  Good for HIDDEN fields.
 */
UBool util_fprintfq(FILE* out, const UnicodeString& str);

#endif
