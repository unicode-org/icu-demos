/**********************************************************************
*   Copyright (C) 1999-2003, International Business Machines
*   Corporation and others.  All Rights Reserved.
***********************************************************************/

/* routines having to do with generic output needs */


#include "locexp.h"
#include <unicode/uscript.h>

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
            u_fprintf_u(lx->OUT, FSWF("inherited_from", "(inherited from %S)"), lx->parLocale->ustr); 
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
        u_fprintf_u(lx->OUT, FSWF("inherited_from", "(inherited from %S)"), lx->locales->ustr); 
        u_fprintf(lx->OUT, "</A>");
        break;

    default:
        if(status != U_ZERO_ERROR)
	{
            u_fprintf(lx->OUT, "(%d - %s)", (int) status,
                      u_errorName(status));
#ifdef SRL_DEBUG
            fprintf(stderr,"locexp: caught Unknown err- %d %s\n", status, u_errorName(status)); 
#endif
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

    } else {
      u_fprintf(lx->OUT, "<A TARGET=\"icu_lx_help\" HREF=\"" LDATA_PATH "help.html#%s\">%S</A>",
                helpTag,str);
    }
}

void printHelpImg(LXContext *lx, const char *helpTag, const UChar *alt, const UChar *src, const UChar *options)
{
    u_fprintf(lx->OUT, "<a href=\"" LDATA_PATH "help.html#%s\" target=\"icu_lx_help\"><img %S src=\"" LDATA_PATH "%S\" align=right alt=\"%S\"></a>",
              helpTag, 
              options, src, alt);
}

void showExploreCloseButton(LXContext *lx, const char *locale, const char *frag)
{
    /* What do we do here? */
    u_fprintf(lx->OUT, "<!-- no CLOSE BUTTON here. -->\r\n");
}

const char *keyToSection(const char *key) {
  if(!strcmp(key, "CollationElements")) { 
    return "col"; 
  } else if(!strcmp(key, "NumberPatterns")) {
    return "num";
  } else if(!strcmp(key, "DateTimePatterns")) {
    return "dat";
  } else if(!strcmp(key, "Calendar")) {
    return "cal";
  } else {
    return key;
  }
}

void showExploreButton( LXContext *lx, UResourceBundle *rb, const char *locale, const UChar *sampleString, const char *key)
{
    UChar nullString[] = { 0x0000 };
    const char *section;
  
    section = keyToSection(key);

    if(!sampleString)
        sampleString = nullString;

    u_fprintf(lx->OUT, "\r\n<form target=\"_new\" method=POST name=EXPLORE_%s action=\"%s#EXPLORE_%s\">\r\n"
              "<input type=hidden name=\"x\" value=\"%s\">"
              "<input type=hidden name=\"str\" value=\"",
              section, getLXBaseURL(lx, kNO_URL | kNO_SECT), section,section);
    writeEscaped(lx, sampleString);
    u_fprintf(lx->OUT, "\">\r\n");
  
    u_fprintf(lx->OUT, "<input type=image valign=top width=48 height=20 border=0 src=\"" LDATA_PATH "explore.gif\" align=right value=\"%S\"></form>",
              FSWF("exploreTitle", "Explore"));
}

void showExploreButtonSort( LXContext *lx, UResourceBundle *rb, const char *locale, const char *key, UBool rightAlign)
{
    const char *section;  
    section = keyToSection(key);

    u_fprintf(lx->OUT, "<a target=\"_new\" href=\"%s&x=%s\">", getLXBaseURL(lx, kNO_URL|kNO_SECT), section);
  u_fprintf(lx->OUT, "<img width=48 height=20 border=0 src=\"" LDATA_PATH "explore.gif\" %s ALT=\"%S\">",
              rightAlign?"ALIGN=RIGHT ":"",
            FSWF("exploreTitle", "Explore") );
  u_fprintf(lx->OUT, "</a>\r\n");
}

void showExploreLink( LXContext *lx, UResourceBundle *rb, const char *locale, const UChar *sampleString, const char *key)
{
    UChar nullString[] = { 0x0000 };
    const char *section;  
    section = keyToSection(key);
  
    if(!sampleString)
        sampleString = nullString;

    u_fprintf(lx->OUT, "<A TARGET=\"lx_explore_%s_%s\" HREF=\"%s&x=%s&str=",
              locale,key,getLXBaseURL(lx, kNO_URL | kNO_SECT),section);
    writeEscaped(lx, sampleString);
    u_fprintf(lx->OUT, "&\">");
}

