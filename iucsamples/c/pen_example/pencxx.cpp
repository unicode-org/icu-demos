// Copyright (c) 2010 IBM Corporation and Others. All Rights Reserved.


#include <unicode/ures.h>
#include <unicode/unistr.h>
#include <unicode/resbund.h>
#include <unicode/ustdio.h>
#include <unicode/putil.h>
#include <unicode/msgfmt.h>
#include <unicode/calendar.h>
#include "../iucsamples.h"

int main()
{
UFILE *out = u_finit(stdout, "en_US", NULL);
// Locale l = Locale::getDefault();
 Locale l("es");
UErrorCode status = U_ZERO_ERROR;
 u_setDataDirectory(".");
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
 {
Formattable arguments[3];
 arguments[0].setString(resourceBundle.getStringEx("Aunt", status)); // e.g. “My Aunt”
 arguments[1].setString(resourceBundle.getStringEx("table", status)); // e.g. “on the table”
 arguments[2].setString(resourceBundle.getStringEx("pen", status)); // e.g. “pen” UnicodeString
UnicodeString pattern = resourceBundle.getStringEx("personPlaceThing", status);
UnicodeString result;
 MessageFormat::format(pattern, arguments, 3, result, status);

 u_fprintf(out, "--> [%s] %S\n", l.getName(), result.getTerminatedBuffer());
 }


 ASSERT_OK(status);
 // ---
 // p41
 {
UnicodeString pattern = "On {0, date} at {0, time} there was {1}.";
 Calendar *c = icu::Calendar::createInstance(status);
 Formattable args[] = {
c->getTime(status), // 0
"a power failure"	// 1
 };
UnicodeString result; MessageFormat::format(pattern, args, 2, result, status);
 u_fprintf(out, "--> [%s] %S\n", l.getName(), result.getTerminatedBuffer());

 }



 ASSERT_OK(status);
 // ----
 // p42
 {
UnicodeString pattern = "On {0, date, full} at {0, time, long} there was {1}.";
 Calendar *c = icu::Calendar::createInstance(status);
 Formattable args[] = {
c->getTime(status), // 0
"a power failure"	// 1
 };
UnicodeString result; MessageFormat::format(pattern, args, 2, result, status);
 u_fprintf(out, "--> [%s] %S\n", l.getName(), result.getTerminatedBuffer());

 }

 ASSERT_OK(status);
 // ----
 // p42++
 {
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
   u_fprintf(out, "42+--> [%s] %S\n", l.getName(), result.getTerminatedBuffer());
 }

 ASSERT_OK(status);
 // ----
 // p44
 {
   UnicodeString pattern("There {1, plural, one{is one file} other{are # files}} in {0}.");
   MessageFormat fmt(pattern,status);
   UnicodeString directoryName = "myDirectory";
   int fileCount=1234;
   Formattable args[] = {
     directoryName,
     fileCount
   };
   UnicodeString result;
   MessageFormat::format(pattern, args, 2, result, status);
   u_fprintf(out, "44?> [%s] %S\n", l.getName(), result.getTerminatedBuffer());   
 }

 ASSERT_OK(status);
 // ----
 // p45
 {
   UnicodeString pattern("There {1, choice, 0#are no files|1#is one file|1<are {1, number, integer} files} in {0}.");
   MessageFormat fmt(pattern,status);
   UnicodeString directoryName = "myDirectory";
   int fileCount=1234;
   Formattable args[] = {
     directoryName,
     fileCount
   };
   UnicodeString result;
   MessageFormat::format(pattern, args, 2, result, status);
   u_fprintf(out, "45?> [%s] %S\n", l.getName(), result.getTerminatedBuffer());   
 }

 u_fclose(out);
 return 0;

}
