/*
*******************************************************************************
*
*   Copyright (C) 2003, International Business Machines
*   Corporation and others.  All Rights Reserved.
*
*******************************************************************************
*   file name:  params.cpp
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

#include "unicode/ucnv.h"
#include "params.h"

#include <stdio.h>

int32_t gMaxStandards;
char gCurrConverter[UCNV_MAX_CONVERTER_NAME_LENGTH] = "";
char gStartBytes[MAX_BYTE_SIZE] = "";
UHashtable *gStandardsSelected = NULL;
const char *gScriptName = NULL;

const char ALL[]="ALL";

U_CFUNC const char *getStandardOptionsURL(UErrorCode *status) {
    static char optionsURL[1024] = "\x00\x00\x00";
	static char *standards = NULL;

	if (!standards) {
		int32_t pos = -1;
		const UHashElement *e;
		const char *standard;
		int32_t len = 0;

		while ((e = uhash_nextElement(gStandardsSelected, &pos)) != NULL) {
			standard = (const char*) e->value.pointer;
			len += sprintf(optionsURL+len, OPTION_SEP_STR"s=%s", standard);
			if (*standard == 0) {
				/* Special case for when browsers are too smart, like Opera */
				len += sprintf(optionsURL+len, "-");
			}
		}
		standards = optionsURL;
	}

    return optionsURL+1;
}

/*
 Only insert standards that we recognize into the hashtable
 All other standards could be bad/malicious data
*/
static void addStandard(const char *newStandard, int32_t nameSize, UErrorCode *status) {
    int32_t i;
    const char *standard;

    if (nameSize <= 0 || *newStandard == '-') {
        uhash_put(gStandardsSelected,
            (void*)ucnv_getStandard(gMaxStandards-1, status),
            (void*)ucnv_getStandard(gMaxStandards-1, status),
            status);
        if (U_FAILURE(*status)) {
            printf("ERROR: uhash_put() -> %s\n", u_errorName(*status));
        }
        return;
    }
    if (uprv_strnicmp(ALL, newStandard, nameSize) == 0) {
        uhash_put(gStandardsSelected, (void*)ALL, (void*)ALL, status);
        if (U_FAILURE(*status)) {
            printf("ERROR: uhash_put() -> %s\n", u_errorName(*status));
        }
        return;
    }
    for (i = 0; i < gMaxStandards; i++) {
        *status = U_ZERO_ERROR;
        standard = ucnv_getStandard((uint16_t)i, status);
        if (U_FAILURE(*status)) {
            printf("ERROR: ucnv_getStandard() -> %s\n", u_errorName(*status));
        }
        if (uprv_strnicmp(standard, newStandard, nameSize) == 0) {
            uhash_put(gStandardsSelected, (void*)standard, (void*)standard, status);
            if (U_FAILURE(*status)) {
                printf("ERROR: uhash_put() -> %s\n", u_errorName(*status));
            }
            return;  // I'm done, I already found it.
        }
    }
    printf("WARNING: Ignoring unknown standard %s<br>\n", newStandard);
}

static const char* getNextOption(const char* src, const char* srcLimit) {
    const char *nextOpt = strchr(src, OPTION_SEPARATOR);
    if (nextOpt != NULL) {
        src = nextOpt;
    }
    else {
        src = srcLimit;
    }
    return src;
}

U_CFUNC void parseAllOptions(const char *queryStr, UErrorCode *status) {
    const char* src = queryStr;
    int srcLen = strlen(queryStr);
    const char* srcLimit = queryStr + srcLen;

    while (src < srcLimit) {
        const char *nextVal = strchr(src, VALUE_SEPARATOR);
        if (!nextVal) {
            nextVal = srcLimit;
        }
        else {
            nextVal++;
        }
        const char *nextOpt = getNextOption(src, srcLimit);
        if (strncmp(src, "s=", 2) == 0) {
            addStandard(nextVal, nextOpt - nextVal, status);
        }
        else if (strncmp(src, "conv=", 5) == 0) {
            UErrorCode myStatus = U_ZERO_ERROR;
            strncpy(gCurrConverter, nextVal, nextOpt - nextVal);
            gCurrConverter[UCNV_MAX_CONVERTER_NAME_LENGTH-1] = 0;   // NULL terminate for safety

            UConverter *cnv = ucnv_open(gCurrConverter, &myStatus);
            if (U_SUCCESS(myStatus)) {
                strcpy(gCurrConverter, ucnv_getName(cnv, &myStatus));
                ucnv_close(cnv);
            }
            gCurrConverter[UCNV_MAX_CONVERTER_NAME_LENGTH-1] = 0;   // NULL terminate for safety
        }
        else if (strncmp(src, "b=", 2) == 0) {
            const char *strItr = gStartBytes;
            strncpy(gStartBytes, nextVal, nextOpt - nextVal);
            gStartBytes[sizeof(gStartBytes)-1] = 0;   // NULL terminate for safety
            gStartBytes[((strlen(gStartBytes)>>1)<<1)] = 0;// Make it an even number of chars
            while (*strItr) {
                if (!isxdigit(*strItr)) {
                    // Bad data! Ignore the whole thing
                    gStartBytes[0] = 0;
                    break;
                }
                strItr++;
            }
        }
        else {
            // Woah! I don't know what this option is.
        }
        src = nextOpt+1;
    }
    if (!gCurrConverter) {
        // Can't use the starter bytes without a converter! Someone made a mistake.
        gStartBytes[0] = 0;
    }
}

/*static void addAllStandards(UHashtable *standardsHashtable, UErrorCode *status) {
    int32_t i;
    const char *standard;

    for (i = 0; i < gMaxStandards; i++) {
        *status = U_ZERO_ERROR;
        standard = ucnv_getStandard((uint16_t)i, status);
        if (U_FAILURE(*status)) {
            printf("ERROR: ucnv_getStandard() -> %s\n", u_errorName(*status));
        }
        uhash_put(standardsHashtable, (void*)standard, (void*)standard, status);
        if (U_FAILURE(*status)) {
            printf("ERROR: ucnv_getStandard() -> %s\n", u_errorName(*status));
        }
    }
}
*/