/* Show the 'short' HTML for a line item. It is short because it has not closed out the table yet - the caller can put in their own push button before closing the table cell/column. */
void showKeyAndStartItemShort(LXContext *lx, const char *key, const UChar *keyName, const char *locale, UBool cumulative, UErrorCode showStatus)
{
    u_fprintf(lx->OUT, "<TABLE summary=\"%S\" BORDER=0 CELLSPACING=0 WIDTH=\"100%%\">", keyName);
    u_fprintf(lx->OUT, "<TR><TD HEIGHT=5 BGCOLOR=\"#AFA8AF\" COLSPAN=2><IMG SRC=\"" LDATA_PATH "c.gif\" ALT=\"---\" WIDTH=0 HEIGHT=0></TD></TR>\r\n");
    u_fprintf(lx->OUT, "<TR><TD COLSPAN=1 WIDTH=0 VALIGN=TOP BGCOLOR=" kXKeyBGColor "><A NAME=%s>", key);
    u_fprintf(lx->OUT,"</A>", keyName);

    if(keyName == NULL)
	keyName = FSWF( key, key );

    printHelpTag(lx, key, keyName);


    if(cumulative == TRUE )
    {
        u_fprintf(lx->OUT, " (%S)", FSWF("cumulative_notshown", "cumulative data from parent not shown"));
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
    u_fprintf(lx->OUT, "<FORM METHOD=POST ACTION=\"%s#EXPLORE_%s\">\r\n",
              getLXBaseURL(lx, kNO_URL), key);

    if(valueFmt)
    {
      
        u_fprintf(lx->OUT, "<INPUT NAME=NP_DBL TYPE=HIDDEN VALUE=\"");
        tmp[0] = 0;
        unum_formatDouble(valueFmt, value, tmp, 1000, 0, &status);
        u_fprintf(lx->OUT, "%S\">", tmp);
    }
    u_fprintf(lx->OUT, "<TEXTAREA ROWS=2 COLS=60 NAME=\"str\">");

    lx->backslashCtx.html = FALSE;

    u_fprintf(lx->OUT, "%S", dstPattern); 

    lx->backslashCtx.html = TRUE;
  
    u_fprintf(lx->OUT, "</TEXTAREA><P>\r\n<INPUT TYPE=SUBMIT VALUE=Format><INPUT TYPE=RESET VALUE=Reset></FORM>\r\n");

}



/* print that little box in the TR corner ----------------------------------------------------- */

void printStatusTable(LXContext *lx)
{
    UChar myChars[1024];
    UErrorCode status = U_ZERO_ERROR;
    UChar *dateStr;
    
    u_fprintf(lx->OUT, "<table border=0 cellspacing=0 width=\"100%%\">");
    u_fprintf(lx->OUT, "<tr><td height=5 bgcolor=\"#0F080F\" colspan=3><img src=\"" LDATA_PATH "c.gif\" alt=\"---\" width=0 height=0></td></tr>\r\n");
    u_fprintf(lx->OUT, "  <tr>\r\n   <td colspan=3 width=0 valign=top bgcolor=" kXKeyBGColor "><a name=%s><b>", "YourSettings");
    
    /* PrintHelpTag */
    u_fprintf_u(lx->OUT, FSWF("statusTableHeader", "Your settings:"));

    if(!lx->inDemo)
    {
        u_fprintf(lx->OUT, " %S",   FSWF("statusTableHeaderChange", "(click to change)"));
    }

    /* /PrintHelpTag */
    u_fprintf(lx->OUT, "</b></a></td>\r\n"
              "  </tr>\r\n"
              "  <tr>\r\n"
              "   <td>");
    u_fprintf(lx->OUT, "<b>%S</b></td>\r\n", FSWF("myConverter", "Encoding:"));
    u_fprintf(lx->OUT, "   <td>");
    /* now encoding */

    if(lx->inDemo == FALSE)
    {
        u_fprintf(lx->OUT, "<a href=\"?converter");
        if(strncmp(lx->queryString, "converter",9)) /* fixme */
            u_fprintf(lx->OUT,"&%s", lx->queryString);
        u_fprintf(lx->OUT, "\">");
    }

    u_fprintf(lx->OUT, "<font size=\"+1\">%s</font>", lx->convName);
  
    if(lx->inDemo == FALSE)
    {
        u_fprintf(lx->OUT, "</a>\r\n");
    }
  
    u_fprintf(lx->OUT, "</td>\r\n");

    u_fprintf(lx->OUT, "<td align=right rowspan=3>\r\n"); /* ====== begin right hand thingy ======== */

    u_fprintf(lx->OUT, "<a href=\"http://oss.software.ibm.com/icu/\"><i>%S</i> %S</a><br>",
              FSWF("poweredby", "Powered by"),
              FSWF( /* NODEFAULT */ "poweredby_vers", "ICU " U_ICU_VERSION) );

#ifdef LX_SET_TZ
    u_fprintf(lx->OUT, "<a href=\"?SETTZ=\">");
#endif
    dateStr = date( NULL,UDAT_FULL, lx->dispLocale,&status);
    u_fprintf(lx->OUT, "%S", dateStr);
    free(dateStr);
#ifdef LX_SET_TZ
    u_fprintf(lx->OUT, "</a>");
#endif
    u_fprintf(lx->OUT, "<br>\r\n");

    if(lx->inDemo == FALSE)
    {
        u_fprintf(lx->OUT, "<a href=\"%s/en/utf-8/?PANICDEFAULT=yes\"><img src=\"" LDATA_PATH "incorrect.gif\" alt=\"Click here if text displays incorrectly\"></A>", lx->scriptName);
    }

    u_fprintf(lx->OUT, "</td></tr>\r\n"); /* end little right hand thingy */

    /* === Begin line 4 - display locale == */
    u_fprintf(lx->OUT, "<tr><td><b>%S</b></td>\r\n", FSWF("myLocale", "Label Locale:"));

    u_fprintf(lx->OUT, "<td>");
  
    if(lx->inDemo == FALSE)
    {
      printChangeA(lx, lx->dispLocale, "d");
    } else {
      uloc_getDisplayName(lx->dispLocale, lx->dispLocale, myChars, 1024, &status);
      u_fprintf_u(lx->OUT, myChars);
      u_fprintf(lx->OUT, " <i>TODO: make this clickable</i> ");
    }
    u_fprintf(lx->OUT, "</td></tr>\r\n");

    u_fprintf(lx->OUT, "<tr><td><b>%S</b></td>\r\n",
              FSWF("encoding_translit_setting", "Transliteration:"));

#if 0  /* Set to 1 if transliteration isn't working. */
    u_fprintf(lx->OUT, "<td><i>%S</i></td>",
              FSWF("off", "off"));
#else
    /* Transliteration is OK */
    if(lx->inDemo == FALSE)  {
      if(!strcmp(lx->convRequested, "transliterated")) {
        u_fprintf(lx->OUT, "<td><b>*%S*</b> / <a href=\"%s/%s/?%s\">%S</a></td>",
                  FSWF("on","on"),
                  lx->scriptName,
                  lx->dispLocale,
                  lx->queryString,
                  FSWF("off","off"));
      } else {
        u_fprintf(lx->OUT, "<td><a href=\"%s/%s/transliterated/?%s\">%S</a> / <b>*%S*</b></td>",
                  lx->scriptName,
                  lx->dispLocale,
                  lx->queryString,
                  FSWF("on","on"),
                  FSWF("off","off"));
      }
    } else { /* indemo */
      u_fprintf(lx->OUT, "<td><b>%S</b></td>", 
                (!strcmp(lx->convRequested, "transliterated"))?
                FSWF("on","on") :
                FSWF("off","off"));
    }
#endif  
  
    u_fprintf(lx->OUT, "</tr>\r\n");

    if(!FSWF_getBundle()) {
      /* No reason to use FSWF, this error means we have nothing to fetch strings from! */
      u_fprintf(lx->OUT, "<TR><TD COLSPAN=3><B><I>Note: Could not open our private resource bundle %s </I></B><P> - [%s]</TD></TR>\r\n",
                FSWF_bundlePath(), u_errorName(FSWF_bundleError()));
    }
    
    if(!isSupportedLocale(lx->dispLocale, TRUE)) {  /* consider it 'supported' if it's been translated. */
      u_fprintf(lx->OUT, "<TD COLSPAN=3 ><FONT COLOR=\"#FF0000\">");
      u_fprintf_u(lx->OUT, FSWF("locale_unsupported", "This display locale, <U>%s</U>, is unsupported."), lx->dispLocale);
      u_fprintf(lx->OUT, "</FONT></TD>");
    }


    u_fprintf(lx->OUT, "<tr><td height=5 bgcolor=\"#AFA8AF\" colspan=3><img src=\"" LDATA_PATH "c.gif\" alt=\"---\" width=0 height=0></TD></TR>\r\n");
    
    u_fprintf(lx->OUT, "</table>\r\n");
    u_fprintf(lx->OUT, "<center>\r\n");
    printHelpTag(lx, "", FSWF("help", "Help"));
    u_fprintf(lx->OUT, " &nbsp; ");
    printHelpTag(lx, "transliteration", FSWF("transliterate_help", "Transliteration Help"));
    u_fprintf(lx->OUT, " &nbsp; ");

    if(lx->curLocaleName[0]) {
      u_fprintf(lx->OUT, "<a target=\"_new\" href=\"http://oss.software.ibm.com/cvs/icu/~checkout~/icu/source/data/locales/%s.txt\">%S</A>", 
                lx->curLocaleBlob.base,
                FSWF("sourceFile", "View Locale Source"));
     
      if(!isExperimentalLocale(lx->curLocaleName) && !lx->noBug) {
        u_fprintf(lx->OUT, " &nbsp; ");
        u_fprintf(lx->OUT, "<a target=\"_new\" href=\"http://oss.software.ibm.com/cvs/icu/~checkout~/locale/common/xml/%s.xml\">%S</A>", 
                  lx->curLocaleBlob.base,
                FSWF("XMLsource", "XML Source"));
      
      u_fprintf(lx->OUT, " &nbsp; ");

      u_fprintf(lx->OUT, "<a target=\"_new\" href=\"http://oss.software.ibm.com/cvs/icu/~checkout~/locale/all_diff_xml/%s.html\">%S</A>", 
                lx->curLocaleBlob.base,
                FSWF("XMLcomp", "Compare"));
      u_fprintf(lx->OUT, " &nbsp; ");

      }
    }
    u_fprintf(lx->OUT," &nbsp; ");
    
    if(!lx->noBug) {
      u_fprintf(lx->OUT, "<a target=\"_new\" HREF=\"http://www.jtcsv.com/cgibin/icu-bugs\">%S</A>",
                FSWF("poweredby_filebug", "File a bug"));
    } else {
      u_fprintf(lx->OUT, "<FONT size=-2 COLOR=red><b>%S</b></FONT>",
          FSWF("warningNoBug", "Please do not file bugs against this locale."));
    }
    
    u_fprintf(lx->OUT, "</center><p>\r\n");

    if(lx->couldNotOpenEncoding) {
      /* Localize this when it actually works! */
      u_fprintf(lx->OUT,"<tr><td colspan=2><font color=\"#FF0000\">Warning, couldn't open the encoding '%s', using a default.</font></td></tr>\r\n", lx->couldNotOpenEncoding); 
    }
    
#if 0
    if(!strcmp(lx->chosenEncoding, "transliterated") && U_FAILURE(lx->xlitCtx.transerr)) {
      u_fprintf(lx->OUT,"<b>%S (%s)- %s</b><p>\r\n", 
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
                u_fprintf(lx->OUT, "%C", *s);
	  
            s++;
	}
    }
    else
        u_fprintf(lx->OUT, "%S", s); 

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
                u_fprintf(lx->OUT, "%C", *s);
	  
            s++;
	}
    }
    else
        u_fprintf(lx->OUT, "%S", s); 

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
    fprintf(stderr, "locexp: err %d trying to %s\n",err,what);
    fprintf(lx->fOUT, "You can try <A HREF=\"%s\">starting over</A>, or complain to srl.<P>\r\n",
            lx->scriptName);
    fflush(lx->fOUT);
    exit(0);
}

