#include "util.h"
#include <string.h>
#include "unicode/unistr.h"

// Encoding we use
#define ENCODING "UTF8"

/**
 * Given a UnicodeString, extract its contents to a char* buffer using
 * ENCODING and return it.  Caller must delete returned pointer.
 * Return value will be 0 if out of memory.
 */
char* util_createChars(const UnicodeString& str) {
    // Preflight for size
    int32_t len = str.extract(0, str.length(), NULL, 0, ENCODING) + 16;
    char* charBuf = new char[len];
    if (charBuf != 0) {
        len = str.extract(0, str.length(), charBuf, len, ENCODING);
        charBuf[len] = 0;
    }
    return charBuf;
}

/**
 * Write the given bytes out to the given file.  The
 * inverse of this operation is util_readFrom().
 */
UBool util_writeTo(FILE* file, const char* data, int32_t length) {
    // Write length
    size_t written = fwrite(&length, sizeof(length), 1, file);
    if (written != 1) {
        return FALSE;
    }
    // Write data
    written = fwrite(data, 1, length, file);
    return written == (size_t)length;
}

/**
 * Return NULL on failure.  Caller owns and must delete result.
 */
char* util_readFrom(FILE* file, int32_t& length) {
    // Read length
    size_t read = fread(&length, sizeof(length), 1, file);
    if (read != 1) {
        return 0;
    }
    // Read data
    char* data = new char[length];
    if (data == 0) {
        return 0; // out of memory
    }
    read = fread(data, 1, length, file);
    if (read != (size_t)length) {
        delete[] data;
        data = 0;
    }
    return data;
}

/**
 * Write the given string out to the given file using ENCODING.  The
 * inverse of this operation is util_readFrom().  Unlike
 * util_printf(), this function writes a length long.
 */
UBool util_writeTo(FILE* file, const UnicodeString& str) {
    char* charBuf = util_createChars(str);
    if (charBuf == 0) {
        return FALSE;
    }
    int32_t size = strlen(charBuf) + 1;
    UBool result = util_writeTo(file, charBuf, size);
    delete[] charBuf;
    return result;
}

/**
 * Return TRUE on success.
 */
UBool util_readFrom(FILE* file, UnicodeString& key) {
    int32_t length = 0;
    char* charBuf = util_readFrom(file, length);
    if (charBuf == 0) {
        return FALSE;
    }
    key = UnicodeString(charBuf, ENCODING);
    delete[] charBuf;
    return TRUE;
}

/**
 * Escape a string for JavaScript, such that JavaScript's unescape()
 * function recovers the original string.  Modifies the string in place.
 */
void util_escapeJavaScriptString(UnicodeString& str) {
    int32_t i;
    char buf[8];
    strcpy(buf, "%00");
    for (i=0; i<str.length(); ++i) {
        UChar c = str[i];
        if ((c >= 48 && c <= 57) ||
            (c >= 65 && c <= 90) ||
            (c >= 97 && c <= 122)) {
            // c is in [0-9A-Za-z]; do nothing
        } else {
            sprintf(buf+1, "%02X", c);
            str.replace(i, 1, UnicodeString(buf, ""));
            i += 2;
        }
    }
}

/**
 * Write the given string to the given FILE*, escaping certain
 * characters assuming we are within a JavaScript double quoted string
 * if inQuote is true.
 */
void util_fprintf(FILE* out, const char* str, UBool inQuote) {
    if (!inQuote) {
        fprintf(out, "%s", str);
        return;
    }
    int32_t i;
    int32_t n = strlen(str);
    for (i=0; i<n; ++i) {
        int32_t c=-1;
        switch (str[i]) {
            // I'm disabling \n and \\ temporarily during development --
            // they seem to muck up setting vars to hidden fields.
            // Soon that's all we'll be doing, so it makes sense to
            // optimize that.
        case 0x0008: c=0x0062 /*b*/; break;
        case 0x0009: c=0x0074 /*t*/; break;
        // case 0x000A: c=0x006E /*n*/; break;
        case 0x000C: c=0x0066 /*f*/; break;
        case 0x000D: c=0x0072 /*r*/; break;
        case 0x0022: c=0x0022 /*"*/; break;
        case 0x0027: c=0x0027 /*'*/; break;
        // case 0x005C: c=0x005C /*\*/; break;
        }
        if (c>=0) {
            fprintf(out, "\\%c", (char)c);
        } else {
            fprintf(out, "%c", str[i]);
        }
    }
}

/**
 * Send the given UnicodeString to 'out' using ENCODING.
 */
UBool util_fprintf(FILE* out, const UnicodeString& str, UBool inQuote) {
    char* charBuf = util_createChars(str);
    if (charBuf == 0) {
        return FALSE;
    }
    util_fprintf(out, charBuf, inQuote);
    delete[] charBuf;
    return TRUE;
}
