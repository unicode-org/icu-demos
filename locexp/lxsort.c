
/**********************************************************************
*   Copyright (C) 1999-2003, International Business Machines
*   Corporation and others.  All Rights Reserved.
***********************************************************************/

#include "locexp.h"

#define isNL(x) ((x)==0x000D || (x)==0x000A || (x)==0x2029 || (x)==0x2028)

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
  lx->backslashCtx.html = FALSE;

  while(*s) {
    switch(*s) {
    case '\r': break;
    case '\n': break;
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
  lx->backslashCtx.html = TRUE;
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
    lineAbove = FALSE;
    lineBelow = FALSE;
  } else {
    /* calculate lineAbove */
    if(num == 0) {  
      lineAbove = TRUE; /* first item - always a line above */
    } else if( aSort->lines[num-1].keySize == aSort->lines[num].keySize  &&
               !memcmp(aSort->lines[num-1].key,
                       aSort->lines[num].key,
                       aSort->lines[num].keySize) ) {
      /* item is identical to previous - no line above */
      lineAbove = FALSE;
    } else {
      /* different than prev - line above. */
      lineAbove = TRUE;
    }

    /* calculate lineBelow */
    if(num == (aSort->count-1)) {
      lineBelow = TRUE; /* last item - always line below */
    } else if( aSort->lines[num+1].keySize == aSort->lines[num].keySize  &&
               !memcmp(aSort->lines[num+1].key,
                       aSort->lines[num].key,
                       aSort->lines[num].keySize) ) {
      /* item is identical to next - no line below */
      lineBelow = FALSE;
    } else {
      /* different than next - line below. */
      lineBelow = TRUE;
    }
  }



#if 1
  if(lineAbove) { u_fprintf(lx->OUT, "<div class=\"box%d\">\r\n", (evenOdd++)%2 ); }
  u_fprintf(lx->OUT, "%U", chars);

  {
    int32_t ii;
    if(aSort  && hasQueryField(lx,"showCollKey")  ) {
      u_fprintf(lx->OUT, "<br><font size=-2 color=\"#999999\"><tt>");

      for(ii=0;ii<aSort->lines[num].keySize;ii++) {
        u_fprintf(lx->OUT, "%02x ", aSort->lines[num].key[ii]);
      }
      u_fprintf(lx->OUT, "</tt></font>\r\n");
    }
  }
  if(lineBelow) { u_fprintf(lx->OUT, "\r\n</div>\r\n"); } else { u_fprintf(lx->OUT, "<br>\r\n"); }
  /* CSS mode */
#else
  u_fprintf(lx->OUT, "%s%s%02d.%U%s%s<BR>\n",
            ((lineBelow)?"<U>":""),
            ((lineAbove)?"":"<font color=\"#AAAAAA\">"),
            (aSort==NULL)?num:(int32_t)aSort->lines[num].userData,
            chars,
            ((lineAbove)?"":"</font>"),
            ((lineBelow)?"</U>":"")
            );

#endif
}


const UChar *showSort_attributeName(UColAttribute attrib)
{
  static const UChar nulls[] = { 0x0000 };

  switch(attrib)
  {
    case UCOL_FRENCH_COLLATION: return FSWF("UCOL_FRENCH_COLLATION","French collation");
    case UCOL_ALTERNATE_HANDLING: return FSWF("UCOL_ALTERNATE_HANDLING","Alternate handling");
    case UCOL_CASE_FIRST: return FSWF("UCOL_CASE_FIRST","Case first");
    case UCOL_CASE_LEVEL: return FSWF("UCOL_CASE_LEVEL","Case level");
    case UCOL_NORMALIZATION_MODE: return FSWF("UCOL_NORMALIZATION_MODE","Normalization mode");
    case UCOL_STRENGTH: return FSWF("UCOL_STRENGTH","Strength");
    case UCOL_HIRAGANA_QUATERNARY_MODE: return FSWF("UCOL_HIRAGANA__QUATERNARY_MODE","Hiragana Quaternary Mode");
    default:  return nulls;
  }
}