/* ============================ */
/* Changes. */
#define kCalendarPart 'K'
#define kCollationPart 'S'
#define kCurrencyPart 'Y'


/* void showKeywordMenuList(LXContext *lx, UEnumeration *e, UErrorCode *status) { */
/*   const char *s = NULL; */
/*   if(U_FAILURE(*status)) { */
/*     return; */
/*   } */
/*   while(s = uenum_next(e, NULL, status)) { */
/*     showKeywordMenu(lx, s, status); */
/*   } */
/*   uenum_close(e); */
/* } */

#if 0
void exPrintChangeLocale(LXContext *lx)
{
  int32_t n;
  UChar dispname[1024];
  UErrorCode status = U_ZERO_ERROR;

  uloc_getDisplayName(lx->curLocaleName, lx->dispLocale, dispname, 1023, &status);
  u_fprintf(lx->OUT, "Now: [%s]<br>\n", lx->curLocaleName);
  u_fprintf(lx->OUT, "= %S<br>\n", dispname);
  u_fprintf(lx->OUT, "<form method=GET action=\"?_=%s\">\n", lx->curLocaleName);

  u_fprintf(lx->OUT, "<select name=l>\n");
  u_fprintf(lx->OUT, " <option value=\"\">\n");
  for(n=0;n<lx->locales->nSubLocs;n++) {
    u_fprintf(lx->OUT, " <option %s value=\"%s\">%S\n",
              !strcmp(lx->locales->subLocs[n]->str,lx->curLocaleL)?"selected":"",
              lx->locales->subLocs[n]->str,
              lx->locales->subLocs[n]->ustr);
  }
  u_fprintf(lx->OUT, "</select>\n");

  u_fprintf(lx->OUT, "<select name=s>\n");
  u_fprintf(lx->OUT, " <option value=\"\">\n");
  for(n=0;n<USCRIPT_CODE_LIMIT;n++) {
    u_fprintf(lx->OUT, " <option %s value=\"%s\">%s\n",
              !strcmp(uscript_getShortName((UScriptCode)n),lx->curLocaleS)?"selected":"",
              uscript_getShortName((UScriptCode)n),
              uscript_getName((UScriptCode)n)); /* change to localized */
  }
  u_fprintf(lx->OUT, "</select>\n");

  u_fprintf(lx->OUT, "<select name=r>\n");
  u_fprintf(lx->OUT, " <option value=\"\">\n");
  for(n=0;n<lx->regions->nSubLocs;n++) {
    char rgn[128];
    
    uloc_getCountry(lx->regions->subLocs[n]->str, rgn, 128, &status);

    u_fprintf(lx->OUT, " <option %s value=\"%s\">%S\n",
              !strcmp(rgn,lx->curLocaleR)?"selected":"",
              rgn,
              lx->regions->subLocs[n]->ustr);
  }
  u_fprintf(lx->OUT, "</select>\n");

  u_fprintf(lx->OUT, "<select name=v>\n");
  u_fprintf(lx->OUT, "</select>\n");

  showKeywordMenuList(lx, ucol_getKeywords(&status), &status);
  if(U_FAILURE(status)) { explainStatus(lx, status, NULL); status=U_ZERO_ERROR; }
  showKeywordMenu(lx, "calendar", &status);
  if(U_FAILURE(status)) { explainStatus(lx, status, NULL); status=U_ZERO_ERROR; }
  showKeywordMenu(lx, "currency", &status);
  if(U_FAILURE(status)) { explainStatus(lx, status, NULL); status=U_ZERO_ERROR; }
  
  u_fprintf(lx->OUT, "<input type=submit>\n");
  u_fprintf(lx->OUT, "</form>\n");
}
#endif

