/**********************************************************************
*   Copyright (C) 1999-2002, International Business Machines
*   Corporation and others.  All Rights Reserved.
***********************************************************************/

/* Routines that show a specific data type */

#include "locexp.h"

#include "unicode/uset.h"

/* Show a resource that's a collation rule list -----------------------------------------------------*/
/**
 * Show a string.  Make it progressive disclosure if it exceeds some length !
 * @param rb the resourcebundle to pull junk out of 
 * @param locale the name of the locale (for URL generation)
 * @param queryString the querystring of the request.
 * @param key the key we're listing
 */
void showCollationElements( LXContext *lx, UResourceBundle *rb, const char *locale, const char *queryString, const char *key )
{
  
    UErrorCode status = U_ZERO_ERROR;
    UErrorCode subStatus = U_ZERO_ERROR;
    const UChar *s  = 0;
    UChar temp[UCA_LEN]={'\0'};
    UChar *scopy = 0;
    UChar *comps = 0;
    UChar *compsBuf = 0;
    UBool bigString     = FALSE; /* is it too big to show automatically? */
    UBool userRequested = FALSE; /* Did the user request this string? */
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
            status = U_USING_DEFAULT_ERROR;
        }
    }
    else
    {
        len = u_strlen(s);
    }

    len2 = len;

    scopy = malloc(len * sizeof(UChar));
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
        bigString = TRUE;
        userRequested = didUserAskForKey(key, queryString);
    }

    showKeyAndStartItemShort(lx, key, NULL, locale, FALSE, status);

    u_fprintf(lx->OUT, "&nbsp;</TD></TR><TR><TD></TD><TD>");
  
    /* Ripped off from ArrayWithDescription. COPY BACK */
    {
        const UChar *sampleString, *sampleString2;
        char *sampleChars;
        UResourceBundle *sampleRB;
        UErrorCode sampleStatus = U_ZERO_ERROR;
        int32_t len;

        /* samplestring will vary with label locale! */
        sampleString =  FSWF(/*NOEXTRACT*/"EXPLORE_CollationElements_sampleString","bad|Bad|Bat|bat|b\\u00E4d|B\\u00E4d|b\\u00E4t|B\\u00E4t|c\\u00f4t\\u00e9|cot\\u00e9|c\\u00f4te|cote");

        sampleRB = ures_open(FSWF_bundlePath(), locale, &sampleStatus);
        if(U_SUCCESS(sampleStatus))
        {
            sampleString2 = ures_getStringByKey(sampleRB, "EXPLORE_CollationElements_sampleString", &len, &sampleStatus);
            ures_close(sampleRB);
        }

        if(U_FAILURE(sampleStatus))
        {
            sampleString2 = sampleString; /* fallback */
        }

        sampleChars = createEscapedSortList(sampleString2);
        showExploreButtonSort(lx, rb,locale, sampleChars, "CollationElements", TRUE);
        free(sampleChars);

    }

    u_fprintf(lx->OUT, "</TD>"); /* Now, we're done with the ShowKey.. cell */

    u_fprintf(lx->OUT, "</TR><TR><TD COLSPAN=2>");

    if(U_SUCCESS(status))
    {

        if(bigString && !userRequested) /* it's hidden. */
	{
            u_fprintf(lx->OUT, "<A HREF=\"?_=%s&SHOW%s=1#%s\"><IMG BORDER=0 WIDTH=16 HEIGHT=16 SRC=\"../_/closed.gif\" ALT=\"\">%U</A>\r\n<P>", locale, key,key, FSWF("bigStringClickToShow","(Omitted due to size. Click here to show.)"));
	}
        else
	{
            if(bigString)
	    {
                u_fprintf(lx->OUT, "<A HREF=\"?_=%s#%s\"><IMG border=0 width=16 height=16 SRC=\"../_/opened.gif\" ALT=\"\"> %U</A><P>\r\n",
                          locale,
                          key,
                          FSWF("bigStringHide", "Hide"));
	    }
	  
            if(U_SUCCESS(status))
	    {

                compsBuf = malloc(sizeof(UChar) * (len*3));
                comps = compsBuf;
              
                {
                    for(i=0;i<(len*3);i++)
                    {
                        comps[i] = 0x0610;
                    }
                }

                len = u_normalize(s,
                                  len,
                                  UCOL_DECOMP_COMPAT_COMP_CAN,
				/*UCOL_DECOMP_CAN_COMP_COMPAT, */
                                  0,
                                  comps,
                                  len*3,
                                  &status);

              
/*              u_fprintf(lx->OUT, "xlit: %d to %d<P>\n",
                len2,len); */
                if(U_FAILURE(status))
                {
                    free(compsBuf);
                    u_fprintf(lx->OUT, "xlit failed -} %s<P>\n",
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
                    u_fprintf(lx->OUT, "<I>%U</I>", FSWF("empty", "(Empty)"));
		}
                else while(len--)
		{
		  
                    if(*comps == '&')
                    {
                        u_fprintf(lx->OUT, "<P>");
                    }
                    else if(*comps == '<')
                    {
                        if((comps != compsBuf) && (comps[-1] != '<'))
                        {
                            /* don't break more than once. */
                            u_fprintf(lx->OUT, "<BR>&nbsp;");
                        }
                        u_fprintf(lx->OUT, "&lt;");
                    } else if(*comps == ']')
                    {
                        u_fprintf(lx->OUT, "]<p>\r\n");
                    }
                    else
                    {
                        if((*comps == 0x000A) || u_isprint(*comps))
                            u_fprintf(lx->OUT, "%K", *comps);
                        else
                            u_fprintf(lx->OUT, "<B>\\u%04X</B>", *comps); /* to emulate the callback */
                    }
		  
                    comps++;
		};

/* **	      ucnv_setFromUCallBack((UConverter*)u_fgetConverter(lx->OUT), oldCallback, &status); */

	    }
            else
                explainStatus(lx, status, key);
	}
    }

    status = U_ZERO_ERROR;
    s = ures_getStringByKey(array, "Version", &len, &status);
    if(U_SUCCESS(status))
    {
        u_fprintf(lx->OUT, "<P><B>%U %U:</B> %U\r\n",
                  FSWF("Collation", "Collation"),
                  FSWF("Version","Version"),s);
    }

    u_fprintf(lx->OUT, "</TD>");
  

    free(scopy);
    if(coll) ucol_close(coll);
  
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

    showKeyAndStartItem(lx, "LocaleCodes", FSWF("LocaleCodes", "Locale Codes"), locale, FALSE, status);

    u_fprintf(lx->OUT, "<TABLE summary=\"%U\" BORDER=1><TR><TD></TD><TD><B>%U</B></TD><TD><B>%U</B></TD><TD><B>%U</B></TD></TR>\r\n",
              FSWF("LocaleCodes", "Locale Codes"),
              FSWF("LocaleCodes_Language", "Language"),
              FSWF("LocaleCodes_Country", "Region"),
              FSWF("LocaleCodes_Variant", "Variant"));
    u_fprintf(lx->OUT, "<TR><TD></TD><TD>");
  
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
  
    u_fprintf(lx->OUT, "</TD><TD>");
  
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
  
    u_fprintf(lx->OUT, "</TD><TD>");
  
    status = U_ZERO_ERROR;
    uloc_getVariant(locale, tempvar, 1000, &status);
    if(U_SUCCESS(status))
        u_fprintf(lx->OUT, tempvar);
    else
        explainStatus(lx, status, "LocaleCodes");

    u_fprintf(lx->OUT, "</TD></TR>\r\n");

    /* 3 letter line */

    u_fprintf(lx->OUT, "<TR><TD>%U</TD>",
              FSWF("LocaleCodes_3", "3"));

    u_fprintf(lx->OUT, "<TD>");

    lang3 = uloc_getISO3Language(locale);
    if(lang3)
    {
        u_fprintf(lx->OUT, "%s", lang3);
    }

    u_fprintf(lx->OUT, "</TD><TD>");

    ctry3 = uloc_getISO3Country(locale);
    if(ctry3)
    {
        u_fprintf(lx->OUT, "%s", ctry3);
    }

    u_fprintf(lx->OUT, "</TD><TD></TD></TR>\r\n");
  
    u_fprintf(lx->OUT, "</TABLE>\r\n");

    u_fprintf(lx->OUT, "</TD>");
    showKeyAndEndItem(lx, "LocaleCodes", locale);  /* End of LocaleCode's sub item */

}

