/*
*******************************************************************************
*
*   Copyright (C) 2000, International Business Machines
*   Corporation and others.  All Rights Reserved.
*
*******************************************************************************
*   file name:  uoptions.h
*   encoding:   US-ASCII
*   tab size:   8 (not used)
*   indentation:4
*
*   created on: 2000apr17
*   created by: Markus W. Scherer
*
*   This file provides a command line argument parser.
*/

#ifndef __QUERYOPTIONS_H__
#define __QUERYOPTIONS_H__

#include "unicode/utypes.h"


/* forward declarations for the function declaration */
struct QueryOption;
typedef struct QueryOption QueryOption;

/* function to be called for a command line option */
typedef int QueryOptionFn(void *context, QueryOption *option);



/* structure describing a command line option */
struct QueryOption {
    const char *name;           /* "foo" */
    const char *value;          /* output placeholder, will point to the argument string, if any */
    int32_t valueLen;
    QueryOptionFn *optionFn;    /* function to be called when this option occurs */
    void *context;              /* parameter for the function */
    char doesOccur;             /* boolean for "this one occured" */
};

/* macro for an entry in a declaration of UOption[] */
#define QUERY_OPTION_DEF(longName, optionFn, context) \
    { longName, NULL, 0, optionFn, context, 0 }


/**
 * Query String parser.
 *
 *
 * @param argv This parameter is modified
 * @param options This parameter is modified
 */
U_CAPI int U_EXPORT2
parseQueryString(const char* src,int32_t srcLen, int32_t optionCount, QueryOption options[]);

#endif


