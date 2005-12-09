/**********************************************************************
*   Copyright (C) 1999-2005, International Business Machines
*   Corporation and others.  All Rights Reserved.
***********************************************************************/

/* routines having to do with generic output needs */


#include "locexp.h"
#include <unicode/uscript.h>
#include "uresimp.h"

/* Explain what the status code means --------------------------------------------------------- */

void explainStatus( LXContext *lx, UErrorCode status, const char *tag )
{
    if(tag == 0)
        tag = "_top_";

    if(status != U_ZERO_ERROR)
        u_fprintf(lx->OUT, " <b><small>");

    switch(status)
    {
    case U_MISSING_RESOURCE_ERROR:
        printHelpTag(lx, "U_MISSING_RESOURCE_ERROR",
                     FSWF("U_MISSING_RESOURCE_ERROR", "(missing resource)"));
        break;

    case U_USING_FALLBACK_WARNING:
        if(lx->parLocale && lx->parLocale->str)
        {
            u_fprintf(lx->OUT, "<a href=\"?_=%s#%s\">", lx->parLocale->str, tag);
            u_fprintf_u(lx->OUT, FSWF("inherited_from", "(inherited from %S)"), lx->parLocale->ustr); 
        }
        else
        {
            u_fprintf(lx->OUT, "<a href=\"?_=root#%s\">", tag);
            u_fprintf_u(lx->OUT, FSWF("inherited", "(inherited)"));
        }

        u_fprintf(lx->OUT, "</a>");
        break;

    case U_USING_DEFAULT_WARNING:
        u_fprintf(lx->OUT, "<a href=\"?_=root#%s\">", tag);
        if(lx->locales) {
            u_fprintf_u(lx->OUT, FSWF("inherited_from", "(inherited from %S)"), lx->locales->ustr); 
        } else {
            u_fprintf(lx->OUT, "inherited ???"); 
        }
        u_fprintf(lx->OUT, "</a>");
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
        u_fprintf(lx->OUT, "</small></b>");
}

/* Convenience function.  print <a href="..."> for a link to the correct Help page.  if str=null it defaults to Help*/

void printHelpTagW(LXContext *lx, const char *helpTag, const UChar *str)
{
    if(str == NULL)
    {
        /* str = FSWF("help", "Help"); */
      
        printHelpImg(lx, helpTag, FSWF("help", "Help"), 
                     FSWF("helpgif", "help.gif"),
                     FSWF("helpgif_opt", "border=\x22\x30\x22 width=\x22\x34\x30\x22  height=\x22\x34\x30\x22")); /* 0, 40, 40 ? */

    } else {
        u_fprintf(lx->OUT, "<a class='mainlink' target=\"icu_lx_help\" href=\"" LDATA_PATH "help.html#%s\">%S</a>",
                  helpTag,str);
    }
}

void printHelpTag(LXContext *lx, const char *helpTag, const UChar *str)
{
    if(str == NULL)
    {
        /* str = FSWF("help", "Help"); */
      
        printHelpImg(lx, helpTag, FSWF("help", "Help"), 
                     FSWF("helpgif", "help.gif"),
                     FSWF("helpgif_opt", "border=\x22\x30\x22 width=\x22\x34\x30\x22  height=\x22\x34\x30\x22"));

    } else {
        u_fprintf(lx->OUT, "<a target=\"icu_lx_help\" href=\"" LDATA_PATH "help.html#%s\">%S</a>",
                  helpTag,str);
    }
}

void printHelpImg(LXContext *lx, const char *helpTag, const UChar *alt, const UChar *src, const UChar *options)
{
    u_fprintf(lx->OUT, "<a href=\"" LDATA_PATH "help.html#%s\" target=\"icu_lx_help\"><img %S src=\"" LDATA_PATH "%S\" align=\"right\" alt=\"%S\" /></a>",
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

    u_fprintf(lx->OUT, "\r\n<form target=\"_new\" method=\"post\" name=\"EXPLORE_%s\" action=\"%s#EXPLORE_%s\">\r\n"
              "<input type=\"hidden\" name=\"x\" value=\"%s\" />"
              "<input type=\"hidden\" name=\"str\" value=\"",
              section, getLXBaseURL(lx, kNO_URL | kNO_SECT), section,section);
    writeEscaped(lx, sampleString);
    u_fprintf(lx->OUT, "\" />\r\n");

    showExploreButtonPicture( lx );
}

void showExploreButtonPicture( LXContext *lx ) {
    u_fprintf(lx->OUT, "<input type=\"image\" alt=\"demo\" src=\"" LDATA_PATH "explore.gif\" align=\"right\" value=\"%S\" /></form>",
              FSWF("exploreTitle", "Explore"));
}

void showExploreButtonSort( LXContext *lx, UResourceBundle *rb, const char *locale, const char *key, UBool rightAlign)
{
    const char *section;  
    section = keyToSection(key);

    u_fprintf(lx->OUT, "<a target=\"_new\" href=\"%s&amp;x=%s\">", getLXBaseURL(lx, kNO_URL|kNO_SECT), section);
  u_fprintf(lx->OUT, "<img width=\"48\" height=\"20\" border=\"0\" src=\"" LDATA_PATH "explore.gif\" %s alt=\"%S\" />",
              rightAlign?"align=\"right\" ":"",
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

    u_fprintf(lx->OUT, "<a target=\"lx_explore_%s_%s\" href=\"%s&amp;x=%s&str=",
              locale,key,getLXBaseURL(lx, kNO_URL | kNO_SECT),section);
    writeEscaped(lx, sampleString);
    u_fprintf(lx->OUT, "\">");
}

/* Show the 'short' HTML for a line item. It is short because it has not closed out the table yet - the caller can put in their own push button before closing the table cell/column. */
void showKeyAndStartItemShort(LXContext *lx, const char *key, const UChar *keyName, const char *locale, UBool cumulative, UErrorCode showStatus)
{
    u_fprintf(lx->OUT, "<table summary=\"%S\" border=\"0\" cellspacing=\"0\" width=\"100%%\">\r\n", keyName);
    u_fprintf(lx->OUT, "<tr><td height=\"2\" bgcolor=\"#cccccc\" colspan=\"2\"><img src=\"" LDATA_PATH "c.gif\" width=\"0\" height=\"0\" alt=\"divider\" /></td></tr>\r\n");
    u_fprintf(lx->OUT, "<tr><td class='tdblue' colspan=\"1\" width=\"0\" valign=\"top\" bgcolor=" kXKeyBGColor ">");

    if(keyName == NULL)
	keyName = FSWF( key, key );
    u_fprintf(lx->OUT, "<span class=\"mainlink\">"); /* was subTitle */
    printHelpTagW(lx, key, keyName);
    u_fprintf(lx->OUT, "</span>\n");


    if(cumulative == TRUE )
    {
        u_fprintf(lx->OUT, " (%S)", FSWF("cumulative_notshown", "cumulative data from parent not shown"));
    }

    u_fprintf(lx->OUT," </td><td bgcolor=" kXKeyBGColor " valign=\"top\" align=\"right\">");
    explainStatus(lx, showStatus, key);
}

void showKeyAndStartItem(LXContext *lx, const char *key, const UChar *keyName, const char *locale, UBool cumulative, UErrorCode showStatus)
{
    showKeyAndStartItemShort(lx, key,keyName,locale, cumulative, showStatus);
    u_fprintf(lx->OUT,"&nbsp;</td></tr><tr><td colspan=\"2\">\r\n");
}

void showKeyAndEndItem(LXContext *lx, const char *key, const char *locale)
{
    u_fprintf(lx->OUT, "<br />\r\n</td>\r\n</tr>\r\n</table>\r\n");
    u_fprintf(lx->OUT, "<!-- %s:%d -->\r\n", __FILE__, __LINE__);
}

/** for explorers, show a pattern as a textarea
 */

void exploreShowPatternForm(LXContext *lx, UChar *dstPattern, const char *locale, const char *key, const char* qs, double value, UNumberFormat *valueFmt)
{
    UErrorCode status = U_ZERO_ERROR;
    UChar tmp[1024];

    /**********  Now we've got the pattern from the user. Now for the form.. ***/
    u_fprintf(lx->OUT, "<form method=\"post\" action=\"%s#EXPLORE_%s\">\r\n",
              getLXBaseURL(lx, kNO_URL), key);

    if(valueFmt)
    {
      
        u_fprintf(lx->OUT, "<input name=\"NP_DBL\" type=\"hidden\" value=\"");
        tmp[0] = 0;
        unum_formatDouble(valueFmt, value, tmp, 1000, 0, &status);
        u_fprintf(lx->OUT, "%S\" />", tmp);
    }
    u_fprintf(lx->OUT, "<textarea rows=\"2\" cols=\"60\" name=\"str\">");

    lx->backslashCtx.html = FALSE;

    u_fprintf(lx->OUT, "%S", dstPattern); 

    lx->backslashCtx.html = TRUE;
  
    u_fprintf(lx->OUT, "</textarea><br /><br />\r\n<input type=\"submit\" value=\"Format\" /><input type=\"reset\" value=\"Reset\" /></form>\r\n");

}



/* print that little box in the TR corner ----------------------------------------------------- */

void printStatusTable(LXContext *lx)
{
    UChar myChars[1024];
    UErrorCode status = U_ZERO_ERROR;
    UChar *dateStr;
    
    u_fprintf(lx->OUT, "<br /><table border=\"0\" cellspacing=\"0\" width=\"100%%\">");
    u_fprintf(lx->OUT, "<tr><td height=\"2\" bgcolor=\"#cccccc\" colspan=\"3\"><img src=\"" LDATA_PATH "c.gif\" width=\"0\" height=\"0\" alt=\"divider\" /></td></tr>\r\n");
    u_fprintf(lx->OUT, "<tr>\r\n   <td colspan=\"3\" width=\"0\" valign=\"top\" bgcolor=" kXKeyBGColor "><a name=\"%s\"><b>", "YourSettings");
    
    /* PrintHelpTag */
    u_fprintf_u(lx->OUT, FSWF("statusTableHeader", "Your settings:"));

    if(!lx->inDemo)
    {
        u_fprintf(lx->OUT, " %S",   FSWF("statusTableHeaderChange", "(click to change)"));
    }

    /* /PrintHelpTag */
    u_fprintf(lx->OUT, "</b></a></td>\r\n"
              "  </tr>\r\n");
#if 0
              "  <tr>\r\n"
              "   <td>");
    u_fprintf(lx->OUT, "<b>%S</b></td>\r\n", FSWF("myConverter", "Encoding:"));
    u_fprintf(lx->OUT, "   <td>");
    /* now encoding */
    if(lx->inDemo == FALSE)
    {
        u_fprintf(lx->OUT, "<a href=\"?converter");
        if(strncmp(lx->queryString, "converter",9)) /* fixme */
            u_fprintf(lx->OUT,"&amp;%s", lx->queryString);
        u_fprintf(lx->OUT, "\">");
    }

    u_fprintf(lx->OUT, "<big>%s</big>", lx->convName);
  
    if(lx->inDemo == FALSE)
    {
        u_fprintf(lx->OUT, "</a>\r\n");
    }
  
    u_fprintf(lx->OUT, "</td>\r\n");
#endif

    u_fprintf(lx->OUT, "<tr><td></td><td></td>");
    u_fprintf(lx->OUT, "<td align=\"right\" rowspan=\"3\">\r\n"); /* ====== begin right hand thingy ======== */

    u_fprintf(lx->OUT, "<a href=\"" ICU_URL "\"><i>%S</i> %S</a><br />",
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
    u_fprintf(lx->OUT, "<br />\r\n");

#if 0
    if(lx->inDemo == FALSE)
    {
        u_fprintf(lx->OUT, "<a href=\"%s/en/utf-8/?PANICDEFAULT=yes\"><img src=\"" LDATA_PATH "incorrect.gif\" alt=\"Click here if text displays incorrectly\" /></a>", lx->scriptName);
    }
#endif

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
      if(strstr(lx->pathInfo,"/transliterated/")) {
        u_fprintf(lx->OUT, "<td><b>*%S*</b> / <a href=\"%s/%s/?",
                  FSWF("on","on"),
                  lx->scriptName,
                  lx->dispLocale);
        writeEscapedHTMLChars(lx, lx->queryString);
        u_fprintf(lx->OUT, "\">%S</a></td>\r\n",
                  FSWF("off","off"));
      } else {
        u_fprintf(lx->OUT, "<td><a rel=\"nofollow\" href=\"%s/%s/transliterated/?",
                  lx->scriptName,
                  lx->dispLocale);
        writeEscapedHTMLChars(lx, lx->queryString);
        u_fprintf(lx->OUT, "\">%S</a> / <b>*%S*</b></td>\r\n",
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
      u_fprintf(lx->OUT, "<tr><td colspan=3><b><i>Note: Could not open our private resource bundle %s </i></b><p> - [%s]</td></tr>\r\n",
                FSWF_bundlePath(), u_errorName(FSWF_bundleError()));
    }
    
    if(!isSupportedLocale(lx->dispLocale, TRUE)) {  /* consider it 'supported' if it's been translated. */
        u_fprintf(lx->OUT, "<tr><td colspan=\"3\" style=\"color: #FF0000;\">");
        u_fprintf_u(lx->OUT, FSWF("locale_unsupported", "This display locale, <strong>%s</strong>, is unsupported."), lx->dispLocale);
      u_fprintf(lx->OUT, "</td></tr>");
    }


    u_fprintf(lx->OUT, "<tr><td height=\"2\" bgcolor=\"#cccccc\" colspan=\"3\"><img src=\"" LDATA_PATH "c.gif\" width=\"0\" height=\"0\" alt=\"divider\" /></td></tr>\r\n");
    
    u_fprintf(lx->OUT, "</table><br />\r\n");
    u_fprintf(lx->OUT, "<div style=\"text-align: center;\">\r\n");
    printHelpTag(lx, "", FSWF("help", "Help"));
    u_fprintf(lx->OUT, " &nbsp;\r\n");
    printHelpTag(lx, "transliteration", FSWF("transliterate_help", "Transliteration Help"));
    u_fprintf(lx->OUT, " &nbsp;\r\n");

    if(lx->curLocaleName[0]) {
#if defined(ICU_HAVE_WEBCVS)
      u_fprintf(lx->OUT, "<a target=\"_new\" href=\"http://dev.icu-project.org/cgi-bin/viewcvs.cgi/*checkout*/icu/source/data/locales/%s.txt\">%S</a>", 
                lx->curLocaleBlob.base,
                FSWF("sourceFile", "View Locale Source"));
#endif      
      if(!isExperimentalLocale(lx->curLocaleName) && !lx->noBug) {
        u_fprintf(lx->OUT, " &nbsp;\r\n");
        u_fprintf(lx->OUT, "<a target=\"_new\" href=\"http://www.unicode.org/cldr/data/common/%s/%s.xml\">%S</a>", 
                  !strcmp(lx->section,"col")?"collation":"main",
                  lx->curLocaleBlob.base,
                FSWF("XMLsource", "XML Source"));
      
      u_fprintf(lx->OUT, " &nbsp;\r\n");

      u_fprintf(lx->OUT, "<a target=\"_new\" href=\"http://www.unicode.org/cldr/data/diff/%s/%s.html\">%S</a>", 
                !strcmp(lx->section,"col")?"collation":"main",
                lx->curLocaleBlob.base,
                FSWF("XMLcomp", "Compare"));
      u_fprintf(lx->OUT, " &nbsp; ");

      }
    }
    u_fprintf(lx->OUT," &nbsp;\r\n");
    
    if(!lx->noBug) {
      u_fprintf(lx->OUT, "<a target=\"_new\" href=\"http://bugs.icu-project.org/cgibin/icu-bugs\">%S</a>",
                FSWF("poweredby_filebug", "File a bug"));
    } else {
      u_fprintf(lx->OUT, "<p style=\"color: red;\"><b>%S</b></p>",
          FSWF("warningNoBug", "Please do not file bugs against this locale."));
    }
    
    u_fprintf(lx->OUT, "</div>\r\n");

    if(lx->couldNotOpenEncoding) {
      /* Localize this when it actually works! */
      u_fprintf(lx->OUT,"<tr><td colspan=\"2\"><font color=\"#FF0000\">Warning, couldn't open the encoding '%s', using a default.</font></td></tr>\r\n", lx->couldNotOpenEncoding); 
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

void writeEscapedHTMLChars(LXContext *lx, const char *s)
{
    lx->backslashCtx.html = FALSE;

    while(*s) {
        switch (*s) {
        case '&': u_fprintf(lx->OUT, "&amp;"); break;
        case '<': u_fprintf(lx->OUT, "&lt;"); break;
        case '>': u_fprintf(lx->OUT, "&gt;"); break;
        default: u_fputc((UChar)*s, lx->OUT); break;
        }
        s++;
    }

    /* should 'get/restore' here. */
    lx->backslashCtx.html = TRUE;
}

void writeEscapedHTMLUChars(LXContext *lx, const UChar *s)
{
    lx->backslashCtx.html = FALSE;

    while(*s) {
        switch (*s) {
        case '&': u_fprintf(lx->OUT, "&amp;"); break;
        case '<': u_fprintf(lx->OUT, "&lt;"); break;
        case '>': u_fprintf(lx->OUT, "&gt;"); break;
        default: u_fputc(*s, lx->OUT); break;
        }
        s++;
    }

    /* should 'get/restore' here. */
    lx->backslashCtx.html = TRUE;
}

void writeEscaped(LXContext *lx, const UChar *s)
{
    lx->backslashCtx.html = FALSE;

    if(1||u_strchr(s, 0x00A0))
    {
        while(*s)
        {
            if(*s == 0x00A0)
                u_fprintf(lx->OUT, " ");
            else if(*s == '"')
		u_fprintf(lx->OUT, "&quot;");
            else
                u_fprintf(lx->OUT, "%C", *s);

            s++;
        }
    }
    else
       writeEscapedHTMLUChars(lx, s);
    /*       u_fprintf(lx->OUT, "%S", s); */

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
    fprintf(lx->fOUT, "<title>ICU LocaleExplorer: Error</title>\r\n");
    fprintf(lx->fOUT, "<h1>ICU LocaleExplorer: Error</h1>\r\n");
    fprintf(lx->fOUT, "<dd>An error of type %d occured while trying to %s.</dd><hr /><p>\r\n",err,what);
#if defined (LX_DEBUG)
    fprintf(stderr, "locexp: err %d trying to %s\n",err,what);
#endif
    fprintf(lx->fOUT, "You can try <a href=\"%s\">starting over</a>, or complain to srl.<p>\r\n",
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
  u_fprintf(lx->OUT, "Now: [%s]<br />\n", lx->curLocaleName);
  u_fprintf(lx->OUT, "= %S<br />\n", dispname);
  u_fprintf(lx->OUT, "<form method=GET action=\"?_=%s\">\n", lx->curLocaleName);

  u_fprintf(lx->OUT, "<select name=l>\n");
  u_fprintf(lx->OUT, " <option value=\"\"></option>\n");
  for(n=0;n<lx->locales->nSubLocs;n++) {
    u_fprintf(lx->OUT, "<option %s value=\"%s\">%S</option>\n",
              !strcmp(lx->locales->subLocs[n]->str,lx->curLocaleL)?"selected":"",
              lx->locales->subLocs[n]->str,
              lx->locales->subLocs[n]->ustr);
  }
  u_fprintf(lx->OUT, "</select>\n");

  u_fprintf(lx->OUT, "<select name=s>\n");
  u_fprintf(lx->OUT, " <option value=\"\"></option>\n");
  for(n=0;n<USCRIPT_CODE_LIMIT;n++) {
    u_fprintf(lx->OUT, " <option %s value=\"%s\">%s</option>\n",
              !strcmp(uscript_getShortName((UScriptCode)n),lx->curLocaleS)?"selected":"",
              uscript_getShortName((UScriptCode)n),
              uscript_getName((UScriptCode)n)); /* change to localized */
  }
  u_fprintf(lx->OUT, "</select>\n");

  u_fprintf(lx->OUT, "<select name=r>\n");
  u_fprintf(lx->OUT, " <option value=\"\"></option>\n");
  for(n=0;n<lx->regions->nSubLocs;n++) {
    char rgn[128];
    
    uloc_getCountry(lx->regions->subLocs[n]->str, rgn, 128, &status);

    u_fprintf(lx->OUT, " <option %s value=\"%s\">%S</option>\n",
              !strcmp(rgn,lx->curLocaleR)?"selected=\"selected\"":"",
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
  
  u_fprintf(lx->OUT, "<input type=submit />\n");
  u_fprintf(lx->OUT, "</form>\n");
}
#endif

#define LX_FSECTION_START  "<small><span style=\"color: #999999;\">"
#define LX_FSECTION_END    "</span></small>"

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
    
    u_fprintf(lx->OUT, "%s%S%s<br />\r\n", LX_FSECTION_START, keyBuf, LX_FSECTION_END);
    u_fprintf(lx->OUT, "<a href=\"%s&amp;x=ch%s%c&amp;ox=%s\">",
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
  u_fprintf(lx->OUT, "<table border=\"3\"><tr>\r\n");
  u_fprintf(lx->OUT, "<td>%s%S%s<br />\r\n", LX_FSECTION_START, FSWF("LocaleCodes_Language", "Language"), LX_FSECTION_END);
  u_fprintf(lx->OUT, "<a href=\"%s&amp;x=ch%s%c&amp;ox=%s\">",
            getLXBaseURL(lx, kNO_URL | kNO_SECT),
            prefix, 'l', lx->section);
  printChangeField(lx, locale, prefix, 'l');
  printChangeField(lx, locale, prefix, 's');
  u_fprintf(lx->OUT, "</a></td>");

  u_fprintf(lx->OUT, "<td>%s%S / %S%s<br />\r\n", LX_FSECTION_START,
            FSWF("LocaleCodes_Country", "Region"),
            FSWF("LocaleCodes_Variant", "Variant"),LX_FSECTION_END);
  u_fprintf(lx->OUT, "<a href=\"%s&amp;x=ch%s%c&amp;ox=%s\">",
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
  u_fprintf(lx->OUT, "<br /><tt>base URL= %s</tt><br />\n", getLXBaseURL(lx,0));
#endif
  printChangeA(lx, lx->curLocaleName, "");
}

static void startCell(LXContext *lx) 
{
  u_fprintf(lx->OUT, "<table border=\"3\"><tr>");
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
    u_fprintf(lx->OUT, "</tr>\n<tr>\r\n");
  }
  selected = (!strcmp(str,current));
  if(selected) {
    u_fprintf(lx->OUT, "<td style=\"background-color: #c8d7e3;\">", lx->OUT);
  } else if(couldBold && !doBold) {
      u_fprintf(lx->OUT, "<td style=\"background-color: #cccccc;\">", lx->OUT); 
  } else {
    u_fprintf(lx->OUT, "<td>");
  }
  u_fprintf(lx->OUT, "<a href=\"%s&amp;%s%s=%s\">%s%S%s</a>",
            myURL,
            prefix,
            partStr,
            str,
            doBold?"<b>":(couldBold?"<span style=\"color: #333333\">":""),
            ustr,
            doBold?"</b>":(couldBold?"</span>":""));
  u_fprintf(lx->OUT, "</td>\r\n");
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
    en = ures_getKeywordValues( NULL, "Currencies", status);
  } else  {
    en = ures_getKeywordValues( NULL, e, status);
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
  /*UBool selected = FALSE;*/
  LocaleBlob *b;
  UErrorCode status = U_ZERO_ERROR;
  int32_t adds = 0;

  ox = queryField(lx, "ox");
  if(ox && *ox) {
    strcpy(oxStr,"&amp;x=");
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
