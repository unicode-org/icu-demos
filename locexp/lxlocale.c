/**********************************************************************
*   Copyright (C) 1999-2002, International Business Machines
*   Corporation and others.  All Rights Reserved.
***********************************************************************/

#include "locexp.h"

/* routines having to do with locales and trees */


/* chooselocale --------------------------------------------------------------------------- */
void chooseLocale(LXContext *lx, const char *qs, UBool toOpen, const char *current, const char *restored, UBool showAll)
{
    UBool  hit = FALSE;
    int32_t n, j;
    UErrorCode status = U_ZERO_ERROR;
    UBool  hadUnsupportedLocales = FALSE;

    u_fprintf(lx->OUT, "<TABLE BORDER=2 CELLPADDING=2 CELLSPACING=2>\r\n");

    u_fprintf(lx->OUT, "<TR><TD><B>%U</B></TD><TD><B>%U</B></TD></TR>\r\n",
              FSWF("localeList_Locale", "Languages"),
              FSWF("localeList_Sublocale", "Regions"));

    u_fprintf(lx->OUT, "<TR><TD COLSPAN=2><A HREF=\"?_=%s\">%U</A></TD></TR>\r\n",
              lx->locales->str, lx->locales->ustr); /* default */ 

    mySort(lx->locales, &status, TRUE); /* need the whole thing sorted */

    for(n=0;n<lx->locales->nSubLocs;n++)
    {
        /* This will hide display locales - LANGUAGE level-  that don't exist. */
        if((toOpen == FALSE) && !isSupportedLocale(lx->locales->subLocs[n].str, FALSE) && showAll == FALSE)
            continue;

        u_fprintf(lx->OUT, "<TR>\r\n");


        hit = !strcmp(lx->locales->subLocs[n].str,current);

        if(hit)
	{
            u_fprintf(lx->OUT,"<TD VALIGN=TOP BGCOLOR=\"#FFDDDD\">");
	}
        else
            u_fprintf(lx->OUT,"<TD VALIGN=TOP>");

        if(hit)  
            u_fprintf(lx->OUT, "<b>");
      
      
        printLocaleLink(lx, toOpen, &(lx->locales->subLocs[n]), current, restored, &hadUnsupportedLocales);

        if(hit)
            u_fprintf(lx->OUT, "</b>");
      
        u_fprintf(lx->OUT, "</TD>");
      
        if(lx->locales->subLocs[n].nSubLocs)
	{
            u_fprintf(lx->OUT, "<TD>");
	  
            for(j=0;j< (lx->locales->subLocs[n].nSubLocs); j++)
	    {
#ifndef LX_SHOW_ALL_DISPLAY_LOCALES
                /* This will hide display locales- COUNTRY/VARIANT level that don't exist. */
                if((toOpen == FALSE) && !isSupportedLocale(lx->locales->subLocs[n].subLocs[j].str, FALSE) && (showAll ==FALSE) )
                    continue;
#endif

                if(j>0)
                    u_fprintf(lx->OUT, ", &nbsp;");
	      
                printLocaleAndSubs(lx, toOpen, &(lx->locales->subLocs[n].subLocs[j]), current, restored, &hadUnsupportedLocales);
	      
	    }

            u_fprintf(lx->OUT, "</TD>");
	}

        u_fprintf(lx->OUT, "</TR>\r\n");


    }

    u_fprintf(lx->OUT, "</TABLE>\r\n");

    if(hadUnsupportedLocales)
        u_fputs(FSWF("locale_experimental", "Locales in <I>Italics</I> are experimental and not officially supported."), lx->OUT);

    if(showAll == FALSE && toOpen == FALSE)
    {
        u_fprintf(lx->OUT, "<A HREF=\"?locale_all&%s\"><IMG BORDER=0 WIDTH=16 HEIGHT=16 SRC=\"../_/closed.gif\" ALT=\"\">%U</A>\r\n<BR>",
                  (qs&&strlen(qs)>7)?(qs+7):"",
                  FSWF("showAll", "Show All"));
    }
}



