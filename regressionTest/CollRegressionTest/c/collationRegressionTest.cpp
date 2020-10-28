/*
**********************************************************************
* Copyright (C) 2008, International Business Machines Corporation
* and others.  All Rights Reserved.
**********************************************************************
*
* File collationRegressionTest.cpp
*
**********************************************************************
*/
#include <unicode/ustdio.h>
#include <unicode/sortkey.h>
#include <unicode/coll.h>
#include <unicode/tblcoll.h>
#include <unicode/ustring.h>
#include <unicode/uversion.h>
#include <unicode/uclean.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/stat.h>
#include <dirent.h>

#define CRT_VERSION "1.0"

#define SMALL_BUFFER_SIZE 64
#define LARGE_BUFFER_SIZE 1024

static void usage();
static void version();

static int32_t startTest(const char *resultDir, const char *dataFile);
static int32_t runTestLoop(Collator *coll, const char *resultDirPath, const char *localeName, const char *versionStringICU, const char *versionStringUCA, UFILE *pDataFile);
static int32_t runTest(Collator *coll, const char *resultDirPath, const char *localeName, const char *versionStringICU, const char *versionStringUCA, UFILE *pDataFile);


int main (int argc, char **argv) {
    int result = 0;
    UBool printUsage = false;
    UBool printVersion = false;
    int32_t optind = 0;
    char *arg;
    char resultDir[LARGE_BUFFER_SIZE] = "";
    char dataFile[LARGE_BUFFER_SIZE] = "";

    if (argc < 2 || argc > 5) {
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
        /* data file */
        if(strcmp(arg, "-f") == 0 || strcmp(arg, "--file") == 0) {
            optind++;
            if (argv[optind][0] == '-' || optind > argc) {
                usage();
                return -1;
            }
            strcpy(dataFile, argv[optind]);
            continue;
        }
        /* version info */
        if(strcmp(arg, "-v") == 0 || strcmp(arg, "--version") == 0) {
            printVersion = true;
        }
        /* usage info */
        else if(strcmp(arg, "-h") == 0 || strcmp(arg, "--help") == 0) {
            printUsage = true;;
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
            printUsage = true;;
        }
        /* done with options, display cal */
        else {
            break;
        }
    }

    if (printUsage) {
        usage();
        return 0;
    }
    if (printVersion) {
        version();
        return 0;
    }

    result = startTest(resultDir, dataFile);

    u_cleanup();

    return result;
}

/* Usage information */
static void usage() {
    puts("Usage: collationRegressionTest -d resultDir -f dataFile");
    puts("");
    puts("Options:");
    puts("  -d, --resultdir   Specify result directory");
    puts("  -f, --file        Specify data file");
    puts("  -h, --help        Print this message and exit.");
    puts("  -v, --version     Print the version number of collationRegressionTest and exit.");
    puts("");
}

static void version() {
    printf("collationRegressionTest version %s (ICU version %s), created by Michael Ow.\n", CRT_VERSION, U_ICU_VERSION);
    puts(U_COPYRIGHT_STRING);

}

static int32_t startTest(const char *resultDirPath, const char *dataFile) {
    int result = 0;
    UErrorCode status = U_ZERO_ERROR;
    UBool getUCAVersion = true;
    UVersionInfo version;
    char versionStringICU[SMALL_BUFFER_SIZE] = "";
    char versionStringUCA[SMALL_BUFFER_SIZE] = "";
    const Locale *locales = NULL;
    int32_t localesCount = 0;
    UFILE *pDataFile = NULL;

    /* Get all the locales available for collation on this system. */
    locales = Collator::getAvailableLocales(localesCount);
    if (locales == NULL || localesCount == 0) {
        printf("Failed getting available locales.\n");
        return -1;
    }

    if ((pDataFile = u_fopen(dataFile, "r", NULL, "UTF-16BE")) == NULL) {
        printf("Failed to open data file.\n");
        return -1;
    }

    /* Get the ICU version number. */
    u_getVersion(version);
    u_versionToString(version, versionStringICU);

    for (int32_t i = 0; i < localesCount && result == 0; i++) {
        const char *localeName = locales[i].getName();
        
        printf("Running test on Locale: %s (%d out of %d)\n", localeName, i+1, localesCount);

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

            getUCAVersion = false;
        }

        if (runTestLoop(coll, resultDirPath, localeName, versionStringICU, versionStringUCA, pDataFile) != 0) {
            result = -1;
            printf("Error iterating through collation attributes\n");
        }

        delete coll;
    }

    u_fclose(pDataFile);

    return result;
}

