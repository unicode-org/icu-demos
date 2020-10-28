/**********************************************************************
*   Copyright (C) 1999-2015, International Business Machines
*   Corporation and others.  All Rights Reserved.
***********************************************************************/

#include "locexp.h"
#include "unicode/udata.h"
#include "unicode/usearch.h"
/* #include "uresimp.h" */

#define G7COUNT 8  /* all 8 of the g7 locales. showSort() */
static const char   G7s[G7COUNT][10] = { "de_DE", "en_GB", "en_US", "fr_CA", "fr_FR", "it_IT", "ja_JP", "sv_SE" };
#define LX_UCOL_SAMPLE_STRS "bad|Bad|Bat|bat|b\\u00E4d|B\\u00E4d|b\\u00E4t|B\\u00E4t|c\\u00f4t\\u00e9|cot\\u00e9|c\\u00f4te|cote|" "black-bird|blackbird|black-birds|blackbirds"

#define isNL(x) ((x)==0x000D || (x)==0x000A || (x)==0x2029 || (x)==0x2028)

#define LX_UCOL_LAST ((UColAttributeValue)-1)      /** internal value for default */
#define LX_UCOL_DEFAULT ((UColAttributeValue)-2)   /** internal value for the end of a list */

void stripComments(UChar *ch) 
{
  int32_t quoteCount = 0;


  while(*ch) {
    if(isNL(*ch)) {
      ch++;
      quoteCount = 0;
      continue;
    } else if(*ch == '\\') {
      ch++;
      if(*ch) {
        ch++; /* skip whatever follows */
      }     
    } else if(*ch == '\'') {
      ch++;
      quoteCount++;
    } else if(*ch == '#') {
      if(quoteCount%2) {
        ch++;
      } else while(*ch && !isNL(*ch)) {
        quoteCount = 0;
        *ch = ' ';
        ch++;
      }
    } else {
      ch++;
    }
  }
}

/* routines having to do with the sort sample */
void printRuleString(LXContext *lx, const UChar*s) 
{
  /* push HTML state, turn off <b> tags in escape */
  UChar lastChar = 0;
  UBool oldHtml;

  oldHtml = lx->backslashCtx.html;
  lx->backslashCtx.html = false;

  while(*s) {
    switch(*s) {
    case '\r': if(!isNL(s[1])) { u_fprintf(lx->OUT, "\r\n"); } break;
    case '\n': if(!isNL(s[1])) { u_fprintf(lx->OUT, "\r\n"); } break;
    case '&':  u_fprintf(lx->OUT, "\n&amp;"); break;
    case ']': u_fprintf(lx->OUT, "]\n"); break;
    case '<': 
    {
      /*
        if((lastChar != '<') && (s[1] != '<')) {
        u_fprintf(lx->OUT, "\n");
        }
      */
      u_fprintf(lx->OUT, "&lt;"); 
      break;
    }
    case '>': u_fprintf(lx->OUT, "&gt;"); break;
    default: u_fputc(*s,lx->OUT);
    }
    lastChar = *s;
    s++;
  }

  /* pop state */
  lx->backslashCtx.html = true;
}

/**
 * Display a single 'word' (line of sorted text)
 * @param lx the locale explorer context
 * @param sort the USort object being displayed - or NULL.
 * @param num the index into the USort. (ignored if sort == NULL)
 * @param chars NULL terminated UChars to be output for this word
 */
void showSort_outputWord(LXContext *lx, USort *aSort, int32_t num, const UChar* chars)
{
  UBool lineAbove;  /* Show box above the current line? */
  UBool lineBelow;  /* Show box below current line? */
  static int32_t evenOdd = 0;

  if(aSort == NULL) {
    /* no USort */
    lineAbove = true;
    lineBelow = true;
  } else {
    /* calculate lineAbove */
    if(num == 0) {  
      evenOdd = 0;
      lineAbove = true; /* first item - always a line above */
    } else if( aSort->lines[num-1].keySize == aSort->lines[num].keySize  &&
               !memcmp(aSort->lines[num-1].key,
                       aSort->lines[num].key,
                       aSort->lines[num].keySize) ) {
      /* item is identical to previous - no line above */
      lineAbove = false;
    } else {
      /* different than prev - line above. */
      lineAbove = true;
    }

    /* calculate lineBelow */
    if(num == (aSort->count-1)) {
      lineBelow = true; /* last item - always line below */
    } else if( aSort->lines[num+1].keySize == aSort->lines[num].keySize  &&
               !memcmp(aSort->lines[num+1].key,
                       aSort->lines[num].key,
                       aSort->lines[num].keySize) ) {
      /* item is identical to next - no line below */
      lineBelow = false;
    } else {
      /* different than next - line below. */
      lineBelow = true;
    }
  }



#if 1
  if(lineAbove) { u_fprintf(lx->OUT, "<div class=\"box%d\">\r\n", (evenOdd++)%2 ); }
  u_fprintf(lx->OUT, "<tt class=\"count\">%02d:</tt>&nbsp;%S", (aSort==NULL)?num+1:(int32_t)aSort->lines[num].userData, chars);

  {
    int32_t ii;
    if( !hasQueryField(lx,"hideCollKey") && lineBelow  ) {
      u_fprintf(lx->OUT, "<br /><tt class=\"key\">");
      if(aSort) {
        for(ii=0;ii<aSort->lines[num].keySize;ii++) {
          u_fprintf(lx->OUT, "%02x ", aSort->lines[num].key[ii]);
        }
      }
      u_fprintf(lx->OUT, "</tt>\r\n");
    }
  }
  if(lineBelow) { u_fprintf(lx->OUT, "\r\n</div>\r\n"); } else { u_fprintf(lx->OUT, "<br />\r\n"); }
  /* CSS mode */
#else
  u_fprintf(lx->OUT, "%s%s%02d.%S%s%s<br />\n",
            ((lineBelow)?"<u>":""),
            ((lineAbove)?"":"<font color=\"#AAAAAA\">"),
            (aSort==NULL)?num:(int32_t)aSort->lines[num].userData,
            chars,
            ((lineAbove)?"":"</font>"),
            ((lineBelow)?"</u>":"")
            );

#endif
}


