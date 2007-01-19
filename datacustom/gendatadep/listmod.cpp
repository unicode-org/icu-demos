/*
*******************************************************************************
*
*   Copyright (C) 2006-2007, International Business Machines
*   Corporation and others.  All Rights Reserved.
*
*******************************************************************************
*   created on: 2006Oct28
*   created by: George Rhoten
*/

#include "listmod.h"
#include "package.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

typedef enum {
    ITEM_HIDE,
    ITEM_ADD,
    ITEM_NAME,
    ITEM_SIZE,
    ITEM_COUNT
} LineTypes;

/* Trim off all line endings. */
static char *trim(char *str) {
    int32_t lastIdx = (int32_t)(strlen(str) - 1);
    while (lastIdx > 0 && (str[lastIdx] == '\r' || str[lastIdx] == '\n')) {
        str[lastIdx--] = 0;
    }
    return str;
}

void ListModifier::parseModificationList(const char *filename, UVector *hiddenItems, UVector *items) {
    FILE *file = fopen(filename, "r");
    int32_t lineNum = 1;
    char line[1024];
    char *str, *prevStr;
    char *itemStr[ITEM_COUNT];
    UErrorCode status;

    if (file == NULL) {
        fprintf(stderr, "Can't open %s\n", filename);
        exit(U_FILE_ACCESS_ERROR);
    }

    while (fgets(line, sizeof(line), file) != NULL) {
        status = U_ZERO_ERROR;
        trim(line);
        if (line[0] != '#' && line[0] != 0) {
            prevStr = line;
            str = prevStr;
            for (int32_t idx = ITEM_HIDE; idx < ITEM_COUNT; idx++) {
                str = strchr(str, ',');
                if (str == NULL) {
                    fprintf(stderr, "Missing required field on line %d\n", lineNum);
                    exit(U_PARSE_ERROR);
                }
                str[0] = 0;
                //fprintf(stderr, "%s", prevStr);
                itemStr[idx] = prevStr;
                str++;
                prevStr = str;
            }
            //fprintf(stderr, "%s,%s,%s,%s\n", itemStr[ITEM_HIDE], itemStr[ITEM_ADD], itemStr[ITEM_NAME], itemStr[ITEM_SIZE]);
            if (itemStr[ITEM_HIDE][0] == '1') {
                hiddenItems->addElement(strdup(itemStr[ITEM_NAME]), status);
            }
            if (itemStr[ITEM_ADD][0] == '1') {
                Item *item = (Item *)malloc(sizeof(struct Item));

                memset(item, 0, sizeof(struct Item));
                item->name = strdup(itemStr[ITEM_NAME]);
                sscanf(itemStr[ITEM_SIZE], "%d", &item->length);
                if (item->length == 0) {
                    fprintf(stderr, "Warning item does not have a length on line %d\n", lineNum);
                }
                items->addElement(item, status);
            }
        }
        lineNum++;
    }
    fclose(file);
}
