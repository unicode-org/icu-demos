/**********************************************************************
*   Copyright (C) 1999-2003, International Business Machines
*   Corporation and others.  All Rights Reserved.
***********************************************************************/

#include "locexp.h"

/* routines having to do with the date sample */

/******************************************************************************
 *  Explorer for dates
 */
    
void showExploreDateTimePatterns( LXContext *lx, UResourceBundle *myRB, const char *locale)
{
    UChar pattern[1024];
    UChar tempChars[1024];
    UChar defChars[1024];
    UChar valueString[1024];
    UDateFormat  *df = NULL, *df_default = NULL;
    UErrorCode   status = U_ZERO_ERROR, defStatus = U_ZERO_ERROR, locStatus = U_ZERO_ERROR;
    UDate now;  /* example date */
    UNumberFormat *nf = NULL; /* for formatting the number */
    const char *tmp;
    int32_t parsePos = 0;

    valueString[0]=0;
    nf = unum_open(0, FSWF("EXPLORE_DateTimePatterns_dateAsNumber", "#"), -1, NULL, NULL, &status);
    status = U_ZERO_ERROR; /* ? */
  
    df_default = udat_open(UDAT_FULL, UDAT_FULL, NULL, NULL, -1, NULL, 0, &status);
    status = U_ZERO_ERROR; /* ? */

    now = ucal_getNow();
  
    showKeyAndStartItem(lx, "EXPLORE_DateTimePatterns",
                        FSWF("EXPLORE_DateTimePatterns", "Explore &gt; Date/Time"),
                        locale, FALSE, U_ZERO_ERROR);

    u_fprintf(lx->OUT, "%S<P>", FSWF("formatExample_DateTimePatterns_What","This example demonstrates the formatting of date and time patterns in this locale."));
  
    /* fetch the current pattern */
    exploreFetchNextPattern(lx,pattern, queryField(lx,"str"));

    df = udat_open(0,0,locale, NULL, -1, NULL, 0, &status);
    udat_applyPattern(df, TRUE, pattern, -1);

    status = U_ZERO_ERROR;
  
    if ((tmp = queryField(lx, "NP_DBL"))) /* Double: UDate format input ============= */
    {
        /* Localized # */

        unescapeAndDecodeQueryField(valueString, 1000, tmp);
        u_replaceChar(valueString, 0x0020, 0x00A0);

        status = U_ZERO_ERROR;
        now = unum_parseDouble(nf, valueString, -1, &parsePos, &status);
    }
    else if((tmp = queryField(lx, "NP_DEF"))) /* Default: 'display' format input ============== */
    {

        /* Localized # */

        unescapeAndDecodeQueryField(valueString, 1000, tmp);
        /*      u_replaceChar(valueString, 0x0020, 0x00A0); */ /* NOt for the default pattern */

        status = U_ZERO_ERROR;
      
        now = udat_parse(df_default, valueString, -1, &parsePos, &status);
    }
    else if((tmp = queryField(lx, "NP_LOC"))) /* Localized: pattern format input ============== */
    {
      /* Localized # */
      
      unescapeAndDecodeQueryField_enc(valueString, 1000, tmp, lx->convName);
      u_replaceChar(valueString, 0x0020, 0x00A0);
      
      status = U_ZERO_ERROR;
      now = udat_parse(df, valueString, -1, &parsePos, &status);
    }

    /* Common handler for input errs */

    if(U_FAILURE(status) || (now == 0))
    {
      int32_t tmplen;
      u_fprintf(lx->OUT, "%S %d<P>\r\n", FSWF("formatExample_errorParse", "Could not parse this, replaced with a default value. Formatted This many chars:"), parsePos);
#if defined(LX_DEBUG)
      u_fprintf(lx->OUT, "<tt>'tmp' was '%s'</tt><br/>\n", tmp);
#endif
      tmplen = u_strlen(valueString);
      if((parsePos >= 0) && (parsePos <= tmplen)) {
        u_fprintf(lx->OUT, "<table border=1><tr><td>");
        u_file_write(valueString, parsePos, lx->OUT);
        u_fprintf(lx->OUT, "<b><font color=red>|</font></b>");
        u_file_write(valueString+parsePos, tmplen-parsePos, lx->OUT);
        u_fprintf(lx->OUT, "</td></tr></table>\n");
      }
      u_fprintf(lx->OUT, "<br>");
      explainStatus(lx,status,"EXPLORE_DateTimePatterns");
      status = U_ZERO_ERROR;
      now = ucal_getNow();
    }
    status = U_ZERO_ERROR;
    /* ======================== End loading input date ================================= */

    if(U_FAILURE(status))
    {
        u_fprintf(lx->OUT, "%S: [%d] <P>", FSWF("formatExample_errorOpen", "Couldn't open the formatter"), (int) status);
        explainStatus(lx, status, "EXPLORE_DateTimePatterns");
        exploreShowPatternForm(lx,pattern, locale, "DateTimePatterns", queryField(lx,"str"), now, nf);
    }
    else
    {
      
        /* now display the form */
        exploreShowPatternForm(lx,pattern, locale, "DateTimePatterns", queryField(lx,"str"), now, nf);
      
    }
  
    status = U_ZERO_ERROR;
    udat_format(df,now,tempChars, 1024, 0, &locStatus);
    udat_format(df_default,now,defChars, 1024, 0, &defStatus);
  
    if(U_FAILURE(status))
        u_fprintf(lx->OUT, "%S<P>", FSWF("formatExample_DateTimePatterns_errorFormat", "Couldn't format the date."));
  
    explainStatus(lx, status,"EXPLORE_DateTimePatterns");



    /* =======================  Now, collect the new date values ====================== */

    /* Now, display the results in <default> and in their locale */
    u_fprintf(lx->OUT, "<TABLE BORDER=1><TR><TD>\r\n");


    /* ============ 'default' side of the table  */

    if(U_FAILURE(defStatus))
    {
        u_fprintf(lx->OUT, "%S<P>", FSWF("formatExample_errorFormatDefault", "Unable to format number using default version of the pattern"));
        explainStatus(lx, status, "EXPLORE_DateTimePatterns");
    }
    else
    {
      
        u_fprintf(lx->OUT, "<B><I>%S</I></B><BR>\r\n", defaultLanguageDisplayName(lx));
        u_fprintf(lx->OUT, "<FORM METHOD=POST ACTION=\"%s#EXPLORE_DateTimePatterns\">\r\n", getLXBaseURL(lx, kNO_URL));
        u_fprintf(lx->OUT, "<INPUT TYPE=HIDDEN NAME=str VALUE=\"");
        writeEscaped(lx, pattern);
        u_fprintf(lx->OUT, "\">\r\n");

        u_fprintf(lx->OUT, "<TEXTAREA NAME=NP_DEF ROWS=1 COLS=50>");

        lx->backslashCtx.html = FALSE;
        u_fprintf(lx->OUT, "%S", defChars); 
        lx->backslashCtx.html = TRUE;
      
        status = U_ZERO_ERROR;
      
        u_fprintf(lx->OUT, "</TEXTAREA><BR><INPUT TYPE=SUBMIT VALUE=\"%S\"></FORM>", FSWF("EXPLORE_change", "Change"));
    }
  
    u_fprintf(lx->OUT, "</TD><TD WIDTH=1 BGCOLOR=\"#EEEEEE\"><IMG src=\"" LDATA_PATH "c.gif\" ALT=\"---\" WIDTH=0 HEIGHT=0></TD><TD>");

    /* ============ 'localized' side ================================= */

    if(U_FAILURE(locStatus))
    {
        u_fprintf(lx->OUT, "%S<P>", FSWF("formatExample_DateTimePatterns_errorFormat", "Couldn't format the date."));
        explainStatus(lx, status, "EXPLORE_DateTimePatterns");
    }
    else
    {
        /*  === local side */
        u_fprintf(lx->OUT, "\r\n\r\n<!--  LOCALIZED SIDE -->\r\n<B>%S</B><BR>\r\n",lx->curLocale?lx->curLocale->ustr:FSWF("NoLocale","MISSING LOCALE NAME") );
        u_fprintf(lx->OUT, "<FORM METHOD=POST ACTION=\"%s#EXPLORE_DateTimePatterns\">\r\n", getLXBaseURL(lx,kNO_URL));
        u_fprintf(lx->OUT, "<INPUT TYPE=HIDDEN NAME=str VALUE=\"");
        writeEscaped(lx, pattern);
        u_fprintf(lx->OUT, "\">\r\n");
      
        u_fprintf(lx->OUT, "<TEXTAREA NAME=NP_LOC ROWS=1 COLS=50>");
        writeEscaped(lx, tempChars);
        u_fprintf(lx->OUT, "</TEXTAREA><BR><INPUT TYPE=SUBMIT VALUE=\"%S\"></FORM>", FSWF("EXPLORE_change", "Change"));
    }
    /*  ============== End of the default/localized split =============== */

    u_fprintf(lx->OUT, "</TD></TR>");
    u_fprintf(lx->OUT, "</TABLE>");
  

    /* =============== All done ========================== */
  
    if(df)
        udat_close(df);

    if(df_default)
        udat_close(df_default);

    if(nf)
        unum_close(nf);

    u_fprintf(lx->OUT, "<P><P>");
    
    {
      char f[300];
      sprintf(f, "%f", now);
      u_fprintf(lx->OUT, "<A HREF=\"%s&NP_DBL=%s\">Calendar Demo...</A><br>\r\n",
                getLXBaseURL(lx,kNO_URL|kNO_SECT), f);
    }
      
    showExploreCloseButton(lx, locale, "DateTimePatterns");

    u_fprintf(lx->OUT, "</TD><TD ALIGN=LEFT VALIGN=TOP>");
    printHelpTag(lx, "EXPLORE_DateTimePatterns", NULL);
    u_fprintf(lx->OUT, "</TD>\r\n");

    showKeyAndEndItem(lx, "EXPLORE_DateTimePatterns", locale);
  

    /* ========= Show LPC's for reference ================= */

    /* ..... */
    showLPC(lx, myRB, locale);
}
