/**********************************************************************
 *   Copyright (C) 1999-2013, International Business Machines
 *   Corporation and others.  All Rights Reserved.
 ***********************************************************************/

/* Routines that show specific data types */


#include "unicode/uset.h"
#include "unicode/ucurr.h"
#include "unicode/putil.h"
#include "unicode/calendar.h"

using namespace icu; // TODO: fix namespace

#include "locexp.h"

/* Move Along.. nothing to see here.. */
U_CAPI UResourceBundle* U_EXPORT2
        ures_getByKeyWithFallback(const UResourceBundle *resB,
                                  const char* inKey,
                                  UResourceBundle *fillIn,
                                  UErrorCode *status);

/* Show a resource that's a collation rule list -----------------------------------------------------*/
/**
 * Show a string.  Make it progressive disclosure if it exceeds some length !
 * @param rb the resourcebundle to pull junk out of
 * @param locale the name of the locale (for URL generation)
 * @param key the key we're listing
 */
void showCollationElements( LXContext *lx, UResourceBundle *rb, const char *locale, const char *key )
{

    UErrorCode status = U_ZERO_ERROR;
    UErrorCode subStatus = U_ZERO_ERROR;
    const UChar *s  = 0;
    UChar temp[UCA_LEN]={'\0'};
    UChar *scopy = 0;
    UChar *comps = 0;
    UChar *compsBuf = 0;
    UBool bigString     = false; /* is it too big to show automatically? */
    UBool userRequested = false; /* Did the user request this string? */
    int32_t len = 0, len2, i;
    UCollator *coll = NULL; /* build an actual collator */
    UResourceBundle *array = NULL, *item = NULL;

    array = ures_getByKey(rb, key, array, &status);
    subStatus = status;

    s = ures_getStringByKey(array, "Sequence", &len, &subStatus);

    if(!s || !s[0] || (status == U_MISSING_RESOURCE_ERROR) || U_FAILURE(subStatus) )
    {

        /* Special case this to fetch what REALLY happens in this case ! */
        status = U_ZERO_ERROR;

        coll = ucol_open(locale, &status);

        if(U_SUCCESS(status))
        {
            if(strcmp(locale,"root")==0)
            {
                len = ucol_getRulesEx(coll,UCOL_FULL_RULES, temp,UCA_LEN);
                s=temp;
            }
            else
            {
                s = ucol_getRules(coll,&len);
            }

        }

        if( (s==0) || (*s == 0) )
        {
            /* FALLBACK - load from root */
            len = ucol_getRulesEx(coll,UCOL_FULL_RULES, temp,UCA_LEN);
            s=temp;
            status = U_USING_DEFAULT_WARNING;
        }
    }
    else
    {
        len = u_strlen(s);
    }

    len2 = len;

    scopy = (UChar*)malloc(len * sizeof(UChar));
    memcpy(scopy, s, len*sizeof(UChar));

    for(i=0;i<len;i++)
    {
        if(scopy[i] == 0x0000)
        {
            scopy[i] = 0x0020; /* normalizer seems to croak on nulls */
        }

    }
    s = scopy;

    if(U_SUCCESS(status) && ( len > kShowStringCutoffSize ) )
    {
        bigString = true;
        userRequested = didUserAskForKey(lx, key);
    }

    showKeyAndStartItemShort(lx, key, NULL, locale, false, status);

    u_fprintf(lx->OUT, "&nbsp;</td>\r\n</tr>\r\n<tr>\r\n<td></td>\r\n<td>");

    showExploreButtonSort(lx, rb,locale,  "CollationElements", true);

    u_fprintf(lx->OUT, "</td>\r\n"); /* Now, we're done with the ShowKey.. cell */

    u_fprintf(lx->OUT, "</tr>\r\n<tr>\r\n<td colspan=\"2\">");

    if(U_SUCCESS(status))
    {

        if(bigString && !userRequested) /* it's hidden. */
        {
            u_fprintf(lx->OUT, "<a href=\"?_=%s&amp;SHOW%s=1#%s\"><img border=\"0\" width=\"16\" height=\"16\" src=\"" LDATA_PATH "closed.gif\" alt=\"+\" />%S</a><br />\r\n<br />\r\n", locale, key,key, FSWF("bigStringClickToShow","(Omitted due to size. Click here to show.)"));
        }
        else
        {
            if(bigString)
            {
                u_fprintf(lx->OUT, "<a href=\"?_=%s#%s\"><img border=\"0\" width=\"16\" height=\"16\" src=\"" LDATA_PATH "opened.gif\" alt=\"-\" /> %S</a>\r\n",
                          locale,
                          key,
                          FSWF("bigStringHide", "Hide"));
            }

            if(U_SUCCESS(status))
            {

                compsBuf = (UChar*)malloc(sizeof(UChar) * (len*3));
                comps = compsBuf;

                {
                    for(i=0;i<(len*3);i++)
                    {
                        comps[i] = 0x0610;
                    }
                }

                len = unorm_normalize(s,
                                      len,
                                      UNORM_NFKC,
                                      0,
                                      comps,
                                      len*3,
                                      &status);


                /*              u_fprintf(lx->OUT, "xlit: %d to %d<P>\n",
                                len2,len); */
                if(U_FAILURE(status))
                {
                    free(compsBuf);
                    u_fprintf(lx->OUT, "xlit failed -} %s<br /><br />\n",
                              u_errorName(status));
                    comps = (UChar*)s;
                    compsBuf = comps;
                    len = len2;
                }

                /* DO NOT collect chars from the normalization rules.
                   Sorry, but they contain decomposed chars which mess up the codepage selection.. */
                /*
                  oldCallback = ucnv_getFromUCallBack((UConverter*)u_fgetConverter(lx->OUT));
                  if(oldCallback != UCNV_FROM_U_CALLBACK_TRANSLITERATED)
                  {
                  ucnv_setFromUCallBack((UConverter*)u_fgetConverter(lx->OUT), COLLECT_lastResortCallback, &status);
                  }
                */
                if(*comps == 0)
                {
                    u_fprintf(lx->OUT, "<i>%S</i>", FSWF("empty", "(Empty)"));
                }
                else while(len--)
                     {

                         if(*comps == '&')
                         {
                             u_fprintf(lx->OUT, "<br /><br />\r\n&amp;");
                         }
                         else if(*comps == '<')
                         {
                             if((comps != compsBuf) && (comps[-1] != '<'))
                             {
                                 /* don't break more than once. */
                                 u_fprintf(lx->OUT, "<br />&nbsp;");
                             }
                             u_fprintf(lx->OUT, "&lt;");
                         } else if(*comps == ']')
                         {
                             u_fprintf(lx->OUT, "]<br /><br />\r\n");
                         }
                         else
                         {
                             if((*comps == 0x000A) || u_isprint(*comps))
                                 u_fprintf(lx->OUT, "%C", *comps);
                             else
                                 u_fprintf(lx->OUT, "<b>\\u%04X</b>", *comps); /* to emulate the callback */
                         }

                         comps++;
                     };

                /* ucnv_setFromUCallBack((UConverter*)u_fgetConverter(lx->OUT), oldCallback, &status); */

            }
            else
                explainStatus(lx, status, key);
        }
    }

    status = U_ZERO_ERROR;
    s = ures_getStringByKey(array, "Version", &len, &status);
    if(U_SUCCESS(status))
    {
        u_fprintf(lx->OUT, "<p><b>%S %S:</b> %S</p>\r\n",
                  FSWF("Collation", "Collation"),
                  FSWF("Version","Version"),s);
    }

    free(scopy);
    if(coll)
        ucol_close(coll);

    showKeyAndEndItem(lx, key, locale);
    ures_close(array);
    ures_close(item);
}

/* These aren't resources anymore. */
void showLocaleCodes(LXContext *lx,  UResourceBundle *rb, const char *locale)
{

    UErrorCode status = U_ZERO_ERROR;
    char tempctry[1000], templang[1000], tempvar[1000];
    const char *ctry3 = NULL, *lang3 = NULL;

    showKeyAndStartItem(lx, "LocaleCodes", FSWF("LocaleCodes", "Locale Codes"), locale, false, status);

    u_fprintf(lx->OUT, "<table class='data-table-1' summary=\"%S\">\r\n<tr><td></td><td><b>%S</b></td><td><b>%S</b></td><td><b>%S</b></td></tr>\r\n",
              FSWF("LocaleCodes", "Locale Codes"),
              FSWF("LocaleCodes_Language", "Language"),
              FSWF("LocaleCodes_Country", "Region"),
              FSWF("LocaleCodes_Variant", "Variant"));
    u_fprintf(lx->OUT, "<tr><td></td><td>");

    status = U_ZERO_ERROR;
    uloc_getLanguage(locale, templang, 1000, &status);
    if(U_SUCCESS(status))
    {
        u_fprintf(lx->OUT, templang);
    }
    else
    {
        explainStatus(lx, status, "LocaleCodes");
        templang[0] = 0;
    }

    u_fprintf(lx->OUT, "</td><td>");

    status = U_ZERO_ERROR;
    uloc_getCountry(locale, tempctry, 1000, &status);
    if(U_SUCCESS(status))
    {
        u_fprintf(lx->OUT, tempctry);
    }
    else
    {
        explainStatus(lx, status, "LocaleCodes");
        tempctry[0] = 0;
    }

    u_fprintf(lx->OUT, "</td><td>");

    status = U_ZERO_ERROR;
    uloc_getVariant(locale, tempvar, 1000, &status);
    if(U_SUCCESS(status))
        u_fprintf(lx->OUT, tempvar);
    else
        explainStatus(lx, status, "LocaleCodes");

    u_fprintf(lx->OUT, "</td></tr>\r\n");

    /* 3 letter line */

    u_fprintf(lx->OUT, "<tr><td>%S</td>",
              FSWF("LocaleCodes_3", "3"));

    u_fprintf(lx->OUT, "<td>");

    lang3 = uloc_getISO3Language(locale);
    if(lang3)
    {
        u_fprintf(lx->OUT, "%s", lang3);
    }

    u_fprintf(lx->OUT, "</td><td>");

    ctry3 = uloc_getISO3Country(locale);
    if(ctry3)
    {
        u_fprintf(lx->OUT, "%s", ctry3);
    }

    u_fprintf(lx->OUT, "</td><td></td></tr>\r\n");

    u_fprintf(lx->OUT, "</table>\r\n");

    showKeyAndEndItem(lx, "LocaleCodes", locale);  /* End of LocaleCode's sub item */

}

