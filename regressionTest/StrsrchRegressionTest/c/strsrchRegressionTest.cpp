/*
**********************************************************************
* Copyright (C) 2008, International Business Machines Corporation
* and others.  All Rights Reserved.
**********************************************************************
*
* File strsrchRegressionTest.cpp
*
**********************************************************************
*/
#include <unicode/ustdio.h>
#include <unicode/strenum.h>
#include <unicode/coll.h>
#include <unicode/tblcoll.h>
#include <unicode/locid.h>
#include <unicode/unistr.h>
#include <unicode/ustring.h>
#include <unicode/uversion.h>
#include <unicode/uclean.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/stat.h>
#include <dirent.h>

#if (U_ICU_VERSION_MAJOR_NUM == 4) && (U_ICU_VERSION_MINOR_NUM == 0) && (U_ICU_VERSION_PATCHLEVEL_NUM == 1)
#define USE_BMS
#include <unicode/colldata.h>
#include <unicode/bmsearch.h>
#else
#include <unicode/stsearch.h>
#endif

#define SSRT_VERSION "1.0"

#define SMALL_BUFFER_SIZE 64
#define LARGE_BUFFER_SIZE 1024

#define isDigit(c) \
    ((c) >= 0x30 && (c) <= 0x39)

static void usage();
static void version();

#ifdef USE_BMS
typedef BoyerMooreSearch SSearch;
#else
typedef StringSearch SSearch;
#endif

static int32_t startTest(const char *inLocale, const char *resultDir, const char *inPattern, const char *textFile);

static int32_t runTestLoop(UnicodeString *pattern, UnicodeString *text, Collator *coll, const char *resultDirPath, const char *localeName, const char *versionStringICU, const char *versionStringUCA, const char *textFile);

static int32_t runTest(
#ifdef USE_BMS
        CollData *collData,
#else
        Collator *coll,
#endif
        UnicodeString *pattern, UnicodeString *text, FILE *pFile);

static int32_t getFileSize(const char *fileName);
static UnicodeString * loadText(const char *textFile);
static UnicodeString * loadPattern(const char *inPattern, const char *textFile);
static int32_t getHexValue(char c);
static FILE * setupResultDirectoryAndLogFile(const char *collatorNameBuffer, const char* localeName, const char *versionStringICU, const char *versionStringUCA, const char *resultDirPath, const char *textFile, const UChar *pttrnBuffer, int32_t pttrnLength);

int main (int argc, char **argv) {
    int result = 0;
    UBool printUsage = FALSE;
    UBool printVersion = FALSE;
    int32_t optind = 0;
    char *arg;
    char resultDir[LARGE_BUFFER_SIZE] = "";
    char textFile[LARGE_BUFFER_SIZE] = "";
    char inPattern[SMALL_BUFFER_SIZE] = "";
    char inLocale[SMALL_BUFFER_SIZE] = "";

    if (argc < 2 || argc > 9) {
        printf("Invalid arguments.\n");
        usage();
        return -1;
    }

    /* parse the options */
    for(optind = 1; optind < argc; ++optind) {
        arg = argv[optind];

        /* result directory */
        if(strcmp(arg, "-d") == 0 || strcmp(arg, "--resultdir") == 0) {
            optind++;
            if (argv[optind][0] == '-' || optind > argc) {
                usage();
                return -1;
            }
            strcpy(resultDir, argv[optind]);
            continue;
        }
        /* text file */
        if(strcmp(arg, "-f") == 0 || strcmp(arg, "--file") == 0) {
            optind++;
            if (argv[optind][0] == '-' || optind > argc) {
                usage();
                return -1;
            }
            strcpy(textFile, argv[optind]);
            continue;
        }
        /* pattern */
        if(strcmp(arg, "-p") == 0 || strcmp(arg, "--pattern") == 0) {
            optind++;
            if (argv[optind][0] == '-' || optind > argc) {
                usage();
                return -1;
            }
            strcpy(inPattern, argv[optind]);
            continue;
        }
        if(strcmp(arg, "-l") == 0 || strcmp(arg, "--locale") == 0) {
            optind++;
            if (argv[optind][0] == '-' || optind > argc) {
               usage();
               return -1;
            }
            strcpy(inLocale, argv[optind]);
            continue;
        }

        /* version info */
        if(strcmp(arg, "-v") == 0 || strcmp(arg, "--version") == 0) {
            printVersion = TRUE;
        }
        /* usage info */
        else if(strcmp(arg, "-h") == 0 || strcmp(arg, "--help") == 0) {
            printUsage = TRUE;;
        }
        /* POSIX.1 says all arguments after -- are not options */
        else if(strcmp(arg, "--") == 0) {
            /* skip the -- */
            ++optind;
            break;
        }
        /* unrecognized option */
        else if(strncmp(arg, "-", strlen("-")) == 0) {
            printf("collationRegressionTest: invalid option -- %s\n", arg+1);
            printUsage = TRUE;;
        }
        /* done with options, display cal */
        else {
            break;
        }
    }

    if (printUsage || resultDir[0] == 0x00 || textFile[0] == 0x00) {
        usage();
        return 0;
    }
    if (printVersion) {
        version();
        return 0;
    }

    result = startTest(inLocale, resultDir, inPattern, textFile);

    u_cleanup();

    return result;
}