const UChar *showSort_attributeVal(UColAttributeValue val)
{
  static const UChar nulls[] = { 0x0000 };

  switch(val)
  {
  /* Duplicate:  UCOL_CE_STRENGTH_LIMIT */
  case UCOL_IDENTICAL: return FSWF("UCOL_IDENTICAL","Identical");
  case UCOL_LOWER_FIRST : return FSWF("UCOL_LOWER_FIRST","Lower first");
  case UCOL_NON_IGNORABLE : return FSWF("UCOL_NON_IGNORABLE","Non-ignorable");
  case UCOL_OFF : return FSWF("UCOL_OFF","Off");
  case UCOL_ON : return FSWF("UCOL_ON","On");
/*  case UCOL_ON_WITHOUT_HANGUL : return FSWF("UCOL_ON_WITHOUT_HANGUL","On,without Hangul"); ????  */
  case UCOL_PRIMARY : return FSWF("UCOL_PRIMARY","Primary");
  case UCOL_QUATERNARY: return FSWF("UCOL_QUATERNARY","Quaternary");
  case UCOL_SECONDARY : return FSWF("UCOL_SECONDARY","Secondary");
  case UCOL_SHIFTED : return FSWF("UCOL_SHIFTED","Shifted");
 /* Duplicate: UCOL_STRENGTH_LIMIT */
  case UCOL_TERTIARY : return FSWF("UCOL_TERTIARY","Tertiary");
  case UCOL_UPPER_FIRST : return FSWF("UCOL_UPPER_FIRST","Upper first");
  default: return nulls;
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
    
    u_fprintf(lx->OUT, "<H4>%U</H4><UL>\r\n", FSWF("usort_attrib", "Attributes"));
    for(attrib=UCOL_FRENCH_COLLATION; attrib < UCOL_ATTRIBUTE_COUNT;
        attrib++) {
      subStatus = U_ZERO_ERROR;
      val = ucol_getAttribute(ourCollator,
                              attrib,
                              &subStatus);
      u_fprintf(lx->OUT, "  <LI><b>%U</b>: %U\r\n",
                showSort_attributeName(attrib),
                showSort_attributeVal(val));
    }
    u_fprintf(lx->OUT, "</UL>\r\n");
    ucol_close(newCollator);
  }
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
  char   ruleUrlChars[SORTSIZE] = "";
  const char *text;
  char *p;
  int32_t length;
  UChar  strChars[SORTSIZE];
  UChar  ruleChars[SORTSIZE]; /* Custom rule UChars */
  UChar  fixedRuleChars[SORTSIZE]; /* Custom rule UChars without comments */
  int    i;
  UBool lxCustSortOpts = FALSE;  /* if TRUE, then user has approved the custom settings.  If FALSE, go with defaults.  See "lxCustSortOpts=" tag. */

  /* The 'g7' locales to demonstrate. Note that there eight.  */
  UErrorCode status = U_ZERO_ERROR;  

  /* For customization: */
  UColAttributeValue  customStrength = UCOL_DEFAULT;
  USort              *customSort     = NULL;
  UCollator          *customCollator = NULL;
  UColAttributeValue  value;
  UColAttribute       attribute;

  /* Mode switch.. */
  enum
  { 
    /* g7 sort - special handling, for the g7+ locales above.
     * Invoked by the locale being set to g7
     */
    kG7Mode, 
    
    /* 'classic' mode- original, with customization
     */
    kSimpleMode,   
  } mode = kSimpleMode;

  if(hasQueryField(lx,"lxCustSortOpts")) {
    lxCustSortOpts = TRUE;
  }

  u_fprintf(lx->OUT, "<style>\r\n<!--.box0 { border: 1px inset gray; margin: 1px }\r\n.box1 { border: 1px inset gray; margin: 1px; background-color: #CCEECC } -->\r\n</style>\r\n");

  strChars[0] = 0;

  if(strstr(locale,"g7") != NULL)
  {
    mode = kG7Mode;
  }

  /* pull out the text to be sorted. Note, should be able to access this 
     as a POST
   */
  text = queryField(lx,"EXPLORE_CollationElements");

  if(text)
  {
    unescapeAndDecodeQueryField_enc(strChars, SORTSIZE,
                                    text, lx->chosenEncoding );
    
    length = strlen(text);
    
    if(length > (SORTSIZE-1))
      length = SORTSIZE-1; /* safety ! */
    
    strncpy(inputChars, text, length); /* make a copy for future use */
    inputChars[length] = 0;
  }
  else
  {
    inputChars[0] = 0;  /* no text to process */
  }


  /* look for custom rules */
  ruleChars[0] = 0;
  text = queryField(lx, "usortRules");

  if(text) {

    unescapeAndDecodeQueryField_enc(ruleChars, SORTSIZE, 
                                    text, lx->chosenEncoding);
    length = strlen(text);
      
    if(length > (SORTSIZE-1)) {
      length = SORTSIZE-1; /* safety ! */
    }

    strncpy(ruleUrlChars, text, length); /* make a copy for future use */
    ruleUrlChars[length] = 0;
  } else {
    ruleChars[0] = 0;
  }
  
  u_fprintf(lx->OUT, "%U<P>\r\n", FSWF("EXPLORE_CollationElements_Prompt", "Type in some lines of text to be sorted."));
  
  u_fprintf(lx->OUT, "<FORM METHOD=\"GET\">");
  u_fprintf(lx->OUT, "<INPUT TYPE=hidden NAME=_ VALUE=%s>", locale);
  u_fprintf(lx->OUT, "<INPUT NAME=EXPLORE_CollationElements VALUE=\"%U\" TYPE=hidden>", strChars); 
  u_fprintf(lx->OUT, "<INPUT TYPE=SUBMIT VALUE=\"%U\">",
            FSWF("EXPLORE_CollationElements_Defaults", "Defaults"));
  u_fprintf(lx->OUT, "</FORM>\r\n ");

  u_fprintf(lx->OUT, "<FORM METHOD=\"POST\" ACTION=\"?_=%s&EXPLORE_CollationElements=&\">", 
            locale);
  
  /* Here, 'configuration information' at the top of the page. ====== */
  switch(mode)
  {
    case kSimpleMode:
    {
      const char *ss;
      int nn;
      UErrorCode customError = U_ZERO_ERROR;
      
      if ( ruleChars[0] ) { /* custom rules */
        UCollator *coll;
        UParseError parseErr;

        u_strcpy(fixedRuleChars, ruleChars);
        stripComments(fixedRuleChars);
        /* u_fprintf(lx->OUT, "R [<pre>%U</pre>]<BR>\r\n", fixedRuleChars); */

        coll = ucol_openRules ( fixedRuleChars, -1, 
                                UCOL_DEFAULT, UCOL_DEFAULT, /* attr val, str */
                                &parseErr,
                                &customError);

        customSort = usort_openWithCollator(coll, TRUE, &customError);
        
        if(U_FAILURE(customError) || !customSort) {
          u_fprintf(lx->OUT, "<B>%U %s %s:</B>", 
                    FSWF("showSort_cantOpenCustomConverter", "Could not open a custom usort/collator for the following locale and reason"), locale);
          explainStatus(lx, customError, NULL); 
          
          u_fprintf(lx->OUT,"<br><table border=1><tr><td>%U (%s):</td></tr><tr>",
                    FSWF("showSort_Context", "Error shown by this mark:"),
                    "<font color=red><u>|</u></font>");

          u_fprintf(lx->OUT, "<td><tt>");
          printRuleString(lx, parseErr.preContext);
          u_fprintf(lx->OUT, "<font color=red><u>|</u></font>");
          printRuleString(lx, parseErr.postContext);
          u_fprintf(lx->OUT, "</tt></tr></td></table><br>\r\n");
          customError = U_ZERO_ERROR;
          customSort = NULL;
        }
      } 

      if(customSort == NULL) {
        customSort = usort_open(locale, UCOL_DEFAULT, TRUE, &customError);
      }

#if 0
          { 
            int q;
            for(q=0;ruleChars[q];q++) {
              u_fprintf(lx->OUT, "&lt;U+%04x&gt;", ruleChars[q]);
            }
            u_fprintf(lx->OUT, "<br>");
          }
#endif


      if(U_FAILURE(customError))
      {
        u_fprintf(lx->OUT, "<B>%U %s :</B>", 
                  FSWF("showSort_cantOpenCustomConverter", "Could not open a custom usort/collator for the following locale and reason"), locale);
        explainStatus(lx, customError, NULL); 
      } 

      customCollator = usort_getCollator(customSort);
      /* for standard: see       showSort_attrib(lx, locale, NULL); */

      /* begin customizables */
      
      u_fprintf(lx->OUT, "<TABLE BORDER=0><TR><TD>");
      /* ------------------------------- UCOL_FRENCH_COLLATION ------------------------------------- */
      attribute = UCOL_FRENCH_COLLATION;
      status = U_ZERO_ERROR;
      value = ucol_getAttribute(customCollator, attribute, &status);
      status = U_ZERO_ERROR; /* we're prepared to just let the collator fail later. */
      if(hasQueryField(lx,"fr"))
      {
        value = UCOL_ON;
      } 
      else if(lxCustSortOpts) /* if we came from the form.. */
      {
        value = UCOL_OFF;
      }

      u_fprintf(lx->OUT, "<input type=hidden name=lxCustSortOpts value=> <input type=checkbox %s name=fr> %U <BR>\r\n",
                (value==UCOL_ON)?"checked":"",  showSort_attributeName(attribute));
      status = U_ZERO_ERROR;
      ucol_setAttribute(customCollator, attribute, value, &status);
      status = U_ZERO_ERROR; /* we're prepared to just let the collator fail later. */
      
      /* ----------------- ALT HANDLING ------------ */
      attribute = UCOL_ALTERNATE_HANDLING;
      status = U_ZERO_ERROR;
      value = ucol_getAttribute(customCollator, attribute, &status);
      status = U_ZERO_ERROR; /* we're prepared to just let the collator fail later. */
      if(hasQueryField(lx, "shft"))
      {
        value = UCOL_SHIFTED;
      } else  if(lxCustSortOpts) {/* if we came from the form.. */
        value = UCOL_NON_IGNORABLE;
      }

      u_fprintf(lx->OUT, "<input type=checkbox %s name=shft> %U <BR>\r\n",
                (value==UCOL_SHIFTED)?"checked":"",  showSort_attributeName(attribute));
      status = U_ZERO_ERROR;
      ucol_setAttribute(customCollator, attribute, value, &status);
      status = U_ZERO_ERROR; /* we're prepared to just let the collator fail later. */

      /* ------------------------------- UCOL_CASE_FIRST ------------------------------------- */
      {
          UColAttributeValue caseVals[] = { UCOL_OFF, UCOL_LOWER_FIRST, UCOL_UPPER_FIRST };
          int i;
          attribute = UCOL_CASE_FIRST;
          status = U_ZERO_ERROR;
          value = ucol_getAttribute(customCollator, attribute, &status);
          status = U_ZERO_ERROR; /* we're prepared to just let the collator fail later. */
          if((ss=queryField(lx, "cas1")))
          {
            value = atoi(ss);
          }
          else
          {
              value = UCOL_OFF;
          }
          u_fprintf(lx->OUT, "%U: <select name=cas1>\r\n", showSort_attributeName(attribute) );

          for(i = 0; i < sizeof(caseVals)/sizeof(caseVals[0])  ; i++)
          {
              u_fprintf(lx->OUT, "<OPTION %s VALUE=\"%d\">%U\r\n",
                        (caseVals[i]==value)?"selected":"",
                        caseVals[i],
                        showSort_attributeVal(caseVals[i]));
          }
          u_fprintf(lx->OUT, "</SELECT><BR>\r\n");

          status = U_ZERO_ERROR;
          ucol_setAttribute(customCollator, attribute, value, &status);
          if(status != U_ZERO_ERROR) { u_fprintf(lx->OUT, "<b>(%s)</b>\r\n", u_errorName(status));}
          status = U_ZERO_ERROR; /* we're prepared to just let the collator fail later. */
      }
      /* ------------------------------- UCOL_CASE_LEVEL ------------------------------------- */
      attribute = UCOL_CASE_LEVEL;
      status = U_ZERO_ERROR;
      value = ucol_getAttribute(customCollator, attribute, &status);
      status = U_ZERO_ERROR; /* we're prepared to just let the collator fail later. */
      if(hasQueryField(lx,"case")) {
        value = UCOL_ON;
      } 
      else if(lxCustSortOpts) /* if we came from the form.. */
      {
        value = UCOL_OFF;
      }
      u_fprintf(lx->OUT, "<input type=checkbox %s name=case> %U <BR>\r\n",
                (value==UCOL_ON)?"checked":"",  showSort_attributeName(attribute));
      status = U_ZERO_ERROR;
      ucol_setAttribute(customCollator, attribute, value, &status);
      status = U_ZERO_ERROR; /* we're prepared to just let the collator fail later. */


      /* ------------------------------- UCOL_DECOMPOSITION_MODE ------------------------------------- */
      attribute = UCOL_DECOMPOSITION_MODE;
      status = U_ZERO_ERROR;
      value = ucol_getAttribute(customCollator, attribute, &status);
      status = U_ZERO_ERROR; /* we're prepared to just let the collator fail later. */
      if(hasQueryField(lx, "dcmp"))  {
        value = UCOL_ON;
      } 
      /* for now - default fr coll to OFF! fix: find out if the user has clicked through once or no */
      else if(lxCustSortOpts) /* if we came from the form.. */
      {
        value = UCOL_OFF;
      }
      u_fprintf(lx->OUT, "<input type=checkbox %s name=dcmp> %U <BR>\r\n",
                (value==UCOL_ON)?"checked":"",  showSort_attributeName(attribute));
      status = U_ZERO_ERROR;
      ucol_setAttribute(customCollator, attribute, value, &status);
      status = U_ZERO_ERROR; /* we're prepared to just let the collator fail later. */
      

      /* -------------------------- UCOL_STRENGTH ----------------------------------- */
      status = U_ZERO_ERROR;
      attribute = UCOL_STRENGTH;
      customStrength = ucol_getAttribute(customCollator, attribute, &status);
      if((ss = queryField(lx,"strength"))) {
        nn = atoi(ss);
        if( (nn || (*ss=='0'))  && /* choice is a number and.. */
            (showSort_attributeVal(nn)[0]) ) /* it has a name (is a valid item) */
        {
          customStrength = nn; 
        }
      }
      status = U_ZERO_ERROR;
      ucol_setAttribute(customCollator, attribute, customStrength, &status);
      if(U_FAILURE(status))
      {
        explainStatus(lx, status, NULL);
        status = U_ZERO_ERROR;
      }

      
      u_fprintf(lx->OUT, "%U: <select name=strength>\r\n", showSort_attributeName(attribute) );

      for(value = UCOL_PRIMARY; value < UCOL_STRENGTH_LIMIT; value++)
      {
        if(showSort_attributeVal(value)[0] != 0x0000)  /* If it's a named attribute, try it */
        {  
          u_fprintf(lx->OUT, "<OPTION %s VALUE=\"%d\">%U\r\n",
                    (customStrength==value)?"selected":"",
                    value,
                    showSort_attributeVal(value));
        }
      }
      u_fprintf(lx->OUT, "</SELECT><BR>\r\n");

      /* ------------------------------- UCOL_HIRAGANA_QUATERNARY_MODE ------------------------------------- */
      attribute = UCOL_HIRAGANA_QUATERNARY_MODE;
      status = U_ZERO_ERROR;
      value = ucol_getAttribute(customCollator, attribute, &status);
      status = U_ZERO_ERROR; /* we're prepared to just let the collator fail later. */
      if(hasQueryField(lx, "hira")) {
        value = UCOL_ON;
      } 
      else if(lxCustSortOpts) /* if we came from the form.. */
      {
        value = UCOL_OFF;
      }
      u_fprintf(lx->OUT, "<input type=checkbox %s name=hira> %U <BR>\r\n",
                (value==UCOL_ON)?"checked":"",  showSort_attributeName(attribute));
      status = U_ZERO_ERROR;
      ucol_setAttribute(customCollator, attribute, value, &status);
      status = U_ZERO_ERROR; /* we're prepared to just let the collator fail later. */

    u_fprintf(lx->OUT, "</TD>");

    /* end customizables ---------------------------------------------------------- */
    u_fprintf(lx->OUT, "<TD>\r\n");
    u_fprintf(lx->OUT, "%U\r\n", FSWF("usortCustomRules","Custom Rules"));
    {
      UChar dispName[1024];
      UErrorCode stat = U_ZERO_ERROR;
      dispName[0] = 0;
      uloc_getDisplayName(lx->curLocaleName, lx->cLocale, dispName, 1024, &stat);
      
      u_fprintf(lx->OUT, "<input type=submit name=\"usortRulesLocale\" value=\"%U %U %U\">",
                FSWF("usortLocaleRules1", "Load rules for"),
                dispName,
                FSWF("usortLocaleRules2","") /* for translation */
                );
    }
    u_fprintf(lx->OUT, "<br>\r\n");
    u_fprintf(lx->OUT, "<textarea name=\"usortRules\" rows=5 cols=50 columns=50>");
    
    if(hasQueryField(lx, "usortRulesLocale")) {
      UErrorCode err = U_ZERO_ERROR;
      UResourceBundle *bund, *array = NULL;
      const UChar *s = 0;
      int32_t len;

      bund = getCurrentBundle(lx, &err);
      if(bund) array = ures_getByKey(bund, "CollationElements", NULL, &err);
      if(array) s = ures_getStringByKey(array, "Sequence", &len, &err);
      if(U_SUCCESS(err) && s && *s) {
        u_fprintf(lx->OUT, "# %s.txt Rules\r\n", lx->curLocaleName, queryField(lx,"usortRulesLocale"));
        printRuleString(lx,s);
      } else { 
        u_fprintf(lx->OUT, "err %s", u_errorName(err));
      }
    } else if (ruleUrlChars[0]) { /* user has entered a custom rule */
      printRuleString(lx,ruleChars);
    }
    u_fprintf(lx->OUT, "</textarea>\r\n");
    
    u_fprintf(lx->OUT, "</TD>\r\n");
    u_fprintf(lx->OUT, "</TR></TABLE>");

    }
    break;

  default:
    break;
    /* no customization */
  } 

  u_fprintf(lx->OUT, "<TABLE BORDER=1 CELLSPACING=1 CELLPADDING=1 WI_DTH=100% HE_IGHT=100%><TR><TD WI_DTH=\"33%\"><B>%U</B></TD>\r\n",
            FSWF("usortSource", "Source"));

  /* Now, the table itself. first the column headings  ============================== */
  if(inputChars[0]) {
    switch(mode) { 
     case  kSimpleMode:
     {
       u_fprintf(lx->OUT, "<TD WI_DTH=\"33%\"><B>%U</B></TD><TD WI_DTH=\"33%\"><B>%U</B></TD>",
                 FSWF("usortOriginal", "Original"),
                 FSWF("usortCollated", "Collated"));
     }
     break;
      
     case kG7Mode:
     {
       for(i=0;i<G7COUNT;i++) {
         UChar junk[1000];
         uloc_getDisplayName(G7s[i], lx->cLocale, junk, 1000, &status);
         u_fprintf(lx->OUT, "<TD WIDTH=\"10%\">%U</TD>",junk);
       }
     }
     break;
    } /* end switch mode */
  } /* end if inputchars[0]  (header) */
      
  u_fprintf(lx->OUT, "</TR>\r\n");
  
  u_fprintf(lx->OUT, "<TR><TD WIDTH=\"20%\">");
  
  /* the source box */
  u_fprintf(lx->OUT, "<TEXTAREA ROWS=10 COLUMNS=20 COLS=20 NAME=\"EXPLORE_CollationElements\">");
  
  writeEscaped(lx, strChars); 
  /* if(*inputChars)
     u_fprintf(lx->OUT, "%s", inputChars);  */
  
  u_fprintf(lx->OUT, "</TEXTAREA><BR>\r\n");
  
  u_fprintf(lx->OUT, "<INPUT TYPE=SUBMIT VALUE=\"%U\"><P>\r\n",
            FSWF("EXPLORE_CollationElements_Sort", "Sort"));

  u_fprintf(lx->OUT, "<INPUT TYPE=SUBMIT NAME=showCollKey VALUE=\"%U\"><P>\r\n",
            FSWF("EXPLORE_CollationElements_ShowCollKey", "Show Collation Key"));

  /* END source box */
  u_fprintf(lx->OUT, "</TD>\r\n");

  /* ========== Do the actual sort ======== */
  if(inputChars[0] != 0)
  {
    int n;
    
    UChar in[SORTSIZE];
    
    /* have some text to sort */
    unescapeAndDecodeQueryField_enc(in, SORTSIZE, inputChars, lx->chosenEncoding);
    u_replaceChar(in, 0x000D, 0x000A); /* CRLF */
    
    switch(mode)
    {
      case kSimpleMode:
      {
        /* Loop through each sort method */
        for(n=0;n<2;n++)
        {
          USort *aSort = NULL;
          UChar *first, *next;
          int32_t i, count=0;
          
          aSort = customSort;
          if(n>0 && !lxCustSortOpts) {
            /* don't setstrength on 1st item (default) if custom options have been set */
            ucol_setStrength(usort_getCollator(aSort), UCOL_DEFAULT);
          }
          
          /* add lines from buffer */
          
          /* For now, we pass TRUE to clone the text. Wasteful! But, 
             it avoids having to modify the in text AND keep track of the
             ptrs. Now if a usort could be cloned and resorted before
             output.. */
          first = in;
          next = first;
          while(*next) {
            if(*next == 0x000A) {
              if(first != next) {
                usort_addLine(aSort, first, next-first, TRUE, (void*)++count);
              }
              first = next+1;
            }
            next++;
          }
          
          if(first != next) { /* get the LAST line */
            usort_addLine(aSort, first, next-first, TRUE, (void*)++count);
          }      
          
          if(n != 0) {
            usort_sort(aSort); /* first item is 'original' */
          }
          
          u_fprintf(lx->OUT, " <TD VALIGN=TOP>");
          
          for(i=0;i<aSort->count;i++) {
            showSort_outputWord(lx, (n==0)?NULL:aSort, i, aSort->lines[i].chars);
          }

          u_fprintf(lx->OUT, "</TD>");	  
          
          usort_remove(aSort); /* clear out lines, prepare for next go round */
        }
        usort_close(customSort);
      }
      break;

      case kG7Mode:
      {
        for(n=0;n<G7COUNT;n++)
        {
          USort *aSort;
          UErrorCode sortErr = U_ZERO_ERROR;
          UChar *first, *next;
          int32_t i, count=0;
          
          aSort = usort_open(G7s[n], UCOL_TERTIARY, TRUE, &sortErr);
          
          /* add lines from buffer */
          
          /* For now, we pass TRUE to clone the text. Wasteful! But, 
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
                usort_addLine(aSort, first, next-first, TRUE, (void*)++count);
              }
              first = next+1;
            }
            next++;
          }
          
          if(first != next) /* get the LAST line */
          {
            usort_addLine(aSort, first, next-first, TRUE, (void*)++count);
          }      
          
          usort_sort(aSort);
          
          u_fprintf(lx->OUT, " <TD VALIGN=TOP>");
          
          for(i=0;i<aSort->count;i++) {
            showSort_outputWord(lx, aSort, i, aSort->lines[i].chars);
          }
          
          u_fprintf(lx->OUT, "</TD>");	  
          
          usort_close(aSort);
        }
      } /* end G7 demo */
      break;
    }
  }
  u_fprintf(lx->OUT, "</TR></TABLE><P>");

  u_fprintf(lx->OUT, "</form>\r\n");
  
  if(mode != kG7Mode)
    u_fprintf(lx->OUT, "<P><P>%U", FSWF("EXPLORE_CollationElements_strength", "You see four different columns as output. The first is the original text for comparison. The lines are numbered to show their original position. The remaining columns show sorting by different strengths (available as a parameter to the collation function). Groups of lines that sort precisely the same are separated by an underline. Since collation treats these lines as identical, lines in the same group could appear in any order (depending on the precise sorting algorithm used). "));
  
  u_fprintf(lx->OUT, "<P>\r\n");
  u_fprintf(lx->OUT, "%U <a href=\"http://oss.software.ibm.com/icu/userguide/Collate_Intro.html\">%U</a><p>\r\n",
            FSWF("EXPLORE_CollationElements_moreInfo1", "For more information, see the"),
            FSWF("EXPLORE_CollationElements_moreInfo2", "ICU userguide"));

  showExploreCloseButton(lx, locale, "CollationElements");
}

