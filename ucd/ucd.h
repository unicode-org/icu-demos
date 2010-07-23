/* Copyright (c) 2010 IBM Corporation and Others. All Rights Reserved. */

#ifndef _UCD_H
#define _UCD_H

#include "unicode/utypes.h"
#include <unicode/ustdio.h>

#include "ucdresult.h"


/* === Special things === */
/**
 * Startup commands
 */
#define UCD_RCFILE ".ucdrc"

/* === Parser defines === */
// controls/space
#define UCD_CMD_CR        0x0A
#define UCD_CMD_LF        0x0D

// Numbers
#define UCD_CMD_0         0x30

// uppercase
#define UCD_CMD_UI        0x49
#define UCD_CMD_UU        0x55

// lowercase
#define UCD_CMD_H         0x68
#define UCD_CMD_P         0x70
#define UCD_CMD_Q         0x71
#define UCD_CMD_I         0x69
#define UCD_CMD_U         0x75

// symbols
#define UCD_CMD_DBLQUOTE  0x22
#define UCD_CMD_HASH      0x23
#define UCD_CMD_PLUS      0x2B
#define UCD_CMD_DOT       0x2E
#define UCD_CMD_SLASH     0x2F
#define UCD_CMD_QUESTION  0x3F
#define UCD_CMD_LEFTSQ    0x5B
#define UCD_CMD_BACKSLASH 0x5C
#define UCD_CMD_RIGHTSQ   0x5D


/* === Functions === */

/**
 * Parse a file
 */
void ucd_parseFile(const char *fileName, UErrorCode &status);
void ucd_parseFile(UFILE *f, const char *fileName, UErrorCode &status);

/**
 * Parse a command (from command line, arg, stack, etc)
 */
void ucd_parseCommand(const UChar *cmd, UErrorCode &status);

/**
 * Show info
 */
void ucd_cmd_info(const UChar *str, int32_t len, UErrorCode &status);
void ucd_cmd_help(UErrorCode &status);


void set_printed(void);
void clear_if_printed(void);

#endif
