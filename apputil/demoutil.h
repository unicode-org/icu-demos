/*
*******************************************************************************
*
*   Copyright (C) 2005-2012, International Business Machines
*   Corporation and others.  All Rights Reserved.
*
*******************************************************************************
*   file name:  demoutil.h
*   encoding:   US-ASCII
*   tab size:   4 (not used)
*   indentation:4
*
*   created on: 2003Mar25
*   created by: George Rhoten
*/

#ifndef DEMOUTIL_H
#define DEMOUTIL_H 1

#include "unicode/utypes.h"
#include "unicode/ures.h"
#include "unicode/ustdio.h"

/**
 * Print a file as UTF-8
 * @param file_out If non-NULL, print to this output, otherwise use stdout.
 * @param displayBundle If non-NULL, try to load this
 *      fragment from a resource bundle instead of from the file system.
 * @param templateFileName If non-NULL, try to load this fragment from the file system.
 * @returns 0 on error and 1 on success.
 */
U_CAPI int printHTMLFragment(UFILE *fileOut, UResourceBundle *displayBundle, const char *templateFileName);


/**
 * Print a file as specified by PATH_INFO
 * @param files array in the form { "/filename", "mime-type", "/filename2", "mime-type", NULL, NULL }"
 * @param pathInfo if NULL, PATH_INFO will be used
 * @return NULL if no file was served, or the filename if a file was served.
 */
U_CAPI const char *serveStaticFile(const char *files[], const char *pathInfo);

/**
 * Serve up a single file
 */
U_CAPI int writeOutFile(const char *fn, const char *mimeType);


#endif
