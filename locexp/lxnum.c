/**********************************************************************
*   Copyright (C) 1999-2002, International Business Machines
*   Corporation and others.  All Rights Reserved.
***********************************************************************/

#include "locexp.h"

/* routines having to do with the number pattern sample */

/*****************************************************************************
 *
 * Explorer for #'s
 */

void showExploreNumberPatterns(LXContext *lx, const char *locale, const char *b)
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

    u_fprintf(lx->OUT, "%U<P>", FSWF("formatExample_NumberPatterns_What","This example demonstrates formatting of numbers in this locale."));

    exploreFetchNextPattern(lx, pattern, strstr(b,"EXPLORE_NumberPatterns")); 

    nf = unum_open(UNUM_DEFAULT,NULL, 0, locale, NULL, &status);
  
    if(U_FAILURE(status))
    {
        u_fprintf(lx->OUT, "</TD></TR></TABLE></TD></TR></TABLE><P><HR>%U: ", FSWF("formatExample_errorOpen", "Couldn't open the formatter"));
        explainStatus(lx, status, "EXPLORE_NumberPattern");
        return; /* ? */
    }
  
    unum_applyPattern(nf, TRUE, pattern, -1, NULL, NULL);
  
    unum_toPattern(nf, FALSE, tempChars, 1024, &status);

    if(U_FAILURE(status))
    {
        u_fprintf(lx->OUT, "</TD></TR></TABLE></TD></TR></TABLE><P><HR>  %U<P>", FSWF("formatExample_errorToPattern", "Couldn't convert the pattern [toPattern]"));
        explainStatus(lx, status, "EXPLORE_NumberPattern");
        return;
    }

    nf_default  = unum_open(UNUM_DEFAULT, NULL, 0, lx->cLocale, NULL, &status);
    nf_spellout = unum_open(UNUM_SPELLOUT,NULL, -1, locale, NULL, &status);
  
    if(U_FAILURE(status))
    {
        u_fprintf(lx->OUT, "</TD></TR></TABLE></TD></TR></TABLE><P><HR>%U<P>", FSWF("formatExample_errorOpenDefault", "Couldn't open the default number fmt"));
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
    if((tmp = strstr(b,"NP_LOC"))) /* localized numbre */
    {
        /* Localized # */
        tmp += 7;

        unescapeAndDecodeQueryField_enc(valueString, 1000, tmp, lx->chosenEncoding);
        u_replaceChar(valueString, 0x0020, 0x00A0);
      

        status = U_ZERO_ERROR;
        value = unum_parseDouble(nf, valueString, -1, 0, &status);
      
        if(U_FAILURE(status))
	{
            status = U_ZERO_ERROR;
            localValueErr = FSWF("formatExample_errorParse_num", "Could not parse this, replaced with a default value.");
	}
    }
    else if ((tmp = strstr(b,"NP_DEF")) || (tmp = strstr(b,"NP_DBL")))
    { /* Default side, or number (NP_DBL) coming from somewhere else */
        /* Localized # */
        tmp += 7;

        unescapeAndDecodeQueryField_enc(valueString, 1000, tmp, lx->chosenEncoding);
        u_replaceChar(valueString, 0x0020, 0x00A0);


        status = U_ZERO_ERROR;
        value = unum_parseDouble(nf_default, valueString, -1, 0, &status);
      
        if(U_FAILURE(status))
        {
            status = U_ZERO_ERROR;
            defaultValueErr = FSWF("formatExample_errorParse3", "Could not parse this, replaced with a default value.");
        }
    }
    else if (tmp = strstr(b, "NP_SPL"))
    {
        tmp += 7;
        unescapeAndDecodeQueryField_enc(valueString, 1000, tmp, lx->chosenEncoding);
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
    exploreShowPatternForm(lx, pattern, locale, "NumberPatterns", strstr(b,"EXPLORE_NumberPatterns"), value, nf_default); 


    /* Now, display the results in <default> and in their locale */
    u_fprintf(lx->OUT, "<TABLE BORDER=1><TR><TD>\r\n");


    /* ============ 'default' side of the table ==========  */

    unum_formatDouble(nf_default,value,tempChars, 1024, 0, &status);

    if(U_FAILURE(status))
    {
        u_fprintf(lx->OUT, "%U<P>", FSWF("formatExample_errorFormatDefault", "Unable to format number using default version of the pattern"));
        explainStatus(lx, status, "EXPLORE_NumberPattern");
    }
    else
    {
      
        u_fprintf(lx->OUT, "<B><I>%U</I></B><BR>\r\n", defaultLanguageDisplayName(lx));
        u_fprintf(lx->OUT, "<FORM METHOD=GET ACTION=\"#EXPLORE_NumberPatterns\">\r\n");
        u_fprintf(lx->OUT, "<INPUT NAME=_ TYPE=HIDDEN VALUE=%s>\r\n", locale);
        u_fprintf(lx->OUT, "<INPUT TYPE=HIDDEN NAME=EXPLORE_NumberPatterns VALUE=\"");
        writeEscaped(lx, pattern);
        u_fprintf(lx->OUT, "\">\r\n");

        u_fprintf(lx->OUT, "<TEXTAREA NAME=NP_DEF ROWS=1 COLS=20>");
        lx->backslashCtx.html = FALSE;
        u_fprintf(lx->OUT, "%U", tempChars); 
        lx->backslashCtx.html = TRUE;
      
        status = U_ZERO_ERROR;
      
        u_fprintf(lx->OUT, "</TEXTAREA><INPUT TYPE=SUBMIT VALUE=\"%U\"></FORM>", FSWF("EXPLORE_change", "Change"));
      
    }
  
    u_fprintf(lx->OUT, "</TD><TD WIDTH=1 BGCOLOR=\"#EEEEEE\"><IMG SRC=\"../_/c.gif\" ALT=\"---\" WIDTH=0 HEIGHT=0></TD><TD>");

    /* ============ 'localized' side ================================= */

    unum_formatDouble(nf,value,tempChars, 1024, 0, &status);

    if(U_FAILURE(status))
    {
        u_fprintf(lx->OUT, "%U<P>", FSWF("formatExample_errorFormat_number", "Couldn't format the number."));
        explainStatus(lx, status, "EXPLORE_NumberPattern");
    }
    else
    {
        /*  === local side */
        u_fprintf(lx->OUT, "\r\n\r\n<!--  LOCALIZED SIDE -->\r\n<B>%U</B><BR>\r\n",lx->curLocale?lx->curLocale->ustr:FSWF("NoLocale","MISSING LOCALE NAME") );
        u_fprintf(lx->OUT, "<FORM METHOD=GET ACTION=\"#EXPLORE_NumberPatterns\">\r\n");
        u_fprintf(lx->OUT, "<INPUT NAME=_ TYPE=HIDDEN VALUE=%s>\r\n", locale);
        u_fprintf(lx->OUT, "<INPUT TYPE=HIDDEN NAME=EXPLORE_NumberPatterns VALUE=\"");
        writeEscaped(lx, pattern);
        u_fprintf(lx->OUT, "\">\r\n");
      
        u_fprintf(lx->OUT, "<TEXTAREA NAME=NP_LOC ROWS=1 COLS=20>");
        writeEscaped(lx, tempChars);
        u_fprintf(lx->OUT, "</TEXTAREA><INPUT TYPE=SUBMIT VALUE=\"%U\"></FORM>", FSWF("EXPLORE_change", "Change"));

        if(localValueErr)
            u_fprintf(lx->OUT, "<P>%U", localValueErr);
    }
    /*  ============== End of the default/localized split =============== */

    u_fprintf(lx->OUT, "</TD></TR>");


    /* ============== Spellout ================== */
    u_fprintf(lx->OUT, "<tr><td colspan=3>\r\n");
    u_fprintf(lx->OUT, "<FORM METHOD=GET ACTION=\"#EXPLORE_NumberPatterns\">\r\n");
    u_fprintf(lx->OUT, "<INPUT NAME=_ TYPE=HIDDEN VALUE=%s>\r\n", locale);
    u_fprintf(lx->OUT, "<INPUT TYPE=HIDDEN NAME=EXPLORE_NumberPatterns VALUE=\"");
    writeEscaped(lx, pattern);
    u_fprintf(lx->OUT, "\">\r\n");

    u_fprintf(lx->OUT, "<B>%U</B> ", FSWF("Spellout", "Spellout"));


    if(strstr(b, "NP_SPL"))
    {  
        u_fprintf(lx->OUT, "<BR>%U<BR>\r\n", valueString);
    }

    unum_formatDouble(nf_spellout, value, tempChars, 1024,0, &status);


    u_fprintf(lx->OUT, "<TEXTAREA NAME=NP_SPL ROWS=1 COLS=60>");
    lx->backslashCtx.html = FALSE;
    if(U_FAILURE(status))
    {
        u_fprintf(lx->OUT, "%U<P>", FSWF("formatExample_errorFormat_number", "Couldn't format the number."));
        explainStatus(lx, status, "EXPLORE_NumberPattern");
    }
    else
    {
        u_fprintf(lx->OUT, "%U", tempChars); 
    }
    lx->backslashCtx.html = TRUE;
  
    status = U_ZERO_ERROR;
  
    u_fprintf(lx->OUT, "</TEXTAREA><INPUT TYPE=SUBMIT VALUE=\"%U\"></FORM>", FSWF("EXPLORE_change", "Change"));
  
    /* == end spellout == */

    u_fprintf(lx->OUT, "</td></tr>\r\n");

    u_fprintf(lx->OUT, "</TABLE>");

    if(nf)
        unum_close(nf);

    if(nf_default)
        unum_close(nf_default);

    if(nf_spellout)
        unum_close(nf_spellout);
  
    showExploreCloseButton(lx, locale, "NumberPatterns");
    u_fprintf(lx->OUT, "</TD><TD ALIGN=LEFT VALIGN=TOP>");
    printHelpTag(lx, "EXPLORE_NumberPatterns", NULL);
    u_fprintf(lx->OUT, "</TD>\r\n");

  
    showKeyAndEndItem(lx, "EXPLORE_NumberPatterns", locale);
}