/* -------------- show script for locale --------------*/
void showLocaleScript(LXContext *lx, UResourceBundle *rb, const char *locale)
{
  
    UErrorCode status = U_ZERO_ERROR;

    UScriptCode  list[16];
    int32_t len, i;

    len = uscript_getCode(locale, list, sizeof(list)/sizeof(list[0]), &status);

    showKeyAndStartItem(lx, "LocaleScript", FSWF("LocaleScript", "Locale Script"), locale, FALSE, status);

    u_fprintf(lx->OUT, "<table summary=\"%U\" border=1>\r\n",
              FSWF("LocaleScript", "Locale Script"));
    u_fprintf(lx->OUT, "<tr><td><b>%U</b></td><td><b>%U</b></td></tr>\r\n",
              FSWF("LocaleScriptAbbreviations", "Short Names"),
              FSWF("LocaleScriptNames", "Long Names")
        );
  
    for(i=0;i<len;i++)
    {
        u_fprintf(lx->OUT, "   <tr><td>%s</td><td>%s</td>\r\n", 
                  uscript_getShortName(list[i]), uscript_getName(list[i]));
    }
    u_fprintf(lx->OUT, "</tr>");
    u_fprintf(lx->OUT, "</table>\r\n");
    u_fprintf(lx->OUT, "</td>\r\n");

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
    showKeyAndStartItem(lx, key, NULL, locale, FALSE, status);

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
    u_fprintf(lx->OUT, "</TD>");
    showKeyAndEndItem(lx, key, locale);
}

/* Show a resource that's a simple string -----------------------------------------------------*/
/**
 * Show a string.  Make it progressive disclosure if it exceeds some length !
 * @param rb the resourcebundle to pull junk out of 
 * @param locale the name of the locale (for URL generation)
 * @param queryString the querystring of the request.
 * @param key the key we're listing
 */

void showString( LXContext *lx, UResourceBundle *rb, const char *locale, const char *queryString, const char *key, UBool PRE )
{
  
    UErrorCode status = U_ZERO_ERROR;
    const UChar *s  = 0;
    UBool bigString = FALSE; /* is it big? */
    UBool userRequested = FALSE; /* Did the user request this string? */
    int32_t len;

    s = ures_getStringByKey(rb, key, &len, &status);

    if(U_SUCCESS(status) && ( u_strlen(s) > kShowStringCutoffSize ) )
    {
        bigString = TRUE;
        userRequested = didUserAskForKey(key, queryString);
    }

    showKeyAndStartItem(lx, key, NULL, locale, FALSE, status);

    if(U_SUCCESS(status))
    {

        if(bigString && !userRequested) /* it's hidden. */
	{
            u_fprintf(lx->OUT, "<A HREF=\"?_=%s&SHOW%s=1#%s\"><IMG BORDER=0 WIDTH=16 HEIGHT=16 SRC=\"../_/closed.gif\" ALT=\"\">%U</A>\r\n<P>", locale, key,key, FSWF("bigStringClickToShow","(Omitted due to size. Click here to show.)"));
	}
        else
	{
            if(bigString)
	    {
                u_fprintf(lx->OUT, "<A HREF=\"?_=%s#%s\"><IMG border=0 width=16 height=16 SRC=\"../_/opened.gif\" ALT=\"\"> %U</A><P>\r\n",
                          locale,
                          key,
                          FSWF("bigStringHide", "Hide"));
	    }
	  
            if(U_SUCCESS(status))
	    {
                if(PRE)
                    u_fprintf(lx->OUT, "<PRE>");

                if(*s == 0)
                    u_fprintf(lx->OUT, "<I>%U</I>", FSWF("empty", "(Empty)"));
                {
                    writeEscaped(lx, s);
                }

                if(PRE)
                    u_fprintf(lx->OUT, "</PRE>");
	    }
	}
    }
    u_fprintf(lx->OUT, "</TD>");
    showKeyAndEndItem(lx, key, locale);
}

