/**********************************************************************
*   Copyright (C) 2003, International Business Machines
*   Corporation and others.  All Rights Reserved.
***********************************************************************/


#include "locexp.h"
#include "unicode/ustdio.h"

int main()
{
  LXContext lx;
  initContext(&lx);
  lx.OUT = u_finit(stdout, "en_US", "iso-8859-1");
  strcpy(lx.cLocale,"en_US");
  lx.locales = createLocaleTree(lx.cLocale, &lx.numLocales);
  lx.curLocale = &(lx.locales->subLocs[0].subLocs[0]);
  printPath(&lx, lx.curLocale,lx.curLocale, TRUE);
  
  printf("done\n");
}