const UChar *showSort_attributeName(UColAttribute attrib)
{
  static const UChar nulls[] = { 0x0000 };

  switch(attrib)
  {
    case UCOL_FRENCH_COLLATION: return FSWF("UCOL_FRENCH_COLLATION","French accents");
    case UCOL_ALTERNATE_HANDLING: return FSWF("UCOL_ALTERNATE_HANDLING","Alternate handling");
    case UCOL_CASE_FIRST: return FSWF("UCOL_CASE_FIRST","Case first");
    case UCOL_CASE_LEVEL: return FSWF("UCOL_CASE_LEVEL","Add case level");
    case UCOL_NORMALIZATION_MODE: return FSWF("UCOL_NORMALIZATION_MODE","Full normalization mode");
    case UCOL_STRENGTH: return FSWF("UCOL_STRENGTH","Strength");
    case UCOL_HIRAGANA_QUATERNARY_MODE: return FSWF("UCOL_HIRAGANA__QUATERNARY_MODE","Add Hiragana Level");
  case UCOL_NUMERIC_COLLATION: return FSWF("UCOL_NUMERIC_COLLATION", "Numeric collation");
    default:  return nulls;
  }
}

static const UChar nulls[] = { 0x0000 };

const UChar *showSort_attributeVal(UColAttributeValue val)
{

  switch(val)
  {
  case LX_UCOL_DEFAULT:  return FSWF("LX_UCOL_DEFAULT", "Don't change");
  /* Duplicate:  UCOL_CE_STRENGTH_LIMIT */
  case UCOL_LOWER_FIRST : return FSWF("UCOL_LOWER_FIRST","Force Lowercase first");
  case UCOL_OFF : return FSWF("UCOL_OFF","Off");
  case UCOL_ON : return FSWF("UCOL_ON","On");
/*  case UCOL_ON_WITHOUT_HANGUL : return FSWF("UCOL_ON_WITHOUT_HANGUL","On,without Hangul"); ????  */
  case UCOL_PRIMARY   :   return FSWF("UCOL_PRIMARY","L1 = Base Letters");
  case UCOL_SECONDARY : return FSWF("UCOL_SECONDARY","L2 = L1 + Accents");
  case UCOL_TERTIARY  :  return FSWF("UCOL_TERTIARY","L3 = L2 + Case");
  case UCOL_QUATERNARY:return FSWF("UCOL_QUATERNARY","L4 = L3 + Punct.");
  case UCOL_IDENTICAL:  return FSWF("UCOL_IDENTICAL","L5 = L4 + Codepoint");

  case UCOL_SHIFTED : return FSWF("UCOL_SHIFTED","Ignore Punctuation");
  case UCOL_NON_IGNORABLE : return FSWF("UCOL_NON_IGNORABLE","Punctuation = Base");
 /* Duplicate: UCOL_STRENGTH_LIMIT */
  case UCOL_UPPER_FIRST : return FSWF("UCOL_UPPER_FIRST","Force Uppercase first");
  default: return nulls;
  }  
}

const UChar *showSort_attributeAndValue(UChar *buf, int32_t bufSize, UColAttributeValue val, UColAttribute attribute) {
    const UChar *name;
    const UChar *value;

    name = showSort_attributeName(attribute);
    value = showSort_attributeVal(val);

    if(u_snprintf(buf, bufSize, "%S: %S", name, value) ==bufSize) {
        return showSort_attributeVal(val);
    } else {
        /*buf[bufSize-1]=0;*/
        return buf;
    }
}

const UChar *showSort_attributeValX(UChar *buf, int32_t bufSize, UColAttributeValue val, UColAttribute attribute) {
 /* special cases */
 if(val == UCOL_OFF) {
   switch(attribute) {
    case UCOL_CASE_FIRST:
        return FSWF("UCOL_noforcecase","Don't force case");
     /* default: fall through.. */
        default:
            return showSort_attributeAndValue(buf, bufSize, val, attribute);
   }
 } else if(val == UCOL_ON) {
    return showSort_attributeAndValue(buf, bufSize, val, attribute);
 }
 return showSort_attributeVal(val);
}

const UChar *showSort_attributeValDefault(UChar *buf, int32_t bufSize, UColAttributeValue val, UColAttributeValue defVal, UColAttribute attribute) {
  if(val != LX_UCOL_DEFAULT) {
    return showSort_attributeValX(buf, bufSize, val, attribute);
  } else {
    const UChar *defMsg0;
    const UChar *defStr;
    const UChar *defMsg1;
    UChar *subBuf;
    int32_t needed;

    subBuf = malloc((bufSize+1)*sizeof(subBuf[0]));
    defMsg0 = FSWF("LX_UCOL_DEFAULTMSG0", "Default (");
    defStr = showSort_attributeValX(subBuf, bufSize, defVal, attribute);
    defMsg1 = FSWF("LX_UCOL_DEFAULTMSG1", ")");

    
    needed = u_strlen(defMsg0)+u_strlen(defStr)+u_strlen(defMsg1);
    if((needed+1)>bufSize) {
        free(subBuf);
        return showSort_attributeValX(buf, bufSize, val, attribute);
    } else {
        u_strcpy(buf, defMsg0);
        u_strcat(buf, defStr);
        u_strcat(buf, defMsg1);
        free(subBuf);
        return buf;
    }
  }
}


/**
 * Show attributes of the collator 
 */
void showSort_attrib(LXContext *lx, const char *locale, UCollator *ourCollator)
{
  UErrorCode  subStatus = U_ZERO_ERROR;

  UCollator *newCollator = NULL;

  if(ourCollator==NULL) {
    newCollator = ucol_open(locale, &subStatus);
    ourCollator = newCollator;
  }

  /* ------------------------------------ */
  
  if(U_FAILURE(subStatus))
  { 
    explainStatus( lx, subStatus, NULL);
  }
  else
  {
    UColAttributeValue val;
    UColAttribute      attrib;
    
    u_fprintf(lx->OUT, "<h4>%S</h4><ul>\r\n", FSWF("usort_attrib", "Attributes"));
    for(attrib=UCOL_FRENCH_COLLATION; attrib < UCOL_ATTRIBUTE_COUNT;
        attrib++) {
      subStatus = U_ZERO_ERROR;
      val = ucol_getAttribute(ourCollator,
                              attrib,
                              &subStatus);
      u_fprintf(lx->OUT, "  <li><b>%S</b>: %S\r\n",
                showSort_attributeName(attrib),
                showSort_attributeVal(val));
    }
    u_fprintf(lx->OUT, "</ul>\r\n");
    ucol_close(newCollator);
  }
}

