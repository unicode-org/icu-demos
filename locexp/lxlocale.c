/**********************************************************************
*   Copyright (C) 1999-2013, International Business Machines
*   Corporation and others.  All Rights Reserved.
***********************************************************************/

#include "locexp.h"

/* routines having to do with locales and trees */


/* Is a locale supported by Locale Explorer? We use the presence of the 'helpPrefix' tag, to tell us */
UBool isSupportedLocale(const char *locale, UBool includeChildren)
{
    UResourceBundle *newRB;
    UErrorCode       status = U_ZERO_ERROR;
    UBool           supp   = TRUE;

    newRB = ures_open(FSWF_bundlePath(), locale, &status);

    if(U_FAILURE(status))
    {
        supp = FALSE;
    }
    else
    {
        if(status == U_USING_DEFAULT_WARNING)
        {
            supp = FALSE;
        }
        else if( (!includeChildren) && (status == U_USING_FALLBACK_WARNING))
        {
            supp = FALSE;
        }
        else
        {
            int32_t len;

            status = U_ZERO_ERROR;
            ures_getStringByKey(newRB, "helpPrefix", &len, &status);

            if(status == U_USING_DEFAULT_WARNING)
            {
                supp = FALSE;
            }
            else
            {
                if( (!includeChildren) && (status == U_USING_FALLBACK_WARNING))
                {
                    supp = FALSE;
                }
            }
        }
        ures_close(newRB);
    }

    return supp;
}


/* Is the locale experimental? It is if its version starts with 'x'. */
UBool isExperimentalLocale(const char *locale)
{
    UResourceBundle *newRB;
    UErrorCode       status = U_ZERO_ERROR;
    UBool           supp   = FALSE;
    int32_t len;

    newRB = ures_open(NULL, locale, &status);
    if(U_FAILURE(status) || (status == U_USING_FALLBACK_WARNING) || (status == U_USING_DEFAULT_WARNING)) {
        supp = TRUE;
    }
    else
    {
        const UChar *s = ures_getStringByKey(newRB, "Version", &len, &status);
      
        if(*s == 0x0078) /* If it starts with an 'x'.. */
            supp = TRUE;

        ures_close(newRB);
    }

    return supp;
}


