/*
*******************************************************************************
*
*   Copyright (C) 2003-2012, International Business Machines
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
#include <strings.h>
#include <string.h>
#include <ctype.h>

int32_t gMaxStandards;
UBool gShowStartBytes = false;
char gCurrConverter[UCNV_MAX_CONVERTER_NAME_LENGTH] = "";
char gStartBytes[MAX_BYTE_SIZE] = "";
std::set<std::string> *gStandardsSelected = NULL;
const char *gScriptName = NULL;
UBool gShowUnicodeSet = false;
UBool gShowLanguages = false;
UBool gShowUnavailable = false;

const char ALL[]="ALL";

U_CFUNC const char *getStandardOptionsURL(UErrorCode *status) {
    static char optionsURL[1024] = "\x00\x00\x00";
    static char *standards = NULL;

    if (!standards) {
        int32_t pos = -1;
        const char *standard;
        int32_t len = 0;

        for(std::set<std::string>::iterator e = gStandardsSelected->begin();
            e!=gStandardsSelected->end();
            ++e) {
            standard = e->c_str();
            if (len == 0) {
                len += sprintf(optionsURL+len, "s=%s", standard);
            }
            else {
                len += sprintf(optionsURL+len, OPTION_SEP_STR"s=%s", standard);
            }
            if (*standard == 0) {
                /* Special case for when browsers are too smart, like Opera */
                len += sprintf(optionsURL+len, "-");
            }
        }
        standards = optionsURL;
    }

    return optionsURL;
}

/*
 Only insert standards that we recognize into the hashtable
 All other standards could be bad/malicious data
*/
static void addStandard(const char *newStandard, int32_t nameSize, UErrorCode *status) {
    int32_t i;
    const char *standard;

    if (nameSize <= 0 || *newStandard == '-') {
      gStandardsSelected->insert(
                std::string(
                            ucnv_getStandard((uint16_t)(gMaxStandards-1), status)));
      if (U_FAILURE(*status)) {
        printf("ERROR: uhash_put() -> %s\n", u_errorName(*status));
      }
      return;
    }
    if (strncasecmp(ALL, newStandard, nameSize) == 0) {
      gStandardsSelected->insert(std::string(ALL));
      return;
    }
    for (i = 0; i < gMaxStandards; i++) {
        *status = U_ZERO_ERROR;
        standard = ucnv_getStandard((uint16_t)i, status);
        if (U_FAILURE(*status)) {
            printf("ERROR: ucnv_getStandard() -> %s\n", u_errorName(*status));
        }
        if (strncasecmp(standard, newStandard, nameSize) == 0) {
          gStandardsSelected->insert(std::string(standard));
            return;  // I'm done, I already found it.
        }
    }
    printf("WARNING: Ignoring unknown standard %s<br>\n", newStandard);
}

static inline int32_t
getHexValue(char c) {
    if('0'<=c && c<='9') {
        return (int32_t)(c-'0');
    } else if('a'<=c && c<='f') {
        return (int32_t)(c-'a'+10);
    } else if('A'<=c && c<='F') {
        return (int32_t)(c-'A'+10);
    } else {
        return -1;
    }
}

/* parse percent-escaped input into a byte array; does not NUL-terminate */
static int32_t
parseEscaped(const char *s, const char *sLimit, char *dest, int32_t destCapacity) {
    int32_t i, hi, lo;
    char c;

    i=0;
    while(s < sLimit) {
        if(i==destCapacity) {
            /* Gave us too much. too bad. */
            break;
        }

        c=*(s++);
        if (c=='%') {
            /* Get the next two bytes, if possible */
            if (s+1 < sLimit && (hi=getHexValue(*s))>=0 && (lo=getHexValue(s[1]))>=0) {
                dest[i++]=(char)((hi<<4)|lo);
                s+=2;
            }
            else {
                /* This looks like garbage */
                return 0;
            }
        } else if(c=='+') {
            dest[i++]=' ';
        } else {
            dest[i++]=c;
        }
    }

    return i;
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
            int32_t len = parseEscaped(nextVal, nextOpt, gCurrConverter, sizeof(gCurrConverter)-1);
//            strncpy(gCurrConverter, nextVal, nextOpt - nextVal);
            gCurrConverter[len] = 0;   // NULL terminate for safety

            if (len > 0) {
                UConverter *cnv = ucnv_open(gCurrConverter, &myStatus);
                if (U_SUCCESS(myStatus)) {
                    strcpy(gCurrConverter, ucnv_getName(cnv, &myStatus));
                    ucnv_close(cnv);
                }
                gCurrConverter[UCNV_MAX_CONVERTER_NAME_LENGTH-1] = 0;   // NULL terminate for safety
            }
            /* else someone gave us garbage */
        }
        else if (strncmp(src, "b=", 2) == 0) {
            const char *strItr = gStartBytes;
            strncpy(gStartBytes, nextVal, nextOpt - nextVal);
            gStartBytes[sizeof(gStartBytes)-1] = 0;   // NULL terminate for safety
            gStartBytes[((strlen(gStartBytes)>>1)<<1)] = 0;// Make it an even number of chars
            gShowStartBytes = true;
            while (*strItr) {
                if (!isxdigit(*strItr)) {
                    // Bad data! Ignore the whole thing
                    gStartBytes[0] = 0;
                    break;
                }
                strItr++;
            }
        }
        else if (strncmp(src, SHOW_UNICODESET, strlen(SHOW_UNICODESET)) == 0) {
            /* Show the Unicode Set */
            gShowUnicodeSet = true;
        }
        else if (strncmp(src, SHOW_UNAVAILABLE, strlen(SHOW_UNAVAILABLE)) == 0) {
            /* Show unavailable converters */
            gShowUnavailable = true;
        }
        else if (strncmp(src, SHOW_LOCALES, strlen(SHOW_LOCALES)) == 0) {
            /* Show the list of languages */
            gShowLanguages = true;
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

