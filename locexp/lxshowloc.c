/**********************************************************************
*   Copyright (C) 1999-2004, International Business Machines
*   Corporation and others.  All Rights Reserved.
***********************************************************************/

#include "locexp.h"

/*  Main function for dumping the contents of a particular locale ---------------------------- */

void showOneLocale(LXContext *lx)
{
  /*char *tmp;*/
    const char *locale = NULL;
    UErrorCode status = U_ZERO_ERROR;
    UResourceBundle *myRB = NULL;

    locale = lx->curLocaleName;
    if(!locale && !*locale) {
      locale = NULL;
    }

    if(! locale )
    {
        /* show the 'choose a locale' page */
        chooseLocale(lx, TRUE, "", "", FALSE);

        /* show the demos */
        u_fprintf(lx->OUT, "<table border=0 width=\"100%%\" summary=\"--\"><tr><td valign=top>");
        u_fprintf(lx->OUT, " %S\r\n",
                  FSWF("explore_G7", "Try Multi-lingual Sorting"));

        showExploreButtonSort(lx, NULL,"g7", "CollationElements", FALSE);

        u_fprintf(lx->OUT, "<td width=\"50%%\"></td><td valign=top>\r\n");

        u_fprintf(lx->OUT, "<b>%S</b>:</td><td valign=top>", FSWF("explore_search", "Search"));
        showExploreSearchForm(lx, NULL);
        u_fprintf(lx->OUT, "</td></tr></table>\r\n");

#ifdef LX_HAVE_XLITOMATIC
        u_fprintf(lx->OUT, "<LI><A HREF=\"/II/xlitomatic/%s/%s/\">%S</A>\r\n",
                  lx->dispLocale, lx->chosenEncoding,
                  FSWF("explore_xlitomatic", "Translit-o-matic"));
#endif
        return; /* BREAK out */
    }

    myRB = ures_open(NULL, locale, &status);


    if(U_FAILURE(status))
    {
        u_fprintf(lx->OUT,"<B>An error occured [%d] opening that resource bundle [%s]. Perhaps it doesn't exist? </B><BR><HR>\r\n",status, locale);
        return;
    }

    explainStatus(lx, status,"top");

    /*   u_fprintf(lx->OUT, "</TD></TR><TR><TD COLSPAN=2>"); */

    /* analyze what kind of locale we've got.  Should this be a little smarter? */
#if 0
    /* "friendly" messages */
    u_fprintf(lx->OUT, "%S", FSWF("localeDataWhat", "This page shows the localization data for the locale listed at left. "));

    if(strlen(locale)==2) /* just the language */
    {
        u_fprintf(lx->OUT, " %S",FSWF("localeDataLanguage","No region is specified, so the data is generic to this language."));
    }
    else if(!strcmp(locale,"root"))
    {
        u_fprintf(lx->OUT, " %S", FSWF("localeDataDefault", "This is the default localization data, which will be used if no other installed locale is specified."));
    }
    else if(locale[2] == '_')
    {
        if(strlen(locale) == 5)
	{
            u_fprintf(lx->OUT, " %S", FSWF("localeDataLangCountry", "This Locale contains data for this language, as well as conventions for this particular region."));
        }
        else
        {
            u_fprintf(lx->OUT, " %S", FSWF("localeDataLangCountryVariant", "This Locale contains data for this language, as well as conventions for a variant within this particular region."));
	}
    }

    if(strstr(locale, "_EURO"))
    {
        u_fprintf(lx->OUT, " %S", FSWF("localeDataEuro", "This Locale contains currency formatting information for use with the Euro currency."));
    }
#endif
  
    status = U_ZERO_ERROR;

    /* Show the explore.. things first. ======================*/
    if(!strcmp(lx->section,"dat"))
    {
        showExploreDateTimePatterns(lx, myRB, locale);
    }
    else if (!strcmp(lx->section, "num"))
    {
      showExploreNumberPatterns(lx, locale);
    }
    else if (!strcmp(lx->section, "cal"))
    {
      showExploreCalendar(lx);
    }
    else if (!strcmp(lx->section, "src"))
    {
      showExploreSearch(lx);
    }
    else if (!strcmp(lx->section, "srl"))
    {
        UChar ustr[] = { 0x0394, 0x03b5, 0x03c5, 0x03c4, 0x03ad, 0x03c1, 0x03b1, 0x0000 };
        u_fprintf(lx->OUT, "<HR>GRK: %S<BR><HR>\n", ustr);
    }
    else if (!strcmp(lx->section, "brk"))
    {
        showExploreBreak(lx, locale);
    }
    else if (!strcmp(lx->section, "col"))
    {
        showSort(lx, locale);
        
        u_fprintf(lx->OUT, "<table width=100%%><tr>\r\n"); 

        u_fprintf(lx->OUT, "<TD VALIGN=TOP ALIGN=RIGHT>");
        printHelpTag(lx, "EXPLORE_CollationElements", NULL);
        u_fprintf(lx->OUT, "</TD>");

        showKeyAndEndItem(lx, "EXPLORE_CollationElements", locale);
    }
    else if(lx->section[0]=='i')
    {
        showICIR(lx);
    }
    else /* ================================= Normal ShowXXXXX calls ===== */
    {
	if(!getenv("SERVER_NAME") || strncmp(getenv("SERVER_NAME"),"oss",3)) {
		u_fprintf(lx->OUT, "<a href=\"%s&x=iloc\">%S</a><br>",
			getLXBaseURL(lx, kNO_SECT),
			FSWF("icirView", "Switch to Survey View"));
	 } else {
		u_fprintf(lx->OUT, "<!-- survey disabled -->");
	}
        /* %%%%%%%%%%%%%%%%%%%%%%%*/
        /*   LOCALE ID section %%%*/
        u_fprintf(lx->OUT, "<table border=0 cellspacing=0 cellpadding=0 width=\"100%%\"><tr><td valign=TOP>");
        showLocaleCodes(lx, myRB, locale);
        u_fprintf(lx->OUT, "</TD><td>&nbsp;</td><td valign=TOP>");

        showKeyAndStartItem(lx, "LocaleID", NULL, locale, FALSE, status);
        u_fprintf(lx->OUT, "0x%X</TD>", uloc_getLCID(locale));
        showKeyAndEndItem(lx, "LocaleID", locale);

        u_fprintf(lx->OUT, "</TD><td>&nbsp;</td><td valign=TOP>");
        showString(lx, myRB, locale, "Version", FALSE);
        u_fprintf(lx->OUT, "</td></tr></table>");

        showLocaleScript(lx, myRB, locale);
        u_fprintf(lx->OUT, "<!-- %s:%d -->\r\n", __FILE__, __LINE__);

        showUnicodeSet(lx, myRB, locale, "ExemplarCharacters", FALSE);
        /*    showString(lx, myRB, locale, "ExemplarCharacters", FALSE); */
    
        showTaggedArray(lx, myRB, locale, "Languages", TRUE);
        showTaggedArray(lx, myRB, locale, "Scripts", TRUE); 
        showTaggedArray(lx, myRB, locale, "Countries", TRUE); 
    

        showDateTime(lx, myRB, locale);

        /* %%%%%%%%%%%%%%%%%%%%%%%*/
        /*     Numbers section %%%*/
        showCurrencies(lx, myRB, locale);
    
        { /*from dcfmtsym */
            const UChar *numDesc[12];
            numDesc[0] = FSWF("NumberElements0", "Decimal Separator");
            numDesc[1] = FSWF("NumberElements1", "Grouping Separator");
            numDesc[2] = FSWF("NumberElements2", "Pattern Separator");
            numDesc[3] = FSWF("NumberElements3", "Percent");
            numDesc[4] = FSWF("NumberElements4", "ZeroDigit");
            numDesc[5] = FSWF("NumberElements5", "Digit");
            numDesc[6] = FSWF("NumberElements6", "Minus Sign");
            numDesc[7] = FSWF("NumberElements7", "Exponential");
            numDesc[8] = FSWF("NumberElements8", "PerMill [/1000]");
            numDesc[9] = FSWF("NumberElements9", "Infinity");
            numDesc[10] = FSWF("NumberElements10", "Not a number");
            numDesc[11] = 0;
            showArrayWithDescription(lx, myRB, locale, numDesc, "NumberElements", kNormal);
        }
    
    
        { /*from dcfmtsym */
            const UChar *numDesc[5];
            numDesc[0] = FSWF("NumberPatterns0", "Decimal Pattern");
            numDesc[1] = FSWF("NumberPatterns1", "Currency Pattern");
            numDesc[2] = FSWF("NumberPatterns2", "Percent Pattern");
            numDesc[3] = FSWF("NumberPatterns3", "Scientific Pattern");
            numDesc[4] = 0;
      
            showArrayWithDescription(lx, myRB, locale, numDesc, "NumberPatterns", kNormal);
        }
    
        showSpelloutExample(lx, myRB, locale);
        showString(lx, myRB, locale, "SpelloutRules", TRUE);
        showString(lx, myRB, locale, "OrdinalRules", TRUE);
        showString(lx, myRB, locale, "DurationRules", TRUE);

        /* %%%%%%%%%%%%%%%%%%%%%%%*/
        /*   Collation section %%%*/

        showCollationElements(lx, myRB, locale, "CollationElements");
    }

    ures_close(myRB);

}