/* -------------- show script for locale --------------*/
void showLocaleScript(LXContext *lx, UResourceBundle *rb, const char *locale)
{

    UErrorCode status = U_ZERO_ERROR;

    UScriptCode  list[32];
    int32_t len, i;

    len = uscript_getCode(locale, list, sizeof(list)/sizeof(list[0]), &status);

    showKeyAndStartItem(lx, "LocaleScript", FSWF("LocaleScript", "Locale Script"), locale, false, status);

    u_fprintf(lx->OUT, "<table summary=\"%S\">\r\n",
              FSWF("LocaleScript", "Locale Script"));
    u_fprintf(lx->OUT, "<tr><td><b>%S</b></td><td><b>%S</b></td></tr>\r\n",
              FSWF("LocaleScriptAbbreviations", "Short Names"),
              FSWF("LocaleScriptNames", "Long Names")
              );

    for(i=0;i<len;i++)
    {
        u_fprintf(lx->OUT, "   <tr><td>%s</td><td>%s</td></tr>\r\n",
                  uscript_getShortName(list[i]), uscript_getName(list[i]));
    }
    u_fprintf(lx->OUT, "</table>\r\n");

    showKeyAndEndItem(lx, "LocaleScript", locale);
}

/* Show a resource that's a simple integer -----------------------------------------------------*/
/**
 * Show an integer
 * @param rb the resourcebundle to pull junk out of
 * @param locale the name of the locale (for URL generation)
 * @param radix base of number to display (Only 10 and 16 are supported)
 * @param key the key we're listing
 */

void showInteger( LXContext *lx, UResourceBundle *rb, const char *locale, const char *key, int radix)
{

    UErrorCode status = U_ZERO_ERROR;
    UResourceBundle *res = NULL;
    int32_t i;

    res = ures_getByKey(rb, key, res, &status);
    i = ures_getInt(res, &status);
    showKeyAndStartItem(lx, key, NULL, locale, false, status);

    if(U_SUCCESS(status))
    {
        if(radix == 10) {
            u_fprintf(lx->OUT, "%d", i);
        } else if(radix == 16) {
            u_fprintf(lx->OUT, "0x%X", i);
        } else {
            u_fprintf(lx->OUT, "(Unknown radix %d for %d)", radix, i);
        }
    }
    showKeyAndEndItem(lx, key, locale);
}

/* Show a resource that's a simple string -----------------------------------------------------*/
/**
 * Show a string.  Make it progressive disclosure if it exceeds some length !
 * @param rb the resourcebundle to pull junk out of
 * @param locale the name of the locale (for URL generation)
 * @param key the key we're listing
 */

void showString( LXContext *lx, UResourceBundle *rb, const char *locale, const char *key, UBool PRE )
{

    UErrorCode status = U_ZERO_ERROR;
    const UChar *s  = 0;
    UBool bigString = false; /* is it big? */
    UBool userRequested = false; /* Did the user request this string? */
    int32_t len;

    s = ures_getStringByKey(rb, key, &len, &status);

    if(U_SUCCESS(status) && ( u_strlen(s) > kShowStringCutoffSize ) )
    {
        bigString = true;
        userRequested = didUserAskForKey(lx, key);
    }

    showKeyAndStartItem(lx, key, NULL, locale, false, status);

    if(U_SUCCESS(status))
    {

        if(bigString && !userRequested) /* it's hidden. */
        {
            u_fprintf(lx->OUT, "<a href=\"?_=%s&amp;SHOW%s=1#%s\"><img border=\"0\" width=\"16\" height=\"16\" src=\"" LDATA_PATH "closed.gif\" alt=\"+\" />%S</a><br />\r\n<br />\r\n", locale, key,key, FSWF("bigStringClickToShow","(Omitted due to size. Click here to show.)"));
        }
        else
        {
            if(bigString)
            {
                u_fprintf(lx->OUT, "<a href=\"?_=%s#%s\"><img border=\"0\" width=\"16\" height=\"16\" src=\"" LDATA_PATH "opened.gif\" alt=\"-\" /> %S</a>\r\n",
                          locale,
                          key,
                          FSWF("bigStringHide", "Hide"));
            }

            if(U_SUCCESS(status))
            {
                if(PRE)
                    u_fprintf(lx->OUT, "<pre>");

                if(*s == 0)
                    u_fprintf(lx->OUT, "<i>%S</i>", FSWF("empty", "(Empty)"));
                {
                    writeEscaped(lx, s);
                }

                if(PRE)
                    u_fprintf(lx->OUT, "</pre>");
            }
        }
    }
    showKeyAndEndItem(lx, key, locale);
}

/* Show a resource that's a UnicodeSet -----------------------------------------------------*/
/**
 * Show a string.  Make it progressive disclosure if it exceeds some length !
 * @param rb the resourcebundle to pull junk out of
 * @param locale the name of the locale (for URL generation)
 * @param key the key we're listing
 */

void showUnicodeSet( LXContext *lx, UResourceBundle *rb, const char *locale, const char *key, UBool PRE )
{

    UErrorCode status = U_ZERO_ERROR;
    const UChar *s  = 0;
    UChar smallString[ kShowUnicodeSetCutoffSize + 1];
    UBool bigString = false; /* is it big? */
    UBool userRequested = false; /* Did the user request this string? */
    int32_t setLen = 0, rulesLen = 0, len;
    int32_t i;
    USet *uset;
    UChar *buf  = NULL;
    int32_t bufSize = 0;
    int32_t howManyChars = 0;

    bufSize = 512;
    buf = (UChar*)malloc(bufSize * sizeof(UChar));

    s = ures_getStringByKey(rb, key, &rulesLen, &status);


    uset = uset_openPattern(s, rulesLen, &status);

    showKeyAndStartItem(lx, key, NULL, locale, false, status);

    if(U_FAILURE(status))
    {
        showKeyAndEndItem(lx, key, locale);
        return;
    }

#if defined (LX_UBROWSE_PATH)
    u_fprintf(lx->OUT, "<form method=GET action=\"%s\">\n", LX_UBROWSE_PATH);
    u_fprintf(lx->OUT, "<input type=\"hidden\" name=GO /><input type=\"hidden\" name=us value=\"%S\" /><input type=\"hidden\" name=gosetn value=\"\" />\n", s);
    u_fprintf(lx->OUT, "<input type=IMAGE width=\"48\" height=\"20\" border=\"0\" src=\"" LDATA_PATH_LOC "explore.gif\"  align=right   ", lx->dispLocale);
    u_fprintf(lx->OUT, " value=\"%S\"></form>",
              FSWF("exploreTitle", "Explore"));
    u_fprintf(lx->OUT, "</form>");
#endif
    setLen = uset_size(uset);

    if( (rulesLen > kShowUnicodeSetCutoffSize ) ||
        (setLen > kShowUnicodeSetCutoffSize) )
    {
        userRequested = didUserAskForKey(lx, key);
        bigString = true;

        if(userRequested) /* it's not hidden. */
        {
            u_fprintf(lx->OUT, "<a href=\"?_=%s#%s\"><img border=\"0\" width=\"16\" height=\"16\" src=\"" LDATA_PATH "opened.gif\" alt=\"-\" /> %S</a>\r\n",
                      locale,
                      key,
                      FSWF("bigStringShorten", "Don't show all"));
        }
    }

    /** RULES **/
    u_fprintf(lx->OUT, "<table border=\"1\" summary=\"Rules\" cellpadding=\"3\" cellspacing=\"3\"><tr><td><b>%S</b></td><td>\r\n",
              FSWF("rules", "Rules"));

    /* Always do this loop */

    if( (rulesLen > kShowUnicodeSetCutoffSize) && !userRequested )
    {
        /* shorten string */
        u_strncpy(smallString, s, kShowUnicodeSetCutoffSize);
        smallString[kShowUnicodeSetCutoffSize] = 0;
        s = smallString;
        len = kShowUnicodeSetCutoffSize;
    }
    else
    {
        len = rulesLen;
    }

    u_fprintf_u(lx->OUT, s);

    if(len != rulesLen)
    {
        u_fprintf(lx->OUT, "%S", FSWF("...", "..."));
        u_fprintf(lx->OUT, "<br /><a href=\"?_=%s&amp;SHOW%s=1#%s\"><img border=\"0\" width=\"16\" height=\"16\" src=\"" LDATA_PATH "closed.gif\" alt=\"+\" /><i>%S</i> ", locale, key,key, FSWF("bigStringClickToExpand","Truncated due to size. Click here to show. "));
        u_fprintf_u(lx->OUT, FSWF("bigStringSize", "(%d of %d shown)"), len, rulesLen);
        u_fprintf(lx->OUT, " </a>\r\n");
    }

    u_fprintf(lx->OUT, "</td></tr>\r\n");

    /** Set **/

    u_fprintf(lx->OUT, "<tr><td><b>%S</b></td><td>\r\n",
              FSWF("set", "Set"));

    for(i=0;i<uset_getItemCount(uset);i++)
    {
        UErrorCode subErr = U_ZERO_ERROR;
        UChar32 start;
        UChar32 end;
        int32_t n;

        n = uset_getItem(uset, i, &start,&end, buf, bufSize, &subErr);
        if(subErr == U_BUFFER_OVERFLOW_ERROR)
        {
            free(buf);
            bufSize = (n+4);
            buf = (UChar*)realloc(buf, sizeof(UChar)*bufSize);
            subErr = U_ZERO_ERROR;
            n = uset_getItem(uset, i, &start,&end, buf, bufSize, &subErr);
        }
        if(U_FAILURE(subErr))
        {
            u_fprintf(lx->OUT, "<B>Fatal: err in showSet, %s @ index %d\n", u_errorName(subErr), i);
            u_fprintf(lx->OUT, "</td></tr></table><hr />\r\n");
            return;
        }
        if(n == 0) /* range */
        {
            int32_t thisRangeLen;
            UChar32 c;

            thisRangeLen =  (end-start)+1;

            if(!userRequested)
            {

                if((howManyChars < kShowUnicodeSetCutoffSize) &&
                   (howManyChars+(end-start)>kShowUnicodeSetCutoffSize))
                {
                    /* truncate */
                    end = start+(kShowUnicodeSetCutoffSize-howManyChars);
                }
            }

            if(userRequested || (howManyChars <= kShowUnicodeSetCutoffSize))
            {
                for(c = start; c<=end; c++)
                {
                    u_fprintf(lx->OUT, "%C", c);
                }
            }
            howManyChars += thisRangeLen;
        }
        else
        {
            if(i != 0)
            {
                u_fprintf(lx->OUT, ", ");
            }
            buf[n]=0;
            u_fprintf(lx->OUT, "\"%S\"", buf);
        }
    }

    if(!userRequested && (howManyChars >= kShowUnicodeSetCutoffSize))
    {
        u_fprintf(lx->OUT, "%S", FSWF("...", "..."));
        u_fprintf(lx->OUT, "<br /><a href=\"?_=%s&amp;SHOW%s=1#%s\"><img border=\"0\" width=\"16\" height=\"16\" src=\"" LDATA_PATH "closed.gif\" alt=\"+\" /><i>%S</i> ", locale, key,key, FSWF("bigStringClickToExpand","Truncated due to size. Click here to show. "));
        u_fprintf_u(lx->OUT, FSWF("bigStringSize", "(%d of %d shown)"), kShowUnicodeSetCutoffSize, howManyChars);
        u_fprintf(lx->OUT, " </a>\r\n");
    }

    u_fprintf(lx->OUT, "</td></tr></table>\r\n");

    showKeyAndEndItem(lx, key, locale);
    free(buf);
    uset_close(uset);
}

