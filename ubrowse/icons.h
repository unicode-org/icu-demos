/*
*******************************************************************************
* Copyright (C) 2005-2005, International Business Machines Corporation and    *
* others. All Rights Reserved.                                                *
*******************************************************************************
*/

#ifndef ICONS_H

#include "unicode/utypes.h"

U_CFUNC int icons_init();
U_CFUNC void icons_write( /* UFILE *OUT, */ const char *path );

#ifndef ICU_URL
# define ICU_URL "http://ibm.com/software/globalization/icu"
#endif


#endif
