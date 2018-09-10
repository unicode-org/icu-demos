// Copyright (c) 2010-2012 IBM Corporation and Others. All Rights Reserved.

#include "../iucsamples.h"
#include <unicode/errorcode.h>
#include <unicode/ustdio.h>
#include <unicode/brkiter.h>
#include <stdio.h>

// interactive break iterator.
void menu() {
    u_printf("[c]urrent(), [f]irst(), [l]ast(), [n]ext(), [p]revious() or [q]uit: ");
}

int main(int argc, const char** argv) {
  const char *locale = "en_US";
  icu::ErrorCode status;
  BreakIterator *breakIterator = BreakIterator::createWordInstance(locale, status);
  ASSERT_OK(status);

  UnicodeString text;
  if (argc <= 1) {
    text = "Hello, world! How are you?";
  } else {
    text = argv[1];
  }
  breakIterator->setText(text);
  
  char buf[200];
  menu();
  while(!feof(stdin) && fgets(buf,200,stdin) && buf[0]!='q') {
    switch(buf[0]) {
    case 'c': u_printf("current() == %d\n", breakIterator->current()); break;
    case 'f': u_printf("first()==%d\n", breakIterator->first()); break;
    case 'l': u_printf("last()==%d\n", breakIterator->last()); break;
    case 'n': u_printf("next()==%d\n", breakIterator->next()); break;
    case 'p': u_printf("previous()==%d\n", breakIterator->previous()); break;
    default:  u_printf(" ( didn't understand \"%c\" )\n", buf[0]);
    }
    menu();
  }
  
  ASSERT_OK(status);
 
  return 0;
}

