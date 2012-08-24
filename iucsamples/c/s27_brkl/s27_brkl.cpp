// Copyright (c) 2010-2012 IBM Corporation and Others. All Rights Reserved.

#include "../iucsamples.h"
#include <unicode/ustdio.h>
#include <unicode/brkiter.h>
#include <stdio.h>

int32_t previousBreak(BreakIterator *breakIterator, UnicodeString &text,
                      int32_t location)
{
    int32_t len = text.length();

    while(location < len) {
        UChar c = text[location];

        if(!u_isWhitespace(c) && !u_iscntrl(c)) {
            break;
        }

        location += 1;
    }

    return breakIterator->preceding(location + 1);
}


int32_t MeasureTheString(const UnicodeString& /*text*/, int32_t start, int32_t limit) {
  /* normally this would be measuring the font, etc */
  return (limit-start);
}

void PrintString(const UnicodeString& text, int32_t start, int32_t limit) {
  UnicodeString chunk;
  if(limit==BreakIterator::DONE) {
    limit = text.length();
  }
  text.extractBetween(start, limit, chunk);
  u_printf("%S|\n", chunk.getTerminatedBuffer());
}

int main() {
  const char *locale = "en_US";
  UErrorCode status = U_ZERO_ERROR;
  BreakIterator *lineIterator = BreakIterator::createLineInstance(locale, status);
  BreakIterator *charIterator = BreakIterator::createCharacterInstance(locale, status);
  ASSERT_OK(status);
  
  UnicodeString text;
  text = "Today's software market is a global one in which it is desirable to develop and maintain one application (single source/single binary) that supports a wide variety of languages. The International Components for Unicode (ICU) libraries provide robust and full-featured Unicode services on a wide variety of platforms to help this design goal.";
  charIterator->setText(text);
  lineIterator->setText(text);

  const int32_t width = 12;
  int32_t n = 0;
  int32_t max = charIterator->last();

  int32_t prev = 0;
  // accumulate chars
  for(n=charIterator->first();n<max && n != BreakIterator::DONE;n=charIterator->next()) {
    // too big?
    if(MeasureTheString(text, prev, n) >= width) {
      int32_t brk = previousBreak(lineIterator, text, n);
      if(brk==prev) {
        // didn't fit.   Hyphenate? Truncate?

        //Jump to next.
        brk = lineIterator->next();
        PrintString(text, prev, brk);
        u_printf("^^^^^\n"); // line too long
      } else {
        PrintString(text, prev, brk);
      }
      prev = brk;
    }
  }
  // remainder
  PrintString(text, prev, n);

  return 0;
}