#define LX_FSECTION_START  "<font size=-2 color=\"#888888\">"
#define LX_FSECTION_END    "</font>"

void printChangeField(LXContext *lx, const char *locale, const char *prefix, char part)
{
  UChar dispname[1024];
  UErrorCode status = U_ZERO_ERROR;
  dispname[0]=0;
  switch(part) {
  case 'l':
    uloc_getDisplayLanguage(locale, lx->dispLocale, dispname, 1023, &status);
    break;
  case 's':
    uloc_getDisplayScript(locale, lx->dispLocale, dispname, 1023, &status);
    break;
  case 'r':
    uloc_getDisplayCountry(locale, lx->dispLocale, dispname, 1023, &status);
    break;
  case 'v':
    uloc_getDisplayVariant(locale, lx->dispLocale, dispname, 1023, &status);
    break;
  }
  if(dispname[0]) {
    if((part == 'v') || (part == 's')) {
      u_fprintf(lx->OUT, " (");
    }
    u_fprintf(lx->OUT, "%S", dispname);
    if((part == 'v') || (part == 's')) {
      u_fprintf(lx->OUT, ")");
    }
  } else if((part != 'v') && (part != 's')) {
    u_fprintf(lx->OUT, "%S", FSWF("localeList_None", "(none)"));
  }
}