/* chooselocale --------------------------------------------------------------------------- */
void chooseLocale(LXContext *lx, UBool toOpen, const char *current, const char *restored, UBool showAll)
{
    UBool  hit = FALSE;
    int32_t n, j;
    UErrorCode status = U_ZERO_ERROR;
    UBool  hadUnsupportedLocales = FALSE;
    static UBool initStr = TRUE;
    U_STRING_DECL(BEGIN_HIT_CELL, "<td valign=\"top\" bgcolor=\"#c8d7e3\"><b>", 38);
    U_STRING_DECL(END_HIT_CELL, "</b></td>\r\n", 12);
    U_STRING_DECL(BEGIN_CELL, "<td valign=\"top\">", 17);
    U_STRING_DECL(END_CELL, "</td>\r\n", 8);
    U_STRING_DECL(COMMA_SPACE, ",&nbsp; ", 9);
    if (initStr) {
        U_STRING_INIT(BEGIN_HIT_CELL, "<td valign=\"top\" bgcolor=\"#c8d7e3\"><b>", 38);
        U_STRING_INIT(END_HIT_CELL, "</b></td>\r\n", 12);
        U_STRING_INIT(BEGIN_CELL, "<td valign=\"top\">", 17);
        U_STRING_INIT(END_CELL, "</td>\r\n", 8);
        U_STRING_INIT(COMMA_SPACE, ", &nbsp;", 9);
        initStr = FALSE;
    }
/* v14-gray-table-border */
    u_fprintf(lx->OUT, "<div id='choose_select'></div><!-- used by javascript only -->\n");
    u_fprintf(lx->OUT, "<noscript><a href='#choose_region'>(Choose by Region)</a></noscript>");
    u_fprintf(lx->OUT, "<table id='choose_lang' class='data-table-1' cellpadding=\"2\" cellspacing=\"2\">\r\n");

    u_fprintf(lx->OUT, "<tr><th class='mbg' align=\"left\">%S</th>\r\n<th class='mbg' align=\"left\">%S</th></tr>\r\n",
              FSWF("localeList_Locale", "Languages"),
              FSWF("localeList_Sublocale", "Regions"));

    u_fprintf(lx->OUT, "<tr><td class='v14-header-4-small' colspan=\"2\"><a class='bs' href=\"?_=%s\">%S</a></td></tr>\r\n",
              lx->locales->str, lx->locales->ustr); /* default */ 

    mySort(lx->locales, &status, TRUE); /* need the whole thing sorted */

    for(n=0;n<lx->locales->nSubLocs;n++)
    {
        /* This will hide display locales - LANGUAGE level-  that don't exist. */
        if((toOpen == FALSE) && !isSupportedLocale(lx->locales->subLocs[n]->str, FALSE) && showAll == FALSE)
            continue;

        u_fprintf(lx->OUT, "<tr>\r\n");


        hit = !strcmp(lx->locales->subLocs[n]->str,current);

        if(hit) {
          u_fprintf_u(lx->OUT, BEGIN_HIT_CELL);
        } else {
          u_fprintf(lx->OUT, "<td class='v14-header-4-small' valign=\"top\">");
        }

        printLocaleLink(lx, toOpen, lx->locales->subLocs[n], current, restored, &hadUnsupportedLocales);

        if(hit) {
          u_fprintf_u(lx->OUT, END_HIT_CELL);
        } else {      
          u_fprintf_u(lx->OUT, END_CELL);  
        }
      
        if(lx->locales->subLocs[n]->nSubLocs)
        {
            u_fprintf_u(lx->OUT, BEGIN_CELL);

            for(j=0;j< (lx->locales->subLocs[n]->nSubLocs); j++)
            {
#ifndef LX_SHOW_ALL_DISPLAY_LOCALES
                /* This will hide display locales- COUNTRY/VARIANT level that don't exist. */
                if((toOpen == FALSE) && !isSupportedLocale(lx->locales->subLocs[n]->subLocs[j]->str, FALSE) && (showAll ==FALSE) )
                    continue;
#endif

                if(j>0)
                    u_fprintf_u(lx->OUT, COMMA_SPACE);

                printLocaleAndSubs(lx, toOpen, lx->locales->subLocs[n]->subLocs[j], current, restored, &hadUnsupportedLocales);

            }

            u_fprintf_u(lx->OUT, END_CELL);
        }

        u_fprintf(lx->OUT, "</tr>\r\n");


    }

    u_fprintf(lx->OUT, "</table>\r\n");

    chooseLocaleRegion(lx, toOpen, current, restored, showAll);

    if(hadUnsupportedLocales)
        u_fputs(FSWF("locale_experimental", "Locales in <I>Italics</I> are Draft and not officially supported."), lx->OUT);

    if(showAll == FALSE && toOpen == FALSE)
    {
        u_fprintf(lx->OUT, "<a href=\"?locale_all&amp;%s\"><img border=\"0\" width=\"16\" height=\"16\" src=\"" LDATA_PATH_LOC "closed.gif\" alt=\"+\"/>%S</a>\r\n<br />",
                  (lx->queryString&&strlen(lx->queryString)>7)?(lx->queryString+7):"",
                  lx->dispLocale,
                  FSWF("showAll", "Show All"));
    }
}