static int32_t runTestLoop(Collator *coll, const char *resultDirPath, const char *localeName, const char *versionStringICU, const char *versionStringUCA, UFILE *pDataFile) {
    int32_t result = 0;
    UErrorCode status = U_ZERO_ERROR;

    for (int32_t a = 0; a < 2; a++) { /* UCOL_FRENCH_COLLATION */
        if (a > 0) {
            coll->setAttribute(UCOL_FRENCH_COLLATION, UCOL_ON, status);
        } else {
            coll->setAttribute(UCOL_FRENCH_COLLATION, UCOL_OFF, status);
        }
        if (U_FAILURE(status)) {
            printf("Error setting French collation attribute.\n");
            result = -1;
            return result;
        }

        for (int32_t b = 0; b < 2; b++) { /* UCOL_ALTERNATE_HANDLING */
            if (b > 0) {
                coll->setAttribute(UCOL_ALTERNATE_HANDLING, UCOL_NON_IGNORABLE, status);
            } else {
                coll->setAttribute(UCOL_ALTERNATE_HANDLING, UCOL_SHIFTED, status);
            }
            if (U_FAILURE(status)) {
                printf("Error setting alternate handling collation attribute.\n");
                result = -1;
                return result;
            }

            for (int32_t c = 0; c < 3; c++) { /* UCOL_CASE_FIRST */
                if (c == 0) {
                    coll->setAttribute(UCOL_CASE_FIRST, UCOL_OFF, status);
                } else if (c == 1) {
                    coll->setAttribute(UCOL_CASE_FIRST, UCOL_UPPER_FIRST, status);
                } else {
                    coll->setAttribute(UCOL_CASE_FIRST, UCOL_LOWER_FIRST, status);
                }
                if (U_FAILURE(status)) {
                    printf("Error setting case first collation attribute.\n");
                    result = -1;
                    return result;
                }

                for (int32_t d = 0; d < 2; d++) { /* UCOL_CASE_LEVEL */
                    if (d > 0) {
                        coll->setAttribute(UCOL_CASE_LEVEL, UCOL_ON, status);
                    } else {
                        coll->setAttribute(UCOL_CASE_LEVEL, UCOL_OFF, status);
                    }
                    if (U_FAILURE(status)) {
                        printf("Error setting case level collation attribute.\n");
                        result = -1;
                        return result;
                    }

                    for (int32_t e = 0; e < 2; e++) { /* UCOL_DECOMPOSITION_MODE */
                        if (e > 0) {
                            coll->setAttribute(UCOL_DECOMPOSITION_MODE, UCOL_ON, status);
                        } else {
                            coll->setAttribute(UCOL_DECOMPOSITION_MODE, UCOL_OFF, status);
                        }
                        if (U_FAILURE(status)) {
                            printf("Error setting decomposition mode collation attribute.\n");
                            result = -1;
                            return result;
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
                                result = -1;
                                return result;
                            }

                            for (int32_t g = 0; g < 2; g++) { /* UCOL_HIRAGANA_QUATERNARY_MODE */
                                if (g > 0) {
                                    coll->setAttribute(UCOL_HIRAGANA_QUATERNARY_MODE, UCOL_ON, status);
                                } else {
                                    coll->setAttribute(UCOL_HIRAGANA_QUATERNARY_MODE, UCOL_OFF, status);
                                }
                                if (U_FAILURE(status)) {
                                    printf("Error setting Hiragana quaternary mode collation attribute.\n");
                                    result = -1;
                                    return result;
                                }

                                for (int32_t h = 0; h < 2; h++) { /* UCOL_NUMERIC_COLLATION */
                                    if (h > 0) {
                                        coll->setAttribute(UCOL_NUMERIC_COLLATION, UCOL_ON, status);
                                    } else {
                                        coll->setAttribute(UCOL_NUMERIC_COLLATION, UCOL_OFF, status);
                                    }
                                    if (U_FAILURE(status)) {
                                        printf("Error setting numeric collation attribute.\n");
                                        result = -1;
                                        return result;
                                    }

                                    /* Now run the test on the collator. */
                                    if ((result = runTest(coll, resultDirPath, localeName, versionStringICU, versionStringUCA, pDataFile)) != 0) {
                                        return result;
                                    }
                                } /* UCOL_NUMERIC_COLLATION */
                            } /* UCOL_HIRAGANA_QUATERNARY_MODE */
                        } /* UCOL_STRENGTH */
                    } /* UCOL_DECOMPOSITION_MODE */
                } /* UCOL_CASE_LEVEL */
            } /* UCOL_CASE_FIRST */
        } /* UCOL_ALTERNATE_HANDLING */
    } /* UCOL_FRENCH_COLLATION */

    return result;
}