/* Show a resource that's a simple string, but explain each character.-----------------------------------------------------*/
/**
 * Show a string.  Make it progressive disclosure if it exceeds some length !
 * @param rb the resourcebundle to pull junk out of
 * @param locale the name of the locale (for URL generation)
 * @param desc array (0 at last item) of char desc
 * @param key the key we're listing
 */

void showStringWithDescription( LXContext *lx, UResourceBundle *rb, const char *locale, const UChar *desc[], const char *key, UBool hidable)
{

    UErrorCode status = U_ZERO_ERROR;
    const UChar *s  = 0;
    UBool bigString = false; /* is it big? */
    UBool userRequested = false; /* Did the user request this string? */
    int32_t i;
    int32_t len;

    s = ures_getStringByKey(rb, key, &len, &status);

    /* we'll assume it's always big, for now. */
    bigString = true;
    userRequested = didUserAskForKey(lx, key);

    showKeyAndStartItem(lx, key, NULL, locale, false, status);

    /** DON'T show the string as a string. */
    /*
      if(U_SUCCESS(status) && s)
      u_fprintf(lx->OUT, "%S<br />\r\n", s);
    */
    if(!hidable)
    {
        userRequested = true;
        bigString = false;
    }


    if(bigString && !userRequested) /* it's hidden. */
    {
        u_fprintf(lx->OUT, "<a href=\"?_=%s&amp;SHOW%s=1#%s\"><img border=\"0\" width=\"16\" height=\"16\" src=\"" LDATA_PATH "closed.gif\" alt=\"+\" />%S</a><br />\r\n", locale, key,key, FSWF("stringClickToShow","(Click here to show.)"));
    }
    else
    {
        if(bigString)
        {
            u_fprintf(lx->OUT, "<a href=\"?_=%s#%s\"><img border=\"0\" width=\"16\" height=\"16\" src=\"" LDATA_PATH "opened.gif\" alt=\"-\" /> %S</a>\r\n",
                      locale,
                      key,
                      FSWF("bigStringHide", "Hide"));
        }

        if(U_SUCCESS(status))
        {
            u_fprintf(lx->OUT, "<table summary=\"String\" width=\"100%%\">");
            u_fprintf(lx->OUT, "<tr><td><b>%S</b></td><td><b>%S</b></td><td><b>%S</b></td></tr>\r\n",
                      FSWF("charNum", "#"),
                      FSWF("char", "Char"),
                      FSWF("charMeaning", "Meaning"));


            for(i=0;desc[i];i++)
            {
                if(!s[i])
                    break;

                u_fprintf(lx->OUT, "<tr><td width=\"5\">%d</td><td>%C</td><td>%S</td></tr>\r\n",
                          i,
                          s[i],
                          desc[i]);
            }
            u_fprintf(lx->OUT, "</table>\r\n");
        }
    }
    u_fprintf(lx->OUT, "<br />\r\n");
    showKeyAndEndItem(lx, key, locale);
}

/* Show a resource that's an array. Useful for types we haven't written viewers for yet --------*/

void showArray( LXContext *lx, UResourceBundle *rb, const char *locale, const char *key, ECal isCal )
{
    UErrorCode status = U_ZERO_ERROR;
    UErrorCode firstStatus = U_ZERO_ERROR;
    UResourceBundle  *array = NULL, *item = NULL;
    int32_t len;
    const UChar *s  = 0;
    int i;
    /*const char *realKey;*/
    char   key2[1024];
    UBool userRequested = false;
    UBool isDefault = false;

    userRequested = didUserAskForKey(lx, key);
    strcpy(key2, key);
    if(isCal == kCal) {
        key2[0]=toupper(key2[0]); /* upcase for compatibility */
        array = loadCalRes(lx, key, &isDefault, &firstStatus);
    }

    if(array == NULL ) {
        array = ures_getByKey(rb, key, array, &firstStatus);
    }

    item = ures_getByIndex(array, 0, item, &firstStatus);

    showKeyAndStartItem(lx, key2, NULL, locale, false, firstStatus);

    /*    if(realKey == key2) {
          u_fprintf(lx->OUT, "(%s)<br />\r\n", lx->defaultCalendar);
          }*/

    u_fprintf(lx->OUT, "<ol>\r\n");

    for(i=0;;i++)
    {
        status = U_ZERO_ERROR;
        if(U_FAILURE(firstStatus)) {
            status = firstStatus; /* ** todo: clean up err handling! */
        }

        if((i > 10) && !userRequested) {
            u_fprintf(lx->OUT, "<li><a href=\"?_=%s&amp;SHOW%s=1#%s\"><img border=\"0\" width=\"16\" height=\"16\" src=\"" LDATA_PATH "closed.gif\" alt=\"+\" />%S</a></li>\r\n",
                      locale, key,key, FSWF("bigStringClickToShow","(Omitted due to size. Click here to show.)"));
            break;
        }

        item = ures_getByIndex(array, i, item, &status);
        s  = ures_getString(item, &len, &status);

        if(U_SUCCESS(status) && s)
            u_fprintf(lx->OUT, "<li>%S</li>\r\n", s);
        else
        {
            u_fprintf(lx->OUT, "<li>");
            explainStatus(lx, status, key);
            u_fprintf(lx->OUT, "</li>\r\n");
            break;
        }
    }
    u_fprintf(lx->OUT, "</ol>");
    if((i>=10) && userRequested) {
        u_fprintf(lx->OUT, "<a href=\"?_=%s#%s\"><img border=\"0\" width=\"16\" height=\"16\" src=\"" LDATA_PATH "opened.gif\" alt=\"-\" /> %S</a>\r\n",
                  locale,
                  key,
                  FSWF("bigStringHide", "Hide"));
    }
    if(isDefault) {
        calPrintDefaultWarning(lx);
    }
    u_fprintf(lx->OUT, "<!-- %s:%d -->\r\n", __FILE__, __LINE__);
    showKeyAndEndItem(lx, key, locale);
    u_fprintf(lx->OUT, "<!-- %s:%d -->\r\n", __FILE__, __LINE__);

    if(U_SUCCESS(status)) {
        ures_close(item);
        ures_close(array);
    }
}

/* Show a resource that's an array, wiht an explanation ------------------------------- */

