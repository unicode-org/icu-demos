/**********************************************************************
*   Copyright (C) 1999-2005, International Business Machines
*   Corporation and others.  All Rights Reserved.
***********************************************************************/

/*--------------------------------------------------------------------------
*
* File locexp.c
*
* Modification History:
*
*   Date        Name        Description
*   7/14/1999    srl        Created
*   8/16/1999    srl        Display overhaul - locale status at bottom, etc.
*  10/14/1999    srl        Update for ICU1.3+, prepare for checkin!!
*   8/17/2000    srl        Update for 1.6
*  10/09/2000    srl        Put .gifs and .htmls, etc into resource bundles
*   7/14/2001    srl        Adding configurable Collation demo
*  18/10/2001    srl        Adding RBNF.  Glad to have RBNF available again!
*  30/10/2001    srl        Adding LocaleScript. updating RBNF.
*   2/20/2003    srl        Adding Robot Exclusion META tags, finally 
*                             renaming 'tmp' to something meaningful.
****************************************************************************
*/

#include "locexp.h"

#include "demo_settings.h"
#include "demoutil.h"

void displayLocaleExplorer(LXContext *lx)
{
    const char *localeParam;
    UErrorCode status = U_ZERO_ERROR;
    char langName[ULOC_FULLNAME_CAPACITY] = {0};

    localeParam = queryField(lx, "_");

    /* -------------- */
    u_fprintf(lx->OUT,"<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Transitional//EN\" \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd\">\r\n");

    uloc_getLanguage(lx->dispLocale, langName, sizeof(langName)/sizeof(langName[0]), &status);
    u_fprintf(lx->OUT,"<html xmlns=\"http://www.w3.org/1999/xhtml\" xml:lang=\"%s\"  lang=\"%s\">", langName, langName);

    u_fprintf(lx->OUT, "\r\n<!-- Locale Explorer %s \r\n  " ICU_URL "  \r\n\r\n-->\r\n",
        U_COPYRIGHT_STRING);

    u_fprintf(lx->OUT, "<head>\r\n<title>");
    lx->backslashCtx.html = FALSE;
    printPath(lx, lx->curLocale, lx->curLocale, FALSE);

    /* TODO: check 'section' here */
    if(strstr(lx->queryString, "EXPLORE"))
    {
        lx->inDemo = TRUE;
        u_fprintf(lx->OUT, " &gt; %S", FSWF("exploreTitle", "Explore"));
    }
    else
    {
        lx->inDemo = FALSE;
    }

    lx->backslashCtx.html =TRUE;
    u_fprintf(lx->OUT, "</title>\r\n");

    /* if(!lx->pathInfo || !(lx->pathInfo[0])) */
    {
        const char *host;
        host = getenv("LX_FORCE_HOST");  /* special variable - to force the hostname */
        if((host == NULL)||!host[0])
        {
            host = getenv("HTTP_HOST");  /* Apache httpd canonical host name */
        }
        if((host == NULL)||!host[0])
        {
            host = getenv("SERVER_NAME"); /* the server's idea of the host name */
        }
        if((host == NULL)||!host[0])
        {
            host = "";
        }
        sprintf(lx->myBaseURL, "http://%s%s/", host, lx->scriptName);
        u_fprintf(lx->OUT, "<base href=\"%s%s/\" />\r\n",  lx->myBaseURL,
                (lx->dispLocale&&lx->dispLocale[0])?lx->dispLocale:"root"); /* Ensure that all relative paths have the cgi name followed by a slash.  */
    }


    /* TODO: check section */
    /* Robot Exclusion */
    if(hasQueryField(lx,"PANICDEFAULT") ||
        (lx->pathInfo && strstr(lx->pathInfo,"transliterated")))
    {
        u_fprintf(lx->OUT, "<meta name=\"robots\" content=\"nofollow,noindex\" />\r\n");
    } else if(!strncmp(lx->queryString, "locale_all", 10) || strstr(lx->queryString,"converter")){
        u_fprintf(lx->OUT, "<meta name=\"robots\" content=\"nofollow\" />\r\n");
    } else if(lx->pathInfo && *lx->pathInfo && lx->pathInfo[1] && !strstr(lx->pathInfo,"en_US")) {
        u_fprintf(lx->OUT, "<meta name=\"robots\" content=\"nofollow,noindex\" />\r\n");
    } else if(lx->convRequested && lx->convRequested[0] && !strstr(lx->convRequested, "utf-8")) {
        u_fprintf(lx->OUT, "<meta name=\"robots\" content=\"nofollow,noindex\" />\r\n");
    } else if(strstr(lx->queryString, "_=")) {
        u_fprintf(lx->OUT, "<meta name=\"robots\" content=\"nofollow\" />\r\n");
    }
    if(lx->convRequested && lx->convRequested[0]) {
        u_fprintf(lx->OUT, "<meta http-equiv=\"content-type\" content=\"text/html; charset=%s\" />\r\n", lx->convRequested);
    }

    printHTMLFragment(lx->OUT, FSWF_getBundle(), DEMO_COMMON_DIR "locexp-header.html");
    showSortStyle(lx);
    u_fprintf(lx->OUT, "%s", "</head>\r\n<body>\r\n");
    if (printHTMLFragment(lx->OUT, FSWF_getBundle(), DEMO_COMMON_MASTHEAD)) {
        u_fprintf(lx->OUT, "%s", DEMO_BEGIN_LEFT_NAV);
        printHTMLFragment(lx->OUT, FSWF_getBundle(), DEMO_COMMON_LEFTNAV);
        u_fprintf(lx->OUT, "%s", DEMO_END_LEFT_NAV);
        u_fprintf(lx->OUT, "%s", DEMO_BEGIN_CONTENT);
    }

    printHelpImg(lx, "display", 
                FSWF("display_ALT", "Display Problems?"),
                FSWF("display_GIF", "displayproblems.gif"),
                FSWF("display_OPTIONS", ""));

    u_fprintf(lx->OUT, "<br />\r\n<hr />\r\n");

#if 0
    {
        const char *agent;
        const char *server;
        agent = getenv("HTTP_USER_AGENT");
        server = getenv("SERVER_SOFTWARE");
        if(agent && strstr(agent,"MSIE") && (!server || strncmp(server,"Null",4))) {
            u_fprintf(lx->OUT, "<i>%S</i><br />\r\n",
                FSWF("ieWarning","IE appears to have a bug causing page load errors; if this happens please Refresh (F9)."));
        }
    }
#endif

    if(!strncmp(lx->section, "ch", 2)) {
        showChangePage(lx);
    } else {
        /* fall through to OLD style checks */
        if(localeParam && *localeParam && !lx->curLocale && strcmp(lx->curLocaleName,"g7"))
        {
            UChar dispName[1024];
            UErrorCode stat = U_ZERO_ERROR;
            dispName[0] = 0;
            uloc_getDisplayName(lx->curLocaleName, lx->dispLocale, dispName, 1024, &stat);
            lx->noBug = TRUE; /* hide bug form */
            u_fprintf(lx->OUT, "<blockquote><b>%S [%S]</b> \r\n",
                FSWF("warningInheritedLocale", "Note: You're viewing a locale that does not have verified data . ICU will support this with inherited information, but that information is not verified to be correct."), dispName);
        }
        else if(lx->curLocaleName[0] &&
            isExperimentalLocale(lx->curLocaleName) && 
            strcmp(lx->curLocaleName,"g7"))
        {
            lx->noBug = TRUE;
            u_fprintf(lx->OUT, "<blockquote><b>%S</b>\r\n",
                FSWF("warningExperimentalLocale", "Note: You're viewing a Draft locale. This locale is not part of the official ICU installation. &nbsp;"  ));
        }

        if(lx->noBug) {
            u_fprintf(lx->OUT, "<br /><font color=red><b>%S</b></FONT></blockquote>",
                FSWF("warningNoBug", "Please do not file bugs against this locale."));
        }

        if(strstr(lx->queryString,"EXPLORE")) {
            const char *suffix = NULL; /* Eventually would like ALL explorers to be able to use this logic */

            u_fprintf(lx->OUT, "<big>");
            printPath(lx, lx->curLocale, lx->curLocale, TRUE);

            if(queryField(lx, "EXPLORE_CollationElements")) {
                u_fprintf(lx->OUT, " &gt; %S", FSWF(/**/"EXPLORE_CollationElements", "Collation Demo"));
            }

            u_fprintf(lx->OUT, "</big><p>");
            if(suffix) {
                printSubLocales(lx, suffix);
            }
        }
        else {
            u_fprintf(lx->OUT, "<table summary=\"%S\" width=\"100%%\">\r\n<tr><td align=\"left\" valign=\"top\">", FSWF("title", "ICU LocaleExplorer"));

            if(lx->curLocaleName[0]) { /* don't show a completely empty locale control */
                u_fprintf(lx->OUT, "<big>");
                printPath(lx, lx->curLocale, lx->curLocale, TRUE);
                u_fprintf(lx->OUT, "</big>");
                printChangeLocale(lx);
            }

            u_fprintf(lx->OUT, "</td><td rowspan=\"2\" align=\"right\" valign=\"top\" width=\"1\">");

            u_fprintf(lx->OUT, "\r\n</td></tr>\r\n<tr><td>");

            printSubLocales(lx, NULL);
            u_fprintf(lx->OUT, "</td></tr></table>\r\n");

        }

        if ( lx->queryString == NULL )
            lx->queryString = ""; /* for sanity */
        if( !lx->curLocaleName[0]
            || hasQueryField(lx, "PANICDEFAULT")) /* They're coming in cold. Give them the spiel.. */
        {
            u_fprintf(lx->OUT, "<div style=\"margin-left: 2.5em\"><p>");
            u_fprintf_u(lx->OUT, 
                FSWF("introSpiel", "This demo illustrates the International Components for Unicode localization data.  The data covers %V different languages, further divided into %V regions and variants.  For each language, data such as days of the week, months, and their abbreviations are defined.</p><p>ICU is an open-source project."),
                (double)(lx->locales->nSubLocs),
                (double)(uloc_countAvailable()-(lx->locales->nSubLocs)));
            u_fprintf(lx->OUT, "</p>\r\n");
#if 0
            u_fprintf(lx->OUT, "<p>%S</p>\r\n",
                FSWF/**/(/**/"specialMessage_2000Oct30",/**/
                "<i>Note: Locale Explorer should be much faster, but.. there's an ongoing problem where (at least) Microsoft Internet Explorer users will be faced with a blank page or an error page.. if this occurs, please simply hit Reload and all should be corrected.</i>"));
#endif

            u_fprintf(lx->OUT, "</div>");
        }


        /* Logic here: */
        if( !lx->curLocaleName[0] || !strcmp(lx->section,"main") || !strcmp(lx->section, "ka") ) {    /* ?locale  or not set: pick locale */
            printHelpTag(lx, "chooseLocale", NULL);
            u_fprintf(lx->OUT, "<h4>%S</h4>\r\n", FSWF("chooseLocale", "Choose Your Locale."));
            chooseLocale(lx, TRUE, (char*)lx->dispLocale, "", !strcmp(lx->section,"ka"));
        } else if (hasQueryField(lx,"converter")) {  /* ?converter */
            const char *restored;

            restored = queryField(lx, "ox");

            /*
            if(lx->setEncoding)
            u_fprintf(lx->OUT, ": %S</H2>\r\n", FSWF("changeEncoding", "Change Your Encoding"));
            else
            u_fprintf(lx->OUT, ": %S</H2>\r\n", FSWF("chooseEncoding", "Choose Your Encoding"));
            */
            u_fprintf(lx->OUT, "<hr />");

            if(lx->queryString[9] == '=')
            {
                /* choose from encodings that match a string */
                char *sample;
                char *end;
                UChar usample[256];

                sample = strdup(lx->queryString + 10);
                end    = strchr(sample, '&');

                if(end == NULL)
                {
                    end = sample + strlen(sample);
                }

                unescapeAndDecodeQueryField(usample, 256, sample);

                *end = 0;
                u_fprintf(lx->OUT, "%S: %s<p>\r\n", FSWF("converter_matchesTo", "Looking for matches to these chars: "), sample);
                chooseConverterMatching(lx, restored, usample);
            }
            else
            {
                /* choose from all the converters */
                chooseConverter(lx, restored);
            }
        }
        else if (!strncmp(lx->queryString,"SETTZ=",6))
        {
            /* lx->newZone is initted early, need it for cookies :) */
            if(u_strlen(lx->newZone))
            {
                UErrorCode subStatus = U_ZERO_ERROR;
                u_fprintf(lx->OUT, "Got zone=%S<p>\n", lx->newZone);
                u_fprintf(lx->OUT, "Time there =%S\n", date(lx->newZone,UDAT_FULL,lx->dispLocale,&subStatus));
            }

            u_fprintf(lx->OUT, "%S: <form><input name=\"SETTZ\" value=\"%S\" /><input type=submit /></form>\r\n", 
                FSWF("zone_set", "Set timezone to:"),
                lx->newZone);
            u_fprintf(lx->OUT, "<div style=\"margin-left: 2.5em\"><i>%S</i></div>\r\n", 
                FSWF("zone_warn","Note: only works if you have cookies turned on."));

            {
                const char *cook;
                cook = getenv("HTTP_COOKIE");
                if(cook)
                {
                    u_fprintf(lx->OUT, "<u>%s</u>\r\n", cook);
                }
            }

        }
        else
        {
            /* show an entire locale */
            showOneLocale(lx);
        }

    } /* END OLD STYLE */

    printStatusTable(lx);
#if 0
    /* "find a better converter" */
    if(COLLECT_getChars()[0] != 0x0000)
    {
        UConverterFromUCallback oldCallback;
        UErrorCode status2 = U_ZERO_ERROR;

        oldCallback = ucnv_getFromUCallBack(((UConverter*)u_fgetConverter(lx->OUT)));

        u_fprintf(lx->OUT, "<table width=\"100%%\" border=1><tr><td>%S<br />", FSWF("encoding_Missing", "The following characters could not be displayed properly by the current encoding:"));

        ucnv_setFromUCallBack((UConverter*)u_fgetConverter(lx->OUT), &UCNV_FROM_U_CALLBACK_ESCAPE, &status2);

        u_fprintf(lx->OUT, "%S", COLLECT_getChars());

        ucnv_setFromUCallBack((UConverter*)u_fgetConverter(lx->OUT), oldCallback, &status2);

        u_fprintf(lx->OUT, "<br /><a href=\"?converter=");
        writeEscaped(lx, COLLECT_getChars());

        if(strncmp(lx->queryString, "converter",9)) /* TODO: FIXME */
            u_fprintf(lx->OUT,"&amp;%s", lx->queryString);
        u_fprintf(lx->OUT, "\">");
        u_fprintf(lx->OUT, "%S</a>\r\n",
            FSWF("encoding_PickABetter", "Click here to search for a better encoding"));

        u_fprintf(lx->OUT, "</td></tr></table>\r\n");
    }
#endif

    u_fprintf(lx->OUT, "</td></tr></table>\r\n");
    printHTMLFragment(lx->OUT, FSWF_getBundle(), DEMO_COMMON_FOOTER);

    /* a last resort. will switch to English if they get lost.. */
    /* DO NOT localize the following */
    /* What this does:  
    - brings them to the 'choose your locale' pane in English, then
    - brings them to the 'choose your encoding' pane in their locale, then
    - lists the locales to browse
    */

#ifndef LXHOST
# define LXHOST ""
#endif

    if(!strcmp(lx->dispLocale,"tlh"))
        u_fprintf(lx->OUT, "<p>Thank you for using the ICU LocaleExplorer, from %s compiled %s on %s<p>\r\n", LXHOSTNAME, lx_version(), LXHOST);

    u_fprintf(lx->OUT, "</body></html>\r\n");

    u_fflush(lx->OUT);

    u_fclose(lx->OUT);

    /*     fflush(stderr); */

    if(lx->dispRB)
        ures_close(lx->dispRB);

    if(lx->curRB)
        ures_close(lx->curRB);
}

