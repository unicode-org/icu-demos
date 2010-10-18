// Copyright (c) 2010 IBM Corporation and Others. All Rights Reserved.


#include <unicode/ures.h>
#include <unicode/unistr.h>
#include <unicode/resbund.h>
#include <unicode/ustdio.h>
#include <unicode/putil.h>
#include <unicode/msgfmt.h>
#include <unicode/calendar.h>
#include "../iucsamples.h"

int show(const char *loc, int n) {
  char nstr[999];
  sprintf(nstr, "%d", n);
  Locale l(loc);

  UFILE *out = u_finit(stdout, loc, NULL);
  // Locale l = Locale::getDefault();

  u_fprintf(out, "\n\nlocale=%s\n", loc);

  UErrorCode status = U_ZERO_ERROR;
  u_setDataDirectory(".:../data/myapp");
  ResourceBundle resourceBundle("../data/myapp", l, status);

  if(U_FAILURE(status)) {
    printf("Can't open resource bundle. Default %s Error is %s\n", l.getName(), u_errorName(status));
    return 1;
  }

  ASSERT_OK(status);
  // ---- begin sample code -----
  // ---- begin sample code -----
  // ---- begin sample code -----
  // ---- begin sample code -----


   UnicodeString pattern("There {1, plural, one{is one file} other{are # files}} in {0}.");
   MessageFormat fmt(pattern,status);
   UnicodeString directoryName = "myDirectory";
   int fileCount=n;
   Formattable args[] = {
     directoryName,
     fileCount
   };
   UnicodeString result;
   MessageFormat::format(pattern, args, 2, result, status);
   u_fprintf(out, "45:#%s> [%s] %S\n", nstr, l.getName(), result.getTerminatedBuffer());   

  // ---- end sample code -----
  // ---- end sample code -----
  // ---- end sample code -----
  // ---- end sample code -----
  // ---- end sample code -----
 
  ASSERT_OK(status);
 
  // cleanup...
  u_fclose(out);
  return 0;
}

int main()
{

  //show("es");
  show("en_US",0);
  show("en_US",1);
  show("en_US",1234);
 
  return 0;

}