static int32_t runTest(Collator *coll, const char *resultDirPath, const char *localeName, const char *versionStringICU, const char *versionStringUCA, UFILE *pDataFile) {
    UErrorCode status = U_ZERO_ERROR;
    int32_t result = 0;
    FILE *pFile = NULL;
    char collatorNameBuffer[SMALL_BUFFER_SIZE] = "";
    char testResultPath[LARGE_BUFFER_SIZE] = "";
    UChar testDataItem[SMALL_BUFFER_SIZE] = { 0x0000 };
    CollationKey collKey;
    const uint8_t *collKeyArray = NULL;
    int32_t collKeyArrayLength = 0;
    DIR *testDir = NULL;
    int32_t count = 0;

    ucol_getShortDefinitionString(((RuleBasedCollator *)coll)->getUCollator(), localeName, collatorNameBuffer, SMALL_BUFFER_SIZE, &status);
    if (U_FAILURE(status)) {
        printf("Error getting collator string.\n");
        result = -1;
        goto exitTest;
    }
    /* Create locale directory. */
    sprintf (testResultPath, "%s/%s", resultDirPath, localeName);
    testDir = opendir(testResultPath);
    if (testDir != NULL) {
        closedir(testDir);
    } else if (mkdir(testResultPath, S_IRWXU) != 0) { /* Create new result directory from locale name and collation string. */
        printf("Unable to create result directory: %s\n", testResultPath);
        result = -1;
        goto exitTest;
    }
    /* Create collation string directory. */
    sprintf (testResultPath, "%s/%s", testResultPath, collatorNameBuffer);
    testDir = opendir(testResultPath);
    if (testDir != NULL) {
        closedir(testDir);
    } else if (mkdir(testResultPath, S_IRWXU) != 0) { /* Create new result directory from locale name and collation string. */
        printf("Unable to create result directory: %s\n", testResultPath);
        result = -1;
        goto exitTest;
    }

    /* Test result file name. */
    sprintf(testResultPath, "%s/%s-%s-ICU4C", testResultPath, versionStringICU, versionStringUCA);
    pFile = fopen(testResultPath, "w");
    if (pFile != NULL) {
        while (!u_feof(pDataFile)) {
            if ((u_fgets(testDataItem, sizeof(testDataItem)/sizeof(UChar), pDataFile)) != NULL) {
                coll->getCollationKey(testDataItem, u_strlen(testDataItem) -1, collKey, status);
                if (U_FAILURE(status)) {
                    printf("Error getting collation key.\n");
                    result = -1;
                    goto exitTest;
                }

                collKeyArray = collKey.getByteArray(collKeyArrayLength);

                fprintf(pFile, "[%d] ", ++count);
                for (int32_t i = 0; i < collKeyArrayLength; i++) {
                    fprintf(pFile, "\\x%02X", collKeyArray[i]);
                }
                fprintf(pFile, "\n");
            } else {
                printf("Error reading data file.\n");
                result = -1;
                goto exitTest;
            }
        }
    } else {
        printf("Error creating file: %s\n", testResultPath);
        result = -1;
    }

exitTest:
    /* Go back to the beginning of the data file. */
    u_frewind(pDataFile);

    if (pFile != NULL) {
        fclose(pFile);
    }
    return result;
}