/* Show a resource that's a UnicodeSeet -----------------------------------------------------*/
/**
 * Show a string.  Make it progressive disclosure if it exceeds some length !
 * @param rb the resourcebundle to pull junk out of 
 * @param locale the name of the locale (for URL generation)
 * @param queryString the querystring of the request.
 * @param key the key we're listing
 */

void showUnicodeSet( LXContext *lx, UResourceBundle *rb, const char *locale, const char *queryString, const char *key, UBool PRE )
{
  
    UErrorCode status = U_ZERO_ERROR;
    const UChar *s  = 0;
    UChar smallString[ kShowUnicodeSetCutoffSize + 1];
    UBool bigString = FALSE; /* is it big? */
    UBool userRequested = FALSE; /* Did the user request this string? */
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

    showKeyAndStartItem(lx, key, NULL, locale, FALSE, status);

    if(U_FAILURE(status))
    {
        u_fprintf(lx->OUT, "</TD>");
        showKeyAndEndItem(lx, key, locale);
        return;
    }

    setLen = uset_size(uset);

    if( (rulesLen > kShowUnicodeSetCutoffSize ) ||
        (setLen > kShowUnicodeSetCutoffSize) )
    {
        userRequested = didUserAskForKey(key, queryString);
        bigString = TRUE;

        if(!userRequested) /* it's hidden. */
        {
#if 0
            u_fprintf(lx->OUT, "<A HREF=\"?_=%s&SHOW%s=1#%s\"><IMG BORDER=0 WIDTH=16 HEIGHT=16 SRC=\"../_/closed.gif\" ALT=\"\">%U ", locale, key,key, FSWF("bigStringClickToShow","(Omitted due to size. Click here to show.)"));
            u_fprintf(lx->OUT, " </A>\r\n<P>");
#endif
        }
        else
        {
            u_fprintf(lx->OUT, "<A HREF=\"?_=%s#%s\"><IMG border=0 width=16 height=16 SRC=\"../_/opened.gif\" ALT=\"\"> %U</A><P>\r\n",
                      locale,
                      key,
                      FSWF("bigStringShorten", "Don't show all"));
        }
    }

    /** RULES **/
    u_fprintf(lx->OUT, "<table border=1 summary=\"Rules\" cellpadding=3 cellspacing=3><tr><td><b>%U</b></td><td>\r\n",
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
        u_fprintf(lx->OUT, "%U", FSWF("...", "..."));
        u_fprintf(lx->OUT, "<br><A HREF=\"?_=%s&SHOW%s=1#%s\"><IMG BORDER=0 WIDTH=16 HEIGHT=16 SRC=\"../_/closed.gif\" ALT=\"\"><I>%U</I> ", locale, key,key, FSWF("bigStringClickToExpand","Truncated due to size. Click here to show. "));
        u_fprintf_u(lx->OUT, FSWF("bigStringSize", "(%d of %d shown)"), len, rulesLen);
        u_fprintf(lx->OUT, " </A>\r\n");
    }
  
    u_fprintf(lx->OUT, "</td></tr>\r\n");

    /** Set **/

    u_fprintf(lx->OUT, "<tr><td><b>%U</b></td><td>\r\n",
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
            buf = (UChar*)malloc(sizeof(UChar)*bufSize);
            subErr = U_ZERO_ERROR;
            n = uset_getItem(uset, i, &start,&end, buf, bufSize, &subErr);
        }
        if(U_FAILURE(subErr))
        {
            u_fprintf(lx->OUT, "<B>Fatal: err in showSet, %s @ index %d\n", u_errorName(subErr), i);
            u_fprintf(lx->OUT, "</td></tr></table><HR>\r\n");
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
                    u_fprintf(lx->OUT, "%K", c);
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
            u_fprintf(lx->OUT, "\"%U\"", buf);
        }
    }

    if(!userRequested && (howManyChars >= kShowUnicodeSetCutoffSize))
    {
        u_fprintf(lx->OUT, "%U", FSWF("...", "..."));
        u_fprintf(lx->OUT, "<br><A HREF=\"?_=%s&SHOW%s=1#%s\"><IMG BORDER=0 WIDTH=16 HEIGHT=16 SRC=\"../_/closed.gif\" ALT=\"\"><I>%U</I> ", locale, key,key, FSWF("bigStringClickToExpand","Truncated due to size. Click here to show. "));
        u_fprintf_u(lx->OUT, FSWF("bigStringSize", "(%d of %d shown)"), kShowUnicodeSetCutoffSize, howManyChars);
        u_fprintf(lx->OUT, " </A>\r\n");
    }
  
    u_fprintf(lx->OUT, "</td></tr></table>\r\n");

    u_fprintf(lx->OUT, "</TD>");
    showKeyAndEndItem(lx, key, locale);
}

/* Show a resource that's a simple string, but explain each character.-----------------------------------------------------*/
/**
 * Show a string.  Make it progressive disclosure if it exceeds some length !
 * @param rb the resourcebundle to pull junk out of 
 * @param locale the name of the locale (for URL generation)
 * @param queryString the querystring of the request.
 * @param desc array (0 at last item) of char desc
 * @param key the key we're listing
 */

