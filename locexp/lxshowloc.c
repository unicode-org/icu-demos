/**********************************************************************
*   Copyright (C) 1999-2002, International Business Machines
*   Corporation and others.  All Rights Reserved.
***********************************************************************/

#include "locexp.h"

/*  Main function for dumping the contents of a particular locale ---------------------------- */

void showOneLocale(LXContext *lx, char *b)
{
    char *tmp, *locale = NULL;
    UErrorCode status = U_ZERO_ERROR;
    UResourceBundle *myRB = NULL;
    UBool doShowSort = FALSE;
    const char *qs;

    qs = b;

    if(*b == '_')
    {
        b++;
      
        if(*b == '=')
	{
	  
            b++;
	  
            tmp =strchr(b,'&');
            if(tmp)
	    { 
                *tmp = 0;
	    }
	  
            locale = b;

            if(tmp)
	    {
                b = tmp;
                b++;
	    }
	}
    }


    if(! locale )
    {
        char *sampleChars;

        /* show the 'choose a locale' page */
        chooseLocale(lx, qs, TRUE, b, "", FALSE);

        /* show the demos */
        u_fprintf(lx->OUT, "<H3>%U</H3>\r\n<UL><LI>",
                  FSWF("demos", "Demos"));

        sampleChars = createEscapedSortList(FSWF("EXPLORE_CollationElements_g7sampleString","bad\\u000DBad\\u000DBat\\u000Dbat\\u000Db\\u00E4d\\u000DBzt\\u000DB\\u00E4d\\u000Db\\u00E4t\\u000DB\\u00E4t"));

        showExploreButtonSort(lx, NULL,"g7", sampleChars, "CollationElements", FALSE);
        
        free(sampleChars);

        u_fprintf(lx->OUT, " %U\r\n",
                  FSWF("explore_G7", "Try Multi-lingual Sorting"));

        u_fprintf(lx->OUT, "<LI>%U: ", FSWF("explore_search", "Search"));
        showExploreSearchForm(lx, NULL);

#ifdef LX_HAVE_XLITOMATIC
        u_fprintf(lx->OUT, "<LI><A HREF=\"/II/xlitomatic/%s/%s/\">%U</A>\r\n",
                  lx->cLocale, lx->chosenEncoding,
                  FSWF("explore_xlitomatic", "Translit-o-matic"));
#endif
        u_fprintf(lx->OUT, "<P></UL>\r\n");

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
    u_fprintf(lx->OUT, "%U", FSWF("localeDataWhat", "This page shows the localization data for the locale listed at left. "));

    if(strlen(locale)==2) /* just the language */
    {
        u_fprintf(lx->OUT, " %U",FSWF("localeDataLanguage","No region is specified, so the data is generic to this language."));
    }
    else if(!strcmp(locale,"root"))
    {
        u_fprintf(lx->OUT, " %U", FSWF("localeDataDefault", "This is the default localization data, which will be used if no other installed locale is specified."));
    }
    else if(locale[2] == '_')
    {
        if(strlen(locale) == 5)
	{
            u_fprintf(lx->OUT, " %U", FSWF("localeDataLangCountry", "This Locale contains data for this language, as well as conventions for this particular region."));
        }
        else
        {
            u_fprintf(lx->OUT, " %U", FSWF("localeDataLangCountryVariant", "This Locale contains data for this language, as well as conventions for a variant within this particular region."));
	}
    }

    if(strstr(locale, "_EURO"))
    {
        u_fprintf(lx->OUT, " %U", FSWF("localeDataEuro", "This Locale contains currency formatting information for use with the Euro currency."));
    }
#endif
  
    status = U_ZERO_ERROR;

    /* Show the explore.. things first. ======================*/
    if(strstr(b,"EXPLORE_DateTimePatterns"))
    {
        showExploreDateTimePatterns(lx, myRB, locale, b);
    }

    else if (strstr(b, "EXPLORE_NumberPatterns"))
    {
        showExploreNumberPatterns(lx, locale, b);
    }
    else if (strstr(b, "EXPLORE_Calendar"))
    {
        showExploreCalendar(lx, b);
    }
    else if (strstr(b, "EXPLORE_search"))
    {
        showExploreSearch(lx, b);
    }
    else if (strstr(b, "EXPLORE_srl"))
    {
        UChar ustr[] = { 0x0394, 0x03b5, 0x03c5, 0x03c4, 0x03ad, 0x03c1, 0x03b1, 0x0000 };
        u_fprintf(lx->OUT, "<HR>GRK: %U<BR><HR>\n", ustr);
    }
    else if (strstr(b, "EXPLORE_CollationElements"))
    {
        showKeyAndStartItem(lx, "EXPLORE_CollationElements", 
                            FSWF("EXPLORE_CollationElements", "Collation (sorting) Example"),
                            locale,
                            FALSE,
                            U_ZERO_ERROR);

        u_fprintf(lx->OUT, "%U<P>", FSWF("usortWhat","This example demonstrates sorting (collation) in this locale."));
        showSort(lx, locale, b);
      
        u_fprintf(lx->OUT, "</TD>");

        u_fprintf(lx->OUT, "<TD VALIGN=TOP ALIGN=RIGHT>");
        printHelpTag(lx, "EXPLORE_CollationElements", NULL);
        u_fprintf(lx->OUT, "</TD>");

        showKeyAndEndItem(lx, "EXPLORE_CollationElements", locale);
    }
    else /* ================================= Normal ShowXXXXX calls ===== */
    {
        /* %%%%%%%%%%%%%%%%%%%%%%%*/
        /*   LOCALE ID section %%%*/
        u_fprintf(lx->OUT, "<table border=0 cellspacing=0 cellpadding=0 width=\"100%\"><tr><td valign=TOP>");
        showLocaleCodes(lx, myRB, locale);
        u_fprintf(lx->OUT, "</TD><td>&nbsp;</td><td valign=TOP>");
        showInteger(lx, myRB, locale, "LocaleID", 16);
        u_fprintf(lx->OUT, "</TD><td>&nbsp;</td><td valign=TOP>");
        showString(lx, myRB, locale, b, "Version", FALSE);
        u_fprintf(lx->OUT, "</td></tr></table>");

        showLocaleScript(lx, myRB, locale);
        u_fprintf(lx->OUT, "<!-- %s:%d -->\r\n", __FILE__, __LINE__);

        showUnicodeSet(lx, myRB, locale, b, "ExemplarCharacters", FALSE);
        /*    showString(lx, myRB, locale, b, "ExemplarCharacters", FALSE); */
    
        showTaggedArray(lx, myRB, locale, b, "Languages");
        showTaggedArray(lx, myRB, locale, b, "Countries"); 
    
      
        /* %%%%%%%%%%%%%%%%%%%%%%%*/
        /*   Date/Time section %%%*/

        showShortLong(lx, myRB, locale, "Day", 
                      FSWF("DayAbbreviations", "Short Names"),
                      FSWF("DayNames", "Long Names"),7);
        showShortLong(lx, myRB, locale, "Month",
                      FSWF("MonthAbbreviations", "Short Names"),
                      FSWF("MonthNames", "Long Names"), 12);

        u_fprintf(lx->OUT, "&nbsp;<table cellpadding=0 cellspacing=0 width=\"100%\"><tr><td VALIGN=\"TOP\">");
    
        {
            const UChar *ampmDesc[3];
            ampmDesc[0] = FSWF("AmPmMarkers0", "am");
            ampmDesc[1] = FSWF("AmPmMarkers1", "pm");
            ampmDesc[2] = 0;
      
            showArrayWithDescription(lx, myRB, locale, ampmDesc, "AmPmMarkers");
        }
        u_fprintf(lx->OUT, "</td><td>&nbsp;</td><td VALIGN=\"TOP\">");
        showArray(lx, myRB, locale, "Eras");
        u_fprintf(lx->OUT, "</td></tr></table>");
    
    
        {
            const UChar *dtpDesc[10]; /* flms */
            dtpDesc[0] = FSWF("DateTimePatterns0", "Full Time");
            dtpDesc[1] = FSWF("DateTimePatterns1", "Long Time");
            dtpDesc[2] = FSWF("DateTimePatterns2", "Medium Time");
            dtpDesc[3] = FSWF("DateTimePatterns3", "Short Time");
            dtpDesc[4] = FSWF("DateTimePatterns4", "Full Date");
            dtpDesc[5] = FSWF("DateTimePatterns5", "Long Date");
            dtpDesc[6] = FSWF("DateTimePatterns6", "Medium Date");
            dtpDesc[7] = FSWF("DateTimePatterns7", "Short Date");
            dtpDesc[8] = FSWF("DateTimePatterns8", "Date-Time pattern.<BR>{0} = time, {1} = date");
            dtpDesc[9] = 0;
      
            showArrayWithDescription(lx, myRB, locale, dtpDesc, "DateTimePatterns");
        }
    
    
        { 
            const UChar *zsDesc[8];
            zsDesc[0] = FSWF("zoneStrings0", "Canonical");
            zsDesc[1] = FSWF("zoneStrings1", "Normal Name");
            zsDesc[2] = FSWF("zoneStrings2", "Normal Abbrev");
            zsDesc[3] = FSWF("zoneStrings3", "Summer/DST Name");
            zsDesc[4] = FSWF("zoneStrings4", "Summer/DST Abbrev");
            zsDesc[5] = FSWF("zoneStrings5", "Example City");
#ifndef LX_NO_USE_UTIMZONE
            zsDesc[6] = FSWF("zoneStrings6", "Raw Offset");
#else
            zsDesc[6] = 0;
#endif
            zsDesc[7] = 0;
      
            show2dArrayWithDescription(lx, myRB, locale, zsDesc, b, "zoneStrings");
        }
    
        /* locale pattern chars */
        {
            const UChar *charDescs[19];
      
            charDescs[0] = FSWF("localPatternChars0", "Era");
            charDescs[1] = FSWF("localPatternChars1", "Year");
            charDescs[2] = FSWF("localPatternChars2", "Month");
            charDescs[3] = FSWF("localPatternChars3", "Day of Month");
            charDescs[4] = FSWF("localPatternChars4", "Hour Of Day 1");
            charDescs[5] = FSWF("localPatternChars5", "Hour Of Day 0"); 
            charDescs[6] = FSWF("localPatternChars6", "Minute");
            charDescs[7] = FSWF("localPatternChars7", "Second");
            charDescs[8] = FSWF("localPatternChars8", "Millisecond");
            charDescs[9] = FSWF("localPatternChars9", "Day Of Week");
            charDescs[10] = FSWF("localPatternChars10", "Day Of Year");
            charDescs[11] = FSWF("localPatternChars11", "Day Of Week In Month");
            charDescs[12] = FSWF("localPatternChars12", "Week Of Year");
            charDescs[13] = FSWF("localPatternChars13", "Week Of Month");
            charDescs[14] = FSWF("localPatternChars14", "Am/Pm");
            charDescs[15] = FSWF("localPatternChars15", "Hour 1");
            charDescs[16] = FSWF("localPatternChars16", "Hour 0");
            charDescs[17] = FSWF("localPatternChars17", "Timezone");
            charDescs[18] = 0;
      
            showStringWithDescription(lx, myRB, locale, b, charDescs, "localPatternChars", TRUE);
        }
        showDateTimeElements(lx, myRB, locale);

        /* %%%%%%%%%%%%%%%%%%%%%%%*/
        /*     Numbers section %%%*/

        {
            const UChar *currDesc[4];
            currDesc[0] = FSWF("CurrencyElements0", "Currency symbol");
            currDesc[1] = FSWF("CurrencyElements1", "Int'l Currency symbol");
            currDesc[2] = FSWF("CurrencyElements2", "Currency separator");
            currDesc[3] = 0;
      
            showArrayWithDescription(lx, myRB, locale, currDesc, "CurrencyElements");
        }
    
    
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
            showArrayWithDescription(lx, myRB, locale, numDesc, "NumberElements");
        }
    
    
        { /*from dcfmtsym */
            const UChar *numDesc[5];
            numDesc[0] = FSWF("NumberPatterns0", "Decimal Pattern");
            numDesc[1] = FSWF("NumberPatterns1", "Currency Pattern");
            numDesc[2] = FSWF("NumberPatterns2", "Percent Pattern");
            numDesc[3] = FSWF("NumberPatterns3", "Scientific Pattern");
            numDesc[4] = 0;
      
            showArrayWithDescription(lx, myRB, locale, numDesc, "NumberPatterns");
        }
    
        showSpelloutExample(lx, myRB, locale);
        showString(lx, myRB, locale, b, "SpelloutRules", TRUE);
        showString(lx, myRB, locale, b, "OrdinalRules", TRUE);
        showString(lx, myRB, locale, b, "DurationRules", TRUE);

        /* %%%%%%%%%%%%%%%%%%%%%%%*/
        /*   Collation section %%%*/

        showCollationElements(lx, myRB, locale, b, "CollationElements");
    }

    ures_close(myRB);

}


