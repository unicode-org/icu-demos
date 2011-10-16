// Copyright (c) 2010-2011 IBM Corporation and Others. All Rights Reserved.


#include <unicode/ures.h>
#include <unicode/unistr.h>
#include <unicode/resbund.h>
#include <unicode/ustdio.h>
#include <unicode/putil.h>
#include <unicode/msgfmt.h>
#include <unicode/calendar.h>
#include "../iucsamples.h"

int show(const char *loc) {
  Locale l(loc);

   

  UFILE *out = u_finit(stdout, "en_US", NULL);
  // Locale l = Locale::getDefault();

  u_fprintf(out, "\n\nlocale=%s\n", loc);

  UErrorCode status = U_ZERO_ERROR;
  u_setDataDirectory("." U_PATH_SEP_STRING ".." U_FILE_SEP_STRING "data");
  ResourceBundle resourceBundle("myapp", l, status);

  if(U_FAILURE(status)) {
    printf("Can't open resource bundle. Default %s Error is %s\n", l.getName(), u_errorName(status));
    return 1;
  }

  // thing will be “pen” or “La pluma”
  UnicodeString thing = resourceBundle.getStringEx("pen", status);
  
  u_fprintf(out, "Thing: %S\n", thing.getTerminatedBuffer());
  
  ASSERT_OK(status);
  // ----
  {  // -------
    UErrorCode status = U_ZERO_ERROR;
    u_setDataDirectory(".");
    ResourceBundle resourceBundle("myapp", l, status);
    if(U_FAILURE(status)) {
      printf("Can't open resource bundle. Default %s Error is %s\n", l.getName(), u_errorName(status));
      return 1;
    }


    Formattable arguments[3];
    arguments[0].setString(resourceBundle.getStringEx("Aunt", status)); // e.g. “My Aunt”
    arguments[1].setString(resourceBundle.getStringEx("table", status)); // e.g. “on the table”
    arguments[2].setString(resourceBundle.getStringEx("pen", status)); // e.g. “pen” UnicodeString
    UnicodeString pattern = resourceBundle.getStringEx("personPlaceThing", status);
    UnicodeString result;
    MessageFormat::format(pattern, arguments, 3, result, status);
   
    u_fprintf(out, "--> [%s] %S\n", l.getName(), result.getTerminatedBuffer());
  }
  // ----
 
  ASSERT_OK(status);
 
  // cleanup...
  u_fclose(out);
  return 0;
}

int main()
{

  show("es");
  show("en_US");
 
  return 0;

}