void printChangeKeyword(LXContext *lx, const char *locale, const char *prefix, 
                        const char *what, char type)
{
    UChar keyBuf[1024];
    UChar valBuf[1024];
    UErrorCode status = U_ZERO_ERROR;

    keyBuf[0]=0;
    uloc_getDisplayKeyword(what,
                           lx->dispLocale,
                           keyBuf,
                           1024,
                           &status);
    if(U_FAILURE(status)) {
      keyBuf[0]=0;
      status = U_ZERO_ERROR;
    }
    valBuf[0]=0;
    uloc_getDisplayKeywordValue(locale,
                                what,
                                lx->dispLocale,
                                valBuf,
                                1024,
                                &status);
    if(U_FAILURE(status)) {
      valBuf[0]=0;
      status = U_ZERO_ERROR;
    }
    
    u_fprintf(lx->OUT, "%s%S%s<br/>", LX_FSECTION_START, keyBuf, LX_FSECTION_END);
    u_fprintf(lx->OUT, "<a href=\"%s&x=ch%s%c&ox=%s\">",
              getLXBaseURL(lx, kNO_URL | kNO_SECT),
              prefix, type, lx->section);
    if(valBuf[0]) {
      u_fprintf(lx->OUT, "%S",  valBuf);
    } else {
      u_fprintf(lx->OUT, "%S", FSWF("localeList_Default", "(default)"));
    }
    u_fprintf(lx->OUT, "</a>");
}

