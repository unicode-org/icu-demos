/**********************************************************************
*   Copyright (C) 1999-2009, International Business Machines
*   Corporation and others.  All Rights Reserved.
***********************************************************************/

#ifndef _LX_HOOK
#define _LX_HOOK

#if defined(HAVE_LX_HOOK)

#include <unicode/utypes.h>
#include <unicode/usort.h>

/**
 * Initialize the hook stuff
 */
U_CAPI void lx_hook_init(void);

/**
 * Finalize the hook. Currently not called.
 */
U_CAPI void lx_hook_fini(void);

/**
 * Get provider capability info.
 * @param providerKeyword pointer to pointer to char. On exit, will point to the provider keyword name ('provider')
 * @return static array of keyword values { "ICU40", "ICU38", ... }
 */
U_CAPI const char ** lx_hook_keywords(const char** kwName);

/**
 * Custom version of usort open
 */
U_CAPI USort*
lx_hook_usort_open(const char *locale, UCollationStrength strength, UBool ownText,
           UErrorCode *status);

/*
custom version of usort close
lx_hook_usort_close
*/

#endif
#endif
