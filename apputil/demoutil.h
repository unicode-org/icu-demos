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

/**
 * @param package load HTML fragment from resource bundle package.
 *      Leave NULL, if no package is used.
 * @param locale to specify which for which HTML fragment to use.
 *      Leave NULL, if demo is unlocalized.
 * @returns 0 on error and 1 on success.
 */
U_CAPI int printHTMLFragment(char * package, char * locale, char *templateFileName);

#endif
