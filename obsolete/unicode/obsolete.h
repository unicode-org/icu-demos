/*
**********************************************************************
* Copyright (c) 2003, International Business Machines
* Corporation and others.  All Rights Reserved.
**********************************************************************
* Author: Alan Liu
* Created: February 25 2003
* Since: ICU 2.6
**********************************************************************
*/
#ifndef ICU_OBSOLETE_H
#define ICU_OBSOLETE_H

#include "unicode/umachine.h"

/**
 * \def U_OBSOLETE_API
 * Set to export library symbols from inside the obsolete library,
 * and undefined to import them from outside.
 * @stable ICU 2.0
 */

#if defined(U_OBSOLETE_IMPLEMENTATION)
#define U_OBSOLETE_API  U_EXPORT
#else
#define U_OBSOLETE_API  U_IMPORT
#endif

/* These UErrorCode constants were renamed to ones with a _WARNING suffix. (from utypes.h) */
enum {
    /** @obsolete ICU 2.4. Use the enum that ends in _WARNING instead since this API will be removed in that release. */
    U_USING_FALLBACK_ERROR    = -128,
    /** @obsolete ICU 2.4. Use the enum that ends in _WARNING instead since this API will be removed in that release. */
    U_ERROR_INFO_START        = U_ERROR_WARNING_START,
    /** @obsolete ICU 2.4. Use the enum that ends in _WARNING instead since this API will be removed in that release. */
    U_USING_DEFAULT_ERROR     = -127,
    /** @obsolete ICU 2.4. Use the enum that ends in _WARNING instead since this API will be removed in that release. */
    U_SAFECLONE_ALLOCATED_ERROR = -126,
    /** @obsolete ICU 2.4. Use the enum that ends in _WARNING instead since this API will be removed in that release. */
    U_ERROR_INFO_LIMIT        = U_ERROR_WARNING_LIMIT
};

#endif