void showArrayWithDescription( LXContext *lx, UResourceBundle *rb, const char *locale, const UChar *desc[], const char *key, ECal isCal )
{
    UErrorCode status = U_ZERO_ERROR;
    UErrorCode firstStatus;
    UResourceBundle *subitem = NULL;
    const UChar *s  = 0;
    const UChar *toShow =0;
    UChar nothing[] = {(UChar)0x00B3, (UChar)0x0000};
    UResourceBundle  *array = NULL, *item = NULL;
    int32_t len;

    enum { kNoExample = 0, kDateTimeExample, kNumberExample } exampleType;
    int32_t i;
    UDate now;  /* example date */
    double d = 1234.567;   /* example number */
    UDateFormat   *exampleDF = 0;
    UNumberFormat *exampleNF = 0;
    UErrorCode exampleStatus = U_ZERO_ERROR;
    UChar tempChars[1024];
    UChar tempDate[1024]; /* for Date-Time */
    UChar tempTime[1024]; /* for Date-Time */
    /*const char *realKey;
      char   key2[1024];*/
    UBool isDefault = false;
    /* figure out what example to use */
    if(!strcmp(key,"DateTimePatterns"))
        exampleType = kDateTimeExample;
    else if(!strcmp(key, "NumberPatterns"))
        exampleType = kNumberExample;
    else
        exampleType = kNoExample;

    tempDate[0]=0;
    tempTime[0]=0;

    /* store the date now..just in case formatting takes multiple seconds! */
    now = ucal_getNow();

    firstStatus = U_ZERO_ERROR;

    if( isCal == kCal) {
        array = loadCalRes(lx, key, &isDefault, &firstStatus);
    }

    if(array == NULL ) {
        array = ures_getByKey(rb, key, array, &firstStatus);
    }

    item = ures_getByIndex(array, 0, item, &firstStatus);
    s = ures_getString(item, &len, &firstStatus);
    showKeyAndStartItemShort(lx, key, NULL, locale, false, firstStatus);

#if 0
    if(realKey == key2) {
        u_fprintf(lx->OUT, "(%s)<br />\r\n", lx->defaultCalendar);
    }
#endif

    if(exampleType != kNoExample)
    {
        toShow = nothing+1;

        exampleStatus = U_ZERO_ERROR;

        switch(exampleType)
        {

            case kDateTimeExample:
                exampleStatus = U_ZERO_ERROR;
#if 1
                toShow = s;
                /* don't localize the pattern */
#else
                exampleDF = udat_open(UDAT_IGNORE,UDAT_IGNORE,locale,NULL, 0, s,-1,&exampleStatus);
                if(U_SUCCESS(exampleStatus))
                {
                    len = udat_toPattern(exampleDF, true, tempChars, 1024,&exampleStatus);
                    if(U_SUCCESS(exampleStatus))
                    {
                        toShow = tempChars;
                    }
                    unum_close(exampleDF);
                    exampleDF = NULL;
                }
#endif
                break;

            case kNumberExample:

#if 1
                toShow= s;
                /* don't localize */
#else
                toShow = nothing;
                exampleNF = unum_open(0, s,-1,locale,NULL, &exampleStatus);
                if(U_SUCCESS(exampleStatus))
                {
                    len = unum_toPattern(exampleNF, true, tempChars, 1024, &exampleStatus);
                    if(U_SUCCESS(exampleStatus))
                    {
                        toShow = tempChars;
                    }
                    unum_close(exampleNF);
                    exampleNF = NULL;
                }
#endif
                break;
            default:
                ;
        }
        exampleStatus = U_ZERO_ERROR;
        showExploreButton(lx, rb, locale, toShow, key);
    }
    else
    {
        u_fprintf(lx->OUT, "&nbsp;");
    }

#ifdef LX_USE_CURRENCY
    /* Currency Converter link */
    if(!strcmp(key, "CurrencyElements"))
    {
        UErrorCode curStatus = U_ZERO_ERROR;
        UChar *curStr = NULL, *homeStr = NULL;

        /* index [1] is the int'l currency symbol */
        item = ures_getByIndex(array, 1, item, &curStatus);
        curStr  = ures_getString(item, &len, &curStatus);
        if(lx->defaultRB)
        {
            item = ures_getByKey(lx->defaultRB, key, item, &curStatus);
            item = ures_getByIndex(item, 1, item, &curStatus);
            curStr  = ures_getString(item, &len, &curStatus);

            /* homeStr = ures_getArrayItem(lx->defaultRB, key, 1, &curStatus); */
        }
        else
            homeStr = (const UChar[]){0x0000};

        /* OANDA doesn't quite follow the same conventions for currency.

           TODO:

           RUR->RUB
           ...
        */


        u_fprintf(lx->OUT, "<form target=\"_currency\" method=\"post\" action=\"http:www.oanda.com/converter/travel\" ENCTYPE=\"x-www-form-encoded\"><input type=\"hidden\" name=\"result\" value=\"1\" /><input type=\"hidden\" name=\"lang\" value=\"%s\" /><input type=\"hidden\" name=\"date_fmt\" value=\"us\" /><input name=\"exch\" type=\"hidden\" value=\"%S\"><input type=\"hidden\" name=\"expr\" value=\"%S\" />",
                  "en", /* lx->dispLocale */
                  curStr,
                  homeStr
                  );

        u_fprintf(lx->OUT, "<input type=IMAGE width=\"48\" height=\"20\" border=\"0\" src=\"" LDATA_PATH_LOC "explore.gif\" align=\"right\" ", lx->dispLocale);
        u_fprintf(lx->OUT, " value=\"%S\"></form>",
                  FSWF("exploreTitle", "Explore"));
        u_fprintf(lx->OUT, "</form>");
    }
#endif
    u_fprintf(lx->OUT, "</td>"); /* Now, we're done with the ShowKey.. cell */

    u_fprintf(lx->OUT, "</tr><tr><td colspan=\"2\"><table class='data-table-2' width=\"100%%\">\r\n");

    for(i=0;desc[i];i++)
    {

        UResType type;

        u_fprintf(lx->OUT, "<tr>\r\n<th>%d</th><td>%S</td><td>",
                  i, desc[i]);

        status = U_ZERO_ERROR;
        exampleStatus = U_ZERO_ERROR;

        item = ures_getByIndex(array, i, item, &status);
        type = ures_getType(item);
        if(type==URES_STRING) {
            s =    ures_getString(item, &len, &status);
        } else if(type==URES_ARRAY) {
            subitem = ures_getByIndex(item, 0, subitem, &status);
            s = ures_getString(subitem, &len, &status);
        } else {
            u_fprintf(lx->OUT, "[Unknown type:%d]</td></tr>", type);
            continue;
        }

        if(i==0)
            firstStatus = status;

        if(U_SUCCESS(status) && s)
        {
            toShow = (UChar*) s;

            switch(exampleType)
            {
                case kDateTimeExample: /* localize pattern.. */
                    if(i < 8)
                    {
                        len = 0;
                        exampleDF = udat_open(UDAT_LONG, UDAT_LONG, locale,NULL, 0, s,-1,&exampleStatus);
                        udat_applyPattern(exampleDF, false, s, -1);
#if 1
                        toShow = s;
#else
                        if(U_SUCCESS(exampleStatus))
                        {
                            len = udat_toPattern(exampleDF, true, tempChars, 1024,&exampleStatus);

                            if(U_SUCCESS(exampleStatus))
                            {
                                toShow = tempChars;
                            }
                        }
#endif
                    }
                    break;

                case kNumberExample:
                    if(i == 3) /* scientific */
                        d = 1234567890;

                    exampleNF = unum_open((UNumberFormatStyle)0, s,-1,locale, NULL, &exampleStatus);
#if 1
                    u_strcpy(tempChars,s);
                    toShow = tempChars;
#else
                    if(U_SUCCESS(exampleStatus))
                    {
                        len = unum_toPattern(exampleNF, true, tempChars, 1024, &exampleStatus);
                        if(U_SUCCESS(exampleStatus))
                        {
                            toShow = tempChars;
                        }
                    }
#endif
                    break;

                default:
                    ;
            }

            u_fprintf(lx->OUT, "%S", toShow);
        }
        else
        {
            s = 0;
            explainStatus(lx, status, key);
            u_fprintf(lx->OUT, "\r\n");
            break;
        }
        if(type==URES_ARRAY) {
            int i;
            int sz = ures_getSize(item);
            for(i=1;i<sz;i++) {
                const UChar* s2;
                subitem = ures_getByIndex(item, i, subitem, &status);
                s2 = ures_getString(subitem, &len, &status);
                if(U_SUCCESS(status)) {
                    u_fprintf(lx->OUT, "<br><font size='-1'>%d: %S</font>", i+1, s);
                } else { u_fprintf(lx->OUT, "<br>%d: <b>err %s</b>", i+1, u_errorName(status));
                    sz=0; /* stop looping */ }
            }
        }
        u_fprintf(lx->OUT, "</td>");

        if(s) /* only if pattern exists */
            switch(exampleType)
            {
                case kDateTimeExample:
                    if(i < 8)
                    {
                        u_fprintf(lx->OUT, "<td>");

                        if(U_SUCCESS(exampleStatus))
                        {
                            exampleStatus = U_ZERO_ERROR; /* clear fallback info from exampleDF */
                            udat_format(exampleDF, now, tempChars, 1024, NULL, &exampleStatus);
                            udat_close(exampleDF);

                            if(U_SUCCESS(exampleStatus))
                                u_fprintf(lx->OUT, "%S", tempChars);

                        }
                        explainStatus(lx, exampleStatus, key);
                        u_fprintf(lx->OUT, "</td>\r\n");

                        if(U_SUCCESS(exampleStatus)) {
                            if(i == 3) /* short time */
                                u_strcpy(tempTime, tempChars);
                            else if(i == 7) /* short date */
                                u_strcpy(tempDate, tempChars);
                        }
                    }
                    else
                    {
                        u_fprintf(lx->OUT, "<td>");
                        exampleStatus = U_ZERO_ERROR;
                        if(s) {
                            if(u_formatMessage(locale, s, -1, tempChars,1024,&exampleStatus,
                                               tempTime,
                                               tempDate)) {
                                if(U_SUCCESS(exampleStatus)) {
                                    u_fprintf(lx->OUT,"%S", tempChars);
                                } else {
                                    explainStatus(lx, exampleStatus, key);
                                }
                            }
                        }
                        u_fprintf(lx->OUT, "</td>\r\n");
                    }
                    break;

                case kNumberExample:
                    {
                        u_fprintf(lx->OUT, "<td>");

                        if(U_SUCCESS(exampleStatus))
                        {
                            exampleStatus = U_ZERO_ERROR; /* clear fallback info from exampleDF */

                            if(i == 3) /* scientific */
                                d = 1234567890;
                            unum_formatDouble(exampleNF, d, tempChars, 1024, NULL, &exampleStatus);

                            if(U_SUCCESS(exampleStatus))
                                u_fprintf(lx->OUT, "%S", tempChars);


                            u_fprintf(lx->OUT, "</td><td>");

                            if(i == 3) /* scientific */
                                d = 0.00000000000000000005;

                            unum_formatDouble(exampleNF, -d, tempChars, 1024, NULL, &exampleStatus);

                            if(U_SUCCESS(exampleStatus))
                                u_fprintf(lx->OUT, "%S", tempChars);

                            unum_close(exampleNF);

                        }
                        explainStatus(lx, exampleStatus, key);
                        u_fprintf(lx->OUT, "</td>\r\n");
                    }
                    break;

                case kNoExample:
                default:
                    break;
            }

        u_fprintf(lx->OUT, "</tr>\r\n");

    }


    u_fprintf(lx->OUT, "</table>");

    if(isDefault) {
        calPrintDefaultWarning(lx);
    }

    showKeyAndEndItem(lx, key, locale);
    ures_close(subitem);
    ures_close(item);
    ures_close(array);
}

void showSpelloutExample( LXContext *lx, UResourceBundle *rb, const char *locale)
{
    UErrorCode status;
    double examples[] = { 0, 123.45, 67890 };
    UNumberFormat *exampleNF = 0;
    UNumberFormatStyle styles[] = { UNUM_SPELLOUT
#if defined (UNUM_ORDINAL)
        , UNUM_ORDINAL, UNUM_DURATION
#endif
    };
    const char *stylen[] = { "spellout", "ordinal", "duration" };
    int n, k;
    const char *key = "SpelloutRulesExample";
    UChar tempChars[245];

    status = U_ZERO_ERROR;
    exampleNF = unum_open(UNUM_SPELLOUT,NULL, -1, locale, NULL, &status);

    showKeyAndStartItem(lx, key, NULL, locale, false, status);
    if(exampleNF) unum_close(exampleNF);

    u_fprintf(lx->OUT, "<table width=\"100%%\">\r\n");

    for(k=0;k<sizeof(styles)/sizeof(styles[0]);k++) {
        u_fprintf(lx->OUT, "<tr><td colspan=\"2\"><b>%s</b></td></tr>\r\n", stylen[k]);
        for(n=0;n<(sizeof(examples)/sizeof(examples[0]));n++) {
            status = U_ZERO_ERROR;
            tempChars[0] = 0;
            exampleNF = unum_open(styles[k],NULL, -1, locale, NULL, &status);
            unum_formatDouble(exampleNF, examples[n], tempChars, 1024,0, &status);
            u_fprintf(lx->OUT, "<tr><td>%f</td><td>%S", examples[n], tempChars);
            unum_close(exampleNF);
            if(U_FAILURE(status)) {
                u_fprintf(lx->OUT, " ");
                explainStatus(lx, status, NULL);
            }
            u_fprintf(lx->OUT, "</td></tr>\r\n");
        }
    }

    u_fprintf(lx->OUT, "</table>");
    showKeyAndEndItem(lx, key, locale);
}

