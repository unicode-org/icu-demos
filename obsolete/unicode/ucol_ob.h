/*
*******************************************************************************
*
*   Copyright (C) 1996-2003, International Business Machines
*   Corporation and others.  All Rights Reserved.
*
*******************************************************************************
*   file name:  ucol_ob.h
*   encoding:   US-ASCII
*   tab size:   8 (not used)
*   indentation:4
*
*   created on: 2003apr30
*   created by: Markus W. Scherer
*
*   This file contains obsolete definitions from ucol.h.
*/

#ifndef __UCOL_OB_H__
#define __UCOL_OB_H__

#include "unicode/utypes.h"

#if !UCONFIG_NO_COLLATION

#include "unicode/uchar.h"
#include "unicode/obsolete.h"

/********************************* Deprecated API ********************************/
/* This is the C API wrapper for CollationIterator that got booted out from here, including just for */
/* include backward compatibility */
/**
 * TODO: Remove after Nov 2002
 */
#include "unicode/ucoleitr.h"

/**
 * Open a UCollator with a specific version.
 * This is the same as ucol_open() except that ucol_getVersion() of
 * the returned object is guaranteed to be the same as the version
 * parameter.
 * This is designed to be used to open the same collator for a given
 * locale even when ICU is updated.
 * The same locale and version guarantees the same sort keys and
 * comparison results.
 *
 * @param loc The locale ID for which to open a collator.
 * @param version The requested collator version.
 * @param status A pointer to a UErrorCode,
 *               must not indicate a failure before calling this function.
 * @return A pointer to a UCollator, or NULL if an error occurred
 *         or a collator with the requested version is not available.
 *
 * @see ucol_open
 * @see ucol_getVersion
 * @obsolete ICU 2.4. Use support for running multiple versions of ICU instead since this API will be removed in that release.
 */
U_CAPI UCollator * U_EXPORT2
ucol_openVersion(const char *loc,
                 UVersionInfo version,
                 UErrorCode *status);

/**
 * Get the normalization mode used in a UCollator.
 * The normalization mode influences how strings are compared.
 * @param coll The UCollator to query.
 * @return The normalization mode, UNORM_NONE or UNORM_NFD.
 * @see ucol_setNormalization
 * @obsolete ICU 2.4. Use ucol_getAttribute() instead since this API will be removed in that release.
 */
U_CAPI UNormalizationMode U_EXPORT2 
ucol_getNormalization(const UCollator* coll);

/**
 * Set the normalization mode used in a UCollator.
 * The normalization mode influences how strings are compared.
 * @param coll The UCollator to set.
 * @param mode The desired normalization mode: One of
 *             UNORM_NONE (expect the text to not need normalization),
 *             UNORM_NFD (normalize)
 * @see ucol_getNormalization
 * @obsolete ICU 2.4. Use ucol_setAttribute() instead since this API will be removed in that release.
 */
U_CAPI void U_EXPORT2 
ucol_setNormalization(  UCollator        *coll,
            UNormalizationMode    mode);

#endif /* #if !UCONFIG_NO_COLLATION */

#endif /* __UCOL_OB_H__ */