/* Usage information */
static void usage() {
    puts("Usage: strsrchRegressionTest -d resultDir -f textFile [-p pattern]");
    puts("");
    puts("Options:");
    puts("  -d, --resultdir   Specify result directory");
    puts("  -f, --file        Specify text file");
    puts("  -p, --pattern     Specify pattern to search for (e.g. hi or '\\u0068\\u0069')");
    puts("  -l, --locale      Specify locale for collator (can be a prefix or starting letter)");
    puts("  -h, --help        Print this message and exit.");
    puts("  -v, --version     Print the version number of collationRegressionTest and exit.");
    puts("");
}

static void version() {
    printf("collationRegressionTest version %s (ICU version %s), created by Michael Ow.\n", SSRT_VERSION, U_ICU_VERSION);
    puts(U_COPYRIGHT_STRING);

}

static int32_t getFileSize(const char *fileName) {
    struct stat file;

    stat(fileName, &file);

    return file.st_size;
}

static int32_t getHexValue(char c) {
    if (c == 'a' || c == 'A') {
        return 10;
    } else if (c == 'b' || c == 'B') {
        return 11;
    } else if (c == 'c' || c == 'C') {
        return 12;
    } else if (c == 'd' || c == 'D') {
        return 13;
    } else if (c == 'e' || c == 'E') {
        return 14;
    } else if (c == 'f' || c == 'F') {
        return 15;
    } else {
        return -1;
    }
}

/* Loads the pattern either from the arguments or the first line of the text file. */
static UnicodeString * loadPattern(const char *inPattern, const char *textFile) {
    UnicodeString * pattern = NULL;
    UChar pttrn[LARGE_BUFFER_SIZE] = { 0x0000 };
    int32_t pttrnLength = 0;
    int32_t patternLength = strlen(inPattern);

    if (patternLength != 0) {
        for (int32_t i = 0; i < patternLength; i++) {
            if (inPattern[i] == '\\') {
                char tmpChar = 0;
                UChar tmpUChar = 0;
                int32_t tmp = 0;

                if (inPattern[++i] == 'u' || inPattern[i] == 'x') {
                    int32_t limit = inPattern[i] == 'x' ? 2 : 4;
                    for (int32_t n = 0; n < limit; n++) {
                        tmpChar  = inPattern[++i];
                        if (isDigit(tmpChar)) {
                            tmp = atoi(&tmpChar);
                        } else {
                            tmp = getHexValue(tmpChar);
                        }

                        if (tmp >= 0) {
                            tmpUChar = tmpUChar << 4 | tmp;
                        } else {
                            /* Error */
                            return NULL;
                        }
                    }
                    pttrn[pttrnLength++] = tmpUChar;
                } else if (inPattern[i] == '\\') {
                    pttrn[pttrnLength++] = inPattern[i];
                } else {
                    /* Error */
                    return NULL;
                }
            } else {
                pttrn[pttrnLength++] = inPattern[i];
            }
        }
    } else {
        /* Load the pattern from the text file. */
        UFILE *pTextFile = NULL;
        if ((pTextFile = u_fopen(textFile, "r", NULL, "UTF-8")) == NULL) {
            printf("Failed to open text file: %s\n", textFile);
            return NULL;
        }
        UChar testFirstChar = 0x0000;
        testFirstChar = u_fgetc(pTextFile);
        if (testFirstChar != 0xffef && testFirstChar != 0xfeff) {
            u_frewind(pTextFile);
        }
        u_fgets(pttrn, LARGE_BUFFER_SIZE, pTextFile);
        pttrnLength = u_strlen(pttrn);
        if (pttrn[pttrnLength-1] == 0x000a) {
            pttrn[pttrnLength-1] = 0x0000;
            pttrnLength--;
        }

        u_fclose(pTextFile);
    }

    pattern = new UnicodeString(pttrn, pttrnLength);

    return pattern;
}