void showStringWithDescription( LXContext *lx, UResourceBundle *rb, const char *locale, const char *qs, const UChar *desc[], const char *key, UBool hidable)
{
  
    UErrorCode status = U_ZERO_ERROR;
    const UChar *s  = 0;
    UBool bigString = FALSE; /* is it big? */
    UBool userRequested = FALSE; /* Did the user request this string? */
    int32_t i;
    int32_t len;

    s = ures_getStringByKey(rb, key, &len, &status);

    /* we'll assume it's always big, for now. */
    bigString = TRUE;
    userRequested = didUserAskForKey(key, qs);

    showKeyAndStartItem(lx, key, NULL, locale, FALSE, status);

    /** DON'T show the string as a string. */
    /* 
       if(U_SUCCESS(status) && s)
       u_fprintf(lx->OUT, "%U<BR>\r\n", s);
    */
    if(!hidable)
    {
        userRequested = TRUE;
        bigString = FALSE;
    }
  

    if(bigString && !userRequested) /* it's hidden. */
    {
        u_fprintf(lx->OUT, "<A HREF=\"?_=%s&SHOW%s=1#%s\"><IMG BORDER=0 WIDTH=16 HEIGHT=16 SRC=\"../_/closed.gif\" ALT=\"\">%U</A>\r\n<P>", locale, key,key, FSWF("stringClickToShow","(Click here to show.)"));
	u_fprintf(lx->OUT, "<P>");
    }
    else
    {
        if(bigString)
	{
            u_fprintf(lx->OUT, "<A HREF=\"?_=%s#%s\"><IMG border=0 width=16 height=16 SRC=\"../_/opened.gif\" ALT=\"\"> %U</A><P>\r\n",
                      locale,
                      key,
                      FSWF("bigStringHide", "Hide"));
	}
  
        if(U_SUCCESS(status))
	{
            u_fprintf(lx->OUT, "<TABLE summary=\"String\" BORDER=1 WIDTH=100%>");
            u_fprintf(lx->OUT, "<TR><TD><B>%U</B></TD><TD><B>%U</B></TD><TD><B>%U</B></TD></TR>\r\n",
                      FSWF("charNum", "#"),
                      FSWF("char", "Char"),
                      FSWF("charMeaning", "Meaning"));
	  
	  
            for(i=0;desc[i];i++)
	    {
                if(!s[i])
                    break;
	      
                u_fprintf(lx->OUT, "<TR><TD WIDTH=5>%d</TD><TD>%K</TD><TD>%U</TD></TR>\r\n",
                          i,
                          s[i],
                          desc[i]);
	    }
            u_fprintf(lx->OUT, "</TABLE>\r\n");
	}
    }
    u_fprintf(lx->OUT, "</TD>");
    showKeyAndEndItem(lx, key, locale);
}
  
/* Show a resource that's an array. Useful for types we haven't written viewers for yet --------*/

void showArray( LXContext *lx, UResourceBundle *rb, const char *locale, const char *key )
{
    UErrorCode status = U_ZERO_ERROR;
    UErrorCode firstStatus = U_ZERO_ERROR;
    UResourceBundle  *array = NULL, *item = NULL;
    int32_t len;
    const UChar *s  = 0;
    int i;

    array = ures_getByKey(rb, key, array, &firstStatus);
    item = ures_getByIndex(array, 0, item, &firstStatus);

    showKeyAndStartItem(lx, key, NULL, locale, FALSE, firstStatus);

    u_fprintf(lx->OUT, "<OL>\r\n");

    for(i=0;;i++)
    {
        status = U_ZERO_ERROR;
        if(U_FAILURE(firstStatus)) {
            status = firstStatus; /* ** todo: clean up err handling! */
        }

        item = ures_getByIndex(array, i, item, &status);
        s  = ures_getString(item, &len, &status);

        if(!s)
            break;

        if(U_SUCCESS(status))
            u_fprintf(lx->OUT, "<LI> %U\r\n", s);
        else
	{
            u_fprintf(lx->OUT, "<LI>");
            explainStatus(lx, status, key);
            u_fprintf(lx->OUT, "\r\n");
            break;
	}

    }
    u_fprintf(lx->OUT, "</OL></TD>");
    u_fprintf(lx->OUT, "<!-- %s:%d -->\r\n", __FILE__, __LINE__);
    showKeyAndEndItem(lx, key, locale);
    u_fprintf(lx->OUT, "<!-- %s:%d -->\r\n", __FILE__, __LINE__);

    ures_close(item);
    ures_close(array);
}

/* Show a resource that's an array, wiht an explanation ------------------------------- */

