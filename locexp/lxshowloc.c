/**********************************************************************
*   Copyright (C) 1999-2012, International Business Machines
*   Corporation and others.  All Rights Reserved.
***********************************************************************/

#include "locexp.h"
#include "unicode/udata.h"
#include "unicode/unumsys.h"
#include "unicode/unum.h"

#define U_ICUDATA_LANG U_ICUDATA_NAME U_TREE_SEPARATOR_STRING "lang"
#define U_ICUDATA_REGION U_ICUDATA_NAME U_TREE_SEPARATOR_STRING "region"
#define U_ICUDATA_CURR U_ICUDATA_NAME U_TREE_SEPARATOR_STRING "curr"
#define U_ICUDATA_ZONE U_ICUDATA_NAME U_TREE_SEPARATOR_STRING "zone"

static void showSubArray(LXContext *lx, const char *locale, const char *transKey, const char *path, const char *keyPath, UBool compareToDisplay) {
  UResourceBundle *subRb = NULL;
  UErrorCode status = U_ZERO_ERROR;

  subRb = ures_open(path, locale, &status);
  if(U_SUCCESS(status)) {
    showTaggedArray(lx,subRb,locale,transKey,compareToDisplay);
  }
  ures_close(subRb);
}



/*  Main function for dumping the contents of a particular locale ---------------------------- */

void showOneLocale(LXContext *lx)
{
  /*char *tmp;*/
    const char *locale = NULL;
    char   nsys[10];
    UErrorCode status = U_ZERO_ERROR;
    UResourceBundle *myRB = NULL;
    
    
    UNumberFormat *ourDecFmt;
    UNumberFormat *ourCurFmt;

    locale = lx->curLocaleName;
    if(!locale && !*locale) {
      locale = NULL;
    }

    if(! locale )
    {
        /* show the 'choose a locale' page */
        chooseLocale(lx, TRUE, "", "", FALSE);

        /* show the demos */
        u_fprintf(lx->OUT, "<table border=\"0\" width=\"100%%\"><tr><td valign=\"top\">");
        u_fprintf(lx->OUT, " %S\r\n",
                  FSWF("explore_G7", "Try Multi-lingual Sorting"));

        showExploreButtonSort(lx, NULL,"g7", "CollationElements", FALSE);

        u_fprintf(lx->OUT, "<td width=\"50%%\"></td><td valign=\"top\">\r\n");

        u_fprintf(lx->OUT, "<b>%S</b>:</td><td valign=\"top\">", FSWF("explore_search", "Search"));
        showExploreSearchForm(lx, NULL);
        u_fprintf(lx->OUT, "</td></tr></table>\r\n");

#ifdef LX_HAVE_XLITOMATIC
        u_fprintf(lx->OUT, "<li><a href=\"/II/xlitomatic/%s/%s/\">%S</a>\r\n",
                  lx->dispLocale, lx->chosenEncoding,
                  FSWF("explore_xlitomatic", "Translit-o-matic"));
#endif
        return; /* BREAK out */
    }

    myRB = ures_open(NULL, locale, &status);


    {
      UErrorCode nsErr = U_ZERO_ERROR;
      const char *theNs = NULL;
      UNumberingSystem *sys = unumsys_open(locale, &nsErr);
      if(!U_FAILURE(nsErr)) {
        theNs = unumsys_getName(sys);
        strcpy(nsys,theNs);
      } else {
        nsys[0]=0;
        u_fprintf(lx->OUT,"<b>An error occured [%s] getting the default number system for [%s]. Perhaps it doesn't exist? </b><br /><hr />\r\n",u_errorName(nsErr), locale);
        return;
      }

      ourDecFmt =  unum_open(UNUM_DEFAULT, NULL, 0, locale, NULL, &status);

      unumsys_close(sys);
    }


    if(U_FAILURE(status))
    {
      u_fprintf(lx->OUT,"<b>An error occured [%s] opening that resource bundle or decimal format [%s]. Perhaps it doesn't exist? </b><br /><hr />\r\n",u_errorName(status), locale);
        return;
    }

    explainStatus(lx, status,"top");

    /*   u_fprintf(lx->OUT, "</td></tr><tr><td colspan=2>"); */

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
        u_fprintf(lx->OUT, "<hr />GRK: %S<br /><hr />\n", ustr);
    }
    else if (!strcmp(lx->section, "brk"))
    {
        showExploreBreak(lx, locale);
    }
    else if (!strcmp(lx->section, "col"))
    {
        showSort(lx, locale);
        
        u_fprintf(lx->OUT, "<table width=\"100%%\"><tr>\r\n"); 

        u_fprintf(lx->OUT, "<td valign=\"top\" align=\"right\">");
        printHelpTag(lx, "EXPLORE_CollationElements", NULL);

        showKeyAndEndItem(lx, "EXPLORE_CollationElements", locale);
    }
    else if(lx->section[0]=='i')
    {
        showICIR(lx);
    }
    else /* ================================= Normal ShowXXXXX calls ===== */
    {
		u_fprintf(lx->OUT, "<br />\r\n");
	/*if(!getenv("SERVER_NAME") || strncmp(getenv("SERVER_NAME"),"oss",3)) {
		u_fprintf(lx->OUT, "<a href=\"%s&amp;x=iloc\">%S</a><br />",
			getLXBaseURL(lx, kNO_SECT),
			FSWF("icirView", "Switch to Survey View"));
	 } else {
		u_fprintf(lx->OUT, "<!-- survey disabled -->");
	}*/
        /* %%%%%%%%%%%%%%%%%%%%%%%*/
        /*   LOCALE ID section %%%*/
        u_fprintf(lx->OUT, "<table border=\"0\" cellspacing=\"0\" cellpadding=\"0\" width=\"100%%\"><tr><td valign=\"top\">");
        showLocaleCodes(lx, myRB, locale);
        u_fprintf(lx->OUT, "</td><td>&nbsp;</td><td valign=\"top\">");

        showKeyAndStartItem(lx, "LocaleID", NULL, locale, FALSE, status);
        u_fprintf(lx->OUT, "0x%X", uloc_getLCID(locale));
        showKeyAndEndItem(lx, "LocaleID", locale);

        u_fprintf(lx->OUT, "</td><td>&nbsp;</td><td valign=\"top\">");
        showString(lx, myRB, locale, "Version", FALSE);
        u_fprintf(lx->OUT, "</td></tr></table>");

        showLocaleScript(lx, myRB, locale);
        u_fprintf(lx->OUT, "<!-- %s:%d -->\r\n", __FILE__, __LINE__);

        showUnicodeSet(lx, myRB, locale, "ExemplarCharacters", FALSE);
        /*    showString(lx, myRB, locale, "ExemplarCharacters", FALSE); */
    
        showSubArray(lx, locale, "Languages",
                     U_ICUDATA_LANG,
                     "Languages",
                     TRUE);
        showSubArray(lx, locale, "Scripts",
                     U_ICUDATA_LANG,
                     "Scripts",
                     TRUE);
        showSubArray(lx, locale, "Countries",
                     U_ICUDATA_REGION,
                     "Countries",
                     TRUE);
        

        /* TODO: can't do this yet, need C wrapper for Calendar.getFDOW etc  */
        showDateTime(lx, myRB, locale);

        /* %%%%%%%%%%%%%%%%%%%%%%%*/
        /*     Numbers section %%%*/


        /* Currency: needs rewrite  */
#if 0
        showCurrencies(lx, myRB, locale);
#endif


#if 0
        u_fprintf(lx->OUT, "<h3>Numbering System: %s</h3>", nsys);
#else
        showNumberSystem(lx, locale, nsys);
#endif

    
#if 0
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
            /* need more .. */
            numDesc[11] = 0;
            showDecimSymsWithDescription(lx, ourDecFmt, locale, nsys, numDesc, "NumberElements", kNormal);
        }