/* show the DateTimeElements string *------------------------------------------------------*/

void showDateTimeElements( LXContext *lx, UResourceBundle *rb, const char *locale)
{
    UErrorCode status = U_ZERO_ERROR;
    const UChar *s  = 0;
    int32_t    len;
    const int32_t   *elements;
    UBool isDefault = false;

    const char *key = "DateTimeElements";
    /*
      0: first day of the week
      1: minimaldaysinfirstweek
    */

    UResourceBundle *item = NULL;

    status = U_ZERO_ERROR;

    showKeyAndStartItem(lx, key, FSWF("DateTimeElements","Date and Time Options"), locale, false, status);


    char id[99];
    strcpy(id,locale);
    strcat(id,"@calendar=");
    strcat(id,lx->defaultCalendar);
    Calendar *c = Calendar::createInstance(Locale(id),status);
    if(c&&U_SUCCESS(status)) {
        u_fprintf(lx->OUT, "%S ", FSWF("DateTimeElements0", "First day of the week: "));


        if(U_SUCCESS(status))
        {
            int32_t  firstDayIndex;

            firstDayIndex = (((c->getFirstDayOfWeek())+6)%7);

            u_fprintf(lx->OUT, " %d \r\n", c->getFirstDayOfWeek());
            /* here's something fun: try to fetch that day from the user's current locale */
            status = U_ZERO_ERROR;

            if(lx->dispRB)
            {
                /* don't use 'array' here because it's the DTE resource */
                item = ures_getByKey(lx->dispRB, "DayNames", item, &status);
                item = ures_getByIndex(item, firstDayIndex, item, &status);
                s    = ures_getString(item, &len, &status);

                if(s && U_SUCCESS(status))
                {
                    u_fprintf(lx->OUT, " = %S \r\n", s);
                } else {
                    u_fprintf(lx->OUT, " ( %d=SUNDAY )\n", UCAL_SUNDAY);
                }
                status = U_ZERO_ERROR;

                item = ures_getByKey(rb, "DayNames", item, &status);
                item = ures_getByIndex(item, firstDayIndex, item, &status);
                s    = ures_getString(item, &len, &status);

                if(s && U_SUCCESS(status))
                {
                    u_fprintf(lx->OUT, " = %S \r\n", s);
                }
            }
            status = U_ZERO_ERROR;
        }
        else
        {
            explainStatus(lx, status, key);
            u_fprintf(lx->OUT, "\r\n");
        }

        u_fprintf(lx->OUT, "<br />\r\n");

        /* minimal days in week ================= */
        u_fprintf(lx->OUT, "%S", FSWF("DateTimeElements1", "Minimal Days in First Week: "));

        if(U_SUCCESS(status)) {
            u_fprintf(lx->OUT, " %d \r\n", c->getMinimalDaysInFirstWeek());
        } else {
            explainStatus(lx, status, key);
            u_fprintf(lx->OUT, "\r\n");
        }

        delete c;

    }
    ures_close(item);

    showKeyAndEndItem(lx, key, locale);
}

void showShortLongCal( LXContext *lx, UResourceBundle *rb, const char *locale, const char *keyStem)
{
    char aKeyStem[400];
    char *q;
    strcpy(aKeyStem, keyStem);
    aKeyStem[0]=toupper(aKeyStem[0]);
    if((q = strstr(aKeyStem, "Names"))) {
        *q = 0;
    }
    /* dayNames -> Day,  monthNames -> Month
       for legacy translations */
    showKeyAndStartItem(lx, aKeyStem, NULL, locale, false, U_ZERO_ERROR); /* No status possible  because we have two items */

    u_fprintf(lx->OUT, "<h4 class='subtitle'>%S</h4>\n", FSWF("Calendar_type_format", "Formatting"));
    showShortLongCalType(lx, rb, locale, keyStem, "format");
    u_fprintf(lx->OUT, "<h4 class='subtitle'>%S</h4>\n", FSWF("Calendar_type_stand-alone", "Stand-alone"));
    showShortLongCalType(lx, rb, locale, keyStem, "stand-alone");

    showKeyAndEndItem(lx, keyStem, locale);
}

/* Show a resource that has a short (*Abbreviations) and long (*Names) version ---------------- */
/* modified showArray */
void showShortLongCalType( LXContext *lx, UResourceBundle *rb, const char *locale, const char *keyStem, const char *type )
{
    UErrorCode status = U_ZERO_ERROR;
    /*UErrorCode shortStatus = U_ZERO_ERROR, longStatus = U_ZERO_ERROR;*/
    /*char       shortKey[100], longKey[100];*/
    /*UResourceBundle *item = NULL;*/
    /*int32_t len;*/
    /*const UChar *s  = 0;*/
    int i,j;
    int stuffCount;
    int maxCount = 0;
    struct {
        const char *style;
        const UChar *title;
        int32_t count;
        UResourceBundle *bund;
        UBool isDefault;
        UErrorCode status;
    } stuff[] = { {"narrow", NULL, -1, NULL, false, U_ZERO_ERROR},
                  {"abbreviated", NULL, -1, NULL, false, U_ZERO_ERROR},
                  {"wide", NULL, -1, NULL, false, U_ZERO_ERROR} };

    stuffCount = sizeof(stuff)/sizeof(stuff[0]);
    stuff[0].title = FSWF("DayNarrow", "Narrow Names");
    stuff[1].title = FSWF("DayAbbreviations", "Short Names");
    stuff[2].title = FSWF("DayNames", "Long Names");

    for(i=0;i<stuffCount;i++) {
        stuff[i].bund = loadCalRes3(lx, keyStem, type, stuff[i].style, &stuff[i].isDefault, &stuff[i].status);
        if(!U_FAILURE(stuff[i].status)) {
            stuff[i].count = ures_getSize(stuff[i].bund);
            if(stuff[i].count > maxCount) {
                maxCount = stuff[i].count;
            }
            //u_fprintf(lx->OUT, "[#%d/%s: %d]", i,stuff[i].style,stuff[i].count);
        } else {
            //u_fprintf(lx->OUT, "[#%d/%s: %s]", i,stuff[i].style,u_errorName(stuff[i].status));
        }
    }

    if(U_FAILURE(status)) {
        explainStatus(lx, status, keyStem);
    } else {
        u_fprintf(lx->OUT, "<table class='data-table-2' width=\"100%%\"><tr><th>#</th>");
        maxCount =0; /* recount max */
        for(i=0;i<stuffCount;i++) {
            if(U_FAILURE(stuff[i].status)) {
                u_fprintf(lx->OUT, "<th>%s</th>", u_errorName(stuff[i].status));
                continue;
            }
            u_fprintf(lx->OUT, "<th>%S", stuff[i].title);
            if((type!=NULL) &&
               (strcmp(type,"format")||(i==0)) &&
               !strcmp(ures_getLocaleByType(stuff[i].bund,ULOC_ACTUAL_LOCALE,&status),"root") &&
               (!lx->curLocaleName[0]||strcmp(lx->curLocaleName,"root"))) {
                UChar tmp[2048]; /* FSWF is not threadsafe. Use a buffer */
                u_fprintf(lx->OUT, "<br />");
                if((type!=NULL) &&strcmp(type,"format")) {
                    u_sprintf(tmp, "%S type",  FSWF("Calendar_type_format", "Formatting"));
                } else if(i==0) {
                    /* narrow (0) inherits from abbreviated (1) */
                    u_strcpy(tmp, stuff[1].title);
                }
                u_fprintf_u(lx->OUT, FSWF(/**/"inherited_from", "from: %S"), tmp);
                stuff[i].count=0;
            } if(stuff[i].isDefault) {
                u_fprintf(lx->OUT, "<br />");
                calPrintDefaultWarning(lx);
            }
            if(stuff[i].count > maxCount) {
                maxCount = stuff[i].count;
            }
            u_fprintf(lx->OUT, "</th>");
        }
        u_fprintf(lx->OUT, "</tr>\n");

        for(j=0;j<maxCount;j++) {
            u_fprintf(lx->OUT, " <tr><th>%d</th>", j);
            for(i=0;i<stuffCount;i++) {
                if(U_FAILURE(stuff[i].status)) {
                    u_fprintf(lx->OUT, "<td>%s</td>\n", u_errorName(stuff[i].status));
                    continue;
                }
                if(j>=stuff[i].count) {
                    u_fprintf(lx->OUT, "<td></td>", i,stuff[i].count);
                } else {
                    const UChar *s;
                    int32_t len;
                    UErrorCode subStatus = U_ZERO_ERROR;
                    s = ures_getStringByIndex(stuff[i].bund, j, &len, &subStatus);
                    if(U_SUCCESS(subStatus) && len) {
                        u_fprintf(lx->OUT, "<td>%S</td>", s);
                    } else if (len==0) {
                        u_fprintf(lx->OUT, "<td><i>%S</i></td>", FSWF("empty","(Empty)"));
                    }else{
                        u_fprintf(lx->OUT, "<td>");
                        explainStatus(lx, subStatus, NULL);
                        u_fprintf(lx->OUT, "</td>");
                    }
                }
            }
            u_fprintf(lx->OUT, "</tr>\n");
        }

        u_fprintf(lx->OUT, "</table>\n");
    }
    if(U_SUCCESS(status)) {
        for(i=0;i<stuffCount;i++) {
            ures_close(stuff[i].bund);
        }
    }
}

/* Show a 2d array  -------------------------------------------------------------------*/

