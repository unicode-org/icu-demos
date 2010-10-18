// Copyright (c) 2010 IBM Corporation and Others. All Rights Reserved.


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

   UnicodeString pattern = "On {when, date, full} at {when, time, long} there was {what}.";
   Calendar *c = icu::Calendar::createInstance(status);
   Formattable args[] = {
     c->getTime(status), // when
     "a power failure"	// what
   };
   UnicodeString names[] = { 
     "when",
     "what"
   };
   UnicodeString result; 
   MessageFormat fmt(pattern, status);
   fmt.format(names, args, 2, result, status);
   u_fprintf(out, "43--> [%s] %S\n", l.getName(), result.getTerminatedBuffer());


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
  show("en_US");
 
  return 0;

}