void printChangeA(LXContext *lx, const char *locale, const char *prefix)
{
  u_fprintf(lx->OUT, "<table border=3><tr>\r\n");
  u_fprintf(lx->OUT, "<td>%s%S%s<br/>", LX_FSECTION_START, FSWF("LocaleCodes_Language", "Language"), LX_FSECTION_END);
  u_fprintf(lx->OUT, "<a href=\"%s&x=ch%s%c&ox=%s\">",
            getLXBaseURL(lx, kNO_URL | kNO_SECT),
            prefix, 'l', lx->section);
  printChangeField(lx, locale, prefix, 'l');
  printChangeField(lx, locale, prefix, 's');
  u_fprintf(lx->OUT, "</a></td>");

  u_fprintf(lx->OUT, "<td>%s%S / %S%s<br/>", LX_FSECTION_START,
            FSWF("LocaleCodes_Country", "Region"),
            FSWF("LocaleCodes_Variant", "Variant"),LX_FSECTION_END);
  u_fprintf(lx->OUT, "<a href=\"%s&x=ch%s%c&ox=%s\">",
            getLXBaseURL(lx, kNO_URL | kNO_SECT),
            prefix, 'r', lx->section);
  printChangeField(lx, locale, prefix, 'r');
  printChangeField(lx, locale, prefix, 'v');
  u_fprintf(lx->OUT, "</a></td>");
  
  if(prefix[0]==0) {
    /* -------------- calendar ---------- */
    u_fprintf(lx->OUT, "<td>");
    printChangeKeyword(lx, locale, prefix, "calendar", kCalendarPart);
    u_fprintf(lx->OUT, "</td><td>");
    printChangeKeyword(lx, locale, prefix, "collation", kCollationPart);
    u_fprintf(lx->OUT, "</td><td>");
    printChangeKeyword(lx, locale, prefix, "currency", kCurrencyPart);
    u_fprintf(lx->OUT, "</td>");
  }
  u_fprintf(lx->OUT, "</tr></table>");
}

void printChangeLocale(LXContext *lx)
{
#if defined(LX_DEBUG)
  u_fprintf(lx->OUT, "<br><tt>base URL= %s</tt><br>\n", getLXBaseURL(lx,0));
#endif
  printChangeA(lx, lx->curLocaleName, "");
}

static void startCell(LXContext *lx) 
{
  u_fprintf(lx->OUT, "<table border=3><tr>");
}