static UnicodeString * loadText(const char *textFile) {
    UnicodeString *text = NULL;
    UFILE *pTextFile = NULL;
    int32_t fileSize= 0;
    int32_t bufferSize = 0;
    UChar *fileBuffer = NULL;
    int32_t fileLength = 0;

    if ((fileSize = getFileSize(textFile)) <= 0) {
        printf("Failed to get the size of text file: %s\n", textFile);
        return NULL;
    }

    fileBuffer = (UChar *)malloc(fileSize);
    if (fileBuffer == NULL) {
        printf("Failed to create text file buffer.\n");
        return NULL;
    }
    bufferSize = fileSize/2;

    if ((pTextFile = u_fopen(textFile, "r", NULL, "UTF-8")) == NULL) {
        printf("Failed to open text file: %s\n", textFile);
        goto cleanupLoadText;
    }

    fileLength = u_file_read(fileBuffer, bufferSize, pTextFile);
    u_fclose(pTextFile);

    if ((text = new UnicodeString(fileBuffer, fileLength)) == NULL) {
        printf("Failed to create UnicodeString from file buffer.\n");
    }

cleanupLoadText:
    free(fileBuffer);

    return text;
}

/* Creates the result directory based on the locale name and collator string. Returns a pointer to the log file. */
static FILE * setupResultDirectoryAndLogFile(const char *collatorNameBuffer, const char* localeName, const char *versionStringICU, const char *versionStringUCA, const char *resultDirPath, const char *textFile, const UChar *pttrnBuffer, int32_t pttrnLength) {
    FILE * pFile = NULL;
    char testResultPath[LARGE_BUFFER_SIZE] = "";
    DIR *testDir = NULL;

    /* Create locale directory. */
    sprintf (testResultPath, "%s/%s", resultDirPath, localeName);
    testDir = opendir(testResultPath);
    if (testDir != NULL) {
        closedir(testDir);
    } else if (mkdir(testResultPath, S_IRWXU) != 0) { /* Create new result directory from locale name and collation string. */
        printf("Unable to create result directory: %s\n", testResultPath);
        return NULL;
    }
    /* Create collation string directory. */
    sprintf (testResultPath, "%s/%s", testResultPath, collatorNameBuffer);
    testDir = opendir(testResultPath);
    if (testDir != NULL) {
        closedir(testDir);
    } else if (mkdir(testResultPath, S_IRWXU) != 0) { /* Create new result directory from locale name and collation string. */
        printf("Unable to create result directory: %s\n", testResultPath);
        return NULL;
    }

    /* Result file name. */
    sprintf(testResultPath, "%s/%s-%s", testResultPath, versionStringICU, versionStringUCA);
#ifdef USE_BMS
    sprintf(testResultPath, "%s-%s", testResultPath, "BMS");
#endif
    pFile = fopen(testResultPath, "w");
    if (pFile != NULL) {
        /* Print text file name and pattern to the result file. */
        fprintf(pFile, "%s \"", textFile);
        for (int32_t n = 0; n < pttrnLength; n++) {
            fprintf(pFile, "\\u%04X", pttrnBuffer[n]);
        }
        fprintf(pFile, "\"\n");
    } else {
        printf("Failed to create log file: %s\n", testResultPath);
    }

    return pFile;
}

/*
 * Loads the pattern and text file and loops through all the available/desirable locales.
 */
