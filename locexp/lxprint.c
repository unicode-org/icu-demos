/**********************************************************************
*   Copyright (C) 1999-2002, International Business Machines
*   Corporation and others.  All Rights Reserved.
***********************************************************************/

/* routines having to do with generic output needs */


#include "locexp.h"


/* Explain what the status code means --------------------------------------------------------- */

void explainStatus( LXContext *lx, UErrorCode status, const char *tag )
{
    if(tag == 0)
        tag = "_top_";

    if(status != U_ZERO_ERROR)
        u_fprintf(lx->OUT, " <B><FONT SIZE=-1>");

    switch(status)
    {
    case U_MISSING_RESOURCE_ERROR:
        printHelpTag(lx, "U_MISSING_RESOURCE_ERROR",
                     FSWF("U_MISSING_RESOURCE_ERROR", "(missing resource)"));
        break;

    case U_USING_FALLBACK_WARNING:
        if(lx->parLocale && lx->parLocale->str)
	{
            u_fprintf(lx->OUT, "<A HREF=\"?_=%s#%s\">", lx->parLocale->str, tag);
            u_fprintf_u(lx->OUT, FSWF("inherited_from", "(inherited from %U)"), lx->parLocale->ustr); 
	}
        else
	{
            u_fprintf(lx->OUT, "<A HREF=\"?_=root#%s\">", tag);
            u_fprintf_u(lx->OUT, FSWF("inherited", "(inherited)"));
	}

        u_fprintf(lx->OUT, "</A>");
        break;

    case U_USING_DEFAULT_WARNING:
	u_fprintf(lx->OUT, "<A HREF=\"?_=root#%s\">", tag);
        u_fprintf_u(lx->OUT, FSWF("inherited_from", "(inherited from %U)"), lx->locales->ustr); 
        u_fprintf(lx->OUT, "</A>");
        break;

    default:
        if(status != U_ZERO_ERROR)
	{
            u_fprintf(lx->OUT, "(%U %d - %s)", FSWF("UNKNOWN_ERROR", "unknown error"), (int) status,
                      u_errorName(status));
            fprintf(stderr,"LRB: caught Unknown err- %d %s\n", status, u_errorName(status)); 
	}
    }

    if(status != U_ZERO_ERROR)
        u_fprintf(lx->OUT, "</FONT></B>");
}

/* Convenience function.  print <A HREF="..."> for a link to the correct Help page.  if str=null it defaults to Help*/

void printHelpTag(LXContext *lx, const char *helpTag, const UChar *str)
{
    if(str == NULL)
    {
        /* str = FSWF("help", "Help"); */
      
        printHelpImg(lx, helpTag, FSWF("help", "Help"), 
                     FSWF("helpgif", "help.gif"),
                     FSWF("helpgif_opt", "BORDER=0"));
        return;

    }

    u_fprintf(lx->OUT, "<A TARGET=\"icu_lx_help\" HREF=\"../_/help.html#%s\">%U</A>",
              helpTag,str);
}

void printHelpImg(LXContext *lx, const char *helpTag, const UChar *alt, const UChar *src, const UChar *options)
{
    u_fprintf(lx->OUT, "<a href=\"../_/help.html#%s\" target=\"icu_lx_help\"><img %U src=\"../_/%U\" alt=\"%U\"></a>",
              helpTag, 
              options, src, alt);
}

void showExploreCloseButton(LXContext *lx, const char *locale, const char *frag)
{
    /* What do we do here? */
    u_fprintf(lx->OUT, "<!-- no CLOSE BUTTON here. -->\r\n");
}

void showExploreButton( LXContext *lx, UResourceBundle *rb, const char *locale, const UChar *sampleString, const char *key)
{
    UChar nullString[] = { 0x0000 };
  
    if(!sampleString)
        sampleString = nullString;

    u_fprintf(lx->OUT, "\r\n<form target=\"_new\" name=EXPLORE_%s action=\"#EXPLORE_%s\">\r\n"
              "<input type=hidden name=_ value=\"%s\">\r\n"
              "<input type=hidden name=\"EXPLORE_%s\" value=\"",
              key, key,locale,key);
    writeEscaped(lx, sampleString);
    u_fprintf(lx->OUT, "\">\r\n");
  
    u_fprintf(lx->OUT, "<input type=image valign=top width=48 height=20 border=0 src=\"../_/explore.gif\" align=right value=\"%U\"></form>",
              FSWF("exploreTitle", "Explore"));
}

