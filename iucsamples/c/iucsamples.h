/* Copyright (c) 2010-2012 IBM Corporation and Others. All Rights Reserved. */

#ifndef _UASSERT
#define _UASSERT

#define ASSERT_OK(x) if(U_FAILURE(x)) { fprintf(stderr,"%s:%d: ICU Failure %s\n", __FILE__, __LINE__, u_errorName(x)); }

#endif

// #ifdef CPLUSPLUS
#include <unicode/utypes.h>
using namespace icu;
// #endif