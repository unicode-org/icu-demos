#ifndef UTIL_H
#define UTIL_H

#include "unicode/utypes.h"
#include <stdio.h>

class UnicodeString;

// Encoding we use
#define ENCODING "UTF8"

/**
 * Given a UnicodeString, extract its contents to a char* buffer using
 * ENCODING and return it.  Caller must delete returned pointer.
 * Return value will be 0 if out of memory.
 */
char* util_createChars(const UnicodeString& str);

/**
 * Send the given UnicodeString to 'out' using ENCODING.
 */
UBool util_fprintf(FILE* out, const UnicodeString& str);

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

#endif