void showExploreButtonSort( LXContext *lx, UResourceBundle *rb, const char *locale, const char *sampleString, const char *key, UBool rightAlign)
{
    if(!sampleString)
        sampleString = "";

    u_fprintf(lx->OUT, "\r\n<FORM TARGET=\"_new\" NAME=EXPLORE_%s ACTION=\"#EXPLORE_%s\">"
              "<INPUT TYPE=HIDDEN NAME=_ VALUE=\"%s\">"
              "<INPUT TYPE=HIDDEN NAME=\"EXPLORE_%s\" VALUE=\"",
              key, key,locale,key);
    u_fprintf(lx->OUT, "%s", sampleString);
    u_fprintf(lx->OUT, "\">");
  
    u_fprintf(lx->OUT, "<INPUT TYPE=IMAGE VALIGN=TOP WIDTH=48 HEIGHT=20 BORDER=0 SRC=\"../_/explore.gif\"  %s ALT=\"%U\"   ",
              rightAlign?"ALIGN=RIGHT ":"",
              FSWF("exploreTitle", "Explore") );
    u_fprintf(lx->OUT, " VALUE=\"%U\"></FORM>",
              FSWF("exploreTitle", "Explore"));
}

void showExploreLink( LXContext *lx, UResourceBundle *rb, const char *locale, const UChar *sampleString, const char *key)
{
    UChar nullString[] = { 0x0000 };
  
    if(!sampleString)
        sampleString = nullString;

    u_fprintf(lx->OUT, "<A TARGET=\"lx_explore_%s_%s\" HREF=\"?_=%s&EXPLORE_%s=",
              locale,key,locale,key);
    writeEscaped(lx, sampleString);
    u_fprintf(lx->OUT, "&\">");
}

/* Show the 'short' HTML for a line item. It is short because it has not closed out the table yet - the caller can put in their own push button before closing the table cell/column. */
void showKeyAndStartItemShort(LXContext *lx, const char *key, const UChar *keyName, const char *locale, UBool cumulative, UErrorCode showStatus)
{
    u_fprintf(lx->OUT, "<P><TABLE BORDER=0 CELLSPACING=0 WIDTH=100%%>");
    u_fprintf(lx->OUT, "<TR><TD HEIGHT=5 BGCOLOR=\"#AFA8AF\" COLSPAN=2><IMG SRC=\"../_/c.gif\" ALT=\"---\" WIDTH=0 HEIGHT=0></TD></TR>\r\n");
    u_fprintf(lx->OUT, "<TR><TD COLSPAN=1 WIDTH=0 VALIGN=TOP BGCOLOR=" kXKeyBGColor "><A NAME=%s>", key);
    u_fprintf(lx->OUT,"</A>", keyName);

    if(keyName == NULL)
	keyName = FSWF( key, key );

    printHelpTag(lx, key, keyName);


    if(cumulative == TRUE )
    {
        u_fprintf(lx->OUT, " (%U)", FSWF("cumulative_notshown", "cumulative data from parent not shown"));
    }

    u_fprintf(lx->OUT," </TD><TD BGCOLOR=" kXKeyBGColor "   VALIGN=TOP ALIGN=RIGHT>");
    explainStatus(lx, showStatus, key);
}

void showKeyAndStartItem(LXContext *lx, const char *key, const UChar *keyName, const char *locale, UBool cumulative, UErrorCode showStatus)
{
    showKeyAndStartItemShort(lx, key,keyName,locale, cumulative, showStatus);
    u_fprintf(lx->OUT,"&nbsp;</TD></TR><TR><TD COLSPAN=2>\r\n");
}

void showKeyAndEndItem(LXContext *lx, const char *key, const char *locale)
{
    u_fprintf(lx->OUT, "</TR></TABLE>\r\n");
    u_fprintf(lx->OUT, "<!-- %s:%d -->\r\n", __FILE__, __LINE__);
}

/** for explorers, show a pattern as a textarea
 */