void printSubLocales(LXContext *lx, const char *suffix) 
{
  UErrorCode status = U_ZERO_ERROR;
  int n;
  UBool hadExperimentalSubLocales = FALSE;

  /* look for sublocs */
  if(lx->curLocale && lx->curLocale->nSubLocs)
    {
      u_fprintf(lx->OUT, "%S", FSWF("sublocales", "Sublocales:"));
      if(!suffix || !*suffix) {
        u_fprintf(lx->OUT, "<br /><div style=\"margin-left: 2.5em; margin-top: 1em; margin-bottom: 1em\">", FSWF("sublocales", "Sublocales:"));
      } 
      mySort(lx->curLocale, &status, FALSE);  /* Sort sub locales */
      
      for(n=0;n<lx->curLocale->nSubLocs;n++)  {
        UBool wasExperimental = FALSE;
        
        if(n != 0) {
          u_fprintf(lx->OUT, ", ");
        }
        
        if(lx->curLocale->subLocs[n]->isVariant) u_fprintf(lx->OUT, " [");
        
        u_fprintf(lx->OUT, "<a href=\"%s&amp;_=%s\">", 
                  getLXBaseURL(lx, kNO_URL|kNO_LOC),
                  lx->curLocale->subLocs[n]->str);
        
        if(isExperimentalLocale(lx->curLocale->subLocs[n]->str)) {
          u_fprintf(lx->OUT, "<i><font color=\"#9999FF\">");
          hadExperimentalSubLocales = TRUE;
          wasExperimental = TRUE;
        }
        u_fprintf_u(lx->OUT, lx->curLocale->subLocs[n]->ustr);
        if(wasExperimental) {
          u_fprintf(lx->OUT, "</font></i>");
        }
        u_fprintf(lx->OUT, "</a>");
        
        if(lx->curLocale->subLocs[n]->isVariant) u_fprintf(lx->OUT, "] ");
      }
      if(!suffix || !*suffix) {
        u_fprintf(lx->OUT, "</div>");
      }
    }
  
  /* Look for cousins with the same leaf component */
  /* For now: ONLY do for xx_YY locales */
  if(lx->curLocale && lx->parLocale &&         /* have locale & parent found (i.e. installed) */
     (lx->parLocale->parent == lx-> locales) ) { /* parent's parent is root */
    int count =0;
    int i;
    const char *stub;
    char buf[500];
    /* safe 'cause all these strings come from getInstalledLocales' */
    stub = lx->curLocale->str + strlen(lx->parLocale->str);
    /* u_fprintf(lx->OUT,"<B>STUB is: %s</B>\n",stub); */
    
    /* OK, now find all children X of my grandparent,  where  (  X.parent.str + stub == X ) */
    for(i=0;i<lx->locales->nSubLocs;i++) {
      if(!strcmp(lx->locales->subLocs[i]->str, lx->parLocale->str)) {
        continue; /* Don't search our parent (same language) */
      }
      
      strcpy(buf, lx->locales->subLocs[i]->str);
      strcat(buf, stub);
      
      if(findLocaleNonRecursive(lx->locales->subLocs[i], buf) != -1) {
        UBool wasExperimental = FALSE;
        
        if((count++) > 0) {
          u_fprintf(lx->OUT, ", ");
        } else { /* header */
          if(suffix && *suffix && lx->curLocale->nSubLocs) { 
            u_fprintf(lx->OUT, ", ");
          }
          u_fprintf_u(lx->OUT, 
                      FSWF("otherLanguageSameCountryLocales", "<b>%S</b> under other languages"),
                      lx->curLocale->ustr);
          u_fprintf(lx->OUT, ": ");
        }
        u_fprintf(lx->OUT, "<a href=\"%s&amp;_=%s\">", 
                  getLXBaseURL(lx, kNO_URL|kNO_LOC), buf);
        
        if(isExperimentalLocale(buf)) {
          u_fprintf(lx->OUT, "<i><font color=\"#9999FF\">");
          hadExperimentalSubLocales = TRUE;
          wasExperimental = TRUE;
        }
        
        u_fprintf_u(lx->OUT, lx->locales->subLocs[i]->ustr);
        
        if(wasExperimental) {
          u_fprintf(lx->OUT, "</font></i>");
        }
        u_fprintf(lx->OUT, "</a>");
      }
    }
    if(count > 0) {
      u_fprintf(lx->OUT, "<br />\r\n");
    }
  }
  
  
  /* this notice covers sublocs and sibling locs */
  if(hadExperimentalSubLocales)
    u_fprintf(lx->OUT, "<br />%S", FSWF("locale_experimental", "Locales in <I>Italics</I> are Draft and not officially supported."));
  
  
}