void appendStringTo(LXContext *lx, UChar *someText, int32_t someTextLen, const UChar * str)
{
  UChar term[] = { '|', 0x0000 }; /* asciism */
  int32_t   left;

  left = someTextLen - u_strlen(someText);
  left += 2; /* null, and pipe | */
  
  if(left > u_strlen(someText)) {
    u_strcat(someText, str);
    u_strcat(someText, term);
  }
}


static void appendSomeOfArrayTo(LXContext *lx, UResourceBundle *aBundle, UChar *someText, int32_t someTextLen, const char *key, int32_t howmany)
{
  UErrorCode status = U_ZERO_ERROR;
  UResourceBundle *sub = NULL;
  const UChar *str;
  int32_t len2;
  const char *key2;
  char *keytmp = strdup(key);
  
  sub = ures_findSubResource(aBundle, keytmp, sub, &status);
  free(keytmp);
  if(U_FAILURE(status)) {
    return;
  }

  while(howmany-- 
        && (str=ures_getNextString(sub, &len2, &key2, &status)) 
        && U_SUCCESS(status)) {
    appendStringTo(lx, someText, someTextLen, str);
  }
}

const char *sortLoadText(LXContext *lx, char *inputChars, const char *locale, UChar *strChars)
{
  const char *text;
  int32_t length;
  

  /* pull out the text to be sorted. ===========================================================
   */
  text = queryField(lx,"str");
  if(!text || !*text) {
    /* attempt load from RB */
    const UChar *sampleString = NULL;
    UResourceBundle *sampleRB;
    UErrorCode sampleStatus = U_ZERO_ERROR;
    int32_t len;
    
    /* try to get localized sample */
    sampleRB = ures_open(FSWF_bundlePath(), locale, &sampleStatus);
    if(!U_FAILURE(sampleStatus))  {
      sampleString = ures_getStringByKey(sampleRB, "EXPLORE_CollationElements_sampleString", &len, &sampleStatus);
      ures_close(sampleRB);
    }
    
    /* if no localized sample, use the bad bat */
    if(U_FAILURE(sampleStatus) || !sampleString) {
      sampleString =  FSWF(/*NOEXTRACT*/ "EXPLORE_CollationElements_sampleString_default",LX_UCOL_SAMPLE_STRS);
    }
    
    /* append some interesting stuff, always */
    {
      UChar  *someText;
      int32_t someTextLen;
      UResourceBundle *aBundle = NULL;

      someTextLen = u_strlen(sampleString) + 1024 + 768;
      someText = malloc(someTextLen * sizeof(someText[0]));
      sampleStatus = U_ZERO_ERROR;
      aBundle = ures_open(NULL, locale, &sampleStatus);

      someText[0] =0 ;

      if(U_SUCCESS(sampleStatus)) {
        appendSomeOfArrayTo(lx, aBundle, someText, someTextLen, "calendar/gregorian/dayNames/format/wide", 3);
        appendSomeOfArrayTo(lx, aBundle, someText, someTextLen, "calendar/gregorian/monthNames/format/wide", 3);
        appendSomeOfArrayTo(lx, aBundle, someText, someTextLen, "Languages", 3);
      }

      appendStringTo(lx, someText, someTextLen, sampleString);

      someText[someTextLen-1]=0;

      text = createEscapedSortList(someText);

      free(someText);
      
      /* sampleString2 = sampleString;  *//* fallback */
      
      /* get some more interesting text.   */
    }
  } /* end if no user-specified text */

  if(text && *text)
  {
    const char *aConv = lx->convRequested;
    if(!aConv||!*aConv) {
        aConv = "utf-8";
    }
    strChars[0]=0;
    u_fprintf(lx->OUT, "<!-- [enc: %s] -->", aConv);
    unescapeAndDecodeQueryField_enc(strChars, SORTSIZE,
                                    text, aConv );
    
    length = strlen(text);
    
    if(length > (SORTSIZE-1))
      length = SORTSIZE-1; /* safety ! */
    
    strncpy(inputChars, text, length); /* make a copy for future use */
    inputChars[length] = 0;
  }
  else
  {
    inputChars[0] = 0;  /* no text to process */
    strChars[0] =0;
	text=0;
  }

  return text;
}

const char *sortLoadSearch(LXContext *lx, char *inputChars, const char *locale, UChar *strChars)
{
  const char *text;
  int32_t length;
  

  /* pull out the text to be sorted. ===========================================================
   */
  text = queryField(lx,"sch");
  if(!text || !*text) {
    /* attempt load from RB */
    const UChar *sampleString = NULL;
    UResourceBundle *sampleRB;
    UErrorCode sampleStatus = U_ZERO_ERROR;
    int32_t len;
    
    /* try to get localized sample */
    sampleRB = ures_open(FSWF_bundlePath(), locale, &sampleStatus);
    if(!U_FAILURE(sampleStatus))  {
      sampleString = ures_getStringByKey(sampleRB, "EXPLORE_CollationElements_sampleString", &len, &sampleStatus);
      ures_close(sampleRB);
    }
    text="";
  } /* end if no user-specified text */

  if(text && *text)
  {
    const char *aConv = lx->convRequested;
    if(!aConv||!*aConv) {
        aConv = "utf-8";
    }
    strChars[0]=0;
    u_fprintf(lx->OUT, "<!-- [enc: %s] -->", aConv);
    unescapeAndDecodeQueryField_enc(strChars, SORTSIZE,
                                    text, aConv );
    
    length = strlen(text);
    
    if(length > (SORTSIZE-1))
      length = SORTSIZE-1; /* safety ! */
    
    strncpy(inputChars, text, length); /* make a copy for future use */
    inputChars[length] = 0;
  }
  else
  {
    inputChars[0] = 0;  /* no text to process */
    strChars[0] =0;
	text=0;
  }

  return text;
}



