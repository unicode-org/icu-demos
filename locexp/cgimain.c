/* Copyright (c) 2000 IBM, Inc. and others, all rights reserved */

/* Run Locale Explorer as a CGI. */

#include "locexp.h"

int main(const char *argv[], int argc);

int main(const char *argv[], int argc)
{
  LXContext  localContext;

  initLX();
  initContext(&localContext);
  localContext.fOUT = stdout;
  runLocaleExplorer(&localContext);
  closeLX();
  
  return 0;
}