void exploreShowPatternForm(LXContext *lx, UChar *dstPattern, const char *locale, const char *key, const char* qs, double value, UNumberFormat *valueFmt)
{
    UErrorCode status = U_ZERO_ERROR;
    UChar tmp[1024];

    /**********  Now we've got the pattern from the user. Now for the form.. ***/
    u_fprintf(lx->OUT, "<FORM METHOD=GET ACTION=\"#EXPLORE_%s\">\r\n",
              key);
    u_fprintf(lx->OUT, "<INPUT NAME=_ TYPE=HIDDEN VALUE=%s>", locale);

    if(valueFmt)
    {
      
        u_fprintf(lx->OUT, "<INPUT NAME=NP_DBL TYPE=HIDDEN VALUE=\"");
        tmp[0] = 0;
        unum_formatDouble(valueFmt, value, tmp, 1000, 0, &status);
        u_fprintf(lx->OUT, "%U\">", tmp);
    }
    u_fprintf(lx->OUT, "<TEXTAREA ROWS=2 COLS=60 NAME=\"EXPLORE_%s\">",
              key);


    lx->backslashCtx.html = FALSE;

    u_fprintf(lx->OUT, "%U", dstPattern); 

    lx->backslashCtx.html = TRUE;
  
    u_fprintf(lx->OUT, "</TEXTAREA><P>\r\n<INPUT TYPE=SUBMIT VALUE=Format><INPUT TYPE=RESET VALUE=Reset></FORM>\r\n");

}



/* print that little box in the TR corner ----------------------------------------------------- */