static int32_t startTest(const char *inLocale, const char *resultDirPath, const char *inPattern, const char *textFile) {
    int result = 0;
    UErrorCode status = U_ZERO_ERROR;
    UVersionInfo version;
    char versionStringICU[SMALL_BUFFER_SIZE] = "";
    char versionStringUCA[SMALL_BUFFER_SIZE] = "";
    const Locale *locales = NULL;
    int32_t localesCount = 0;
    UnicodeString *text = NULL;
    UnicodeString *pattern = NULL;
    UBool getUCAVersion = TRUE;

    if ((pattern = loadPattern(inPattern, textFile)) == NULL) {
        printf("Failed loading pattern.\n");
        return -1;
    }

    if ((text = loadText(textFile)) == NULL) {
        printf("Failed loading text file:  %s\n", textFile);
        goto cleanup;
    }

    /* Get all the locales available for collation on this system. */
    locales = Collator::getAvailableLocales(localesCount);
    if (locales == NULL || localesCount == 0) {
        printf("Failed getting available locales.\n");
        result = -1;
        goto cleanup;
    }

    /* Get the ICU version number. */
    u_getVersion(version);
    u_versionToString(version, versionStringICU);
    for (int32_t i = 0; i < localesCount && result == 0; i++) {
        const char *localeName = locales[i].getName();
        /* If a locale is given, only use the locale that is "common" to the one given. */
        if (strstr(localeName, inLocale) == NULL) {
            continue;
        }

        Collator *coll = Collator::createInstance(locales[i], status);
        if (U_FAILURE(status)) {
            printf("Unable to create collator with locale: %s\n", localeName);
            result = -1;
            break;
        }

        if (getUCAVersion) {
            /* Get the UCA version number. */
            ucol_getUCAVersion(((RuleBasedCollator *)coll)->getUCollator(), version);
            u_versionToString(version, versionStringUCA);

            getUCAVersion = FALSE;
        }

        if (runTestLoop(pattern, text, coll, resultDirPath, localeName, versionStringICU, versionStringUCA, textFile) != 0) {
            result = -1;
            printf("Error iterating through collation attributes\n");
        }

        delete coll;
    }

cleanup:
    delete text;
    delete pattern;

    return result;
}

/*
 * Iterates over the desired collator combinations and calls runTest to do the actual searching.
 */
static int32_t runTestLoop(UnicodeString *pattern, UnicodeString *text, Collator *coll, const char *resultDirPath, const char *localeName, const char *versionStringICU, const char *versionStringUCA, const char *textFile) {
    int32_t result = 0;
#ifdef USE_BMS
    CollData *collData = NULL;
#endif
    UErrorCode status = U_ZERO_ERROR;
    FILE *pFile = NULL;

    for (int32_t b = 0; b < 2; b++) { /* UCOL_ALTERNATE_HANDLING */
        if (b > 0) {
            coll->setAttribute(UCOL_ALTERNATE_HANDLING, UCOL_NON_IGNORABLE, status);
        } else {
            coll->setAttribute(UCOL_ALTERNATE_HANDLING, UCOL_SHIFTED, status);
        }
        if (U_FAILURE(status)) {
            printf("Error setting alternate handling collation attribute.\n");
            return -1;
        }

        for (int32_t e = 0; e < 2; e++) { /* UCOL_DECOMPOSITION_MODE */
            if (e > 0) {
                coll->setAttribute(UCOL_DECOMPOSITION_MODE, UCOL_ON, status);
            } else {
                coll->setAttribute(UCOL_DECOMPOSITION_MODE, UCOL_OFF, status);
            }
            if (U_FAILURE(status)) {
                printf("Error setting decomposition mode collation attribute.\n");
                return -1;
            }

            for (int32_t f = 0; f < 5; f++) { /* UCOL_STRENGTH */
                switch (f) {
                case 0:
                    coll->setAttribute(UCOL_STRENGTH, UCOL_PRIMARY, status);
                    break;
                case 1:
                    coll->setAttribute(UCOL_STRENGTH, UCOL_SECONDARY, status);
                    break;
                case 2:
                    coll->setAttribute(UCOL_STRENGTH, UCOL_TERTIARY, status);
                    break;
                case 3:
                    coll->setAttribute(UCOL_STRENGTH, UCOL_QUATERNARY, status);
                    break;
                case 4:
                    coll->setAttribute(UCOL_STRENGTH, UCOL_IDENTICAL, status);
                    break;
                default :
                    /* Should never occur. */
                    break;
                }
                if (U_FAILURE(status)) {
                    printf("Error setting collation strength attribute.\n");
                    return -1;
                }

                for (int32_t g = 0; g < 2; g++) { /* UCOL_HIRAGANA_QUATERNARY_MODE */
                    if (g > 0) {
                        coll->setAttribute(UCOL_HIRAGANA_QUATERNARY_MODE, UCOL_ON, status);
                    } else {
                        coll->setAttribute(UCOL_HIRAGANA_QUATERNARY_MODE, UCOL_OFF, status);
                    }
                    if (U_FAILURE(status)) {
                        printf("Error setting Hiragana quaternary mode collation attribute.\n");
                        return -1;
                    }

                    for (int32_t d = 0; d < 2; d++) { /* UCOL_CASE_LEVEL */
                        if (coll->getAttribute(UCOL_STRENGTH, status) <= UCOL_SECONDARY) {
                            if (d > 0) {
                                coll->setAttribute(UCOL_CASE_LEVEL, UCOL_ON, status);
                            } else {
                                coll->setAttribute(UCOL_CASE_LEVEL, UCOL_OFF, status);
                            }
                            if (U_FAILURE(status)) {
                                printf("Error setting case level collation attribute.\n");
                                return -1;
                            }
                        } else {
                            /* Run loop once and then get out. */
                            d = 2;
                        }
#ifdef USE_BMS
                        collData = CollData::open((UCollator *)(((RuleBasedCollator *)coll)->getUCollator()), status);
                        if (U_FAILURE(status)) {
                            printf("Failed to create CollData object.\n");
                            result = -1;
                        }
#endif
                        if (result == 0) {
                            char collatorNameBuffer[SMALL_BUFFER_SIZE] = "";

                            ucol_getShortDefinitionString(((RuleBasedCollator *)coll)->getUCollator(), localeName, collatorNameBuffer, SMALL_BUFFER_SIZE, &status);
                            if (U_FAILURE(status)) {
                                printf("Error getting collator string.\n");
                                return -1;
                            }

                            pFile = setupResultDirectoryAndLogFile(collatorNameBuffer, localeName, versionStringICU, versionStringUCA, resultDirPath, textFile, pattern->getBuffer(), pattern->length());
                            if (pFile == NULL) {
                                return -1;
                            }
                        }

                        result = runTest(
#ifdef USE_BMS
                                    collData,
#else
                                    coll,
#endif
                                    pattern, text, pFile);
                        fclose(pFile);
                        if (result != 0) {
                            return result;
                        }

#ifdef USE_BMS
                        CollData::close(collData);
                        CollData::freeCollDataCache();
#endif
                    } /* UCOL_CASE_LEVEL */
                } /* UCOL_HIRAGANA_QUATERNARY_MODE */
            } /* UCOL_STRENGTH */
        } /* UCOL_DECOMPOSITION_MODE */
    } /* UCOL_ALTERNATE_HANDLING */

    return result;
}