void printPath(LXContext *lx, const MySortable *leaf, const MySortable *current, UBool styled)
{
  if(!leaf) { /* top level */
    if(styled) {
      u_fprintf(lx->OUT, "%s<span class=\"bctl\">", DEMO_BREAD_CRUMB_BAR);
    }
    if(styled && !lx->section[0]) {
      u_fprintf(lx->OUT, "<a href=\"%s\">", getLXBaseURL(lx, kNO_URL | kNO_LOC)); /* Reset */
    }
    
    u_fprintf_u(lx->OUT, FSWF("title", "Locale Explorer"));
      
    if(styled && !lx->section[0]) {
      u_fprintf(lx->OUT, "</a>");
    }

    if(styled) {
      u_fprintf(lx->OUT, "</span>");
    }
    
    return;
  }

  /* reverse order recursively */
  printPath(lx, leaf->parent,current,styled);
  u_fprintf(lx->OUT, " &gt; ");
  if(leaf->isVariant) {
    u_fprintf(lx->OUT, " [");
  }
  if(styled) {
    if(leaf == current) {
      u_fprintf(lx->OUT, "<b>");
    }
    u_fprintf(lx->OUT, "<a class=\"bctl\" href=\"%s&amp;_=%s\">", getLXBaseURL(lx, kNO_URL|kNO_LOC), leaf->str);
  }
  u_fprintf_u(lx->OUT, leaf->ustr);
  if(styled) {
    u_fprintf(lx->OUT, "</a>");
    if(leaf == current)
      u_fprintf(lx->OUT, "</b>");
  }
  if(leaf->isVariant) {
    u_fprintf(lx->OUT, "] ");
  }
}


void printLocaleLink(LXContext *lx, UBool toOpen, MySortable *l, const char *current, const char *restored, UBool *hadUnsupportedLocales)
{
    UBool supported;
    static UBool initStr = TRUE;
    U_STRING_DECL(BEGIN_LINK, "<a class='bs' href=\"", 21);
    U_STRING_DECL(MIDDLE_LINK, "\">", 3);
    U_STRING_DECL(END_LINK, "</a>", 5);
    U_STRING_DECL(BEGIN_UNSUP, "<i><font color=\"#9999FF\">", 25);
    U_STRING_DECL(END_UNSUP, "</font></i>", 12);
    if (initStr) {
        U_STRING_INIT(BEGIN_LINK, "<a class='bs' href=\"", 21);
        U_STRING_INIT(MIDDLE_LINK, "\">", 3);
        U_STRING_INIT(END_LINK, "</a>", 5);
        U_STRING_INIT(BEGIN_UNSUP, "<i><font color=\"#9999FF\">", 25);
        U_STRING_INIT(END_UNSUP, "</font></i>", 12);
        initStr = FALSE;
    }

    if ( toOpen == FALSE)
    {
        /* locales for LOCEXP TEXT */
        supported = isSupportedLocale(l->str, TRUE);
    }
    else
    {
        /* locales for VIEWING */
        supported = !isExperimentalLocale(l->str);
    }


    u_fprintf_u(lx->OUT, BEGIN_LINK);
  
    if(toOpen == TRUE)
    {
        u_fprintf(lx->OUT, "%s", getLXBaseURL(lx, kNO_LOC));
        u_fprintf(lx->OUT,"&amp;_=%s", l->str);
    }
    else
    {
        u_fprintf(lx->OUT, "%s/%s/",
                  lx->scriptName,
                  l->str);
        if(lx->convSet)
            u_fprintf(lx->OUT,"%s/", lx->convRequested);
      
        if(restored)
            u_fprintf(lx->OUT, "?%s", restored);
    }
  
  
    u_fprintf_u(lx->OUT, MIDDLE_LINK);

    if(!supported)
    {
        u_fprintf_u(lx->OUT, BEGIN_UNSUP);
        *hadUnsupportedLocales = TRUE;
    }

    if(toOpen == TRUE) 
    {
      u_fprintf_u(lx->OUT, l->ustr);
    }
    else
    {
        UErrorCode status = U_ZERO_ERROR;
        UChar displayName[1024] = { 0x0000 };
    
        uloc_getDisplayVariant(l->str,l->str ,displayName, 1024, &status);
        if(u_strlen(displayName)==0 || U_FAILURE(status)) {
            status = U_ZERO_ERROR;
            uloc_getDisplayCountry(l->str,l->str ,displayName, 1024, &status);
            if(u_strlen(displayName)==0 || U_FAILURE(status)) {
                status = U_ZERO_ERROR;
                uloc_getDisplayLanguage(l->str,l->str ,displayName, 1024, &status);
            }
        }
    
        u_fprintf_u(lx->OUT, displayName);
    }

    if(!supported)
        u_fprintf_u(lx->OUT, END_UNSUP);
  
    u_fprintf_u(lx->OUT, END_LINK);

  
}

