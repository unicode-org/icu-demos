/*
*******************************************************************************
*                                                                             *
* COPYRIGHT:                                                                  *
*   (C) Copyright International Business Machines Corporation, 1999           *
*   Licensed Material - Program-Property of IBM - All Rights Reserved.        *
*   US Government Users Restricted Rights - Use, duplication, or disclosure   *
*   restricted by GSA ADP Schedule Contract with IBM Corp.                    *
*                                                                             *
*******************************************************************************
*   file name:  gennames.c
*   encoding:   US-ASCII
*   tab size:   8 (not used)
*   indentation:4
*
*   created on: 1999nov01
*   created by: Markus W. Scherer
*
*   This program reads a binary file and creates a C source code file
*   with a byte array that contains the data of the binary file.
*/

#ifdef WIN32
#   include <windows.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include "cmemory.h"
#include "cstring.h"
#include "unicode/utypes.h"

static uint16_t column=0xffff;

/* prototypes --------------------------------------------------------------- */

static char *
getLongPathname(char *pathname);

static void
writeCCode(const char *filename);

static void
getOutFilename(const char *inFilename, char *outFilename, char *entryName);

static const char *
findBasename(const char *filename);

static void
write8(FILE *out, uint8_t byte);

/* -------------------------------------------------------------------------- */

extern int
main(int argc, char *argv[]) {
    if(argc<=1) {
        fprintf(stderr,
            "usage: %s filename\n"
            "\tread the binary input file and \n"
            "\tcreate a .c file with a byte array that contains the input file's data\n",
            argv[0]);
    } else {
        writeCCode(getLongPathname(argv[1]));
    }

    return 0;
}

static char *
getLongPathname(char *pathname) {
#ifdef WIN32
    /* anticipate problems with "short" pathnames */
    static WIN32_FIND_DATA info;
    HANDLE file=FindFirstFile(pathname, &info);
    if(file!=INVALID_HANDLE_VALUE) {
        if(info.cAlternateFileName[0]!=0) {
            /* this file has a short name, get and use the long one */
            const char *basename=findBasename(pathname);
            if(basename!=pathname) {
                /* prepend the long filename with the original path */
                icu_memmove(info.cFileName+(basename-pathname), info.cFileName, icu_strlen(info.cFileName)+1);
                icu_memcpy(info.cFileName, pathname, basename-pathname);
            }
            pathname=info.cFileName;
        }
        FindClose(file);
    }
#endif
    return pathname;
}

static void
writeCCode(const char *filename) {
    char buffer[4096], entry[40];
    FILE *in, *out;
    size_t i, length;

    in=fopen(filename, "rb");
    if(in==NULL) {
        fprintf(stderr, "genccode: unable to open input file %s\n", filename);
        exit(U_FILE_ACCESS_ERROR);
    }

    getOutFilename(filename, buffer, entry);
    out=fopen(buffer, "w");
    if(out==NULL) {
        fprintf(stderr, "genccode: unable to open output file %s\n", buffer);
        exit(U_FILE_ACCESS_ERROR);
    }

    fprintf(out,
        "#include \"unicode/utypes.h\"\n"
        "U_CAPI const uint8_t U_EXPORT2 %s[]={\n",
        entry);

    for(;;) {
        length=fread(buffer, 1, sizeof(buffer), in);
        if(length==0) {
            break;
        }
        for(i=0; i<length; ++i) {
            write8(out, (uint8_t)buffer[i]);
        }
    }

    fprintf(out, "\n};\n");

    if(ferror(in)) {
        fprintf(stderr, "genccode: file read error while generating from file %s\n", filename);
        exit(U_FILE_ACCESS_ERROR);
    }

    if(ferror(out)) {
        fprintf(stderr, "genccode: file write error while generating from file %s\n", filename);
        exit(U_FILE_ACCESS_ERROR);
    }

    fclose(out);
    fclose(in);
}

static void
getOutFilename(const char *inFilename, char *outFilename, char *entryName) {
    const char *basename=findBasename(inFilename), *suffix=icu_strrchr(basename, '.');

    /* copy path */
    while(inFilename<basename) {
        *outFilename++=*inFilename++;
    }

    if(suffix==NULL) {
        /* the filename does not have a suffix */
        icu_strcpy(entryName, inFilename);
        icu_strcpy(outFilename, inFilename);
        icu_strcat(outFilename, ".c");
    } else {
        /* copy basename */
        while(inFilename<suffix) {
            *outFilename++=*entryName++=*inFilename++;
        }

        /* replace '.' by '_' */
        *outFilename++=*entryName++='_';
        ++inFilename;

        /* copy suffix */
        while(*inFilename!=0) {
            *outFilename++=*entryName++=*inFilename++;
        }

        *entryName=0;

        /* add ".c" */
        *outFilename++='.';
        *outFilename++='c';
        *outFilename=0;
    }
}

static const char *
findBasename(const char *filename) {
    const char *basename, *basename2;

    basename=icu_strrchr(filename, '/');
    if(basename==NULL) {
        basename=filename;
    } else {
        ++basename;
    }

    basename2=icu_strrchr(basename, '\\');
    if(basename2!=NULL) {
        return basename2+1;
    } else {
        return basename;
    }
}

static void
write8(FILE *out, uint8_t byte) {
    if(column==0xffff) {
        /* first byte */
        fprintf(out, "%u", byte);
        column=1;
    } else if(column<16) {
        fprintf(out, ",%u", byte);
        ++column;
    } else {
        fprintf(out, ",\n%u", byte);
        column=1;
    }
}