void showArrayWithDescription( LXContext *lx, UResourceBundle *rb, const char *locale, const UChar *desc[], const char *key )
{
    UErrorCode status = U_ZERO_ERROR;
    UErrorCode firstStatus;
    const UChar *s  = 0;
    UChar *toShow =0;
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

    /* figure out what example to use */
    if(!strcmp(key,"DateTimePatterns"))
        exampleType = kDateTimeExample;
    else if(!strcmp(key, "NumberPatterns"))
        exampleType = kNumberExample;
    else
        exampleType = kNoExample;

    /* store the date now..just in case formatting takes multiple seconds! */
    now = ucal_getNow();

    firstStatus = U_ZERO_ERROR;
    array = ures_getByKey(rb, key, array, &firstStatus);
    item = ures_getByIndex(array, 0, item, &firstStatus);
    s = ures_getString(item, &len, &firstStatus);
    showKeyAndStartItemShort(lx, key, NULL, locale, FALSE, firstStatus);

    if(exampleType != kNoExample)
    {
        toShow = nothing+1;

        exampleStatus = U_ZERO_ERROR;

        switch(exampleType)
	{

	case kDateTimeExample:
            exampleStatus = U_ZERO_ERROR;
            exampleDF = udat_open(UDAT_IGNORE,UDAT_IGNORE,locale,NULL, 0, s,-1,&exampleStatus);
            if(U_SUCCESS(exampleStatus))
	    {
                len = udat_toPattern(exampleDF, TRUE, tempChars, 1024,&exampleStatus);
                if(U_SUCCESS(exampleStatus))
		{
                    toShow = tempChars;
		}
	    }
            break;
	  
	case kNumberExample:

            toShow = nothing;

            exampleNF = unum_open(0, s,-1,locale,NULL, &exampleStatus);
            if(U_SUCCESS(exampleStatus))
	    {
                len = unum_toPattern(exampleNF, TRUE, tempChars, 1024, &exampleStatus);
                if(U_SUCCESS(exampleStatus))
		{
                    toShow = tempChars;
		}
                unum_close(exampleNF);
	    }
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

      
        u_fprintf(lx->OUT, "<FORM TARGET=\"_currency\" METHOD=\"POST\" ACTION=\"http:www.oanda.com/converter/travel\" ENCTYPE=\"x-www-form-encoded\"><INPUT TYPE=\"hidden\" NAME=\"result\" VALUE=\"1\"><INPUT TYPE=\"hidden\" NAME=\"lang\" VALUE=\"%s\"><INPUT TYPE=\"hidden\" NAME=\"date_fmt\" VALUE=\"us\"><INPUT NAME=\"exch\" TYPE=HIDDEN VALUE=\"%U\"><INPUT TYPE=HIDDEN NAME=\"expr\" VALUE=\"%U\">",
                  "en", /* lx->cLocale */
                  curStr,
                  homeStr
            );

        u_fprintf(lx->OUT, "<INPUT TYPE=IMAGE WIDTH=48 HEIGHT=20 BORDER=0 SRC=\"../_/explore.gif\"  ALIGN=RIGHT   ");
        u_fprintf(lx->OUT, " VALUE=\"%U\"></FORM>",
                  FSWF("exploreTitle", "Explore"));
        u_fprintf(lx->OUT, "</FORM>");
    }
#endif
    u_fprintf(lx->OUT, "</TD>"); /* Now, we're done with the ShowKey.. cell */

    u_fprintf(lx->OUT, "</TR><TR><TD COLSPAN=2><TABLE BORDER=2 WIDTH=\"100%\" HEIGHT=\"100%\">\r\n");

    for(i=0;desc[i];i++)
    {
      
        u_fprintf(lx->OUT, "<TR><TD WIDTH=5>%d</TD><TD>%U</TD><TD>",
                  i, desc[i]);

        status = U_ZERO_ERROR;
        exampleStatus = U_ZERO_ERROR;

        item = ures_getByIndex(array, i, item, &status);
        s =    ures_getString(item, &len, &status);

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

                    exampleDF = udat_open(UDAT_IGNORE, UDAT_IGNORE, locale,NULL, 0, s,-1,&exampleStatus);
                    if(U_SUCCESS(exampleStatus))
		    {
                        len = udat_toPattern(exampleDF, TRUE, tempChars, 1024,&exampleStatus);

                        if(U_SUCCESS(exampleStatus))
			{
                            toShow = tempChars;
			}
		    }	   
		}
                break;

	    case kNumberExample:
                if(i == 3) /* scientific */
                    d = 1234567890;

                exampleNF = unum_open(0, s,-1,locale, NULL, &exampleStatus);
                if(U_SUCCESS(exampleStatus))
		{
                    len = unum_toPattern(exampleNF, TRUE, tempChars, 1024, &exampleStatus);
                    if(U_SUCCESS(exampleStatus))
		    {
                        toShow = tempChars;
		    }
		}
                break;

	      
	    default:
                ;
	    }
	  
            u_fprintf(lx->OUT, "%U\r\n", toShow);
	}
        else
	{
            s = 0;
            explainStatus(lx, status, key);
            u_fprintf(lx->OUT, "\r\n");
            break;
	}
        u_fprintf(lx->OUT, "</TD>");
      
        if(s) /* only if pattern exists */
            switch(exampleType)
            {
            case kDateTimeExample:
                if(i < 8)
                {
                    u_fprintf(lx->OUT, "<TD>");

                    if(U_SUCCESS(exampleStatus))
                    {
                        exampleStatus = U_ZERO_ERROR; /* clear fallback info from exampleDF */
                        udat_format(exampleDF, now, tempChars, 1024, NULL, &exampleStatus);
                        udat_close(exampleDF);
		
                        if(U_SUCCESS(exampleStatus))
                            u_fprintf(lx->OUT, "%U", tempChars);

                    }
                    explainStatus(lx, exampleStatus, key);
                    u_fprintf(lx->OUT, "</TD>\r\n");

                    if(i == 3) /* short time */
                        u_strcpy(tempTime, tempChars);
                    else if(i == 7) /* short date */
                        u_strcpy(tempDate, tempChars);
                }
                else
                {
                    u_fprintf(lx->OUT, "<TD>");
                    exampleStatus = U_ZERO_ERROR;
                    if(s)
                        if(u_formatMessage(locale, s, -1, tempChars,1024,&exampleStatus, 
                                           tempTime,
                                           tempDate))
                            u_fprintf(lx->OUT,"%U", tempChars);
                    u_fprintf(lx->OUT, "</TD>\r\n");
                }
                break;

            case kNumberExample:
            {
                u_fprintf(lx->OUT, "<TD>");

                if(U_SUCCESS(exampleStatus))
                {
                    exampleStatus = U_ZERO_ERROR; /* clear fallback info from exampleDF */

                    if(i == 3) /* scientific */
                        d = 1234567890;
                    unum_formatDouble(exampleNF, d, tempChars, 1024, NULL, &exampleStatus);
		
                    if(U_SUCCESS(exampleStatus))
                        u_fprintf(lx->OUT, "%U", tempChars);

		
                    u_fprintf(lx->OUT, "</TD><TD>");

                    if(i == 3) /* scientific */
                        d = 0.00000000000000000005;

                    unum_formatDouble(exampleNF, -d, tempChars, 1024, NULL, &exampleStatus);
		
                    if(U_SUCCESS(exampleStatus))
                        u_fprintf(lx->OUT, "%U", tempChars);

                    unum_close(exampleNF);

                }
                explainStatus(lx, exampleStatus, key);
                u_fprintf(lx->OUT, "</TD>\r\n");
            }
            break;

            case kNoExample:
            default:
                break;
            }

        u_fprintf(lx->OUT, "</TR>\r\n");

    }
  

    u_fprintf(lx->OUT, "</TABLE>");

    u_fprintf(lx->OUT, "</TD>");

    showKeyAndEndItem(lx, key, locale);
    ures_close(item);
    ures_close(array);
}

