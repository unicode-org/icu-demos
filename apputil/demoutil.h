/*
*******************************************************************************
*
*   Copyright (C) 2005-2005, International Business Machines
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
 * @returns 0 on error and 1 on success.
 */
U_CAPI int printHTMLFragment(UFILE *fileOut, UResourceBundle *displayBundle, char *templateFileName);

#endif