static void showSort_doCustom(LXContext *lx, UColAttribute attribute, UCollator *customCollator, const char *field, const UColAttributeValue values[], UBool lxSortReset)
{
   UChar attribBuf[1024]; /* 1 kibibyte */
   const char *ss;
   UErrorCode status = U_ZERO_ERROR;
   UColAttributeValue  value, defaultvalue;

    int i;
/*    u_fprintf(lx->OUT, "<span class=\"optionname\"><b>%S</b></span><br />\n", showSort_attributeName(attribute)); */
    status = U_ZERO_ERROR;
    if(customCollator==NULL) {
       u_fprintf(lx->OUT, "<!-- customCollator == NULL -->");
    }
    defaultvalue = value = ucol_getAttribute(customCollator, attribute, &status);
    status = U_ZERO_ERROR; /* we're prepared to just let the collator fail later. */
    if(!lxSortReset && (ss=queryField(lx, field))) {
        value = atoi(ss);
    } else {
        value = LX_UCOL_DEFAULT;
    }

    u_fprintf(lx->OUT, "<label><select id=\"options_%s\" class=\"wide\" name=\"%s\">\r\n", field, field);

    for(i = 0; values[i]!=LX_UCOL_LAST; i++)
    {
      u_fprintf(lx->OUT, "<option %s value=\"%d\">%S</option>\r\n",
                (values[i]==value)?"selected=\"selected\"":"",
                values[i],
                showSort_attributeValDefault(attribBuf, 1024, values[i], defaultvalue, attribute));
    }
    u_fprintf(lx->OUT, "</select></label><br />\r\n");

    status = U_ZERO_ERROR;
    if(LX_UCOL_DEFAULT != value) {
      ucol_setAttribute(customCollator, attribute, value, &status);
      if(status != U_ZERO_ERROR) { u_fprintf(lx->OUT, "<b>(%s)</b>\r\n", u_errorName(status));}
    }
    status = U_ZERO_ERROR; /* we're prepared to just let the collator fail later. */
}


/**
 * Demonstrate sorting.  Call usort as a library to do the actual sorting.
 * URL description:
 *    - if the 'locale' is g7:  g7 sorting (http://...../localeexplorer/?_=g7& ... )
 *    - if the tag 'cust' is present, custom:   ?.... &cust=...&... 
 *          - strength=n  [ 0..15 an enum for strength ]
 *          - Boolean options, present or not:  fr=, dcmp=, case=
 *    - EXPLORE_CollationElements= takes the text to be tested, in display codepage BUT with \u format supported.
 *       Ex:  '%5Cu0064'  ==>  \u0064 = 'd'
 * @param locale The view locale.
 */