void showSpelloutExample( LXContext *lx, UResourceBundle *rb, const char *locale)
{
    UErrorCode status;
    double examples[] = { 0, 123.45, 67890 };
    UNumberFormat *exampleNF = 0;

    int n;
    const char *key = "SpelloutRulesExample";
    UChar tempChars[245];

    status = U_ZERO_ERROR;
    exampleNF = unum_open(UNUM_SPELLOUT,NULL, -1, locale, NULL, &status);

    showKeyAndStartItem(lx, key, NULL, locale, FALSE, status);
    if(exampleNF) unum_close(exampleNF);

    u_fprintf(lx->OUT, "<TABLE BORDER=2 WIDTH=\"100%\" HEIGHT=\"100%\">\r\n");

    for(n=0;n<3;n++)
    {
        status = U_ZERO_ERROR;
        tempChars[0] = 0;
        exampleNF = unum_open(UNUM_SPELLOUT,NULL, -1, locale, NULL, &status);
        unum_formatDouble(exampleNF, examples[n], tempChars, 1024,0, &status);
        u_fprintf(lx->OUT, "<TR><TD>%f</TD><TD>%U", examples[n], tempChars);
        unum_close(exampleNF);
        if(U_FAILURE(status))
        {
            u_fprintf(lx->OUT, " ");
            explainStatus(lx, status, NULL);
        }
        u_fprintf(lx->OUT, "</TD></TR>\r\n");
    }
    u_fprintf(lx->OUT, "</TABLE>");
    showKeyAndEndItem(lx, key, locale);
}

/* show the DateTimeElements string *------------------------------------------------------*/

void showDateTimeElements( LXContext *lx, UResourceBundle *rb, const char *locale)
{
    UErrorCode status = U_ZERO_ERROR;
    const UChar *s  = 0;
    int32_t    len;
    const int32_t   *elements;

    UResourceBundle *array = NULL, *item = NULL;

    const char *key = "DateTimeElements";
    /*
      0: first day of the week 
      1: minimaldaysinfirstweek 
    */

    status = U_ZERO_ERROR;

    array = ures_getByKey(rb, key, array, &status);
    elements = ures_getIntVector(array, &len, &status);

    showKeyAndStartItem(lx, key, FSWF("DateTimeElements","Date and Time Options"), locale, FALSE, status);

    if(len < 2)
    {
        u_fprintf(lx->OUT, "%U ", FSWF("DateTimeElements_short", "Error- resource is too short (should be 2 elements)!"));
        ures_close(array);
        showKeyAndEndItem(lx, key, locale);
        return;
    }

    /* First day of the week ================= */
    u_fprintf(lx->OUT, "%U ", FSWF("DateTimeElements0", "First day of the week: "));

    if(U_SUCCESS(status))
    {
        int32_t  firstDayIndex;

        firstDayIndex = (((elements[0])+6)%7); 
      
        u_fprintf(lx->OUT, " %d \r\n", elements[0]);
        /* here's something fun: try to fetch that day from the user's current locale */
        status = U_ZERO_ERROR;
      
        if(lx->defaultRB && U_SUCCESS(status))
	{
            /* don't use 'array' here because it's the DTE resource */
            item = ures_getByKey(lx->defaultRB, "DayNames", item, &status);
            item = ures_getByIndex(item, firstDayIndex, item, &status);
            s    = ures_getString(item, &len, &status);
            
            if(s && U_SUCCESS(status))
	    {
                u_fprintf(lx->OUT, " = %U \r\n", s);
	    }
            status = U_ZERO_ERROR;

            item = ures_getByKey(rb, "DayNames", item, &status);
            item = ures_getByIndex(item, firstDayIndex, item, &status);
            s    = ures_getString(item, &len, &status);

            if(s && U_SUCCESS(status))
	    {
                u_fprintf(lx->OUT, " = %U \r\n", s);
	    }
	}
        status = U_ZERO_ERROR;
    }
    else
    {
        explainStatus(lx, status, key);
        u_fprintf(lx->OUT, "\r\n");
    }

    u_fprintf(lx->OUT, "<BR>\r\n");

    /* minimal days in week ================= */
    u_fprintf(lx->OUT, "%U", FSWF("DateTimeElements1", "Minimal Days in First Week: "));
  
    if(U_SUCCESS(status))
        u_fprintf(lx->OUT, " %d \r\n", elements[1]);
    else
    {
        explainStatus(lx, status, key);
        u_fprintf(lx->OUT, "\r\n");
    }

    u_fprintf(lx->OUT, "</TD>");

    showKeyAndEndItem(lx, key, locale);
    ures_close(array);
    ures_close(item);
}

