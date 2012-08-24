/* Copyright (c) 2010-2012 IBM Corporation and Others. All Rights Reserved. */


#include "../iucsamples.h"
#include <unicode/ures.h>
#include <unicode/putil.h>
#include <unicode/ustdio.h>
#include <stdio.h>
static void show(void);
static void printString(const UChar* str);

void printString(const UChar* str) {
  u_printf("thing=`%S'\n", str);
}

void show(void)
{
  /* BEGIN EXAMPLE */
  UErrorCode status = U_ZERO_ERROR;
  int32_t length;
  UResourceBundle *resourceBundle = NULL;
  const UChar *thing;
  resourceBundle = ures_open("myapp", NULL, &status);
  if(U_FAILURE(status)) {
    printf("Can't open resource bundle. Error is %s\n", u_errorName(status));
    return;
  }
  
  thing = ures_getStringByKey(resourceBundle, "pen", &length, &status);
  printString(thing);   /* thing will be “pen” or “La pluma” */
  ures_close(resourceBundle);  /* 'thing' is no longer valid */
  /* END EXAMPLE */
}


int main() {
  u_setDataDirectory("." U_PATH_SEP_STRING ".." U_FILE_SEP_STRING "data");

  show();
  return 0;
}
