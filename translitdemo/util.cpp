#include "util.h"
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
 * Send the given UnicodeString to 'out' using ENCODING.
 */
UBool util_fprintf(FILE* out, const UnicodeString& str) {
    char* charBuf = util_createChars(str);
    if (charBuf == 0) {
        return FALSE;
    }
    fprintf(out, "%s", charBuf);
    delete[] charBuf;
    return TRUE;
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
    return written == length;
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
    if (read != length) {
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