void printStatusTable(LXContext *lx)
{
    UChar myChars[1024];
    UErrorCode status = U_ZERO_ERROR;
    UChar *dateStr;
    const char *qs = getenv("QUERY_STRING");

    if (qs == NULL)
    {
        qs = "";
    }
    
    u_fprintf(lx->OUT, "<p><table border=0 cellspacing=0 width=100%%>");
    u_fprintf(lx->OUT, "<tr><td height=5 bgcolor=\"#0F080F\" colspan=3><img src=\"../_/c.gif\" alt=\"---\" width=0 height=0></td></tr>\r\n");
    u_fprintf(lx->OUT, "  <tr>\r\n   <td colspan=3 width=0 valign=top bgcolor=" kXKeyBGColor "><a name=%s><b>", "YourSettings");
    
    /* PrintHelpTag */
    u_fprintf_u(lx->OUT, FSWF("statusTableHeader", "Your settings:"));

    if(!lx->inDemo)
    {
        u_fprintf(lx->OUT, " %U",   FSWF("statusTableHeaderChange", "(click to change)"));
    }

    /* /PrintHelpTag */
    u_fprintf(lx->OUT, "</b></a></td>\r\n"
              "  </tr>\r\n"
              "  <tr>\r\n"
              "   <td>");
    u_fprintf(lx->OUT, "<b>%U</b></td>\r\n", FSWF("myConverter", "Encoding:"));
    u_fprintf(lx->OUT, "   <td>");
    /* now encoding */

    if(lx->inDemo == FALSE)
    {
        u_fprintf(lx->OUT, "<a href=\"?converter");
        if(strncmp(qs, "converter",9))
            u_fprintf(lx->OUT,"&%s", qs);
        u_fprintf(lx->OUT, "\">");
    }

    u_fprintf(lx->OUT, "<font size=+1>%s</font>", lx->ourCharsetName);
  
    if(lx->inDemo == FALSE)
    {
        u_fprintf(lx->OUT, "</a>\r\n");
    }
  
    u_fprintf(lx->OUT, "</td>\r\n");

    u_fprintf(lx->OUT, "<td align=right rowspan=3>\r\n"); /* ====== begin right hand thingy ======== */

    u_fprintf(lx->OUT, "<a href=\"http://oss.software.ibm.com/icu/\"><i>%U</i> %U</a><br>",
              FSWF("poweredby", "Powered by"),
              FSWF( /* NODEFAULT */ "poweredby_vers", "ICU " U_ICU_VERSION) );

#ifdef LX_SET_TZ
    u_fprintf(lx->OUT, "<a href=\"?SETTZ=\">");
#endif
    dateStr = date( NULL,UDAT_FULL, lx->cLocale,&status);
    u_fprintf(lx->OUT, "%U", dateStr);
    free(dateStr);
#ifdef LX_SET_TZ
    u_fprintf(lx->OUT, "</a>");
#endif
    u_fprintf(lx->OUT, "<br>\r\n");

    if(lx->inDemo == FALSE)
    {
        u_fprintf(lx->OUT, "<a href=\"%s/en/iso-8859-1/?PANICDEFAULT\"><img src=\"../_/incorrect.gif\" alt=\"Click here if text displays incorrectly\"></A>", getenv("SCRIPT_NAME"));
    }

    u_fprintf(lx->OUT, "</td></tr>\r\n"); /* end little right hand thingy */

    /* === Begin line 4 - display locale == */
    u_fprintf(lx->OUT, "<tr><td><b>%U</b></td>\r\n", FSWF("myLocale", "Label Locale:"));

    u_fprintf(lx->OUT, "<td>");
  
    if(lx->inDemo == FALSE)
    {
        u_fprintf(lx->OUT, "<a href=\"?locale");
        if(strncmp(qs, "locale",6))
            u_fprintf(lx->OUT,"&%s", qs);
        u_fprintf(lx->OUT, "\">");
    }
    uloc_getDisplayName(lx->cLocale, lx->cLocale, myChars, 1024, &status);
    u_fprintf_u(lx->OUT, myChars);
    if(lx->inDemo == FALSE)
    {
        u_fprintf(lx->OUT, "</a>");
    }
    u_fprintf(lx->OUT, "</td></tr>\r\n");

    u_fprintf(lx->OUT, "<tr><td><b>%U</b></td>\r\n",
              FSWF("encoding_translit_setting", "Transliteration:"));

#if 0  /* Set to 1 if transliteration isn't working. */
    u_fprintf(lx->OUT, "<td><i>%U</i></td>",
              FSWF("off", "off"));
#else
    /* Transliteration is OK */
    if(lx->inDemo == FALSE)
    {
        if(!strcmp(lx->chosenEncoding, "transliterated"))
        {
            u_fprintf(lx->OUT, "<td><b>*%U*</b> / <a href=\"%s/%s/?%s\">%U</a></td>",
                      FSWF("on","on"),
                      getenv("SCRIPT_NAME"),
                      lx->cLocale,
                      qs,
                      FSWF("off","off"));
        }
        else
        {
            u_fprintf(lx->OUT, "<td><a href=\"%s/%s/transliterated/?%s\">%U</a> / <b>*%U*</b></td>",
                      getenv("SCRIPT_NAME"),
                      lx->cLocale,
                      qs,
                      FSWF("on","on"),
                      FSWF("off","off"));
        }
    }
    else
    { /* indemo */
        u_fprintf(lx->OUT, "<td><b>%U</b></td>", 
                  (!strcmp(lx->chosenEncoding, "transliterated"))?
                  FSWF("on","on") :
                  FSWF("off","off"));
    }
#endif  
  
    u_fprintf(lx->OUT, "</tr>\r\n");

    if(!FSWF_getBundle())
    {
        /* No reason to use FSWF, this error means we have nothing to fetch strings from! */
        u_fprintf(lx->OUT, "<TR><TD COLSPAN=3><B><I>Note: Could not open our private resource bundle %s </I></B><P> - [%s]</TD></TR>\r\n",
                  FSWF_bundlePath(), u_errorName(FSWF_bundleError()));
    }

    if(!isSupportedLocale(lx->cLocale, TRUE))  /* consider it 'supported' if it's been translated. */
    {
        u_fprintf(lx->OUT, "<TD COLSPAN=3 ><FONT COLOR=\"#FF0000\">");
        u_fprintf_u(lx->OUT, FSWF("locale_unsupported", "This display locale, <U>%s</U>, is unsupported."), lx->cLocale);
        u_fprintf(lx->OUT, "</FONT></TD>");
    }


    u_fprintf(lx->OUT, "<tr><td height=5 bgcolor=\"#AFA8AF\" colspan=3><img src=\"../_/c.gif\" alt=\"---\" width=0 height=0></TD></TR>\r\n");

    u_fprintf(lx->OUT, "</table>\r\n");

    u_fprintf(lx->OUT, "<center>\r\n");

    printHelpTag(lx, "", FSWF("help", "Help"));
  
    u_fprintf(lx->OUT, " &nbsp; ");

    printHelpTag(lx, "transliteration", FSWF("transliterate_help", "Transliteration Help"));

    u_fprintf(lx->OUT, " &nbsp; ");

    if(lx->curLocaleName[0])
    {
        u_fprintf(lx->OUT, "<a target=\"_new\" href=\"http://oss.software.ibm.com/cvs/icu/~checkout~/icu/source/data/locales/%s.txt\">%U</A>", 
                  lx->curLocaleName,
                  FSWF("sourceFile", "View Locale Source"));

        u_fprintf(lx->OUT, " &nbsp; ");
        u_fprintf(lx->OUT, "<a target=\"_new\" href=\"http://oss.software.ibm.com/cvs/icu/~checkout~/locale/icu/xml/%s.xml\">%U</A>", 
                  lx->curLocaleName,
                  FSWF("LDMLsourceFile", "View LDML Source"));

        u_fprintf(lx->OUT, " &nbsp; ");
    }


    u_fprintf(lx->OUT, "<a target=\"_new\" HREF=\"http://www.jtcsv.com/cgibin/icu-bugs\">%U</A>",
              FSWF("poweredby_filebug", "File a bug"));
  
    u_fprintf(lx->OUT, "</center><p>\r\n");

    if(lx->couldNotOpenEncoding)
    {
        /* Localize this when it actually works! */
        u_fprintf(lx->OUT,"<tr><td colspan=2><font color=\"#FF0000\">Warning, couldn't open the encoding '%s', using a default.</font></td></tr>\r\n", lx->couldNotOpenEncoding); 
    }

#if 0
    if(!strcmp(lx->chosenEncoding, "transliterated") && U_FAILURE(lx->xlitCtx.transerr))
    {
        u_fprintf(lx->OUT,"<b>%U (%s)- %s</b><p>\r\n", 
                  FSWF("translit_CantOpenPair", "Warning: Could not open the transliterator for the locale pair."),
                  lx->xlitCtx.locale,
                  u_errorName(lx->xlitCtx.transerr));
    }
#endif

}