/*
 * Creates the String Search object and performs the search writing the results to the log file.
 */
static int32_t runTest(
#ifdef USE_BMS
        CollData *collData,
#else
        Collator *coll,
#endif
        UnicodeString *pattern, UnicodeString *text, FILE *pFile) {
    UErrorCode status = U_ZERO_ERROR;
    int32_t result = 0;
    SSearch *strsrch = NULL;

#ifdef USE_BMS
    strsrch = new BoyerMooreSearch(collData, *pattern, text, status);
#else
    strsrch = new StringSearch(*pattern, *text, (RuleBasedCollator *)coll, NULL, status);
#endif
    if (strsrch == NULL || U_FAILURE(status)) {
        return -1;
    }

    /* Search text forward. */
#ifdef USE_BMS
    int32_t length, start, end;
    UBool found;

    found = strsrch->search(0, start, end);
    while (found) {
        length = end - start;
        fprintf(pFile, "%d %d\n", start, length);

        found = strsrch->search(start + length, start, end);
    }
#else
    int32_t match = strsrch->first(status);
    if (U_FAILURE(status)) {
        printf("Error finding first match going forward.\n");
        result = -1;
        goto exitTest;
    }
    while (match != USEARCH_DONE) {
        fprintf(pFile, "%d %d\n", match, strsrch->getMatchedLength());
        match = strsrch->next(status);
        if (U_FAILURE(status)) {
            printf("Error finding next match going forward.\n");
            result = -1;
            goto exitTest;
        }
    }
#endif

#if 0
    /* Skip backwards searching */
    /* Search text backwards. */
    match = strsrch->last(status);
    if (U_FAILURE(status)) {
        printf("Error finding first match going backwards.\n");
        result = -1;
        goto exitTest;
    }
    while (match != USEARCH_DONE) {
        fprintf(pFile, "%d %d\n", match, strsrch->getMatchedLength());
        match = strsrch->previous(status);
        if (U_FAILURE(status)) {
            printf("Error finding next match going backward.\n");
            result = -1;
            goto exitTest;
        }
    }
#endif

exitTest:
    delete strsrch;

    return result;
}
