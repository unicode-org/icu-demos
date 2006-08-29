/**********************************************************************
*   Copyright (C) 1999-2006, International Business Machines
*   Corporation and others.  All Rights Reserved.
***********************************************************************/

#include "locexp.h"

/* routines having to do with the number pattern sample */

/*****************************************************************************
 *
 * Explorer for #'s
 */

void showExploreNumberPatterns(LXContext *lx, const char *locale)
{
    UChar pattern[1024];
    UChar tempChars[1024];
    UNumberFormat  *nf = NULL; /* numfmt in the current locale */
    UNumberFormat  *nf_default = NULL; /* numfmt in the default locale */
    UNumberFormat  *nf_spellout = NULL;
    UErrorCode   status = U_ZERO_ERROR;
    double   value;
    UChar valueString[1024];
  
    const UChar *defaultValueErr = 0;
    const UChar *localValueErr   = 0;
  
    const char *tmp;
  
    showKeyAndStartItem(lx, "EXPLORE_NumberPatterns", FSWF("EXPLORE_NumberPatterns", "Explore &gt; Numbers"), locale, FALSE, U_ZERO_ERROR);

    u_fprintf(lx->OUT, "%S<p>", FSWF("formatExample_NumberPatterns_What","This example demonstrates formatting of numbers in this locale."));

    exploreFetchNextPattern(lx, pattern, queryField(lx, "str")); 

    nf = unum_open(UNUM_DEFAULT,NULL, 0, locale, NULL, &status);
  
    if(U_FAILURE(status))
    {
        u_fprintf(lx->OUT, "</td></tr></table></td></tr></table><br /><hr />%S: ", FSWF("formatExample_errorOpen", "Couldn't open the formatter"));
        explainStatus(lx, status, "EXPLORE_NumberPattern");
        return; /* ? */
    }
  
    unum_applyPattern(nf, TRUE, pattern, -1, NULL, NULL);
  
    unum_toPattern(nf, FALSE, tempChars, 1024, &status);

    if(U_FAILURE(status))
    {
        u_fprintf(lx->OUT, "</td></tr></table></td></tr></table><br /><hr />  %S<p>", FSWF("formatExample_errorToPattern", "Couldn't convert the pattern [toPattern]"));
        explainStatus(lx, status, "EXPLORE_NumberPattern");
        return;
    }

    nf_default  = unum_open(UNUM_DEFAULT, NULL, 0, lx->dispLocale, NULL, &status);
    nf_spellout = unum_open(UNUM_SPELLOUT,NULL, -1, locale, NULL, &status);
  
    if(U_FAILURE(status))
    {
        u_fprintf(lx->OUT, "</td></tr></table></td></tr></table><br /><hr />%S<p>", FSWF("formatExample_errorOpenDefault", "Couldn't open the default number fmt"));
        explainStatus(lx, status, "EXPLORE_NumberPattern");
        return;
    }
  
    /* Load the default with a simplistic pattern .. */
    unum_applyPattern(nf_default, FALSE, FSWF("EXPLORE_NumberPatterns_defaultPattern", "#,###.###############"), -1, NULL, NULL);
      
    /* Allright. we've got 'nf' which is our custom pattern in the target 
       locale, and we've got 'nf_default' which is a pattern that we hope is
       reasonable for displaying a number in the *default* locale

       Confused yet?
    */

    value = 12345.6789; /* for now */

    /* Now, see if the user is trying to change the value. */
    if((tmp = queryField(lx,"NP_LOC"))) /* localized numbre */
    {
        /* Localized # */
        unescapeAndDecodeQueryField_enc(valueString, 1000, tmp, lx->convRequested);
        u_replaceChar(valueString, 0x0020, 0x00A0);
      

        status = U_ZERO_ERROR;
        value = unum_parseDouble(nf, valueString, -1, 0, &status);
      
        if(U_FAILURE(status))
	{
            status = U_ZERO_ERROR;
            localValueErr = FSWF("formatExample_errorParse_num", "Could not parse this, replaced with a default value.");
	}
    }
    else if ((tmp = queryField(lx,"NP_DEF")) || (tmp = queryField(lx,"NP_DBL")))
    { /* Default side, or number (NP_DBL) coming from somewhere else */
        /* Localized # */
        unescapeAndDecodeQueryField_enc(valueString, 1000, tmp, lx->convRequested);
        u_replaceChar(valueString, 0x0020, 0x00A0);


        status = U_ZERO_ERROR;
        value = unum_parseDouble(nf_default, valueString, -1, 0, &status);
      
        if(U_FAILURE(status))
        {
            status = U_ZERO_ERROR;
            defaultValueErr = FSWF("formatExample_errorParse3", "Could not parse this, replaced with a default value.");
        }
    }
    else if ((tmp = queryField(lx, "NP_SPL")))
    {
        unescapeAndDecodeQueryField_enc(valueString, 1000, tmp, lx->convRequested);
        u_replaceChar(valueString, 0x00A0, 0x0020);  /* Spellout doesn't want to see NBSP's */
    

        status = U_ZERO_ERROR;
        value = unum_parseDouble(nf_spellout, valueString, -1, 0, &status);
    
        if(U_FAILURE(status))
        {
            status = U_ZERO_ERROR;
            defaultValueErr = FSWF("formatExample_errorParse3", "Could not parse this, replaced with a default value.");
        }
    }

    /** TODO: replace with:
      
        case NP_LOC:
        value = unum_parseDouble(nf, str, ... )
        break;
      
        case NP_DEF:
        value = unum_parseDouble(defaultNF, str, ... );
        break;
    **/

    /* NOW we are ready ! */

    /* display the FORM, and fetch the current pattern */
    exploreShowPatternForm(lx, pattern, locale, "NumberPatterns", queryField(lx, "str"), value, nf_default); 


    /* Now, display the results in <default> and in their locale */
    u_fprintf(lx->OUT, "<table border=\"1\"><tr><td>\r\n");


    /* ============ 'default' side of the table ==========  */

    unum_formatDouble(nf_default,value,tempChars, 1024, 0, &status);

    if(U_FAILURE(status))
    {
        u_fprintf(lx->OUT, "%S<p>", FSWF("formatExample_errorFormatDefault", "Unable to format number using default version of the pattern"));
        explainStatus(lx, status, "EXPLORE_NumberPattern");
    }
    else
    {
      
        u_fprintf(lx->OUT, "<b><i>%S</i></b><br />\r\n", defaultLanguageDisplayName(lx));
        u_fprintf(lx->OUT, "<form method=\"post\" action=\"%s#EXPLORE_NumberPatterns\">\r\n",
                  getLXBaseURL(lx, kNO_URL));
        u_fprintf(lx->OUT, "<input type=HIDDEN name=\"str\" VALUE=\"");
        writeEscaped(lx, pattern);
        u_fprintf(lx->OUT, "\" />\r\n");

        u_fprintf(lx->OUT, "<textarea name=NP_DEF rows=1 cols=20>");
        lx->backslashCtx.html = FALSE;
        u_fprintf(lx->OUT, "%S", tempChars); 
        lx->backslashCtx.html = TRUE;
      
        status = U_ZERO_ERROR;
      
        u_fprintf(lx->OUT, "</textarea><input type=submit value=\"%S\" /></form>", FSWF("EXPLORE_change", "Change"));
      
    }
  
    u_fprintf(lx->OUT, "</td><td width=1 bgcolor=\"#EEEEEE\"><img src=\"" LDATA_PATH "c.gif\" width=\"0\" height=\"0\" alt=\"-\" /></td><td>");

    /* ============ 'localized' side ================================= */

    unum_formatDouble(nf,value,tempChars, 1024, 0, &status);

    if(U_FAILURE(status))
    {
        u_fprintf(lx->OUT, "%S<p>", FSWF("formatExample_errorFormat_number", "Couldn't format the number."));
        explainStatus(lx, status, "EXPLORE_NumberPattern");
    }
    else
    {
        /*  === local side */
        u_fprintf(lx->OUT, "\r\n\r\n<!--  LOCALIZED SIDE -->\r\n<B>%S</B><br />\r\n",lx->curLocale?lx->curLocale->ustr:FSWF("NoLocale","MISSING LOCALE NAME") );
        u_fprintf(lx->OUT, "<form method=\"post\" action=\"%s#EXPLORE_NumberPatterns\">\r\n",
                  getLXBaseURL(lx, kNO_URL));
        u_fprintf(lx->OUT, "<input type=\"hidden\" name=\"str\" value=\"");
        writeEscaped(lx, pattern);
        u_fprintf(lx->OUT, "\" />\r\n");
      
        u_fprintf(lx->OUT, "<textarea name=\"NP_LOC\" rows=\"1\" cols=\"20\">");
        writeEscaped(lx, tempChars);
        u_fprintf(lx->OUT, "</textarea><input type=\"submit\" value=\"%S\" /></form>", FSWF("EXPLORE_change", "Change"));

        if(localValueErr)
            u_fprintf(lx->OUT, "<p>%S", localValueErr);
    }
    /*  ============== End of the default/localized split =============== */

    u_fprintf(lx->OUT, "</td></tr>");


    /* ============== Spellout ================== */
    u_fprintf(lx->OUT, "<tr><td colspan=3>\r\n");
    u_fprintf(lx->OUT, "<form method=\"post\" action=\"%s#EXPLORE_NumberPatterns\">\r\n",
              getLXBaseURL(lx, kNO_URL));
    u_fprintf(lx->OUT, "<input type=\"hidden\" name=\"str\" value=\"");
    writeEscaped(lx, pattern);
    u_fprintf(lx->OUT, "\" />\r\n");

    u_fprintf(lx->OUT, "<b>%S</b> ", FSWF("Spellout", "Spellout"));


    if(hasQueryField(lx, "NP_SPL"))
    {  
        u_fprintf(lx->OUT, "<br />%S<br />\r\n", valueString);
    }

    unum_formatDouble(nf_spellout, value, tempChars, 1024,0, &status);


    u_fprintf(lx->OUT, "<textarea name=NP_SPL rows=1 cols=60>");
    lx->backslashCtx.html = FALSE;
    if(U_FAILURE(status))
    {
        u_fprintf(lx->OUT, "%S<p>", FSWF("formatExample_errorFormat_number", "Couldn't format the number."));
        explainStatus(lx, status, "EXPLORE_NumberPattern");
    }
    else
    {
        u_fprintf(lx->OUT, "%S", tempChars); 
    }
    lx->backslashCtx.html = TRUE;
  
    status = U_ZERO_ERROR;
  
    u_fprintf(lx->OUT, "</textarea><input type=\"submit\" value=\"%S\" /></form>", FSWF("EXPLORE_change", "Change"));
  
    /* == end spellout == */

    u_fprintf(lx->OUT, "</td></tr>\r\n");

    u_fprintf(lx->OUT, "</table>");

    if(nf)
        unum_close(nf);

    if(nf_default)
        unum_close(nf_default);

    if(nf_spellout)
        unum_close(nf_spellout);
  
    showExploreCloseButton(lx, locale, "NumberPatterns");
    u_fprintf(lx->OUT, "</td><td align=\"left\" valign=\"top\">");
    printHelpTag(lx, "EXPLORE_NumberPatterns", NULL);

    showKeyAndEndItem(lx, "EXPLORE_NumberPatterns", locale);
}

