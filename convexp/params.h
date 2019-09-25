/*
*******************************************************************************
*
*   Copyright (C) 2003-2012, International Business Machines
*   Corporation and others.  All Rights Reserved.
*
*******************************************************************************
*   file name:  params.h
*   encoding:   US-ASCII
*   tab size:   4 (not used)
*   indentation:4
*
*   created on: 2003Mar29
*   created by: George Rhoten
*
*   This is a fairly crude converter explorer. It allows you to browse the
*   converter alias table without requiring the people to know the alias table syntax.
*
*/

#include <string>
#include <set>
#include <unicode/utypes.h>

#define MAX_BYTE_SIZE 64

U_CFUNC int32_t gMaxStandards;
U_CFUNC char gCurrConverter[UCNV_MAX_CONVERTER_NAME_LENGTH];
U_CFUNC char gStartBytes[MAX_BYTE_SIZE];
U_CFUNC std::set<std::string> *gStandardsSelected;
U_CFUNC const char *gScriptName;
U_CFUNC UBool gShowStartBytes;
U_CFUNC UBool gShowUnicodeSet;
U_CFUNC UBool gShowLanguages;
U_CFUNC UBool gShowUnavailable;

#define OPTION_SEPARATOR    '&'
#define VALUE_SEPARATOR     '='
#define OPTION_SEP_STR      "&amp;"
#define NBSP                "\xC2\xA0"  /* UTF-8 &nbsp; */
#define PROGRAM_NAME        "Converter Explorer"
#define SHOW_LOCALES        "ShowLocales"
#define SHOW_UNICODESET     "ShowSet"
#define SHOW_UNAVAILABLE    "ShowUnavailable"

U_CFUNC const char ALL[];

U_CFUNC const char *getStandardOptionsURL(UErrorCode *status);
U_CFUNC void parseAllOptions(const char *queryStr, UErrorCode *status);