/* convert from unicode format with the pipe (|) character as the separator,
   to fully escaped ( '%5CuXXXX' ) format. */
char *createEscapedSortList(const UChar *source)
{
    int32_t i, inlen;
    int32_t len;
    int32_t maxlen;
    char *out, *p;

    inlen = u_strlen(source);

    len = 0;
    maxlen = (inlen * 6)+1;
  
    /* sputid imp. */
    out = malloc(maxlen);
    p = out;
  
    for(i=0;i<inlen;i++)
    {
        if(source[i] == '|')
        {
            strcpy(p, "\\u");
            p += 2;
            sprintf(p, "%04X", 0x000D);
            p += 4;
        }
        else if((source[i] == '\\') && (source[i+1] == 'u'))
        {
            int togo = 6; /* copy the next 6 */
            for(;(i<inlen) && (togo--);i++)
            {
                *p++ = (char)source[i];
            }
            i--; 
        }
        else
        {
            strcpy(p, "\\u");
            p += 2;
            sprintf(p, "%04X", source[i]);
            p += 4;
        }
    }
    *p = 0; /* null */
    return out;
}
void writeEscaped(LXContext *lx, const UChar *s)
{
    lx->backslashCtx.html = FALSE;

    if(u_strchr(s, 0x00A0))
    {
        while(*s)
	{
            if(*s == 0x00A0)
                u_fprintf(lx->OUT, " ");
            else
                u_fprintf(lx->OUT, "%K", *s);
	  
            s++;
	}
    }
    else
        u_fprintf(lx->OUT, "%U", s); 

    /* should 'get/restore' here. */
    lx->backslashCtx.html = TRUE;
}

void writeEscapedQuery(LXContext *lx, const UChar *s)
{
    lx->backslashCtx.html = FALSE;

    if(u_strchr(s, 0x00A0))
    {
        while(*s)
	{
            if(*s == 0x00A0)
                u_fprintf(lx->OUT, " ");
            else
                u_fprintf(lx->OUT, "%K", *s);
	  
            s++;
	}
    }
    else
        u_fprintf(lx->OUT, "%U", s); 

    lx->backslashCtx.html = TRUE;
}


  
/* do a fatal error. may not have a content type yet. --------------------------------------- */
/* TODO: this doesn't actually work yet. Should it be localized ? probably. */
void doFatal(LXContext *lx, const char *what, UErrorCode err)
{
    fprintf(lx->fOUT, "Content-type:text/html\r\n\r\n");
    fprintf(lx->fOUT, "<TITLE>ICU LocaleExplorer: Error</TITLE>\r\n");
    fprintf(lx->fOUT, "<H1>ICU LocaleExplorer: Error</H1>\r\n");
    fprintf(lx->fOUT, "<dd>An error of type %d occured while trying to %s.</dd><HR><P>\r\n",err,what);
    fprintf(stderr, "listrb: err %d trying to %s\n",err,what);
    fprintf(lx->fOUT, "You can try <A HREF=\"%s\">starting over</A>, or complain to srl.<P>\r\n",
            getenv("SCRIPT_NAME"));
    fflush(lx->fOUT);
    exit(0);
}
