/**********************************************************************
*   Copyright (C) 1999-2002, International Business Machines
*   Corporation and others.  All Rights Reserved.
***********************************************************************/

#include "locexp.h"

/* routines having to do with the sort sample */

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
void showSort_attrib(LXContext *lx, const char *locale)
{
  UErrorCode  subStatus = U_ZERO_ERROR;

  UCollator *ourCollator = ucol_open(locale, &subStatus);

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
            attrib++)
          {
            subStatus = U_ZERO_ERROR;
            val = ucol_getAttribute(ourCollator,
                                    attrib,
                                    &subStatus);
            u_fprintf(lx->OUT, "  <LI><b>%U</b>: %U\r\n",
                      showSort_attributeName(attrib),
                      showSort_attributeVal(val));
          }
        u_fprintf(lx->OUT, "</UL>\r\n");
        ucol_close(ourCollator);
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
 * @param b The remainder of the query string, for the sort code to consume
 */

void showSort(LXContext *lx, const char *locale, const char *b)
{
  char   inputChars[SORTSIZE];
  char *text;
  char *p;
  int32_t length;
  UChar  strChars[SORTSIZE];
  int    i;

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
    
    /* 'classic' mode- original, default, pri+sec, sec only 
     */
    kSimpleMode,   
    
    /* Custom mode- user can choose any other options they wish. 
     * denoted by 'cust=' in the URL. 
     */
    kCustomMode
  } mode = kSimpleMode;

  strChars[0] = 0;

  if(strstr(locale,"g7") != NULL)
  {
    mode = kG7Mode;
  }
  else if(strstr(b, "&cust=") != NULL)
  {
    mode = kCustomMode;
  }

  /* pull out the text to be sorted. Note, should be able to access this 
     as a POST
   */
  text = strstr(b, "EXPLORE_CollationElements=");

  if(text)
  {
    text += strlen("EXPLORE_CollationElements=");

    unescapeAndDecodeQueryField_enc(strChars, SORTSIZE,
                                    text, lx->chosenEncoding );
    
    p = strchr(text, '&');
    if(p) /* there is a terminating ampersand */
      length = p - text;
    else
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
  
  u_fprintf(lx->OUT, "%U<P>\r\n", FSWF("EXPLORE_CollationElements_Prompt", "Type in some lines of text to be sorted."));

  /* Here, 'configuration information' at the top of the page. ====== */
  switch(mode)
  {
    case kSimpleMode:
    {
       showSort_attrib(lx, locale);
       
       u_fprintf(lx->OUT, "<A HREF=\"?_=%s&%s&cust=\"><IMG BORDER=0 WIDTH=16 HEIGHT=16 SRC=\"../_/closed.gif\" ALT=\"\">%U</A>\r\n<P>", locale, b, FSWF("usort_Custom","Customize..."));
    }
    break;

    case kCustomMode:
    {
      const char *ss;
      int nn;
      UErrorCode customError = U_ZERO_ERROR;

      customSort = usort_open(locale, UCOL_DEFAULT, TRUE, &customError);
      if(U_FAILURE(customError))
      {
        u_fprintf(lx->OUT, "<B>%U %s :</B>", 
                  FSWF("showSort_cantOpenCustomConverter", "Could not open a custom usort/collator for the following locale and reason"), locale);
        explainStatus(lx, customError, NULL); 
        return;
      } 

      customCollator = usort_getCollator(customSort);

      u_fprintf(lx->OUT, "<FORM>");
      u_fprintf(lx->OUT, "<INPUT TYPE=hidden NAME=_ VALUE=%s>", locale);
      u_fprintf(lx->OUT, "<INPUT NAME=EXPLORE_CollationElements VALUE=\"%U\" TYPE=hidden>", strChars);
      u_fprintf(lx->OUT, "<INPUT TYPE=hidden NAME=cust VALUE=>");
      
      /* begin customizables */

      /* -------------------------- UCOL_STRENGTH ----------------------------------- */
      status = U_ZERO_ERROR;
      attribute = UCOL_STRENGTH;
      customStrength = ucol_getAttribute(customCollator, attribute, &status);
      if(ss = strstr(b, "strength="))
      {
        ss += 9; /* skip 'strength=' */
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

      /* ------------------------------- UCOL_FRENCH_COLLATION ------------------------------------- */
      attribute = UCOL_FRENCH_COLLATION;
      status = U_ZERO_ERROR;
      value = ucol_getAttribute(customCollator, attribute, &status);
      status = U_ZERO_ERROR; /* we're prepared to just let the collator fail later. */
      if(strstr(b, "&fr=")) 
      {
        value = UCOL_ON;
      } 
#if 1 
      /* for now - default fr coll to OFF! fix: find out if the user has clicked through once or no */
      else
      {
        value = UCOL_OFF;
      }
#endif
      u_fprintf(lx->OUT, "<input type=checkbox %s name=fr> %U <BR>\r\n",
                (value==UCOL_ON)?"checked":"",  showSort_attributeName(attribute));
      status = U_ZERO_ERROR;
      ucol_setAttribute(customCollator, attribute, value, &status);
      status = U_ZERO_ERROR; /* we're prepared to just let the collator fail later. */
      

      /* ------------------------------- UCOL_CASE_LEVEL ------------------------------------- */
      attribute = UCOL_CASE_LEVEL;
      status = U_ZERO_ERROR;
      value = ucol_getAttribute(customCollator, attribute, &status);
      status = U_ZERO_ERROR; /* we're prepared to just let the collator fail later. */
      if(strstr(b, "&case=")) 
      {
        value = UCOL_ON;
      } 
#if 1 
      /* for now - default fr coll to OFF! fix: find out if the user has clicked through once or no */
      else
      {
        value = UCOL_OFF;
      }
#endif
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
      if(strstr(b, "&dcmp=")) 
      {
        value = UCOL_ON;
      } 
#if 1 
      /* for now - default fr coll to OFF! fix: find out if the user has clicked through once or no */
      else
      {
        value = UCOL_OFF;
      }
#endif
      u_fprintf(lx->OUT, "<input type=checkbox %s name=dcmp> %U <BR>\r\n",
                (value==UCOL_ON)?"checked":"",  showSort_attributeName(attribute));
      status = U_ZERO_ERROR;
      ucol_setAttribute(customCollator, attribute, value, &status);
      status = U_ZERO_ERROR; /* we're prepared to just let the collator fail later. */
      

      /* end customizables ---------------------------------------------------------- */
      u_fprintf(lx->OUT, "<input type=submit value=Change></FORM>\r\n");
      
       u_fprintf(lx->OUT, "<A HREF=\"?_=%s&EXPLORE_CollationElements=%s\"><IMG BORDER=0 WIDTH=16 HEIGHT=16 SRC=\"../_/opened.gif\" ALT=\"\">%U</A>\r\n<P>", locale, inputChars, FSWF("usort_CustomClose","Remove Customization"));
    }
    break;
  } 

  u_fprintf(lx->OUT, "<TABLE BORDER=1 CELLSPACING=1 CELLPADDING=1 WI_DTH=100% HE_IGHT=100%><TR><TD WIDTH=\"20%\"><B>%U</B></TD>\r\n",
            FSWF("usortSource", "Source"));

  /* Now, the table itself. first the column headings  ============================== */
  if(inputChars[0])
  {
    switch(mode)
    { 
     case  kSimpleMode:
     {
       u_fprintf(lx->OUT, "<TD WIDTH=\"20%\"><B>%U</B></TD><TD WIDTH=\"20%\"><B>%U</B></TD><TD WIDTH=\"20%\"><B>%U</B></TD><TD WIDTH=\"20%\"><B>%U</B></TD>",
                 FSWF("usortOriginal", "Original"),
                 FSWF("usortTertiary", "Default"),
                 FSWF("usortSecondary", "Primary & Secondary"),
                 FSWF("usortPrimary", "Primary only"));
     }
     break;
      
     case kCustomMode:
     {
       u_fprintf(lx->OUT, "<TD WIDTH=\"20%\"><B>%U</B></TD><TD WIDTH=\"20%\"><B>%U</B></TD>",
                 FSWF("usortOriginal", "Original"),
                 FSWF("usortSorted", "Sorted"));
     }
     break;
      
     case kG7Mode:
     {
       for(i=0;i<G7COUNT;i++)
         {
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
  u_fprintf(lx->OUT, "<FORM ACTION=\"#EXPLORE_CollationElements\" METHOD=GET>\r\n");
  u_fprintf(lx->OUT, "<INPUT TYPE=HIDDEN NAME=\"_\" VALUE=\"%s\">\r\n", locale);
  u_fprintf(lx->OUT, "<TEXTAREA ROWS=10 COLUMNS=20 COLS=20 NAME=\"EXPLORE_CollationElements\">");
  
  writeEscaped(lx, strChars); 
  /* if(*inputChars)
     u_fprintf(lx->OUT, "%s", inputChars);  */
  
  u_fprintf(lx->OUT, "</TEXTAREA><BR>\r\n");
  
  if(mode != kCustomMode) /* for now ...  TODO fix: HIDE the Sort button inside custom. Why? It would remove
                             all customization. */
  {
    u_fprintf(lx->OUT, "<INPUT TYPE=SUBMIT VALUE=\"%U\"></FORM><P>\r\n",
              FSWF("EXPLORE_CollationElements_Sort", "Sort"));
  }

  /* END source box */
  u_fprintf(lx->OUT, "</TD>\r\n");

  /* ========== Do the actual sort ======== */
  if(inputChars[0] != 0)
  {
    UCollationStrength sortTypes[] = { UCOL_IDENTICAL /* not used */, UCOL_DEFAULT, UCOL_SECONDARY, UCOL_PRIMARY };
    int n;
    
    UChar in[SORTSIZE];
    UErrorCode status2 = U_ZERO_ERROR;
    
    /* have some text to sort */
    unescapeAndDecodeQueryField_enc(in, SORTSIZE, inputChars, lx->chosenEncoding);
    u_replaceChar(in, 0x000D, 0x000A); /* CRLF */
    
    switch(mode)
    {
      case kSimpleMode:
      {
        /* Loop through each sort method */
        for(n=0;n<4;n++) /* not 4 */
        {
          USort *aSort;
          UErrorCode sortErr = U_ZERO_ERROR;
          UChar *first, *next;
          int32_t i, count=0;
          
          aSort = usort_open(locale, sortTypes[n], TRUE, &sortErr);
          
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
          
          if(n != 0)
            usort_sort(aSort); /* first item is 'original' */
          
          
          u_fprintf(lx->OUT, " <TD VALIGN=TOP>");
          
          for(i=0;i<aSort->count;i++)
          {
            UBool doUnderline = TRUE;
            
            if( ((i+1)<aSort->count) &&
                (aSort->lines[i].keySize == aSort->lines[i+1].keySize) &&
                !memcmp(aSort->lines[i].key,
                        aSort->lines[i+1].key,
                        aSort->lines[i].keySize))
            {
              doUnderline = FALSE;
            }
            
            u_fprintf(lx->OUT, "%s%02d.%U%s<BR>\n",
                      (doUnderline?"<U>":""),
                      (int32_t)aSort->lines[i].userData, aSort->lines[i].chars,
                      (doUnderline?"</U>":"")
                      );
          }
          
          u_fprintf(lx->OUT, "</TD>");	  
          
          usort_close(aSort);
        }
      }
      break;

      case kCustomMode: 
      {
          USort *aSort;
          UErrorCode sortErr = U_ZERO_ERROR;
          UChar *first, *next;
          int32_t i, count=0;
          
          u_fprintf(lx->OUT, "<TD valign=top>");
          
          aSort = customSort; /* from above */
        
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
                *next = 0; /* we are the only user of this text! */
                          
                usort_addLine(aSort, first, next-first, TRUE, (void*)++count);
                u_fprintf(lx->OUT, "%02d.%U<BR>\n",
                          count, first);

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
          
          u_fprintf(lx->OUT, "</TD> <TD VALIGN=TOP>");
          
          for(i=0;i<aSort->count;i++)
          {
            UBool doUnderline = TRUE;
            
            if( ((i+1)<aSort->count) &&
                (aSort->lines[i].keySize == aSort->lines[i+1].keySize) &&
                !memcmp(aSort->lines[i].key,
                        aSort->lines[i+1].key,
                        aSort->lines[i].keySize))
            {
              doUnderline = FALSE;
            }
            
            u_fprintf(lx->OUT, "%s%02d.%U%s<BR>\n",
                      doUnderline?"<U>":"",
                      (int32_t)aSort->lines[i].userData, aSort->lines[i].chars,
                      doUnderline?"</U>":""
                      );
          }
          
          u_fprintf(lx->OUT, "</TD>");	  
          
          usort_close(aSort);

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
          
          for(i=0;i<aSort->count;i++)
          {
            UBool doUnderline = TRUE;
            
            if( ((i+1)<aSort->count) &&
                (aSort->lines[i].keySize == aSort->lines[i+1].keySize) &&
                !memcmp(aSort->lines[i].key,
                        aSort->lines[i+1].key,
                        aSort->lines[i].keySize))
            {
              doUnderline = FALSE;
            }
            
            u_fprintf(lx->OUT, "%s%02d.%U%s<BR>\n",
                      doUnderline?"<U>":"",
                      (int32_t)aSort->lines[i].userData, aSort->lines[i].chars,
                      doUnderline?"</U>":""
                      );
          }
          
          u_fprintf(lx->OUT, "</TD>");	  
          
          usort_close(aSort);
        }
      } /* end G7 demo */
      break;
    }
  }
  u_fprintf(lx->OUT, "</TR></TABLE><P>");
  
  if(mode != kG7Mode)
    u_fprintf(lx->OUT, "<P><P>%U", FSWF("EXPLORE_CollationElements_strength", "You see four different columns as output. The first is the original text for comparison. The lines are numbered to show their original position. The remaining columns show sorting by different strengths (available as a parameter to the collation function). Groups of lines that sort precisely the same are separated by an underline. Since collation treats these lines as identical, lines in the same group could appear in any order (depending on the precise sorting algorithm used). "));
  
  u_fprintf(lx->OUT, "<P>\r\n");
  showExploreCloseButton(lx, locale, "CollationElements");
}

