/**********************************************************************
*   Copyright (C) 1999-2013, International Business Machines
*   Corporation and others.  All Rights Reserved.
***********************************************************************/

#include "locexp.h"

#include "unicode/region.h"

#define RGNTYPE(x) case x: return #x

static const char* typestr(URegionType t) {
  switch(t) {
    RGNTYPE(URGN_UNKNOWN);
    RGNTYPE(URGN_TERRITORY);
    RGNTYPE(URGN_WORLD);
    RGNTYPE(URGN_CONTINENT);
    RGNTYPE(URGN_SUBCONTINENT);
    RGNTYPE(URGN_GROUPING);
    RGNTYPE(URGN_DEPRECATED);
  default: return "badtype";
  }
}

static void showTree(LXContext *lx, const Region &top, int indent, UErrorCode &status) {
  if(U_FAILURE(status)) return;
  UChar tmp[1000];
  char tmploc[1000];
  strcpy(tmploc, "und_");
  const char *code = top.getRegionCode();
  strcat(tmploc, code);
  uloc_getDisplayCountry(tmploc, lx->dispLocale, tmp, 1000, &status);
  if(U_FAILURE(status)) return;

  u_fprintf(lx->OUT, "<div class='rgn rgn-%s' id='%s'>", typestr(top.getType()), code);

  // for(int i=0;i<indent;i++) {
  //   u_fprintf(lx->OUT, "&nbsp; ");
  // }
  u_fprintf(lx->OUT, "<div class='rgn-title' id='rgn-%s'>%s - %S</div>", code, code, tmp);

  indent++;


  // linear search..
  u_fprintf(lx->OUT, "<div class='rgn-locs'>");
  int32_t total = uloc_countAvailable();
  for(int32_t i=0; i<total; i++) {
    const char *aloc = uloc_getAvailable(i);
    char argn[100];
    uloc_getCountry(aloc, argn, 100, &status);

    if(!strcmp(argn, code)) {
      UChar tmp2[1000];
      uloc_getDisplayName(aloc, lx->dispLocale, tmp2, 1000, &status);
      if(U_FAILURE(status)) return;
      u_fprintf(lx->OUT, "<a class='rgn-loc' href='%s&amp;_=%s'>%S</a>\n", 
                getLXBaseURL(lx, kNO_URL|kNO_LOC),
                aloc,
                tmp2);
    }
  }
  u_fprintf(lx->OUT, "</div>");

  LocalPointer<StringEnumeration> set(top.getContainedRegions());
  if(set.isValid()) {
    const char *str;
    for(;(str=set->next(NULL, status))!=NULL &&U_SUCCESS(status);) {
      const Region *r = Region::getInstance(str, status);

      if (r!=NULL && U_SUCCESS(status)) {
        showTree(lx, *r, indent, status);
      }
    }
  }
  u_fprintf(lx->OUT, "</div>");
}


void chooseLocaleRegion(LXContext *lx, UBool toOpen, const char *current, const char *restored, UBool showAll) {
  u_fprintf(lx->OUT, "<div id='choose_region'>\n");
  u_fprintf(lx->OUT, "<h1>%S</h1>", 
            FSWF("chooseByRegion", "Choose by Region"));

  UErrorCode status = U_ZERO_ERROR;

  const Region *world = Region::getInstance(001, status);

  if(U_FAILURE(status)) {
    u_fprintf(lx->OUT, "Problem: can't find the world (001) %s\n", u_errorName(status));
    return;
  }

  showTree(lx, *world, 0, status);

  if(U_FAILURE(status)) {
    u_fprintf(lx->OUT, "Error during processing %s\n", u_errorName(status));
  }

  u_fprintf(lx->OUT, "</div>\n");
}