static void printCell(LXContext *lx, const char *myURL, const char *prefix, char part, const char *str, 
               const UChar* ustr, int32_t n, const char* current)
{
  UBool selected;
  UBool doBold = FALSE; /* boldface this item? */
  UBool couldBold = FALSE; /* boldface any items? (i.e., gray out others) */
  char partStr[20];
  partStr[0] = part;
  partStr[1] = 0;
  switch(part) {
  case 'l':
    if((!prefix||(*prefix!='d')) && (lx->rLocale)) {
      int32_t i, j;
      /* see which languages have this region*/
      couldBold = TRUE;
#if defined (LX_DEBUG)
      fprintf(stderr, "rLocale = %s\n", lx->rLocale->str);
#endif
      for(i=0;!doBold&&i<lx->locales->nSubLocs;i++) {
        if(!strcmp(str, lx->locales->subLocs[i]->str)) {
          for(j=0;!doBold&&j<lx->locales->subLocs[i]->nSubLocs;j++) {
            UErrorCode status = U_ZERO_ERROR;
            char r[200];
            uloc_getCountry(lx->locales->subLocs[i]->subLocs[j]->str, r, 200, &status);
            if(U_FAILURE(status)) {
              continue;
            }
#if defined (LX_DEBUG)
            fprintf(stderr, "[L] %s vs %s\n", lx->curLocaleBlob.r, r);
#endif
            if(!strcmp(lx->curLocaleBlob.r,r)) {
              doBold = TRUE;
            }
          }
        }
      }
    }
    break;
  case 'r':
    if((!prefix||(*prefix!='d')) && (lx->lLocale)) {
      const MySortable *m;

      int32_t i;
      char r1[200];
      char *rx;
      strcpy(r1,str);
      if((rx=strchr(r1,'_'))) {
        *rx = 0;
      }
      couldBold = TRUE;
      m = lx->lLocale; /* check all Regions under current Language */
      for(i=0;!doBold && i<m->nSubLocs;i++) {
        UErrorCode status = U_ZERO_ERROR;
        char r[200];
        uloc_getCountry(m->subLocs[i]->str, r, 200, &status);
        if(U_FAILURE(status)) {
          continue;
        }
#if defined (LX_DEBUG)
        fprintf(stderr, "[L] %s vs %s\n", r1, r);
#endif
        if(!strcmp(r1,r)) {
          doBold = TRUE;
        }
      }
    }
    break;
  case kCalendarPart:
    prefix="";
    strcpy(partStr, "calendar"); break;
  case kCollationPart:
    prefix="";
    strcpy(partStr, "collation"); break;
  case kCurrencyPart:
    prefix="";
    strcpy(partStr, "currency"); break;
  default: ;
  }
  if(n>0 && (n%5)==0) {
    u_fprintf(lx->OUT, "</tr>\n<tr>");
  }
  selected = (!strcmp(str,current));
  if(selected) {
    u_fprintf(lx->OUT, "<td bgcolor=\"#BBBBFF\">", lx->OUT);
  } else if(couldBold && !doBold) {
    u_fprintf(lx->OUT, "<td b_gcolor=\"#888888\">", lx->OUT); 
  } else {
    u_fprintf(lx->OUT, "<td>");
  }
  u_fprintf(lx->OUT, " <a href=\"%s&%s%s=%s&\">%s%S%s</a>\n",
            myURL,
            prefix,
            partStr,
            str,
            doBold?"<b>":(couldBold?"<font color=\"#444444\">":""),
            ustr,
            doBold?"</b>":(couldBold?"</font>":""));
  u_fprintf(lx->OUT, "</td>");
}

static void endCell(LXContext *lx)
{
  u_fprintf(lx->OUT, "</tr></table>");
}


void showKeywordMenu(LXContext *lx, const char *e, const char *kwVal, int32_t *n, const char *myURL, const char *prefix, char part, UErrorCode *status) {


  UEnumeration *en;
  const char *s;

  if(U_FAILURE(*status)) {
    return;
  }
  /*u_fprintf(lx->OUT, "%s=\n", e);*/
  if(!strcmp(e, "collation")) {
    en = ucol_getKeywordValues(e, status);
  } else if(!strcmp(e, "currency")) {
    en = ures_getKeywordValues( "ICUDATA", "Currencies", status);
  } else  {
    en = ures_getKeywordValues( "ICUDATA", e, status);
  }
  while((s = uenum_next(en, NULL, status))) {
    UChar u[1024];
    char floc[345];
    sprintf(floc, "@%s=%s", e, s);
    uloc_getDisplayKeywordValue(floc, e, lx->dispLocale, u, 1024, status);
    printCell(lx, myURL, prefix, part, s, u, *n, kwVal);
    (*n) ++;
  }
  uenum_close(en);
}

