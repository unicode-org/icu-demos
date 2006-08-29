/**********************************************************************
*   Copyright (C) 1999-2006, International Business Machines
*   Corporation and others.  All Rights Reserved.
***********************************************************************/

#include "locexp.h"

/* routines having to do with the date sample */

/******************************************************************************
 *  Explorer for dates
 */
#define PATTERN_LEN 1024
    
void showExploreDateTimePatterns( LXContext *lx, UResourceBundle *myRB, const char *locale)
{
    UChar pattern[PATTERN_LEN];
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

    u_fprintf(lx->OUT, "%S<p>", FSWF("formatExample_DateTimePatterns_What","This example demonstrates the formatting of date and time patterns in this locale."));
  
    /* fetch the current pattern */
    exploreFetchNextPattern(lx,pattern, queryField(lx,"str"));

    df = udat_open(UDAT_LONG,UDAT_LONG,locale, NULL, -1, NULL, 0, &status);
    if(pattern[0]) {
        udat_applyPattern(df, FALSE, pattern, -1);
    } else {
        udat_toPattern(    df,
                        FALSE,
                        pattern,
                        PATTERN_LEN,
                        &status);
    }

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
      u_fprintf(lx->OUT, "%S %d<p>\r\n", FSWF("formatExample_errorParse", "Could not parse this, replaced with a default value. Formatted This many chars:"), parsePos);
#if defined(LX_DEBUG)
      u_fprintf(lx->OUT, "<tt>'tmp' was '%s'</tt><br/>\n", tmp);
#endif
      tmplen = u_strlen(valueString);
      if((parsePos >= 0) && (parsePos <= tmplen)) {
        u_fprintf(lx->OUT, "<table border=\"1\"><tr><td>");
        u_file_write(valueString, parsePos, lx->OUT);
        u_fprintf(lx->OUT, "<b><font color=red>|</font></b>");
        u_file_write(valueString+parsePos, tmplen-parsePos, lx->OUT);
        u_fprintf(lx->OUT, "</td></tr></table>\n");
      }
      u_fprintf(lx->OUT, "<br />");
      explainStatus(lx,status,"EXPLORE_DateTimePatterns");
      status = U_ZERO_ERROR;
      now = ucal_getNow();
    }
    status = U_ZERO_ERROR;
    /* ======================== End loading input date ================================= */

    if(U_FAILURE(status))
    {
        u_fprintf(lx->OUT, "%S: [%d] <p>", FSWF("formatExample_errorOpen", "Couldn't open the formatter"), (int) status);
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
        u_fprintf(lx->OUT, "%S<p>", FSWF("formatExample_DateTimePatterns_errorFormat", "Couldn't format the date."));
  
    explainStatus(lx, status,"EXPLORE_DateTimePatterns");



    /* =======================  Now, collect the new date values ====================== */

    /* Now, display the results in <default> and in their locale */
    u_fprintf(lx->OUT, "<table border=\"1\"><tr><td>\r\n");


    /* ============ 'default' side of the table  */

    if(U_FAILURE(defStatus))
    {
        u_fprintf(lx->OUT, "%S<p>", FSWF("formatExample_errorFormatDefault", "Unable to format number using default version of the pattern"));
        explainStatus(lx, status, "EXPLORE_DateTimePatterns");
    }
    else
    {
      
        u_fprintf(lx->OUT, "<b><i>%S</i></b><br />\r\n", defaultLanguageDisplayName(lx));
        u_fprintf(lx->OUT, "<form method=\"post\" action=\"%s#EXPLORE_DateTimePatterns\">\r\n", getLXBaseURL(lx, kNO_URL));
        u_fprintf(lx->OUT, "<input type=\"hidden\" name=\"str\" value=\"");
        writeEscaped(lx, pattern);
        u_fprintf(lx->OUT, "\" />\r\n");

        u_fprintf(lx->OUT, "<textarea name=\"NP_DEF\" rows=\"1\" cols=\"50\">");

        lx->backslashCtx.html = FALSE;
        u_fprintf(lx->OUT, "%S", defChars); 
        lx->backslashCtx.html = TRUE;
      
        status = U_ZERO_ERROR;
      
        u_fprintf(lx->OUT, "</textarea><br /><input type=\"submit\" value=\"%S\" /></form>", FSWF("EXPLORE_change", "Change"));
    }
  
    u_fprintf(lx->OUT, "</td><td width=\"1\" bgcolor=\"#EEEEEE\"><img src=\"" LDATA_PATH "c.gif\" width=\"0\" height=\"0\" alt=\"-\" /></td><td>");

    /* ============ 'localized' side ================================= */

    if(U_FAILURE(locStatus))
    {
        u_fprintf(lx->OUT, "%S<p>", FSWF("formatExample_DateTimePatterns_errorFormat", "Couldn't format the date."));
        explainStatus(lx, status, "EXPLORE_DateTimePatterns");
    }
    else
    {
        /*  === local side */
        u_fprintf(lx->OUT, "\r\n\r\n<!--  LOCALIZED SIDE -->\r\n<b>%S</b><br />\r\n",lx->curLocale?lx->curLocale->ustr:FSWF("NoLocale","MISSING LOCALE NAME") );
        u_fprintf(lx->OUT, "<form method=\"post\" action=\"%s#EXPLORE_DateTimePatterns\">\r\n", getLXBaseURL(lx,kNO_URL));
        u_fprintf(lx->OUT, "<input type=\"hidden\" name=\"str\" value=\"");
        writeEscaped(lx, pattern);
        u_fprintf(lx->OUT, "\" />\r\n");
      
        u_fprintf(lx->OUT, "<textarea name=\"NP_LOC\" rows=\"1\" cols=\"50\">");
        writeEscaped(lx, tempChars);
        u_fprintf(lx->OUT, "</textarea><br /><input type=\"submit\" value=\"%S\" /></form>", FSWF("EXPLORE_change", "Change"));
    }
    /*  ============== End of the default/localized split =============== */

    u_fprintf(lx->OUT, "</td></tr>");
    u_fprintf(lx->OUT, "</table>");
  

    /* =============== All done ========================== */
  
    if(df)
        udat_close(df);

    if(df_default)
        udat_close(df_default);

    if(nf)
        unum_close(nf);

    u_fprintf(lx->OUT, "<br /><br />");
    
    {
      char f[300];
      sprintf(f, "%f", now);
      u_fprintf(lx->OUT, "<a href=\"%s&amp;x=cal&amp;NP_DBL=%s\">Calendar Demo...", 
                getLXBaseURL(lx,kNO_URL|kNO_SECT), f);
        showExploreButtonPicture( lx );
      u_fprintf(lx->OUT, "</a><br />\r\n");
    }
      
    showExploreCloseButton(lx, locale, "DateTimePatterns");

    u_fprintf(lx->OUT, "</td><td align=\"left\" valign=\"top\">");
    printHelpTag(lx, "EXPLORE_DateTimePatterns", NULL);

    showKeyAndEndItem(lx, "EXPLORE_DateTimePatterns", locale);
  

    /* ========= Show LPC's for reference ================= */

    /* ..... */
    { 
        UResourceBundle *rootRB;
        rootRB = ures_open(NULL, "root", &status);
        showLPC(lx, rootRB, "root", FALSE);
    }
}
