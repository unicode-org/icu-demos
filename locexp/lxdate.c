/**********************************************************************
*   Copyright (C) 1999-2002, International Business Machines
*   Corporation and others.  All Rights Reserved.
***********************************************************************/

#include "locexp.h"

/* routines having to do with the date sample */

/******************************************************************************
 *  Explorer for dates
 */
    
void showExploreDateTimePatterns( LXContext *lx, UResourceBundle *myRB, const char *locale, const char *b)
{
    UChar pattern[1024];
    UChar tempChars[1024];
    UChar defChars[1024];
    UChar valueString[1024];
    UDateFormat  *df = NULL, *df_default = NULL;
    UErrorCode   status = U_ZERO_ERROR, defStatus = U_ZERO_ERROR, locStatus = U_ZERO_ERROR;
    UDate now;  /* example date */
    UNumberFormat *nf = NULL; /* for formatting the number */
    char *tmp;
    int32_t parsePos = 0;

    nf = unum_open(0, FSWF("EXPLORE_DateTimePatterns_dateAsNumber", "#"), -1, NULL, NULL, &status);
    status = U_ZERO_ERROR; /* ? */
  
    df_default = udat_open(UDAT_SHORT, UDAT_SHORT, NULL, NULL, -1, NULL, 0, &status);
    status = U_ZERO_ERROR; /* ? */

    now = ucal_getNow();
  
    showKeyAndStartItem(lx, "EXPLORE_DateTimePatterns",
                        FSWF("EXPLORE_DateTimePatterns", "Explore &gt; Date/Time"),
                        locale, FALSE, U_ZERO_ERROR);

    u_fprintf(lx->OUT, "%U<P>", FSWF("formatExample_DateTimePatterns_What","This example demonstrates the formatting of date and time patterns in this locale."));
  
    /* fetch the current pattern */
    exploreFetchNextPattern(lx,pattern, strstr(b,"EXPLORE_DateTimePatterns"));

    df = udat_open(0,0,locale, NULL, -1, NULL, 0, &status);
    udat_applyPattern(df, TRUE, pattern, -1);

    status = U_ZERO_ERROR;
  
    if ((tmp = strstr(b,"NP_DBL"))) /* Double: UDate format input ============= */
    {
        /* Localized # */
        tmp += 7;

        unescapeAndDecodeQueryField(valueString, 1000, tmp);
        u_replaceChar(valueString, 0x0020, 0x00A0);

        status = U_ZERO_ERROR;
        now = unum_parseDouble(nf, valueString, -1, &parsePos, &status);
    }
    else if(tmp = strstr(b, "NP_DEF")) /* Default: 'display' format input ============== */
    {

        /* Localized # */
        tmp += 7;

        unescapeAndDecodeQueryField(valueString, 1000, tmp);
        /*      u_replaceChar(valueString, 0x0020, 0x00A0); */ /* NOt for the default pattern */

        status = U_ZERO_ERROR;
      
        now = udat_parse(df_default, valueString, -1, &parsePos, &status);
    }
    else if(tmp = strstr(b, "NP_LOC")) /* Localized: pattern format input ============== */
    {


        /* Localized # */
        tmp += 7;

        unescapeAndDecodeQueryField(valueString, 1000, tmp);
        u_replaceChar(valueString, 0x0020, 0x00A0); 

        status = U_ZERO_ERROR;
        now = udat_parse(df, valueString, -1, &parsePos, &status);
    }

    /* Common handler for input errs */

    if(U_FAILURE(status) || (now == 0))
    {
        status = U_ZERO_ERROR;
        u_fprintf(lx->OUT, "%U %d<P>\r\n", FSWF("formatExample_errorParse", "Could not parse this, replaced with a default value. Formatted This many chars:"), parsePos);
        now = ucal_getNow();
    }
    status = U_ZERO_ERROR;
    /* ======================== End loading input date ================================= */

    if(U_FAILURE(status))
    {
        u_fprintf(lx->OUT, "%U: [%d] <P>", FSWF("formatExample_errorOpen", "Couldn't open the formatter"), (int) status);
        explainStatus(lx, status, "EXPLORE_DateTimePatterns");
        exploreShowPatternForm(lx,pattern, locale, "DateTimePatterns", strstr(b,"EXPLORE_DateTimePatterns"), now, nf);
    }
    else
    {
      
        /* now display the form */
        exploreShowPatternForm(lx,pattern, locale, "DateTimePatterns", strstr(b,"EXPLORE_DateTimePatterns"), now, nf);
      
    }
  
    status = U_ZERO_ERROR;
    udat_format(df,now,tempChars, 1024, 0, &locStatus);
    udat_format(df_default,now,defChars, 1024, 0, &defStatus);
  
    if(U_FAILURE(status))
        u_fprintf(lx->OUT, "%U<P>", FSWF("formatExample_DateTimePatterns_errorFormat", "Couldn't format the date."));
  
    explainStatus(lx, status,"EXPLORE_DateTimePatterns");



    /* =======================  Now, collect the new date values ====================== */

    /* Now, display the results in <default> and in their locale */
    u_fprintf(lx->OUT, "<TABLE BORDER=1><TR><TD>\r\n");


    /* ============ 'default' side of the table  */

    if(U_FAILURE(defStatus))
    {
        u_fprintf(lx->OUT, "%U<P>", FSWF("formatExample_errorFormatDefault", "Unable to format number using default version of the pattern"));
        explainStatus(lx, status, "EXPLORE_DateTimePatterns");
    }
    else
    {
      
        u_fprintf(lx->OUT, "<B><I>%U</I></B><BR>\r\n", defaultLanguageDisplayName(lx));
#if 0
        /* Just the pattern */
        u_fprintf(lx->OUT, "%U", defChars);
#else
        u_fprintf(lx->OUT, "<FORM METHOD=GET ACTION=\"#EXPLORE_DateTimePatterns\">\r\n");
        u_fprintf(lx->OUT, "<INPUT NAME=_ TYPE=HIDDEN VALUE=%s>\r\n", locale);
        u_fprintf(lx->OUT, "<INPUT TYPE=HIDDEN NAME=EXPLORE_DateTimePatterns VALUE=\"");
        writeEscaped(lx, pattern);
        u_fprintf(lx->OUT, "\">\r\n");

        u_fprintf(lx->OUT, "<TEXTAREA NAME=NP_DEF ROWS=1 COLS=30>");

        lx->backslashCtx.html = FALSE;
        u_fprintf(lx->OUT, "%U", defChars); 
        lx->backslashCtx.html = TRUE;
      
        status = U_ZERO_ERROR;
      
        u_fprintf(lx->OUT, "</TEXTAREA><BR><INPUT TYPE=SUBMIT VALUE=\"%U\"></FORM>", FSWF("EXPLORE_change", "Change"));
#endif
    }
  
    u_fprintf(lx->OUT, "</TD><TD WIDTH=1 BGCOLOR=\"#EEEEEE\"><IMG src=\"../_/c.gif\" ALT=\"---\" WIDTH=0 HEIGHT=0></TD><TD>");

    /* ============ 'localized' side ================================= */

    if(U_FAILURE(locStatus))
    {
        u_fprintf(lx->OUT, "%U<P>", FSWF("formatExample_DateTimePatterns_errorFormat", "Couldn't format the date."));
        explainStatus(lx, status, "EXPLORE_DateTimePatterns");
    }
    else
    {
        /*  === local side */
        u_fprintf(lx->OUT, "\r\n\r\n<!--  LOCALIZED SIDE -->\r\n<B>%U</B><BR>\r\n",lx->curLocale?lx->curLocale->ustr:FSWF("NoLocale","MISSING LOCALE NAME") );
#if 0
        u_fprintf(lx->OUT, "%U", tempChars);
#else
        u_fprintf(lx->OUT, "<FORM METHOD=GET ACTION=\"#EXPLORE_DateTimePatterns\">\r\n");
        u_fprintf(lx->OUT, "<INPUT NAME=_ TYPE=HIDDEN VALUE=%s>\r\n", locale);
        u_fprintf(lx->OUT, "<INPUT TYPE=HIDDEN NAME=EXPLORE_DateTimePatterns VALUE=\"");
        writeEscaped(lx, pattern);
        u_fprintf(lx->OUT, "\">\r\n");
      
        u_fprintf(lx->OUT, "<TEXTAREA NAME=NP_LOC ROWS=1 COLS=30>");
        writeEscaped(lx, tempChars);
        u_fprintf(lx->OUT, "</TEXTAREA><BR><INPUT TYPE=SUBMIT VALUE=\"%U\"></FORM>", FSWF("EXPLORE_change", "Change"));
#endif

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

    showExploreCloseButton(lx, locale, "DateTimePatterns");

    u_fprintf(lx->OUT, "</TD><TD ALIGN=LEFT VALIGN=TOP>");
    printHelpTag(lx, "EXPLORE_DateTimePatterns", NULL);
    u_fprintf(lx->OUT, "</TD>\r\n");

    showKeyAndEndItem(lx, "EXPLORE_DateTimePatterns", locale);
  

    /* ========= Show LPC's for reference ================= */

    /* ..... */
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
	
	showStringWithDescription(lx, myRB, locale, "SHOWlocalPatternChars", charDescs, "localPatternChars", FALSE);
    }
}
