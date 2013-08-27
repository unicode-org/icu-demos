// Copyright (c) 2010-2012 IBM Corporation and Others. All Rights Reserved.


#include <unicode/ures.h>
#include <unicode/unistr.h>
#include <unicode/resbund.h>
#include <unicode/ustdio.h>
#include <unicode/putil.h>
#include <unicode/msgfmt.h>
#include <unicode/calendar.h>
#include "../iucsamples.h"

void show(const char *loc) {
  Locale l(loc);
   
  // Locale l = Locale::getDefault();

  u_printf("\n\nlocale=%s\n", loc);

  u_setDataDirectory("." U_PATH_SEP_STRING ".." U_FILE_SEP_STRING "data");

// BEGIN SAMPLE
  UErrorCode status = U_ZERO_ERROR;
  ResourceBundle resourceBundle("myapp", loc, status);
  if(U_FAILURE(status)) {
    u_printf("Can't open resource bundle. Error is %s\n", u_errorName(status));
    return;
  }
  
  // thing will be "pen" or "La pluma"
  UnicodeString thing = resourceBundle.getStringEx("pen", status);

// END SAMPLE

  u_printf("Got the 'pen' for %s, status was %s\n", loc, u_errorName(status));

  u_printf("Thing: %S\n", thing.getTerminatedBuffer());

  ASSERT_OK(status);
  return;
}

int main()
{

  show("es");
  show("en_US");
 
  return 0;

}