/* Show a resource that has a short (*Abbreviations) and long (*Names) version ---------------- */
/* modified showArray */
void showShortLong( LXContext *lx, UResourceBundle *rb, const char *locale, const char *keyStem, const UChar *shortName, const UChar *longName, int32_t num )
{
    UErrorCode status = U_ZERO_ERROR;
    UErrorCode shortStatus = U_ZERO_ERROR, longStatus = U_ZERO_ERROR;
    char       shortKey[100], longKey[100];
    UResourceBundle  *shortArray = NULL, *longArray = NULL, *item = NULL;
    int32_t len;
    const UChar *s  = 0;
    int i;

    showKeyAndStartItem(lx, keyStem, NULL, locale, FALSE, U_ZERO_ERROR); /* No status possible  because we have two items */

    sprintf(shortKey, "%sNames", keyStem);
    sprintf(longKey,  "%sAbbreviations", keyStem);

    /* pre load the status of these things */
    shortArray = ures_getByKey(rb, shortKey, shortArray, &shortStatus);
    longArray  = ures_getByKey(rb, longKey, longArray, &longStatus);
    item       = ures_getByIndex(shortArray, 0, item, &shortStatus);
    item       = ures_getByIndex(longArray, 0, item, &longStatus);

    u_fprintf(lx->OUT, "<TABLE BORDER=1 WIDTH=100%% HEIGHT=100%%><TR><TD><B>#</B></TD><TD><B>%U</B> ", shortName);
    explainStatus(lx, shortStatus, keyStem);
    u_fprintf(lx->OUT, "</TD><TD><B>%U</B> ", longName);
    explainStatus(lx, longStatus, keyStem);
    u_fprintf(lx->OUT, "</TD></TR>\r\n");

 
    for(i=0;i<num;i++)
    {
        char *key;

        u_fprintf(lx->OUT, " <TR><TD>%d</TD><TD>", i);

        /* get the normal name */
        status = U_ZERO_ERROR;
        key = longKey;
        item = ures_getByIndex(longArray, i, item, &status);
        s    = ures_getString(item, &len, &status);

        if(i==0)
            longStatus = status;
  
        if(U_SUCCESS(status))
            u_fprintf(lx->OUT, " %U ", s);
        else
            explainStatus(lx, status, keyStem); /* if there was an error */

        u_fprintf(lx->OUT, "</TD><TD>");

        /* get the short name */
        status = U_ZERO_ERROR;
        key = shortKey;
        item = ures_getByIndex(shortArray, i, item, &status);
        s    = ures_getString(item, &len, &status);

        if(i==0)
            shortStatus = status;
  
        if(U_SUCCESS(status))
            u_fprintf(lx->OUT, " %U ", s);
        else
            explainStatus(lx, status, keyStem); /* if there was an error */

        u_fprintf(lx->OUT, "</TD></TR>");
    }

    u_fprintf(lx->OUT, "</TABLE>");
    u_fprintf(lx->OUT, "</TD>");

    showKeyAndEndItem(lx, keyStem, locale);
    ures_close(item);
    ures_close(shortArray);
    ures_close(longArray);

}

/* Show a 2d array  -------------------------------------------------------------------*/

void show2dArrayWithDescription( LXContext *lx, UResourceBundle *rb, const char *locale, const UChar *desc[], const char *queryString, const char *key )
{
    UErrorCode status = U_ZERO_ERROR;
    UErrorCode firstStatus;
    const UChar *s  = 0;
    int32_t h,v;
    int32_t rows,cols;
    UBool bigString = FALSE; /* is it big? */
    UBool userRequested = FALSE; /* Did the user request this string? */
    UBool isTZ = FALSE; /* do special TZ processing */
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
        bigString = TRUE;
        userRequested = didUserAskForKey(key, queryString);
    }

    showKeyAndStartItem(lx, key, NULL, locale, TRUE, status);

    if(bigString && !userRequested) /* it's hidden. */
    {
        /* WIERD!! outputting '&#' through UTF8 seems to be -> '?' or something */
	u_fprintf(lx->OUT, "<A HREF=\"?_=%s&SHOW%s=1#%s\"><IMG BORDER=0 WIDTH=16 HEIGHT=16 SRC=\"../_/closed.gif\" ALT=\"\">%U</A>\r\n<P>", locale, key,key, FSWF("bigStringClickToShow","(Omitted due to size. Click here to show.)"));
    }
    else
    {
        if(bigString)
	{
            u_fprintf(lx->OUT, "<A HREF=\"?_=%s#%s\"><IMG border=0 width=16 height=16 SRC=\"../_/opened.gif\" ALT=\"\"> %U</A><P>\r\n",
                      locale,
                      key,
                      FSWF("bigStringHide", "Hide"));
	}

        firstStatus = status;  /* save this for the next column.. */

        if(U_SUCCESS(status))
        {	
            u_fprintf(lx->OUT,"<TABLE BORDER=1>\r\n");
	  
            /* print the top row */
            u_fprintf(lx->OUT,"<TR><TD></TD>");
            for(h=0;h<cols;h++)
            {
                if(!desc[h])
                    break;

                u_fprintf(lx->OUT, "<TD><B>");
                if(h == 0)
                {
                    u_fprintf(lx->OUT, "<A TARGET=lx_tz HREF=\"http://oss.software.ibm.com/cvs/icu/~checkout~/icu/docs/tz.htm?content-type=text/html\">");
                }
                u_fprintf(lx->OUT,"%U", desc[h]);
                if(h == 0)
                {
                    u_fprintf(lx->OUT, "</A>");
                }
                u_fprintf(lx->OUT, "</B></TD>\r\n");
            }
            u_fprintf(lx->OUT,"</TR>\r\n");
	  
            for(v=0;v<rows;v++)
            {
                const UChar *zn = NULL;
	      
                row   = ures_getByIndex(array, v, row, &status);

                if(U_FAILURE(status)) {
                    u_fprintf(lx->OUT, "<TR><TD><B>ERR: ");
                    explainStatus(lx, status, NULL);
                    status = U_ZERO_ERROR;
                    continue;
                }

                u_fprintf(lx->OUT,"<TR><TD><B>%d</B></TD>", v);
                for(h=0;h<cols;h++)
                {
                    status = U_ZERO_ERROR;

		  
#ifndef LX_NO_USE_UTIMZONE
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
                        u_fprintf(lx->OUT, "<TD>%U</TD>\r\n", s);
                    else
                    {
                        u_fprintf(lx->OUT, "<TD BGCOLOR=" kStatusBG " VALIGN=TOP>");
                        explainStatus(lx, status, key);
                        u_fprintf(lx->OUT, "</TD>\r\n");
                        break;
                    }
                }
                u_fprintf(lx->OUT, "</TR>\r\n");
            }
            u_fprintf(lx->OUT, "</TABLE>\r\n<BR>");
        }
    }

    ures_close(item);
    ures_close(row);
    ures_close(array);
    showKeyAndEndItem(lx, key, locale);
}