void show2dArrayWithDescription( LXContext *lx, UResourceBundle *rb, const char *locale, const UChar *desc[], const char *key )
{
    UErrorCode status = U_ZERO_ERROR;
    UErrorCode firstStatus;
    const UChar *s  = 0;
    int32_t h,v;
    int32_t rows,cols;
    UBool bigString = false; /* is it big? */
    UBool userRequested = false; /* Did the user request this string? */
    UBool isTZ = false; /* do special TZ processing */
    int32_t len;

    UResourceBundle *array = ures_getByKey(rb, key, NULL, &status);
    UResourceBundle *row   = ures_getByIndex(array, 0, NULL, &status);
    UResourceBundle *item = NULL;

    rows = ures_getSize(array);
    cols = ures_getSize(row);

#ifndef LX_NO_USE_UTIMZONE
    isTZ = !strcmp(key, "zoneStrings");
    if(isTZ)
        cols = 7;
#endif

    if(U_SUCCESS(status) && ((rows > kShow2dArrayRowCutoff) || (cols > kShow2dArrayColCutoff)) )
    {
        bigString = true;
        userRequested = didUserAskForKey(lx, key);
    }

    showKeyAndStartItem(lx, key, NULL, locale, true, status);

    if(bigString && !userRequested) /* it's hidden. */
    {
        u_fprintf(lx->OUT, "<a href=\"?_=%s&amp;SHOW%s=1#%s\"><img border=\"0\" width=\"16\" height=\"16\" src=\"" LDATA_PATH "closed.gif\" alt=\"+\" />%S</a><br />\r\n<br />\r\n", locale, key,key, FSWF("bigStringClickToShow","(Omitted due to size. Click here to show.)"));
    }
    else
    {
        if(bigString)
        {
            u_fprintf(lx->OUT, "<a href=\"?_=%s#%s\"><img border=\"0\" width=\"16\" height=\"16\" src=\"" LDATA_PATH "opened.gif\" alt=\"-\" /> %S</a>\r\n",
                      locale,
                      key,
                      FSWF("bigStringHide", "Hide"));
        }

        firstStatus = status;  /* save this for the next column.. */

        if(U_SUCCESS(status))
        {
            u_fprintf(lx->OUT,"<table>\r\n");

            /* print the top row */
            u_fprintf(lx->OUT,"<tr><td></td>");
            for(h=0;h<cols;h++)
            {
                if(!desc[h])
                    break;

                u_fprintf(lx->OUT, "<td><b>");
                if(h == 0)
                {
                    u_fprintf(lx->OUT, "<a target=\"lx_tz\" href=\"http://icu.sourceforge.net/userguide/dateTimezone.html\">");
                }
                u_fprintf(lx->OUT,"%S", desc[h]);
                if(h == 0)
                {
                    u_fprintf(lx->OUT, "</a>");
                }
                u_fprintf(lx->OUT, "</b></td>\r\n");
            }
            u_fprintf(lx->OUT,"</tr>\r\n");

            for(v=0;v<rows;v++)
            {
                const UChar *zn = NULL;
                status = U_ZERO_ERROR;

                row   = ures_getByIndex(array, v, row, &status);

                if(U_FAILURE(status)) {
                    u_fprintf(lx->OUT, "<tr><td><b>ERR: ");
                    explainStatus(lx, status, NULL);
                    status = U_ZERO_ERROR;
                    continue;
                }

                u_fprintf(lx->OUT,"<tr><td><b>%d</b></td>", v);
                for(h=0;h<cols;h++)
                {
                    status = U_ZERO_ERROR;


#if 0 /* def LX_NO_USE_UTIMZONE */
                    if(isTZ && (h == 6))
                    {
                        UTimeZone *zone = utz_open(zn);

                        s = NULL;

                        if(zone == NULL)
                            s = FSWF("zoneStrings_open_failed", "<I>Unknown</I>");
                        else
                        {
                            s = utz_hackyGetDisplayName(zone);
                            utz_close(zone); /* s will be NULL, so nothing will get printed below. */
                        }
                    }
                    else
#endif
                    {
                        item   = ures_getByIndex(row, h, item, &status);
                        s = ures_getString(item, &len, &status);
                    }

                    if(isTZ && (h == 0)) /* save off zone for later use */
                        zn = s;

                    /*      if((h == 0) && (v==0))
                            firstStatus = status; */ /* Don't need to track firstStatus, countArrayItems should do that for us. */

                    if(U_SUCCESS(status) && s)
                        u_fprintf(lx->OUT, "<td>%S</td>\r\n", s);
                    else
                    {
                        u_fprintf(lx->OUT, "<td bgcolor=" kStatusBG " valign=\"top\">");
                        explainStatus(lx, status, key);
                        u_fprintf(lx->OUT, "</td>\r\n");
                        break;
                    }
                }
                u_fprintf(lx->OUT, "</tr>\r\n");
            }
            u_fprintf(lx->OUT, "</table>\r\n<br />");
        }
    }

    ures_close(item);
    ures_close(row);
    ures_close(array);
    showKeyAndEndItem(lx, key, locale);
}

/* Show a Tagged Array  -------------------------------------------------------------------*/

void showTaggedArray( LXContext *lx, UResourceBundle *rb, const char *locale, const char *key, UBool compareToDisplay )
{
    UErrorCode status = U_ZERO_ERROR;
    UErrorCode firstStatus;
    const UChar *s  = 0;
    int32_t v;
    int32_t rows;
    UBool bigString = false; /* is it big? */
    UBool userRequested = false; /* Did the user request this string? */
    int32_t len;
    UResourceBundle *item = NULL;

    // Todo - fix countArrayItems, which fails for many locales
    // See https://unicode-org.atlassian.net/browse/ICU-21581
    return;

    if (rb) {
        rows = ures_countArrayItems(rb, key, &status);
    } else {
        fprintf(stderr, "showTaggedArray(1591). rb is NULL for locale %s\n", locale);
        fflush(stderr);
        return;
    }

    if(U_SUCCESS(status) && ((rows > kShow2dArrayRowCutoff))) {
        bigString = true;
        userRequested = didUserAskForKey(lx, key);
    }

    showKeyAndStartItem(lx, key, NULL, locale, true, status);

    if(bigString && !userRequested) /* it's hidden. */  {
        u_fprintf(lx->OUT, "<a href=\"?_=%s&amp;SHOW%s=1#%s\"><img border=\"0\" width=\"16\" height=\"16\" src=\"" LDATA_PATH "closed.gif\" alt=\"+\" />%S</a><br />\r\n<br />\r\n", locale, key,key, FSWF("bigStringClickToShow","(Omitted due to size. Click here to show.)"));
    } else {
        if(bigString) {
            u_fprintf(lx->OUT, "<a href=\"?_=%s#%s\"><img border=\"0\" width=\"16\" height=\"16\" src=\"" LDATA_PATH "opened.gif\" alt=\"-\" /> %S</a>\r\n",
                      locale,
                      key,
                      FSWF("bigStringHide", "Hide"));
        }

        firstStatus = status;  /* save this for the next column.. */

        if(U_SUCCESS(status)) {
            UResourceBundle *tagged =  ures_getByKey(rb, key, NULL, &status);
            UResourceBundle *defaultTagged = NULL;
            UResourceBundle *taggedItem = NULL;
            if(lx->dispRB) {
                defaultTagged =  ures_getByKey(lx->dispRB, key, NULL, &status);
            }
            u_fprintf(lx->OUT,"<table class='data-table-2'>\r\n");

            /* print the top row */
            u_fprintf(lx->OUT,"<tr><td><b>%S</b></td>",
                      FSWF("taggedarrayTag", "Tag"));

            if(compareToDisplay) {
                u_fprintf(lx->OUT, "<td><i>%S</i></td>",
                          defaultLanguageDisplayName(lx));
            }

            u_fprintf(lx->OUT, "<td><b>%S</b></td></tr>",
                      lx->curLocale ? lx->curLocale->ustr : FSWF("none","None"));

            for(v=0;v<rows;v++) {
                const char *tag;

                status = U_ZERO_ERROR;
                taggedItem = ures_getByIndex(tagged, v, NULL, &status);
                tag = ures_getKey(taggedItem);

                /*tag = ures_getTaggedArrayTag(rb, key, v, &status);*/
                if(!tag)
                    break;

                u_fprintf(lx->OUT,"<tr>");

                if(U_SUCCESS(status)) {
                    u_fprintf(lx->OUT, "<th><tt>%s</tt></th>", tag);


                    if(compareToDisplay) {
                        if(lx->dispRB) {
                            item = ures_getByKey(defaultTagged, tag, item, &status);
                            s = ures_getString(item, &len, &status);

                            if(s)
                                u_fprintf(lx->OUT, "<td><i>%S</i></td>", s);
                            else
                                u_fprintf(lx->OUT, "<td></td>");
                        } else {
                            u_fprintf(lx->OUT, "<td></td>");
                        }
                    }

                    status = U_ZERO_ERROR;
                    switch(ures_getType(taggedItem)) {

                        case URES_STRING:
                            s = ures_getString(taggedItem, &len, &status);

                            if(s) {
#if 0
                                /* attempt to Bidi-shape the string if Arabic */
                                UChar junk[8192];
                                UErrorCode she=U_ZERO_ERROR;
                                int32_t dstl;
                                int32_t j;
                                UChar temp;

                                dstl=u_shapeArabic(s, u_strlen(s), junk, 8192, U_SHAPE_LETTERS_SHAPE, &she);
                                junk[dstl]=0;
                                /* Reverses the string */
                                for (j = 0; j < (dstl / 2); j++){
                                    temp = junk[(dstl-1) - j];
                                    junk[(dstl-1) - j] = junk[j];
                                    junk[j] = temp;
                                }
                                u_fprintf(lx->OUT, "<td>%S [a]</td>", junk);
#else
                                u_fprintf(lx->OUT, "<td>%S</td>", s);
#endif
                            } else {
                                u_fprintf(lx->OUT, "<td bgcolor=" kStatusBG " valign=\"top\">");
                                explainStatus(lx, status, key);
                                u_fprintf(lx->OUT, "</td>\r\n");
                            }
                            break;

                        case URES_ARRAY:
                            {
                                UResourceBundle *subItem = NULL;
                                while((s = ures_getNextString(taggedItem, &len, NULL, &status)) && U_SUCCESS(status)) {
                                    u_fprintf(lx->OUT, "<td>%S</td>", s);
                                }

                                if(U_FAILURE(status) && (status != U_INDEX_OUTOFBOUNDS_ERROR)) {
                                    u_fprintf(lx->OUT, "<td bgcolor=" kStatusBG " valign=\"top\">");
                                    explainStatus(lx, status, key);
                                    u_fprintf(lx->OUT, "</td>\r\n");
                                }
                                ures_close(subItem);
                            }
                            break;

                        default:
                            u_fprintf(lx->OUT, "<td><i>unknown resource type=%d</i></td>", ures_getType(taggedItem));
                    } /* switch */
                }
                u_fprintf(lx->OUT, "</tr>\r\n");
            }
            u_fprintf(lx->OUT, "</table>\r\n<br />");
            ures_close(taggedItem); /* todo: mem. management? */
        }
    }

    showKeyAndEndItem(lx, key, locale);
}