void showChangePage(LXContext *lx)
{
  const char *changeWhat;
  char part = 0;
  UBool isDisp = FALSE;
  char prefix[4];
  char oxStr[20];
  char myURL[2048];
  const char *ox;
  const char *baseU;
  int32_t n =0;
  UBool selected = FALSE;
  LocaleBlob *b;
  UErrorCode status = U_ZERO_ERROR;
  int32_t adds = 0;

  ox = queryField(lx, "ox");
  if(ox && *ox) {
    strcpy(oxStr,"&x=");
    strcat(oxStr, ox);
  } else {
    oxStr[0]=0;
  }
  changeWhat = lx->section + strlen("ch");
  if(*changeWhat == 'd') {
    isDisp = TRUE;
    changeWhat++;
    strcpy(prefix,"d_");
    b = &lx->dispLocaleBlob;
    u_fprintf(lx->OUT, "<h4>%S</h4>\r\n", FSWF("changeLocale", "Change the Locale used for Labels"));
  } else {
    u_fprintf(lx->OUT, "<h4>%S</h4>\r\n", FSWF("chooseLocale", "Choose Your Locale."));
    strcpy(prefix, "_");
    b = &lx->curLocaleBlob;
  }
  part = *changeWhat;
  switch(part) {
  case kCalendarPart: adds |= kNO_CAL; break;
  case kCollationPart: adds |= kNO_COLL; break;
  case kCurrencyPart: adds |= kNO_CURR; break;
  default: adds=0;
  }
  baseU = getLXBaseURL(lx, kNO_URL | kNO_SECT | adds);
  strcpy(myURL, baseU);
  strcat(myURL, oxStr);
  switch (part) {
  case 'l':
    if(isDisp) {
      resetSortsInTheirLocales(lx->locales, &status, FALSE);
    }

    mySort(lx->locales, &status, FALSE); /* sort top level */
    u_fprintf(lx->OUT, "<h3>%S</h3>\n", FSWF("localeList_Locale", "Languages"));
    startCell(lx);
    printCell(lx, myURL, prefix, part, "root", lx->locales->ustr, n, b->l);  /* Language doesn't need a 'none' - use root */
    for(n=0;n<lx->locales->nSubLocs;n++) {
      printCell(lx, myURL, prefix, part, lx->locales->subLocs[n]->str, 
                lx->locales->subLocs[n]->ustr, n+1, b->l);
    }
    /*printCell(lx, myURL, prefix, part, "", FSWF("localeList_None", "(none)"), n, b->l); */ /* Language doesn't need a 'none' - use root */
    endCell(lx);
    break;
  case 's':
    startCell(lx);
    printCell(lx, myURL, prefix, part, "", FSWF("localeList_None", "(none)"), n, b->s);
    endCell(lx);
    break;
  case 'r':
    u_fprintf(lx->OUT, "<h3>%S</h3>\n", FSWF("localeList_Sublocale", "Regions"));
    startCell(lx);
    mySort(lx->regions, &status, FALSE); /* need the whole thing sorted */
    for(n=0;n<lx->regions->nSubLocs;n++) {
      int32_t len, len2; 
      char rgn[128];
      len = uloc_getCountry(lx->regions->subLocs[n]->str, rgn, 128, &status);
      rgn[len] = '_';
      len2 = uloc_getVariant(lx->regions->subLocs[n]->str, rgn+len+1, 128-len-1, &status);
      if(len2 == 0) { /* didn't have a variant */
        rgn[len]=0;
      }
      printCell(lx, myURL, prefix, part, rgn,
                lx->regions->subLocs[n]->ustr, n, b->r);
    }
    printCell(lx, myURL, prefix, part, "", FSWF("localeList_None", "(none)"), n, b->r);
    endCell(lx);
    break;

  case 'v':
    startCell(lx);
    printCell(lx, myURL, prefix, part, "", FSWF("localeList_None", "(none)"), n, b->v);
    endCell(lx);
    break;

  case kCalendarPart:
    u_fprintf(lx->OUT, "<h3>%S</h3>\n", FSWF("localeList_Calendar", "Calendars"));
    startCell(lx);
    showKeywordMenu(lx, "calendar", b->calendar, &n, myURL, prefix, part, &status);
    printCell(lx, myURL, prefix, part, "", FSWF("localeList_Default", "(default)"), n, b->calendar);
    endCell(lx);
    break;

  case kCollationPart:
    u_fprintf(lx->OUT, "<h3>%S</h3>\n", FSWF("localeList_Collation", "Collation"));
    startCell(lx);
    showKeywordMenu(lx, "collation", b->collation, &n, myURL, prefix, part, &status);
    printCell(lx, myURL, prefix, part, "", FSWF("localeList_Default", "(default)"), n, b->collation);
    endCell(lx);
    break;

  case kCurrencyPart:
    u_fprintf(lx->OUT, "<h3>%S</h3>\n", FSWF("localeList_Currency", "Currency"));
    startCell(lx);
    showKeywordMenu(lx, "currency", b->currency, &n, myURL, prefix, part, &status);
    printCell(lx, myURL, prefix, part, "", FSWF("localeList_Default", "(default)"), n, b->currency);
    endCell(lx);
    break;

  default:
    u_fprintf(lx->OUT, "Err - unknkown type code '%c'\n", part);
  }
}