const UChar *defaultLanguageDisplayName(LXContext *lx)
{
    UErrorCode status = U_ZERO_ERROR;
    UChar *displayName = lx->displayName;
    
    if(displayName[0] == 0x0000)
    {
        uloc_getDisplayLanguage(lx->dispLocale, lx->dispLocale ,displayName, 1024, &status);
    }
    
    return displayName;
}

UBool didUserAskForKey(LXContext *lx, const char *key)
{
    const char *start, *limit, *tmp1, *tmp2;
    
    
    /* look to see if they asked for it */
    start = lx->queryString;
    while( (start = strstr(start, "SHOW")) )
    {
        start += 4;
        
        tmp1 = strchr(start,'='); /*look for end of that field */
        tmp2 = strchr(start,'&'); /*look for end of that field */
        
        limit = tmp1;
        if( (tmp1 && tmp2) && (tmp1 > tmp2) ) /* pin equalsign to same field */	
        {
            limit = tmp2;
        }
        else if(tmp1 == NULL) /* no equalsign */
        {
            limit = tmp2;
        }
        
        if(limit==NULL)
            limit = start + strlen(start);
        
        
        if(((size_t)(limit-start) == strlen(key)) && !strncmp(start, key, (limit-start)))
        {
            return TRUE;
        }
        if(limit==start)
        {
            return FALSE;
        }
        start=limit;
    }
    return FALSE;
}