/* Show a Tagged Array  -------------------------------------------------------------------*/

void showTaggedArray( LXContext *lx, UResourceBundle *rb, const char *locale, const char *queryString, const char *key )
{
    UErrorCode status = U_ZERO_ERROR;
    UErrorCode firstStatus;
    const UChar *s  = 0;
    int32_t v;
    int32_t rows;
    UBool bigString = FALSE; /* is it big? */
    UBool userRequested = FALSE; /* Did the user request this string? */
    int32_t len;
    UResourceBundle *item = NULL;

    rows = ures_countArrayItems(rb, key, &status);

    if(U_SUCCESS(status) && ((rows > kShow2dArrayRowCutoff)))
    {
        bigString = TRUE;
        userRequested = didUserAskForKey(key, queryString);
    }

    showKeyAndStartItem(lx, key, NULL, locale, TRUE, status);

    if(bigString && !userRequested) /* it's hidden. */
    {
        /* WIERD!! outputting '&#' through UTF8 seems to be -> '?' or something */
        u_fprintf(lx->OUT, "<A HREF=\"?_=%s&SHOW%s=1#%s\"><IMG BORDER=0 WIDTH=16 HEIGHT=16 SRC=\"../_/closed.gif\" ALT=\"\">%U</A>\r\n<P>", locale, key,key, FSWF("bigStringClickToShow","(Omitted due to size. Click here to show.)"));
    }
    else
    {
        if(bigString)
        {
            u_fprintf(lx->OUT, "<A HREF=\"?_=%s#%s\"><IMG border=0 width=16 height=16 SRC=\"../_/opened.gif\" ALT=\"\"> %U</A><P>\r\n",
                      locale,
                      key,
                      FSWF("bigStringHide", "Hide"));
        }

        firstStatus = status;  /* save this for the next column.. */

        if(U_SUCCESS(status))
        {	
            UResourceBundle *tagged =  ures_getByKey(rb, key, NULL, &status);
            UResourceBundle *defaultTagged = NULL;
            UResourceBundle *taggedItem = NULL;
            if(lx->defaultRB)
                defaultTagged =  ures_getByKey(lx->defaultRB, key, NULL, &status);
            u_fprintf(lx->OUT,"<TABLE BORDER=1>\r\n");
	  
            /* print the top row */
            u_fprintf(lx->OUT,"<TR><TD><B>%U</B></TD><TD><I>%U</I></TD><TD><B>%U</B></TD></TR>",
                      FSWF("taggedarrayTag", "Tag"),
                      defaultLanguageDisplayName(lx),
                      lx->curLocale ? lx->curLocale->ustr : FSWF("none","None"));
	  
            for(v=0;v<rows;v++)
	    {
                const char *tag;

                status = U_ZERO_ERROR;
                taggedItem = ures_getByIndex(tagged, v, NULL, &status);
                tag = ures_getKey(taggedItem);
	      
                /*tag = ures_getTaggedArrayTag(rb, key, v, &status);*/
                if(!tag)
                    break;
	      
                u_fprintf(lx->OUT,"<TR> ");

                if(U_SUCCESS(status))
		{
                    u_fprintf(lx->OUT, "<TD><TT>%s</TT></TD> ", tag);
		  
                    if(lx->defaultRB)
		    {
                        item = ures_getByKey(defaultTagged, tag, item, &status);
                        s = ures_getString(item, &len, &status);

                        if(s)
                            u_fprintf(lx->OUT, "<TD><I>%U</I></TD>", s);
                        else
                            u_fprintf(lx->OUT, "<TD></TD>");
		    }
                    else
			u_fprintf(lx->OUT, "<TD></TD>");
		  
                    status = U_ZERO_ERROR;

                    s = ures_getString(taggedItem, &len, &status);

                    if(s)
                    {
#if 0
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

		    
                        u_fprintf(lx->OUT, "<TD>%U [a]</TD>", junk);
#else
                        u_fprintf(lx->OUT, "<TD>%U</TD>", s);
#endif
                    }
                    else
		    {
                        u_fprintf(lx->OUT, "<TD BGCOLOR=" kStatusBG " VALIGN=TOP>");
                        explainStatus(lx, status, key);
                        u_fprintf(lx->OUT, "</TD>\r\n");
		    }
		}
                u_fprintf(lx->OUT, "</TR>\r\n");
	    }
            u_fprintf(lx->OUT, "</TABLE>\r\n<BR>");
            ures_close(taggedItem); /* todo: mem. management? */
	}
    }

    u_fprintf(lx->OUT, "</TD>");
    showKeyAndEndItem(lx, key, locale);
}