#endif

#if 0
        { /*from dcfmtsym */
          const UChar *00numDesc[5]; /* MOVED TO LXNUM.C */
            numDesc[0] = FSWF("NumberPatterns0", "Decimal Pattern");
            numDesc[1] = FSWF("NumberPatterns1", "Currency Pattern");
            numDesc[2] = FSWF("NumberPatterns2", "Percent Pattern");
            numDesc[3] = FSWF("NumberPatterns3", "Scientific Pattern");
            numDesc[4] = 0;
      
            showArrayWithDescription(lx, myRB, locale, numDesc, "NumberPatterns", kNormal);
        }

        {
            UResourceBundle* nfrb = ures_open(U_ICUDATA_NAME U_TREE_SEPARATOR_STRING "rbnf", locale, &status);
            if (status != U_USING_DEFAULT_WARNING) {
                showSpelloutExample(lx, nfrb, locale);
                showString(lx, nfrb, locale, "SpelloutRules", TRUE);
                showString(lx, nfrb, locale, "OrdinalRules", TRUE);
                showString(lx, nfrb, locale, "DurationRules", TRUE);
            }
            else {
                /* No data at all :-( */
                showString(lx, myRB, locale, "SpelloutRules", TRUE);
                showString(lx, myRB, locale, "OrdinalRules", TRUE);
                showString(lx, myRB, locale, "DurationRules", TRUE);
            }
            ures_close(nfrb);
        }
#endif

        /* %%%%%%%%%%%%%%%%%%%%%%%*/
        /*   Collation section %%%*/

        showCollationElements(lx, myRB, locale, "CollationElements");
    }

    ures_close(myRB);

}


