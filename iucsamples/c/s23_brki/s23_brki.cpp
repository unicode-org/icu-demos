// Copyright (c) 2010-2012 IBM Corporation and Others. All Rights Reserved.

#include "../iucsamples.h"
#include <unicode/ustdio.h>
#include <unicode/brkiter.h>
#include <stdio.h>

// interactive break iterator.
void menu() {
    u_printf("[c]urrent(), [f]irst(), [l]ast(), [n]ext(), [p]revious() or [q]uit: ");
}

int main() {
  const char *locale = "en_US";
  UErrorCode status = U_ZERO_ERROR;
  BreakIterator *characterIterator = BreakIterator::createCharacterInstance(locale, status);
  ASSERT_OK(status);

  UnicodeString text;
  text = "Text";
  characterIterator->setText(text);
  
  char buf[200];
  menu();
  while(!feof(stdin) && fgets(buf,200,stdin) && buf[0]!='q') {
    switch(buf[0]) {
    case 'c': u_printf("current() == %d\n", characterIterator->current()); break;
    case 'f': u_printf("first()==%d\n", characterIterator->first()); break;
    case 'l': u_printf("last()==%d\n", characterIterator->last()); break;
    case 'n': u_printf("next()==%d\n", characterIterator->next()); break;
    case 'p': u_printf("previous()==%d\n", characterIterator->previous()); break;
    default:  u_printf(" ( didn't understand \"%c\" )\n", buf[0]);
    }
    menu();
  }
  
  ASSERT_OK(status);
 
  return 0;
}