void printLocaleAndSubs(LXContext *lx, UBool toOpen, MySortable *l, const char *current, const char *restored, UBool *hadUnsupportedLocales)
{
    int32_t n;
    
    
    if(l->isVariant)
        u_fprintf(lx->OUT, "[&nbsp;");
    printLocaleLink(lx, toOpen,l,current,restored, hadUnsupportedLocales);
    if(l->isVariant)
        u_fprintf(lx->OUT, "&nbsp;]");
    
    /* SRL todo: cull unsupported locales!  */
    if(l->nSubLocs)
    {
        u_fprintf(lx->OUT, "&nbsp;[<small>&nbsp;");
        
        for(n=0;n<(l->nSubLocs);n++)
        {
            if(n > 0)
            {
                u_fprintf(lx->OUT, ", ");
            }
            printLocaleAndSubs(lx, toOpen, l->subLocs[n], current, restored, hadUnsupportedLocales);
        }
        
        u_fprintf(lx->OUT, "&nbsp;</small>]");
    }
}


void setupLocaleTree(LXContext *lx)
{
  const char *loc = lx->curLocaleBlob.base;

  /* setup base locale */
  lx->locales = createLocaleTree(lx->dispLocale, &lx->numLocales);
  lx->regions = createRegionList(lx->dispLocale, lx->locales);
  if(*loc) {
    /* setup cursors.. */
    lx->curLocale = findLocale(lx->locales, loc);
    
    if(lx->curLocale) {
      lx->parLocale = lx->curLocale->parent;
      if(lx->parLocale == lx->locales) {
        /* locale is a Language */
        lx->lLocale = lx->curLocale;
      } else if(lx->curLocale != lx->locales) { /* if it's not root.. */
        /* locale is a region */
        lx->rLocale = lx->curLocale;
        lx->lLocale = lx->rLocale->parent;
      }
    }
    if(lx->curLocaleBlob.l[0] && !lx->lLocale) {
      int i;
      for(i=0;!lx->lLocale && (i<lx->locales->nSubLocs);i++) {
        if(!strcmp(lx->locales->subLocs[i]->str, lx->curLocaleBlob.l)) {
          lx->lLocale = lx->locales->subLocs[i];
        }
      }
    }
    if(lx->curLocaleBlob.r[0] && !lx->rLocale) {
      int i;
      for(i=0;!lx->rLocale && (i<lx->regions->nSubLocs);i++) {
        UErrorCode status = U_ZERO_ERROR;
        char r[200];
        uloc_getCountry(lx->regions->subLocs[i]->str, r, 200, &status);
        if(U_FAILURE(status)) {
          continue;
        }
#if defined(LX_DEBUG)
        fprintf(stderr, "[RGN] %s vs %s\n", lx->curLocaleBlob.r, r);
#endif
        if(!strcmp(r, lx->curLocaleBlob.r)) {
          lx->rLocale = lx->regions->subLocs[i];
        }
      }
    }
  }
}
