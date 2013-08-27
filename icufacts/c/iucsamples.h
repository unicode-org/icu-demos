/* Copyright (c) 2010-2013 IBM Corporation and Others. All Rights Reserved. */
/* These are just here to reduce code size of samples */

#ifndef _IUCSAMPLES
#define _IUCSAMPLES

#include <unicode/putil.h>
#include <unicode/uclean.h>
#include <unicode/uloc.h>
#include <unicode/unistr.h>
#include <unicode/ures.h>
#include <unicode/ustdio.h>

// C++ includes
#ifdef __cplusplus
#include <unicode/resbund.h>
#include <unicode/msgfmt.h>
#include <unicode/calendar.h>
#endif

/**
 * \def ASSERT_OK
 * @param x status param
 * if U_FAILURE(x) then a message will be printed.
 */
#define ASSERT_OK(x) if(U_FAILURE(x)) { u_printf("%s:%d: ICU Failure %s\n", __FILE__, __LINE__, u_errorName(x)); }

#endif
