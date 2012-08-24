// Copyright (c) 2010-2011 IBM Corporation and Others. All Rights Reserved.


#include <unicode/ures.h>
#include <unicode/unistr.h>
#include <unicode/resbund.h>
#include <unicode/ustdio.h>
#include <unicode/putil.h>
#include <unicode/msgfmt.h>
#include <unicode/calendar.h>
#include <unicode/brkiter.h>
#include <unicode/uniset.h>
#include "../iucsamples.h"

// slide 26
int32_t countWords(BreakIterator *wordIterator, UnicodeString &text)
{
    UErrorCode status = U_ZERO_ERROR;
    UnicodeString word;
    UnicodeSet letters(UnicodeString("[:letter:]"), status);

    int32_t wordCount = 0;
    int32_t start = wordIterator->first();

    for(int32_t end = wordIterator->next();
        end != BreakIterator::DONE;
        start = end, end = wordIterator->next())
    {
        text.extractBetween(start, end, word);
        
        if(letters.containsSome(word)) {
            wordCount += 1;
        }
    }

    return wordCount;
}

int show(const char *loc) {
  Locale locale(loc);

  UFILE *out = u_finit(stdout, loc, NULL);
  // Locale l = Locale::getDefault();

  u_fprintf(out, "\n\nlocale=%s\n", loc);

  UErrorCode status = U_ZERO_ERROR;
  u_setDataDirectory("." U_PATH_SEP_STRING ".." U_FILE_SEP_STRING "data");
  ResourceBundle resourceBundle("myapp", locale, status);

  if(U_FAILURE(status)) {
    printf("Can't open resource bundle. Default %s Error is %s\n", locale.getName(), u_errorName(status));
    return 1;
  }

  ASSERT_OK(status);
  // ---- begin sample code -----
  // ---- begin sample code -----
  // ---- begin sample code -----
  // ---- begin sample code -----

  
  // SLIDE 24

  //[see above] Locale	locale(“th”); // locale to use for break iterators
  //[see above] UErrorCode status = U_ZERO_ERROR;
  BreakIterator *characterIterator = BreakIterator::createCharacterInstance(locale, status);
  BreakIterator *wordIterator = BreakIterator::createWordInstance(locale, status);
  BreakIterator *lineIterator = BreakIterator::createLineInstance(locale, status);
  BreakIterator *sentenceIterator = BreakIterator::createSentenceInstance(locale, status);
  ASSERT_OK(status);

  // slide 25
  UnicodeString text;
  //readFile(file, text);
  text = "Unicode provides a unique number for every character, no matter what the platform, no matter what the program, no matter what the language.";
  wordIterator->setText(text);
  
  // slide 26
  int32_t wordCount = countWords(wordIterator, text);
  u_fprintf(out, "%s: Words: %d\n", loc, wordCount);

  // slide 28
  delete characterIterator; 
  delete wordIterator; 
  delete lineIterator; 
 delete sentenceIterator;

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
  show("th");
 
  return 0;

}
