/**********************************************************************
*   Copyright (C) 1999-2002, International Business Machines
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
    U_STRING_DECL(BEGIN_HIT_CELL, "<td valign=top bgcolor=\"#FFDDDD\"><b>", 36);
    U_STRING_DECL(END_HIT_CELL, "</b></td>", 10);
    U_STRING_DECL(BEGIN_CELL, "<td valign=top>", 15);
    U_STRING_DECL(END_CELL, "</td>", 6);
    U_STRING_DECL(COMMA_SPACE, ",&nbsp; ", 9);
    if (initStr) {
        U_STRING_INIT(BEGIN_HIT_CELL, "<td valign=top bgcolor=\"#FFDDDD\"><b>", 36);
        U_STRING_INIT(END_HIT_CELL, "</b></td>", 10);
        U_STRING_INIT(BEGIN_CELL, "<td valign=top>", 15);
        U_STRING_INIT(END_CELL, "</td>", 6);
        U_STRING_INIT(COMMA_SPACE, ", &nbsp;", 9);
        initStr = FALSE;
    }

    u_fprintf(lx->OUT, "<table border=2 cellpadding=2 cellspacing=2>\r\n");

    u_fprintf(lx->OUT, "<tr><td><b>%U</b></td><td><b>%U</b></td></tr>\r\n",
              FSWF("localeList_Locale", "Languages"),
              FSWF("localeList_Sublocale", "Regions"));

    u_fprintf(lx->OUT, "<tr><td colspan=2><a href=\"?_=%s\">%U</a></td></tr>\r\n",
              lx->locales->str, lx->locales->ustr); /* default */ 

    mySort(lx->locales, &status, TRUE); /* need the whole thing sorted */

    for(n=0;n<lx->locales->nSubLocs;n++)
    {
        /* This will hide display locales - LANGUAGE level-  that don't exist. */
        if((toOpen == FALSE) && !isSupportedLocale(lx->locales->subLocs[n]->str, FALSE) && showAll == FALSE)
            continue;

        u_fprintf(lx->OUT, "<tr>\r\n");


        hit = !strcmp(lx->locales->subLocs[n]->str,current);

        if(hit)
        {
            u_fprintf_u(lx->OUT, BEGIN_HIT_CELL);
        }
        else
        {
            u_fprintf_u(lx->OUT, BEGIN_CELL);
        }

        printLocaleLink(lx, toOpen, lx->locales->subLocs[n], current, restored, &hadUnsupportedLocales);

        if(hit)
        {
            u_fprintf_u(lx->OUT, END_HIT_CELL);
        }
        else
        {      
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

    if(hadUnsupportedLocales)
        u_fputs(FSWF("locale_experimental", "Locales in <I>Italics</I> are experimental and not officially supported."), lx->OUT);

    if(showAll == FALSE && toOpen == FALSE)
    {
        u_fprintf(lx->OUT, "<a href=\"?locale_all&%s\"><img border=0 width=16 height=16 src=\"../_/closed.gif\" alt=\"\">%U</A>\r\n<BR>",
                  (lx->queryString&&strlen(lx->queryString)>7)?(lx->queryString+7):"",
                  FSWF("showAll", "Show All"));
    }
}



void printPath(LXContext *lx, const MySortable *leaf, const MySortable *current, UBool styled)
{

    if(!leaf) /* top level */
    {
        if(styled) 
            u_fprintf(lx->OUT, "<a href=\"?\">"); /* Reset */
      
        u_fprintf_u(lx->OUT, FSWF("title", "ICU LocaleExplorer"));
      
        if(styled) 
            u_fprintf(lx->OUT, "</a>");

        return;
    }

  
    /* reverse order recursively */
    printPath(lx, leaf->parent,current,styled);


    u_fprintf(lx->OUT, " &gt; ");
  
    if(leaf->isVariant) u_fprintf(lx->OUT, " [");


    if(styled)
    {
        if(leaf == current)
            u_fprintf(lx->OUT, "<b>");

        u_fprintf(lx->OUT, "<a href=\"?_=%s\">", leaf->str);
    }


    u_fprintf_u(lx->OUT, leaf->ustr);


    if(styled)
    {
        u_fprintf(lx->OUT, "</a>");

        if(leaf == current)
            u_fprintf(lx->OUT, "</b>");
    }

    if(leaf->isVariant) u_fprintf(lx->OUT, "] ");
}


void printLocaleLink(LXContext *lx, UBool toOpen, MySortable *l, const char *current, const char *restored, UBool *hadUnsupportedLocales)
{
    UBool supported;
    static UBool initStr = TRUE;
    U_STRING_DECL(BEGIN_LINK, "<a href=\"", 10);
    U_STRING_DECL(MIDDLE_LINK, "\">", 3);
    U_STRING_DECL(END_LINK, "</a>", 5);
    U_STRING_DECL(BEGIN_UNSUP, "<i><font color=\"#9999FF\">", 25);
    U_STRING_DECL(END_UNSUP, "</font></i>", 12);
    if (initStr) {
        U_STRING_INIT(BEGIN_LINK, "<a href=\"", 10);
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
        u_fprintf(lx->OUT, "%s/%s/",
                  lx->scriptName,
                  (char*)lx->cLocale);
        if(lx->setEncoding)
            u_fprintf(lx->OUT,"%s/", lx->chosenEncoding);	  
        u_fprintf(lx->OUT,"?_=%s", l->str);
    }
    else
    {
        u_fprintf(lx->OUT, "%s/%s/",
                  lx->scriptName,
                  l->str);
        if(lx->setEncoding)
            u_fprintf(lx->OUT,"%s/", lx->chosenEncoding);
      
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
        u_fprintf(lx->OUT, "&nbsp;[<font size=-1>&nbsp;");
        
        for(n=0;n<(l->nSubLocs);n++)
        {
            if(n > 0)
            {
                u_fprintf(lx->OUT, ", ");
            }
            printLocaleAndSubs(lx, toOpen, l->subLocs[n], current, restored, hadUnsupportedLocales);
        }
        
        u_fprintf(lx->OUT, "&nbsp;</font>]");
    }
}


void setupLocaleTree(LXContext *lx)
{
    const char *qs, *amp;
    char       *loc = lx->curLocaleName;

    /* setup base locale */
    lx->locales = createLocaleTree(lx->cLocale, &lx->numLocales);

    qs = lx->queryString;
    if(   qs &&
          (*(qs++) == '_') &&
          (*(qs++) == '='))
    {
        amp = strchr(qs,'&');
        if(!amp)
            amp = qs+strlen(qs);
      
        if((amp-qs)>100) /* safety */
	{
            strncpy(loc,qs,100);
            loc[100] = 0;
	}
        else
	{
            strncpy(loc,qs,amp-qs);
            loc[amp-qs] = 0;
	}
      
        /* setup cursors.. */
        lx->curLocale = findLocale(lx->locales, loc);

        if(lx->curLocale)
            lx->parLocale = lx->curLocale->parent;
        
    }
}