void showSort(LXContext *lx, const char *locale)
{
  char   inputChars[SORTSIZE];
  char   schInputChars[SORTSIZE];
  char   ruleUrlChars[SORTSIZE] = "";
  const char *text;
  const char *schText;
  UChar  strChars[SORTSIZE];
  UChar  schChars[SORTSIZE];
  UChar  ruleChars[SORTSIZE]; /* Custom rule UChars */
  UChar  fixedRuleChars[SORTSIZE]; /* Custom rule UChars without comments */
  UBool lxCustSortOpts = false;  /* if true, then user has approved the custom settings.  If false, go with defaults.  See "lxCustSortOpts=" tag. */
  /* The 'g7' locales to demonstrate. Note that there eight.  */
  UErrorCode status = U_ZERO_ERROR;  

  /* For customization: */
  UColAttributeValue  customStrength = UCOL_DEFAULT;
  USort              *customSort     = NULL;
  UCollator          *customCollator = NULL;

  UBool isG7 = false;

  UBool lxSortReset = false;

  if(hasQueryField(lx,"lxCustSortOpts")) {
    lxCustSortOpts = true;
  }

  if(hasQueryField(lx,"lxSortReset")) {
    lxCustSortOpts = false;
    lxSortReset = true;
  }

  u_fprintf(lx->OUT, "<p><b>%S</b></p>", FSWF("usortWhat","This example demonstrates sorting (collation) in this locale."));

#if defined (LX_DEBUG)
  {
    char funcLoc[1024];
    UErrorCode funcStat = U_ZERO_ERROR;
    UBool isAvail;
    
    ures_getFunctionalEquivalent(funcLoc, 1023, U_ICUDATA_NAME U_TREE_SEPARATOR_STRING  "coll", "collations", "collation", locale, &isAvail, false, &funcStat);
    u_fprintf(lx->OUT, "<p>FE: %s</p>\n", funcLoc);
  }
#endif

  strChars[0] = 0;
  schChars[0] = 0;

  if(strstr(locale,"g7") != NULL)
  {
    isG7 = true;
  }

  /* load text to be sorted */
  text = sortLoadText(lx, inputChars, locale, strChars);
  schText = sortLoadSearch(lx, schInputChars, locale, schChars);


  /* look for custom rules =========================================================================== */
    /* ? erases txt here */
/*    u_fprintf(lx->OUT, "T: (%S), I: (%s)<br />", strChars, inputChars); */
  text = NULL;
  ruleChars[0] = 0;

  if(!lxSortReset) {
    text = queryField(lx, "usortRules");
  }

  if(text) {
    int32_t length;
    unescapeAndDecodeQueryField_enc(ruleChars, SORTSIZE, 
                                    text, lx->convRequested);
    length = strlen(text);
      
    if(length > (SORTSIZE-1)) {
      length = SORTSIZE-1; /* safety ! */
    }

    strncpy(ruleUrlChars, text, length); /* make a copy for future use */
    ruleUrlChars[length] = 0;
  } else {
    ruleChars[0] = 0;
  }


/*  u_fprintf(lx->OUT, "%S<P>\r\n", FSWF("EXPLORE_CollationElements_Prompt", "Type in some lines of text to be sorted.")); */

  /* Table begin ============================================================================== */
  u_fprintf(lx->OUT, "<form method=\"post\" action=\"%s\">", 
            getLXBaseURL(lx, kNO_URL));
  u_fprintf(lx->OUT, "<table id=\"main\" class=\"wide\" border=\"1\">\r\n");
  /* the source box  =======================================================================================*/
  u_fprintf(lx->OUT, " <tr> <td %s ><label for=\"source\"><b>%S</b></label>\r\n", /* top is only 1 row for now */
            isG7?" rowspan=\"2\" ": /* width=\"22%\" */ " rowspan=\"1\" ",
            FSWF("usortSource", "Source"));

  u_fprintf(lx->OUT, "<p><textarea id=\"source\" %s rows=\"20\" name=\"str\">", 
            (isG7?"":" class=\"wide\" cols=\"20\" "));
  
  writeEscaped(lx, strChars); 
  /* if(*inputChars)
     u_fprintf(lx->OUT, "%s", inputChars);  */
  
  u_fprintf(lx->OUT, "</textarea>\r\n");


  /* the search box  =======================================================================================*/
  if(!isG7) {
      const char *overlap;
      UBool doOverlap = false;
      overlap = queryField(lx, "usearch_overlap");
      doOverlap = overlap && *overlap;
      
      u_fprintf(lx->OUT, "<label for=\"search\"><b>%S</b></label>\r\n", /* top is only 1 row for now */
                FSWF("usortSearch", "Search for: (or leave blank for no search)"));
      u_fprintf(lx->OUT, "<label><input type=checkbox %s name='usearch_overlap'>Overlap?</label>\n", 
        doOverlap?"checked":"");
      u_fprintf(lx->OUT, "<p><textarea id=\"search\" %s rows=\"4\" name=\"sch\">", 
                (0?"":" class=\"wide\" cols=\"20\" "));
      
      writeEscaped(lx, schChars); 
      
      u_fprintf(lx->OUT, "</textarea>\r\n");

  }

  if(!isG7) {
    u_fprintf(lx->OUT, "</p>\n</td>\r\n");
    u_fprintf(lx->OUT, "<td rowspan=\"1\">"); /* two :   buttons +  options*/ /* top only 1 row for now - fixme */
  }
  /* submit buttons ===========================================================================*/
  u_fprintf(lx->OUT, "<input type=\"submit\" %s value=\"%S\" /><br />\r\n",
            isG7?"":"class=\"wide\" ",
            FSWF("EXPLORE_CollationElements_Sort", "Sort"));

  u_fprintf(lx->OUT, "<input id=\"hidecollkey\" type=\"checkbox\" %s name=\"hideCollKey\" /><label for=\"showcollkey\">%S</label><br />\r\n",
            (hasQueryField(lx,"hideCollKey")?"checked=\"checked\"":""),
            FSWF("EXPLORE_CollationElements_HideCollKey", "Hide Collation Key"));

/*  u_fprintf(lx->OUT, "</td>\r\n"); */
  
  /* Here, 'configuration information' at the top of the page. ==================== */
  if(!isG7) { /* was kSimpleMode */
      UErrorCode customError = U_ZERO_ERROR;

      u_fprintf(lx->OUT, "<br /><hr width=\"20%%\" />\r\n");
      u_fprintf(lx->OUT, "<label for=\"options\"><b><font size='+1'>%S</font></b></label><br />\r\n", FSWF("EXPLORE_CollationElements_options", "Options"));
      
      if ( ruleChars[0] ) { /* custom rules */
        UCollator *coll;
        UParseError parseErr;

        u_strcpy(fixedRuleChars, ruleChars);
        stripComments(fixedRuleChars);
        /* u_fprintf(lx->OUT, "R [<pre>%S</pre>]<br />\r\n", fixedRuleChars); */

        coll = ucol_openRules ( fixedRuleChars, -1, 
                                UCOL_DEFAULT, UCOL_DEFAULT, /* attr val, str */
                                &parseErr,
                                &customError);

        customSort = usort_openWithCollator(coll, true, &customError);
        
        if(U_FAILURE(customError) || !customSort) {
          u_fprintf(lx->OUT, "<B>%S %s %s:</B>", 
                    FSWF("showSort_cantOpenCustomConverter", "Could not open a custom usort/collator for the following locale and reason"), locale);
          explainStatus(lx, customError, NULL); 
          
          u_fprintf(lx->OUT,"<br /><table border=\"1\"><tr><td>%S (%s):</td></tr><tr>",
                    FSWF("showSort_Context", "Error shown by this mark:"),
                    "<font color=red><u>|</u></font>");

          u_fprintf(lx->OUT, "<td><tt>");
          printRuleString(lx, parseErr.preContext);
          u_fprintf(lx->OUT, "<font color=red><u>|</u></font>");
          printRuleString(lx, parseErr.postContext);
          u_fprintf(lx->OUT, "</tt></tr></td></table><br />\r\n");
          customError = U_ZERO_ERROR;
          customSort = NULL;
        }
      } 

      if(customSort == NULL) {
        const char *shortstr = queryField(lx, "col_ss");
        if(shortstr && *shortstr) {
           UCollator *coll;
           coll = ucol_openFromShortString(shortstr,
                                            false,
                                            NULL,
                                            &customError );
            u_fprintf(lx->OUT, "%S <tt style='border: solid black; margin: 2px; padding: 2px;'>%s</tt>", 
                  FSWF("showSort_shortString", "Using Short String"), shortstr);
           customSort = usort_openWithCollator(coll, true, &customError);
        }
      }
            
      if(customSort == NULL) {
#if defined(HAVE_LX_HOOK)
        customSort = lx_hook_usort_open(locale, UCOL_DEFAULT, true, &customError);
#else
        customSort = usort_open(locale, UCOL_DEFAULT, true, &customError);
#endif
        /*        fprintf(stderr, "** COL SORT OPEN: %s \n", locale);*/
      }

      if(U_FAILURE(customError))
      {
        u_fprintf(lx->OUT, "<b>%S %s :</b>", 
                  FSWF("showSort_cantOpenCustomConverter", "Could not open a custom usort/collator for the following locale and reason"), locale);
        explainStatus(lx, customError, NULL); 
        customError=U_ZERO_ERROR;
        customSort = usort_open(locale, UCOL_DEFAULT, true, &customError);
      } 

      if(U_FAILURE(customError)) {
        u_fprintf(lx->OUT, "<!-- could not open custom collator - %s -->\n", u_errorName(customError));
        return;
      }

      customCollator = usort_getCollator(customSort);
      /* for standard: see       showSort_attrib(lx, locale, NULL); */

      /* begin customizables */
      
      /* -------------------------- UCOL_STRENGTH ----------------------------------- */
      {
        const UColAttributeValue strengthVals[] = { LX_UCOL_DEFAULT, UCOL_PRIMARY, UCOL_SECONDARY, UCOL_TERTIARY, UCOL_QUATERNARY, UCOL_IDENTICAL, LX_UCOL_LAST };
        showSort_doCustom(lx, UCOL_STRENGTH, customCollator, "strength", strengthVals, lxSortReset);
      }
      /* ------------------------------- UCOL_CASE_FIRST ------------------------------------- */
      {
          UColAttributeValue ourOptions[] = { LX_UCOL_DEFAULT, UCOL_OFF, UCOL_LOWER_FIRST, UCOL_UPPER_FIRST, LX_UCOL_LAST };
          showSort_doCustom(lx, UCOL_CASE_FIRST, customCollator, "cas1", ourOptions, lxSortReset);
      }
      /* ----------------- ALT HANDLING ------------ */
      {
          UColAttributeValue ourOptions[] = { LX_UCOL_DEFAULT, UCOL_SHIFTED, UCOL_NON_IGNORABLE, LX_UCOL_LAST};
          showSort_doCustom(lx, UCOL_ALTERNATE_HANDLING, customCollator, "shft", ourOptions, lxSortReset);
      }
      /* ------------------------------- UCOL_FRENCH_COLLATION ------------------------------------- */
      {
          UColAttributeValue ourOptions[] = { LX_UCOL_DEFAULT, UCOL_OFF, UCOL_ON, LX_UCOL_LAST};
          showSort_doCustom(lx, UCOL_FRENCH_COLLATION, customCollator, "fr", ourOptions, lxSortReset);
      }
      /* ------------------------------- UCOL_CASE_LEVEL ------------------------------------- */
      {
          UColAttributeValue ourOptions[] = { LX_UCOL_DEFAULT, UCOL_OFF, UCOL_ON, LX_UCOL_LAST};
          showSort_doCustom(lx, UCOL_CASE_LEVEL, customCollator, "case", ourOptions, lxSortReset);
      }
      /* ------------------------------- UCOL_DECOMPOSITION_MODE ------------------------------------- */
      {
          UColAttributeValue ourOptions[] = { LX_UCOL_DEFAULT, UCOL_OFF, UCOL_ON, LX_UCOL_LAST};
          showSort_doCustom(lx, UCOL_DECOMPOSITION_MODE, customCollator, "dcmp", ourOptions, lxSortReset);
      }
      /* ------------------------------- UCOL_HIRAGANA_QUATERNARY_MODE ------------------------------------- */
      {
          UColAttributeValue ourOptions[] = { LX_UCOL_DEFAULT, UCOL_OFF, UCOL_ON, LX_UCOL_LAST};
          showSort_doCustom(lx, UCOL_HIRAGANA_QUATERNARY_MODE, customCollator, "hira", ourOptions, lxSortReset);
      }
      /* ------------------------------- UCOL_NUMERIC_COLLATION -------------------------------- */
      {
          UColAttributeValue ourOptions[] = { LX_UCOL_DEFAULT, UCOL_OFF, UCOL_ON, LX_UCOL_LAST};
          showSort_doCustom(lx, UCOL_NUMERIC_COLLATION, customCollator, "nume", ourOptions, lxSortReset);
      }

      /* reset ----------------------------------------- */

      u_fprintf(lx->OUT, "<br /><hr width=\"20%%\" />\r\n");

    u_fprintf(lx->OUT, "<input id=\"lxsortreset\" type=\"submit\" name=\"lxSortReset\" class=\"wide\" value=\"%S\" />",
              FSWF("EXPLORE_CollationElements_Defaults", "Reset Rules and Options"));
    {
      UChar dispName[1024];
      UErrorCode stat = U_ZERO_ERROR;
      dispName[0] = 0;
      uloc_getDisplayName(lx->curLocaleName, lx->dispLocale, dispName, 1024, &stat);
      
      u_fprintf(lx->OUT, "<input type=\"submit\" class=\"wide\" name=\"usortRulesLocale\" value=\"%S %S%S\" />",
                FSWF("usortLocaleRules1", "Fetch rules for"),
                FSWF("locale","locale"),
                FSWF("usortLocaleRules2","") /* for translation */
        );
    }

    /* u_fprintf(lx->OUT, "</td>"); */

    /* end customizables ---------------------------------------------------------- */
    /* ======================================================================================= */

  }

/*  output  =================================================================================== */ 



  /* ========== Do the actual sort ======== */
  if(inputChars[0] != 0)
  {
    int n;
    UChar in[SORTSIZE];
    const char *aConv = lx->convRequested;
    if(!aConv || !*aConv) {
        aConv = "utf-8";
    }
    
    /* have some text to sort */
    unescapeAndDecodeQueryField_enc(in, SORTSIZE, inputChars, aConv); /* TODO: Didn't we just convert this? */
    u_replaceChar(in, 0x000D, 0x000A); /* CRLF */
    
    if(schInputChars[0]!=0) {
        /* string search! */
        UStringSearch *usearch = NULL;
        int32_t *pos = NULL;
        int32_t posLen = 0;
        int32_t i  = 0;
        int32_t j = 0;
        const char *overlap = NULL;
        UCollator *coll;
        int32_t match;
        UErrorCode searchErr = U_ZERO_ERROR;
        
        coll = usort_getCollator(customSort);
        
#if 0
//        {
//            UErrorCode icuStatus = U_ZERO_ERROR;
//           coll = ucol_open("en_US", &icuStatus );
//           ucol_setAttribute(coll, UCOL_STRENGTH, UCOL_PRIMARY, &icuStatus);
//        }
#endif
        
        usearch = usearch_openFromCollator( schChars,
                                   u_strlen(schChars),
                                   strChars,
                                   u_strlen(strChars),
                                   coll,
                                   NULL,
                                   &searchErr );
        overlap = queryField(lx, "usearch_overlap");
        if(0 && overlap && *overlap) {
           usearch_setAttribute( usearch,
                                 USEARCH_OVERLAP,
                                 USEARCH_ON,
                                 &searchErr );
        }
        pos = calloc(sizeof(pos[0]),u_strlen(strChars));
        if(U_SUCCESS(searchErr)) {
            for(match=usearch_first(usearch, &searchErr);
                U_SUCCESS(searchErr) && match != USEARCH_DONE;
                match = usearch_next(usearch, &searchErr)) {
                pos[posLen++] = match;
            }
        }
        usearch_close(usearch);
        if(U_FAILURE(searchErr)) {
            u_fprintf(lx->OUT, "</td><td> <b>Search error:  %s</b></td>", 
                u_errorName(searchErr));
        } else {
            u_fprintf(lx->OUT, "</td><td WIDTH=\"22%%\" colspan=\"2\" rowspan=\"2\"><p><b>%S</b> &mdash; %d</p>\r\n",
                      FSWF("usortSearchResults", "Search Results"), posLen);
            if(1 || (posLen>0)) {
                i=0;
                for(j=0;j<u_strlen(strChars);j++) {
                    if(i<posLen&&j==pos[i]) {
                        u_fprintf(lx->OUT, "<span style='color: red;'>|</span>");
                        i++;
                    }
                    u_fprintf(lx->OUT, "%C", strChars[j]);
                }
                u_fprintf(lx->OUT, "<hr>\n");
                i=0;
                for(j=0;j<u_strlen(strChars);j++) {
                    if(0==(j%8)) {
                        u_fprintf(lx->OUT, "<br>\n");
                    }
                    if(i<posLen&&j==pos[i]) {
                        u_fprintf(lx->OUT, "<span style='color: red;'>|</span>");
                        i++;
                    }
                    u_fprintf(lx->OUT, "\\u%04X", (uint32_t)strChars[j]);
                }
                if(!hasQueryField(lx,"hideCollKey"))
                {
                    int32_t ce = UCOL_NULLORDER;
                    UCollationElements *uci =  ucol_openElements(coll,
                                          strChars,
                                                u_strlen(strChars),
                                                &searchErr);
                    u_fprintf(lx->OUT,"<hr>\n");
                    while((ce = ucol_next(uci, &searchErr)) != UCOL_NULLORDER) {
                        u_fprintf(lx->OUT, "%08X ", ce);
                    }
                    ucol_closeElements(uci);
                    u_fprintf(lx->OUT, "\n");
                }
                u_fprintf(lx->OUT, "<hr>\n");
                for(i=0;i<posLen;i++) {
                    u_fprintf(lx->OUT, "&lt;%d,..&gt;", pos[i]);
                }
                u_fprintf(lx->OUT, "<hr><h3>searching for:</h3><div style='border: 1px dashed green;'>\n");
                for(i=0;i<u_strlen(schChars);i++) {
                    if(0==(i%8)) {
                        u_fprintf(lx->OUT, "<br>\n");
                    }
                    u_fprintf(lx->OUT, "\\u%04X", (uint32_t)schChars[i]);
                }
                if(!hasQueryField(lx,"hideCollKey"))
                {
                    int32_t ce = UCOL_NULLORDER;
                    UCollationElements *uci =  ucol_openElements(coll,
                                          schChars,
                                                u_strlen(schChars),
                                                &searchErr);
                    u_fprintf(lx->OUT,"<hr>\n");
                    while((ce = ucol_next(uci, &searchErr)) != UCOL_NULLORDER) {
                        u_fprintf(lx->OUT, "%08X ", ce);
                    }
                    ucol_closeElements(uci);
                    u_fprintf(lx->OUT, "\n");
                }
                u_fprintf(lx->OUT, "</div>\n");
            }
        } 
        u_fprintf(lx->OUT, "</td>");
        /* end search demo */
    } else if(!isG7) {
        /* Loop through each sort method */
        for(n=0;n<2;n++)
        {
          USort *aSort = NULL;
          UChar *first, *next;
          int32_t i, count=0;

          if(n == 0) { 
            u_fprintf(lx->OUT, "</td><td " /* WIDTH=\"22%%\" */ " rowspan=\"2\"><p><b>%S</b></p>\r\n",
                      FSWF("usortOriginal", "Original"));
          } else {
            u_fprintf(lx->OUT, "</td><td " /* WIDTH=\"22%%\" */ " rowspan=\"2\"><p><b>%S</b></p>\r\n",
                      FSWF("usortCollated", "Collated"));
          }

          aSort = customSort;
          if(n>0 && !lxCustSortOpts) {
            /* don't setstrength on 1st item (default) if custom options have been set */
            /*ucol_setStrength(usort_getCollator(aSort), UCOL_DEFAULT);*/
          }
          
          /* add lines from buffer */
          
          /* For now, we pass true to clone the text. Wasteful! But, 
             it avoids having to modify the in text AND keep track of the
             ptrs. Now if a usort could be cloned and resorted before
             output.. */
          first = in;
          next = first;
          while(*next) {
            if(*next == 0x000A) {
              if(first != next) {
                usort_addLine(aSort, first, next-first, true, (void*)++count);
              }
              first = next+1;
            }
            next++;
          }
          
          if(first != next) { /* get the LAST line */
            usort_addLine(aSort, first, next-first, true, (void*)++count);
          }      
          
          if(n != 0) {
            usort_sort(aSort); /* first item is 'original' */
          }
          
          for(i=0;i<aSort->count;i++) {
            showSort_outputWord(lx, (n==0)?NULL:aSort, i, aSort->lines[i].chars);
          }
          usort_remove(aSort); /* clear out lines, prepare for next go round */
        }
        usort_close(customSort);
        u_fprintf(lx->OUT, "</td>");  
      } else   { /* G7 */
        for(n=0;n<G7COUNT;n++) {
          UChar dispName[1024];
          UErrorCode stat = U_ZERO_ERROR;
          dispName[0] = 0;

          if(!strncmp(G7s[n],"en",2) ||
             !strncmp(G7s[n],"fr",2) ) {
            uloc_getDisplayName(G7s[n], lx->dispLocale, dispName, 1024, &stat);
          } else {
            uloc_getDisplayLanguage(G7s[n], lx->dispLocale, dispName, 1024, &stat);
          }
          u_fprintf(lx->OUT, "<td>");
          if(U_SUCCESS(stat)) {
            u_fprintf(lx->OUT, "<center>%S</center>\r\n", dispName);
          } else {
            u_fprintf(lx->OUT, "<center>%s</center>\r\n", G7s[n]);
          }
          u_fprintf(lx->OUT, "</td>");

        }

        u_fprintf(lx->OUT, "</tr><tr>\r\n");

        for(n=0;n<G7COUNT;n++)
        {
          USort *aSort;
          UErrorCode sortErr = U_ZERO_ERROR;
          UChar *first, *next;
          int32_t i, count=0;
          
          aSort = usort_open(G7s[n], UCOL_TERTIARY, true, &sortErr);
          
          /* add lines from buffer */
          
          /* For now, we pass true to clone the text. Wasteful! But, 
             it avoids having to modify the in text AND keep track of the
             ptrs. Now if a usort could be cloned and resorted before
             output.. */
          first = in;
          next = first;
          while(*next)
          {
            if(*next == 0x000A)
            {
              if(first != next)
              {
                usort_addLine(aSort, first, next-first, true, (void*)++count);
              }
              first = next+1;
            }
            next++;
          }
          
          if(first != next) /* get the LAST line */
          {
            usort_addLine(aSort, first, next-first, true, (void*)++count);
          }      
          
          usort_sort(aSort);
          
          u_fprintf(lx->OUT, " <td " /* width=\"20%%\" */ " valign=\"top\">");
          
          for(i=0;i<aSort->count;i++) {
            showSort_outputWord(lx, aSort, i, aSort->lines[i].chars);
          }
          
          u_fprintf(lx->OUT, "</td>");  
          
          usort_close(aSort);
        }
      } /* end G7 demo */
  }
  u_fprintf(lx->OUT, "</tr>\r\n");

  
    /* -===================================================================== custom rules ... */
  if(!isG7) {
    u_fprintf(lx->OUT, "<tr><td colspan=\"2\">\r\n");
    u_fprintf(lx->OUT, "<label for=\"usortrules\"> %S</label>\r\n", FSWF("usortCustomRules","Custom Rules"));
    if(!hasQueryField(lx, "usortRulesLocale") || lxSortReset) {
        u_fprintf(lx->OUT, "<i>(%S '%S %S%S' %S)</i>\r\n",
            FSWF("usortCustomRules_note0","Click"),
                FSWF("usortLocaleRules1", "Fetch rules for"),
                FSWF("locale","locale"),
                FSWF("usortLocaleRules2",""), /* for translation */
            FSWF("usortCustomRules_note1","above, to edit rules")
        );
    }
    u_fprintf(lx->OUT, "<br />\r\n");
    u_fprintf(lx->OUT, "<textarea id=\"usortrules\" class=\"wide\" name=\"usortRules\" rows=\"5\" cols=\"50\">");
    
    if(hasQueryField(lx, "usortRulesLocale") && !lxSortReset) {
      UErrorCode err = U_ZERO_ERROR;
      UResourceBundle *bund, *array = NULL;
      const UChar *s = 0;
      int32_t len;
      const char *comp="?";

      bund = getCollationBundle(lx, &err); if(U_SUCCESS(err)) comp = "bundle";
      if(bund) array = ures_getByKey(bund, "collations", NULL, &err); if(U_SUCCESS(err)) comp = "collations";
      if(array) array = ures_getByKey(array, "standard", NULL, &err);  if(U_SUCCESS(err)) comp = "standard";
      if(array) s = ures_getStringByKey(array, "Sequence", &len, &err);  if(U_SUCCESS(err)) comp = "Sequence";
      if(U_SUCCESS(err) && s && *s) {
        u_fprintf(lx->OUT, "# %s.txt Rules\r\n\r\n", lx->curLocaleName, queryField(lx,"usortRulesLocale"));
        printRuleString(lx,s);
      } else { 
        u_fprintf(lx->OUT, "# err %s - failed after %s\r\n", u_errorName(err), comp);
        if(err == U_USING_DEFAULT_WARNING) {
          u_fprintf(lx->OUT, "# ( problem loading Root (UCA) rules \n");
        }
      }
    } else if (ruleUrlChars[0]) { /* user has entered a custom rule */
      printRuleString(lx,ruleChars); 
    }
    u_fprintf(lx->OUT, "</textarea>\r\n");
    
    u_fprintf(lx->OUT, "</td></tr>\r\n");
  } /* end !G7 */
    /* ========== end rule */ 

  u_fprintf(lx->OUT, "</table>");

  u_fprintf(lx->OUT, "</form>\r\n");

  u_fprintf(lx->OUT, "<p><br /></p>\r\n");
  u_fprintf(lx->OUT, "%S\r\n",  FSWF(/*NOEXTRACT*/"sortHelp",""));
  u_fprintf(lx->OUT, "<p><br />\r\n");
  u_fprintf(lx->OUT, "%S <a href=\"http://icu-project.org/userguide/Collate_Intro.html\">%S</a><br /></p>\r\n",
            FSWF("EXPLORE_CollationElements_moreInfo1", "For more information, see the"),
            FSWF("EXPLORE_CollationElements_moreInfo2", "ICU userguide"));

  /* showExploreCloseButton(lx, locale, "CollationElements"); */
  
}
