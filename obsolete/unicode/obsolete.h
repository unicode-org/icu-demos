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

#endif