void printPath(LXContext *lx, const MySortable *leaf, const MySortable *current, UBool styled)
{

    if(!leaf) /* top level */
    {
        if(styled) 
            u_fprintf(lx->OUT, "<A HREF=\"?\">"); /* Reset */
      
        u_fprintf(lx->OUT, "%U",
                  FSWF("title", "ICU LocaleExplorer"));
      
        if(styled) 
            u_fprintf(lx->OUT, "</A>");

        return;
    }

  
    /* reverse order recursively */
    printPath(lx, leaf->parent,current,styled);


    u_fprintf(lx->OUT, " &gt; ");
  


    if(styled)
    {
        if(leaf == current)
            u_fprintf(lx->OUT, "<B>");

        u_fprintf(lx->OUT, "<A HREF=\"?_=%s\">", leaf->str);
    }

  
  
    u_fprintf(lx->OUT, "%U", leaf->ustr);



    if(styled)
    {
        u_fprintf(lx->OUT, "</A>");

        if(leaf == current)
            u_fprintf(lx->OUT, "</B>");

    }
}


void printLocaleLink(LXContext *lx, UBool toOpen, MySortable *l, const char *current, const char *restored, UBool *hadUnsupportedLocales)
{
    UBool supported;

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


    u_fprintf(lx->OUT, "<A HREF=\"");
  
    if(toOpen == TRUE)
    {
        u_fprintf(lx->OUT, "%s/%s/",
                  getenv("SCRIPT_NAME"),
                  (char*)lx->cLocale);
        if(lx->setEncoding)
            u_fprintf(lx->OUT,"%s/", lx->chosenEncoding);	  
        u_fprintf(lx->OUT,"?_=%s", l->str);
    }
    else
    {
        u_fprintf(lx->OUT, "%s/%s/",
                  getenv("SCRIPT_NAME"),
                  l->str);
        if(lx->setEncoding)
            u_fprintf(lx->OUT,"%s/", lx->chosenEncoding);
      
        if(restored)
            u_fprintf(lx->OUT, "?%s", restored);
    }
  
  
    u_fprintf(lx->OUT,"\">");

    if(!supported)
    {
        u_fprintf(lx->OUT, "<I><FONT COLOR=\"#9999FF\">");
        *hadUnsupportedLocales = TRUE;
    }

    if(toOpen == TRUE) 
    {
        u_fprintf(lx->OUT, "%U", l->ustr);
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
    
        u_fprintf(lx->OUT, "%U", displayName);
    }

    if(!supported)
        u_fprintf(lx->OUT, "</FONT></I>");
  
    u_fprintf(lx->OUT,"</A>");

  
}

void printLocaleAndSubs(LXContext *lx, UBool toOpen, MySortable *l, const char *current, const char *restored, UBool *hadUnsupportedLocales)
{
    int32_t n;


    printLocaleLink(lx, toOpen,l,current,restored, hadUnsupportedLocales);
  
    /* SRL todo: cull unsupported locales!  */
    if(l->nSubLocs)
    {
	u_fprintf(lx->OUT, "&nbsp;[<FONT SIZE=-1>&nbsp;");

        for(n=0;n<(l->nSubLocs);n++)
	{
            if(n > 0)
            {
                u_fprintf(lx->OUT, ", ");
            }
            printLocaleAndSubs(lx, toOpen, &(l->subLocs[n]), current, restored, hadUnsupportedLocales);
	}
      
	u_fprintf(lx->OUT, "&nbsp;</FONT SIZE=-1>]");
    }
}


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
        if(status == U_USING_DEFAULT_ERROR)
        {
            supp = FALSE;
        }
        else if( (!includeChildren) && (status == U_USING_FALLBACK_ERROR))
        {
            supp = FALSE;
        }
        else
        {
            int32_t len;

            status = U_ZERO_ERROR;
            ures_getStringByKey(newRB, "helpPrefix", &len, &status);

            if(status == U_USING_DEFAULT_ERROR)
            {
                supp = FALSE;
            }
            else
            {
                if( (!includeChildren) && (status == U_USING_FALLBACK_ERROR))
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
    if(U_FAILURE(status))
        supp = TRUE;
    else
    {
        const UChar *s = ures_getStringByKey(newRB, "Version", &len, &status);
      
        if(*s == 0x0078) /* If it starts with an 'x'.. */
            supp = TRUE;

        ures_close(newRB);
    }

    return supp;
}



void setupLocaleTree(LXContext *lx)
{
    const char *qs, *amp;
    char       *loc = lx->curLocaleName;

    /* setup base locale */
    lx->locales = createLocaleTree(lx->cLocale, &lx->numLocales);

    qs = getenv("QUERY_STRING");
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