void exploreFetchNextPattern(LXContext *lx, UChar *dstPattern, const char *patternText)
{
  /* make QS point to the first char of the field data */
  const char *qs = patternText;
  if(qs == NULL) {
    *dstPattern = 0;
    return;
  }
  
  /*  unescapeAndDecodeQueryField(dstPattern, 1000, qs); */
  unescapeAndDecodeQueryField_enc(dstPattern, 1000, qs, lx->convRequested);
  u_replaceChar(dstPattern, 0x0020, 0x00A0);
}



const char *getLXBaseURL(LXContext* lx, uint32_t o) {
    if(!(o&kNO_URL)) {
        strcpy(lx->myURL, lx->myBaseURL);
    } else {
        lx->myURL[0] = 0;
    }
    strcat(lx->myURL, "?");
    if(!(o&kNO_LOC) && lx->curLocaleBlob.base[0]) {
        strcat(lx->myURL, "_=");
        strcat(lx->myURL, lx->curLocaleBlob.base);
        strcat(lx->myURL, "&amp;");
    }
    if(!(o&kNO_DISPLOC) && lx->dispLocaleBlob.base[0]) {
        strcat(lx->myURL, "d_=");
        strcat(lx->myURL, lx->dispLocaleBlob.base);
        strcat(lx->myURL, "&amp;");
    }
    if(!(o&kNO_SECT) && lx->section[0]) {
        strcat(lx->myURL, "x=");
        strcat(lx->myURL, lx->section);
        strcat(lx->myURL, "&amp;");
    }
    if(!(o&kNO_LOC)) {
        if(!(o&kNO_COLL) && lx->curLocaleBlob.collation[0]) {
            strcat(lx->myURL, "collation=");
            strcat(lx->myURL, lx->curLocaleBlob.collation);
            strcat(lx->myURL, "&amp;");
        }
        if(!(o&kNO_CAL) && lx->curLocaleBlob.calendar[0]) {
            strcat(lx->myURL, "calendar=");
            strcat(lx->myURL, lx->curLocaleBlob.calendar);
            strcat(lx->myURL, "&amp;");
        }
        if(!(o&kNO_CURR) && lx->curLocaleBlob.currency[0]) {
            strcat(lx->myURL, "currency=");
            strcat(lx->myURL, lx->curLocaleBlob.currency);
            strcat(lx->myURL, "&amp;");
        }
    }
    /* Remove trailing &amp; */
    {
        char *lastAmp = NULL, *ampIter = lx->myURL;
        while (ampIter && (ampIter = strstr(ampIter, "&amp;"))) {
            lastAmp = ampIter;
            ampIter++;
        }
        if(lastAmp != NULL && strcmp(lastAmp, "&amp;") == 0) {
            *lastAmp=0;
        }
    }
    return lx->myURL;
}