void showCurrencies( LXContext *lx, UResourceBundle *rb, const char *locale )
{
    UErrorCode status = U_ZERO_ERROR;
    UErrorCode firstStatus;
    const UChar *s  = 0;
    int32_t v;
    int32_t rows;
    UBool bigString = false; /* is it big? */
    UBool userRequested = false; /* Did the user request this string? */
    int32_t len;
    const char *key = "Currencies";
    UChar  cflu[9] = { 0, 0, };
    char cfl[4] = {0};
    UBool sawDefault = false;

    rows = ures_countArrayItems(rb, key, &status);

    if(U_SUCCESS(status) && ((rows > kShow2dArrayRowCutoff))) {
        bigString = true;
        userRequested = didUserAskForKey(lx, key);
    }

    showKeyAndStartItem(lx, key, NULL, locale, true, status);

    if(U_SUCCESS(status)) {
        UErrorCode defCurSt = U_ZERO_ERROR;
        ucurr_forLocale(locale, cflu, sizeof(cflu)-1, &defCurSt);
        if(U_FAILURE(defCurSt) || !cflu[0]) {
            u_fprintf(lx->OUT, "%S: <!-- for %s -->", FSWF("currNoDefault", "No Default Currency"), locale);
            explainStatus(lx,defCurSt,key);
            u_fprintf(lx->OUT, "<br />\r\n");
        } else {
            u_UCharsToChars(cflu, cfl, 4);
            cfl[3]=0;
        }
    }

    if(bigString && !userRequested) /* it's hidden. */  {
        u_fprintf(lx->OUT, "<a href=\"?_=%s&amp;SHOW%s=1#%s\"><img border=\"0\" width=\"16\" height=\"16\" src=\"" LDATA_PATH "closed.gif\" alt=\"+\" />%S</a><br />\r\n<br />\r\n", locale, key,key, FSWF("bigStringClickToShow","(Omitted due to size. Click here to show.)"));
    } else {
        if(bigString) {
            u_fprintf(lx->OUT, "<a href=\"?_=%s#%s\"><img border=\"0\" width=\"16\" height=\"16\" src=\"" LDATA_PATH "opened.gif\" alt=\"-\" /> %S</a>\r\n",
                      locale,
                      key,
                      FSWF("bigStringHide", "Hide"));
        }

        firstStatus = status;  /* save this for the next column.. */

        if(U_SUCCESS(status)) {
            UResourceBundle *tagged =  ures_getByKey(rb, key, NULL, &status);
            UResourceBundle *defaultTagged = NULL;
            UResourceBundle *taggedItem = NULL;
            if(lx->dispRB) {
                defaultTagged =  ures_getByKey(lx->dispRB, key, NULL, &status);
            }
            u_fprintf(lx->OUT,"<table class='data-table-2'>\r\n");

            /* print the top row */
            u_fprintf(lx->OUT,"<tr><th>%S</th><th>%S</th><th>%S</th><th>%S</th></tr>\r\n",
                      FSWF("currCode", "Code"),
                      FSWF("currSymbol", "Symbol"),
                      FSWF("currName", "Name"),
                      FSWF("currDigits", "Decimal Digits"));

            for(v=0;v<rows;v++) {
                const char *tag;
                UBool isDefault = false;

                status = U_ZERO_ERROR;
                taggedItem = ures_getByIndex(tagged, v, NULL, &status);
                tag = ures_getKey(taggedItem);

                if(!tag)
                    break;

                if(!strcmp(tag,cfl)) {
                    isDefault = true;
                }

                u_fprintf(lx->OUT,"<tr> ");

                if(U_SUCCESS(status)) {
                    u_fprintf(lx->OUT, "<td><tt>%s%s%s</tt></td> ",
                              isDefault?"<b>":"",
                              tag,
                              isDefault?"</b>":"");

                    if(isDefault) {
                        sawDefault = true;
                    }

                    status = U_ZERO_ERROR;
                    switch(ures_getType(taggedItem)) {

                        case URES_STRING:  /* old format ICU data */
                            s = ures_getString(taggedItem, &len, &status);

                            if(s) {
                                u_fprintf(lx->OUT, "<td>");
                                writeEscapedHTMLUChars(lx, s);
                                u_fprintf(lx->OUT, "</td>");
                            } else {
                                u_fprintf(lx->OUT, "<td bgcolor=" kStatusBG " valign=\"top\">");
                                explainStatus(lx, status, key);
                                u_fprintf(lx->OUT, "</td>\r\n");
                            }
                            break;

                        case URES_ARRAY:
                            {
                                UResourceBundle *subItem = NULL;
                                while((s = ures_getNextString(taggedItem, &len, NULL, &status)) && U_SUCCESS(status)) {
                                    u_fprintf(lx->OUT, "<td>");
                                    writeEscapedHTMLUChars(lx, s);
                                    u_fprintf(lx->OUT, "</td>");
                                    /* u_fprintf(lx->OUT, "<td>%S</td>", s);*/
                                }

                                if(U_FAILURE(status) && (status != U_INDEX_OUTOFBOUNDS_ERROR)) {
                                    u_fprintf(lx->OUT, "<td bgcolor=" kStatusBG " valign=\"top\">");
                                    explainStatus(lx, status, key);
                                    u_fprintf(lx->OUT, "</td>\r\n");
                                }
                                ures_close(subItem);
                            }
                            break;

                        default:
                            u_fprintf(lx->OUT, "<td><i>unknown resource type=%d</i></td>", ures_getType(taggedItem));
                    } /* switch */
                }

                /* Currency additions */
                {
                    UChar ucn[8];
                    u_charsToUChars(tag, ucn,4);
                    u_fprintf(lx->OUT, "<th>%d</th>", ucurr_getDefaultFractionDigits(ucn, &status));
                }

                if(isDefault) {
                    u_fprintf(lx->OUT, "<td><b>%S</b></td>",
                              FSWF("currDefault","Default Currency for this locale"));
                }

                u_fprintf(lx->OUT, "</tr>\r\n");
            }
            if(!sawDefault && cflu[0]) {
                UBool isChoiceFormat = false;
                int32_t len = 0;
                UErrorCode subSta = U_ZERO_ERROR;
                u_fprintf(lx->OUT, "<tr><td><b>%S</b></td><td><b>%S</b></td><td><b>%S</b></td><td>%d</td>\r\n",
                          cflu,
                          ucurr_getName(cflu,locale,UCURR_SYMBOL_NAME,&isChoiceFormat,&len,&subSta),
                          ucurr_getName(cflu,locale,UCURR_LONG_NAME,&isChoiceFormat,&len,&subSta),
                          ucurr_getDefaultFractionDigits(cflu, &status)
                          );
                if(U_FAILURE(subSta)) {
                    u_fprintf(lx->OUT, "<td>");
                    explainStatus(lx, subSta, key);
                    u_fprintf(lx->OUT, "</td>");
                }
                u_fprintf(lx->OUT, "<td><b>%S</b><br />",
                          FSWF("currDefault","Default Currency for this locale"));
                u_fprintf(lx->OUT, "<i>%S</i></td>\r\n", FSWF("currNotInLoc", "Note: localization for this currency was not found in this locale"));
                u_fprintf(lx->OUT, "</tr>\r\n");
            }
            u_fprintf(lx->OUT, "</table>\r\n<br />");
            ures_close(taggedItem); /* todo: mem. management? */
        }
    }
    showKeyAndEndItem(lx, key, locale);
}

UResourceBundle *loadCalRes(LXContext *lx, const char *keyStem, UBool *isDefault, UErrorCode *status) {
    /* Yes, this is a near-reimplementation of icu::CalendarData.  */
    UResourceBundle *item1 = NULL;
    *isDefault = false;
    if(U_FAILURE(*status)) { return NULL; }
    if(!lx->calMyBundle) {
        u_fprintf(lx->OUT, "LCR - no bundle!<br/>");
#if defined(LX_DEBUG)
        fprintf(stderr, "loadCalRes - no calMyBundle ! \n");
#endif
        *status = U_INTERNAL_PROGRAM_ERROR;
        return NULL;
    } else {
        item1 = ures_getByKeyWithFallback(lx->calMyBundle, keyStem, item1, status);
        /* u_fprintf(lx->OUT, "loading [%s]-%s<br/>", keyStem, u_errorName(*status)); */
    }

    if((*status == U_MISSING_RESOURCE_ERROR) && (lx->calFbBundle)) {
        *status = U_ZERO_ERROR;
        *isDefault = true;
        item1 = ures_getByKeyWithFallback(lx->calFbBundle, keyStem, item1, status);
        /*u_fprintf(lx->OUT, "loading3 [%s]-%s<br/>", keyStem, u_errorName(*status));*/
    }

    if(U_FAILURE(*status)) {
        ures_close(item1);
        return NULL;
    } else {
        return item1;
    }
}

UResourceBundle *loadCalRes3(LXContext *lx, const char *keyStem, const char *type, const char *style, UBool *isDefault, UErrorCode *status) {
    return loadCalRes3x(lx,keyStem,type,style,isDefault,lx->calMyBundle,status);
}

UResourceBundle *loadCalRes3x(LXContext *lx, const char *keyStem, const char *type, const char *style, UBool *isDefault, UResourceBundle *bnd, UErrorCode *status) {
    /* Yes, this is a near-reimplementation of icu::CalendarData.  */
    UResourceBundle *item1 = NULL;
    UResourceBundle *item2 = NULL;
    UResourceBundle *item3 = NULL;
    *isDefault = false;
    if(U_FAILURE(*status)) { return NULL; }
    if(!bnd) {
#if defined(LX_DEBUG)
        fprintf(stderr, "loadCalRes3 - no calMyBundle ! \n");
#endif
        *status = U_INTERNAL_PROGRAM_ERROR;
        return NULL;
    } else {
        //u_fprintf(lx->OUT, "<tt>lcr3(%s/%s/%s) </tt><br/>", keyStem, type, style);
        item1 = ures_getByKeyWithFallback(bnd, keyStem, item1, status);
        //u_fprintf(lx->OUT, "<tt>0[%s]:=[%s]</tt></br>", keyStem, u_errorName(*status));
        if(type) {
            item2 = ures_getByKeyWithFallback(item1, type, item2, status);
        } else {
            item2 = item1;
        }
        //u_fprintf(lx->OUT, "<tt>1[%s]:=[%s]</tt></br>", type, u_errorName(*status));
        item3 = ures_getByKeyWithFallback(item2, style, item3, status);
        //u_fprintf(lx->OUT, "<tt>2[%s]:=[%s]</tt></br>", style, u_errorName(*status));
    }

    if((*status == U_MISSING_RESOURCE_ERROR) && (lx->calFbBundle)) {
        *status = U_ZERO_ERROR;
        *isDefault = true;
        item1 = ures_getByKeyWithFallback(lx->calFbBundle, keyStem, item1, status);
        if(type) {
            item2 = ures_getByKeyWithFallback(item1, type, item2, status);
        } else {
            item2 = item1;
        }
        item3 = ures_getByKeyWithFallback(item2, style, item3, status);
    }

    ures_close(item1);
    if(type) {
        ures_close(item2);
    }
    if(U_FAILURE(*status)) {
        ures_close(item3);
        return NULL;
    } else {
        return item3;
    }
}

void calPrintDefaultWarning(LXContext *lx) {
    UErrorCode status = U_ZERO_ERROR;

    /*UChar keyBuf[1024];*/
    UChar valBuf[1024];
    char loc[1024];
    /*     keyBuf[0]=0; */
    /*     uloc_getDisplayKeyword("calendar", */
    /*                            lx->dispLocale, */
    /*                            keyBuf, */
    /*                            1024, */
    /*                            &status); */
    sprintf(loc, "@%s=%s", "calendar", "gregorian");
    uloc_getDisplayKeywordValue(loc,
                                "calendar",
                                lx->dispLocale,
                                valBuf,
                                1024,
                                &status);
    u_fprintf(lx->OUT, "<small>");
    u_fprintf_u(lx->OUT, FSWF(/**/"inherited_from", "from: %S"), valBuf);
    u_fprintf(lx->OUT, "</small>\n");
}

void loadDefaultCalendar(LXContext *lx, UResourceBundle *myRB, const char *locale) {
    UErrorCode status = U_ZERO_ERROR;
    UResourceBundle *fillin1 = NULL;
    UResourceBundle *fillin2 = NULL;
    const char *key = "DefaultCalendar";
    const UChar *s;
    int32_t len;

    strcpy(lx->defaultCalendar, "gregorian");
    fillin1 = ures_getByKey(myRB, "calendar", fillin1, &status);
    fillin2 = ures_getByKeyWithFallback(fillin1, "default", fillin2, &status);

    if(U_SUCCESS(status)) {
        s  = ures_getString(fillin2, &len, &status);
        if (s) {
            {
                char defCalStr[200];
                if(len > 199) {
                    len = 199;
                }
                u_UCharsToChars(s, defCalStr, len);
                defCalStr[len]=0;
                if(defCalStr[0]) {
                    strcpy(lx->defaultCalendar,defCalStr);
                }
            }
        }
    }
}

void showDefaultCalendar(LXContext *lx, UResourceBundle *myRB, const char *locale) {
    /*const char *urlCal = lx->curLocaleBlob.calendar;*/
    UErrorCode status = U_ZERO_ERROR;
    UResourceBundle *fillin1 = NULL;
    UResourceBundle *fillin2 = NULL;
    const char *key = "DefaultCalendar";


    showKeyAndStartItem(lx, key, NULL, locale, false, status);


    if(U_SUCCESS(status)) {
        UChar keyBuf[1024];
        UChar valBuf[1024];
        char loc[1024];
        keyBuf[0]=0;
        uloc_getDisplayKeyword("calendar",
                               lx->dispLocale,
                               keyBuf,
                               1024,
                               &status);
        sprintf(loc, "@%s=%s", "calendar", lx->defaultCalendar);
        uloc_getDisplayKeywordValue(loc,
                                    "calendar",
                                    lx->dispLocale,
                                    valBuf,
                                    1024,
                                    &status);
        u_fprintf(lx->OUT, "%S %S: %S<br />", FSWF("keyword_Default", "Default"), keyBuf, valBuf);
        if(lx->curLocaleBlob.calendar[0]) {
            sprintf(loc, "@%s=%s", "calendar", lx->curLocaleBlob.calendar);
            uloc_getDisplayKeywordValue(loc,
                                        "calendar",
                                        lx->dispLocale,
                                        valBuf,
                                        1024,
                                        &status);
            u_fprintf(lx->OUT, "%S %S: %S<br />", FSWF("keyword_Current", "Current"), keyBuf, valBuf);
        }
    }

    showKeyAndEndItem(lx, key, locale);
}

UResourceBundle *loadCalendarStuffFor(LXContext *lx, UResourceBundle *myRB, const char *locale, const char *defCal)
{
    UErrorCode status = U_ZERO_ERROR;
    /*UBool typeFallback = false;*/
    UResourceBundle *calBundle = NULL; /* "calendar" */
    UResourceBundle *myBundle = NULL;
    /*UResourceBundle *myBundle = NULL;*/ /* 'type' */
    /*UResourceBundle *fbBundle = NULL;*/ /* gregorian */

    calBundle = ures_getByKey(myRB, "calendar", NULL, &status);

    if(U_FAILURE(status)) {
        u_fprintf(lx->OUT, "Can't load 'calendar': ");
        explainStatus(lx, status, "calendar");
        return NULL;
    }

    if(!strcmp(defCal,"gregorian")) {
        myBundle = ures_getByKeyWithFallback(calBundle, "gregorian", NULL, &status);
        if(U_FAILURE(status)) {
            u_fprintf(lx->OUT, "Can't load 'calendar/%s': ", defCal);
            explainStatus(lx, status, "calendar");
            return NULL;
        }
    } else {
        myBundle = ures_getByKeyWithFallback(calBundle, defCal, NULL, &status);
        if(U_FAILURE(status)) {
            u_fprintf(lx->OUT, "Can't load 'calendar/%s': ", defCal);
            explainStatus(lx, status, "calendar");
            return NULL;
        }
        lx->calFbBundle = ures_getByKeyWithFallback(calBundle, "gregorian", NULL, &status);
        if(U_FAILURE(status)) {
            u_fprintf(lx->OUT, "Can't load 'calendar/%s': ", "gregorian");
            explainStatus(lx, status, "calendar");
            return NULL;
        }
    }

    if(U_FAILURE(status)) {
        explainStatus(lx, status, "calendar");
        return NULL;
    }

    return myBundle;
}

void loadCalendarStuff(LXContext *lx, UResourceBundle *myRB, const char *locale)
{
    lx->calMyBundle = loadCalendarStuffFor(lx,myRB,locale,lx->defaultCalendar);
}

void showDateTime(LXContext *lx, UResourceBundle *myRB, const char *locale)
{
    loadDefaultCalendar(lx, myRB, locale);
    showDefaultCalendar(lx, myRB, locale); /* and setup lx->defaultCalendar */
    if(lx->curLocaleBlob.calendar[0]) {
        strcpy(lx->defaultCalendar, lx->curLocaleBlob.calendar);
    }
    loadCalendarStuff(lx, myRB, locale);
    /* %%%%%%%%%%%%%%%%%%%%%%%*/
    /*   Date/Time section %%%*/
    /* user selection overrides default */

    showShortLongCal(lx, myRB, locale, "dayNames");
    showShortLongCal(lx, myRB, locale, "monthNames");

    u_fprintf(lx->OUT, "&nbsp;<table cellpadding=\"0\" cellspacing=\"0\" width=\"100%%\"><tr><td valign=\"top\">");

    {
        const UChar *ampmDesc[3];
        ampmDesc[0] = FSWF("AmPmMarkers0", "am");
        ampmDesc[1] = FSWF("AmPmMarkers1", "pm");
        ampmDesc[2] = 0;

        showArrayWithDescription(lx, myRB, locale, ampmDesc, "AmPmMarkers", kCal);
    }
    u_fprintf(lx->OUT, "</td><td>&nbsp;</td><td valign=\"top\">");
    /* showArray(lx, myRB, locale, "eras", kCal); */
    {
        showKeyAndStartItem(lx, "eras", NULL, locale, true, U_ZERO_ERROR);
        showShortLongCalType( lx, myRB, locale, "eras", NULL);
        showKeyAndEndItem(lx, "eras", locale);
    }
    u_fprintf(lx->OUT, "</td></tr></table>");


    {
        const UChar *dtpDesc[10]; /* flms */
        dtpDesc[0] = FSWF("DateTimePatterns0", "Full Time");
        dtpDesc[1] = FSWF("DateTimePatterns1", "Long Time");
        dtpDesc[2] = FSWF("DateTimePatterns2", "Medium Time");
        dtpDesc[3] = FSWF("DateTimePatterns3", "Short Time");
        dtpDesc[4] = FSWF("DateTimePatterns4", "Full Date");
        dtpDesc[5] = FSWF("DateTimePatterns5", "Long Date");
        dtpDesc[6] = FSWF("DateTimePatterns6", "Medium Date");
        dtpDesc[7] = FSWF("DateTimePatterns7", "Short Date");
        dtpDesc[8] = FSWF("DateTimePatterns8", "Date-Time pattern.<br />{0} = time, {1} = date");
        dtpDesc[9] = 0;

        showArrayWithDescription(lx, myRB, locale, dtpDesc, "DateTimePatterns", kCal);
    }


    {
        const UChar *zsDesc[8];
        zsDesc[0] = FSWF("zoneStrings0", "Canonical");
        zsDesc[1] = FSWF("zoneStrings1", "Normal Name");
        zsDesc[2] = FSWF("zoneStrings2", "Normal Abbrev");
        zsDesc[3] = FSWF("zoneStrings3", "Summer/DST Name");
        zsDesc[4] = FSWF("zoneStrings4", "Summer/DST Abbrev");
        zsDesc[5] = FSWF("zoneStrings5", "Example City");
#ifndef LX_NO_USE_UTIMZONE
        zsDesc[6] = FSWF("zoneStrings6", "Raw Offset");
#else
        zsDesc[6] = 0;
#endif
        zsDesc[7] = 0;

        /*  show2dArrayWithDescription(lx, myRB, locale, zsDesc, "zoneStrings");   */
        /* not calendrical */
    }
    /* showLPC(lx, myRB, locale, true); */
    showDateTimeElements(lx, myRB, locale); /* not calendrical? */
}

/* locale pattern chars */
void showLPC(LXContext *lx, UResourceBundle *myRB, const char *locale, UBool hidable)
{
    const UChar *charDescs[25];

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
    charDescs[18] = FSWF("localPatternChars18", "Year (of 'Week of Year')");
    charDescs[19] = FSWF("localPatternChars19", "Day of Week (1=first day according to locale)");
    charDescs[20] = FSWF(/**/"localPatternChars20", "extended year");
    charDescs[21] = FSWF(/**/"localPatternChars21", "julian day");
    charDescs[22] = FSWF(/**/"localPatternChars22", "millis in day");
    charDescs[23] = FSWF(/**/"localPatternChars23", "timezone rfc");
    charDescs[24] = 0;

    showStringWithDescription(lx, myRB, locale, charDescs,"localPatternChars", hidable); /* calendrical? */
}
