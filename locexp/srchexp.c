/**********************************************************************
*   Copyright (C) 2002-2005, International Business Machines
*   Corporation and others.  All Rights Reserved.
***********************************************************************/

/* Search Explorer... search the Locales looking for strings */

#include "locexp.h"
#include "unicode/ustdio.h"
#include "unicode/ures.h"
#include "unicode/usearch.h"


/* Recursive search of a bundle.  'lastTag' is the "path" to the current object.. it's an in/out parameter. 
    TRUE will be returned if anything was found at all. */
static UBool bundleHasString( UResourceBundle *r, LXContext *lx, const UChar *str, MySortable *loc,
        int *totalHits,  char * lastTag, UStringSearch *search, UErrorCode *status)
{
  UResourceBundle *sub = NULL;
  UErrorCode s2 = U_ZERO_ERROR;
  const char *key;
  int lastTagLen;

  if(U_FAILURE(*status)) return FALSE;
    
  lastTagLen = strlen(lastTag);
  key = ures_getKey(r);
  if(key != NULL)  
  {
        if(lastTag[0] != 0)
        {
            strcat(lastTag, ":");
        }
        strcat(lastTag, key);
  }
    
  switch(ures_getType(r))
  {
    case RES_NONE: break; 
    
    case RES_STRING:
      {
        int pos;
	const UChar *u;
	int32_t len = 0;
	u = ures_getString(r, &len, &s2);
        
        if(len  > 0)
        {
            if(U_FAILURE(s2)) { fprintf(lx->fOUT, " Err %s\n", u_errorName(s2));  return FALSE; }
            
            
            usearch_setText(     search,u,len,status);
            if(U_FAILURE(*status)) { fprintf(lx->fOUT, " Err on setText %s\n", u_errorName(*status));  return FALSE; }
            for (pos = usearch_first(search, status); 
                                        pos != USEARCH_DONE; 
                                        pos = usearch_next(search, status))
            {
	      (*totalHits)++;
              return TRUE;
            }
        }
      }
      break; /* End of RES_STRING */

    case RES_TABLE: 
    {
	int i;
	for(i=0;i<ures_getSize(r);i++)
	{
	  s2 = U_ZERO_ERROR;
	  sub = ures_getByIndex(r, i, sub, &s2);
	  if(U_FAILURE(s2)) break;
          
          /* Recurse. */
	  if(bundleHasString(sub, lx, str, loc, totalHits, lastTag, search, status) == TRUE)
          {
            /* Close up shop and get out. */
            ures_close(sub);
            return TRUE;
          }
        }
      }
    break; /* End of RES_TABLE */

    case RES_ARRAY:
      {
	int i;
	for(i=0;i<ures_getSize(r);i++)
	{
	  s2 = U_ZERO_ERROR;
	  sub = ures_getByIndex(r, i, sub, &s2);
	  if(U_FAILURE(s2)) break;
          
          /* Recurse */
	  if(bundleHasString(sub, lx, str, loc, totalHits, lastTag, search, status) == TRUE)
          {
            ures_close(sub);
            return TRUE;
          }
        }
      }
      break; /* END of RES_ARRAY */
      
    default:
    /* Don't care about other types. */
        break;
    }
    
    lastTag[lastTagLen]=0; /*Chop the string back to what it was when we were called */
    return FALSE;
}


/* Search in a specified Locale by name  (and its sublocales) */
static void doSearch( LXContext *lx, const UChar *str, MySortable *loc, int *totalHits, UStringSearch *search)
{
  int i;
  UResourceBundle *r = NULL;
  UErrorCode status = U_ZERO_ERROR;
  
  char lastTag[200];
  
  if(U_FAILURE(status))
        return;
        
  r = ures_open(NULL, loc->str, &status);

  lastTag[0] = 0;
  
  /* Do the search .. */
  if(bundleHasString(r, lx, str, loc, totalHits, lastTag, search, &status))
  {
    char *p = NULL;
    char *q = NULL;
    p = strchr(lastTag, ':');
    if(p)
    {
        *p = 0;
        q = p+1;
        p = " : ";
    }
    u_fprintf(lx->OUT, "<li><a href=\"?_=%s\">%S</a> :", loc->str, loc->ustr);
    u_fprintf(lx->OUT, " <a href=\"?_=%s#%s\">%S</a>",  loc->str, lastTag, FSWF/**/(/**/lastTag,lastTag));
    u_fprintf(lx->OUT, "%s%s\r\n", p?p:"", q?q:"");
  }
  
  if(U_FAILURE(status)) u_fprintf(lx->OUT,"err %s\n", u_errorName(status));

  /* Search all sublocales */
  for(i=0;i<loc->nSubLocs;i++)
  {
    doSearch(lx, str, loc->subLocs[i], totalHits, search);
  }

  ures_close(r);
}

void showExploreSearchForm(LXContext *lx, const UChar *valueString)
{
    UChar nulls[] = { 0x0000 } ;
    if(valueString == NULL) { valueString = nulls; }

    u_fprintf(lx->OUT, "<form><input type=\"hidden\" name=\"_\" value=\"%s\" />\r\n",
              lx->curLocaleName);
    u_fprintf(lx->OUT, "<input value=\"%S\" name=\"EXPLORE_search\" /> \r\n", valueString);
    u_fprintf(lx->OUT, "<input type=\"submit\" value=\"%S\" />\r\n", FSWF("explore_search", "Search"));
    u_fprintf(lx->OUT, "</form>\r\n");
}


/* This is the entrypoint from Locale Explorer */
extern void showExploreSearch( LXContext *lx)
{
  /*char *tmp;*/
    UChar valueString[1024];
    UErrorCode status = U_ZERO_ERROR;
    int32_t i;
    char *term = "";
    /*const char *p;*/
    char *inputChars = NULL;
    int length;
    int totalHits = 0; /* cumulative - for circuit breaking purposes */
    UStringSearch *search = NULL;
    
    term = (char*)queryField(lx, "EXPLORE_search");

    if(term) { 
      length = strlen(term);
    
      if(length > (1024-1)) {
        length = 1024-1; /* safety ! */
        term[1024-1]=0; /* we own the result of term - for now */
      }
      
      unescapeAndDecodeQueryField_enc(valueString, 1000, term, lx->convRequested);
      inputChars = strdup(term);
      inputChars[length] = 0;
      for(i=0;i<length;i++)
        if(inputChars[i]=='+') inputChars[i]=' ';
    } else {
      valueString[0] = 0;
    }

    showExploreSearchForm(lx, valueString);
    
    if(!valueString || u_strlen(valueString)<=0)
    {
        return;
    }
        
    u_fprintf(lx->OUT,"<hr />\r\n");
    
    if(U_FAILURE(status)) { fprintf(lx->fOUT, " showExploreSearch() - ? %s\n", u_errorName(status));  return; }
    
    /* Set up the search.. */
    search = usearch_open(valueString, -1, valueString, -1, lx->dispLocale,  NULL, &status);
    if(U_FAILURE(status))
    {
        u_fprintf(lx->OUT, "Err opening search in showExploreSearch, err %s\n", u_errorName(status));           
        return;
    }
    
    /* Set up its collator .. */
    ucol_setStrength(usearch_getCollator(search) , UCOL_PRIMARY);
    usearch_reset(search); /* Reset the search so it knows we have changed the collator. */
    usearch_setPattern(search, valueString, -1, &status);
    
    if(U_FAILURE(status))
    {
        u_fprintf(lx->OUT, "Err configuring search in showExploreSearch, err %s\n", u_errorName(status));           
        usearch_close(search);
        return;
    }
    
    /* Do the actual search */
    u_fprintf(lx->OUT, "<I>");
    u_fprintf_u(lx->OUT, FSWF("EXPLORE_search_searching","Searching for <B>%S</B>..."),
            valueString);
    u_fprintf(lx->OUT, "</I>\r\n<ol>\r\n");

    doSearch(lx, valueString, lx->curLocale?lx->curLocale:lx->locales, &totalHits, search);
    u_fprintf(lx->OUT, "</ol>\r\n");
            
    if(totalHits == 0) 
    {
      u_fprintf(lx->OUT, "%S\r\n",  FSWF("EXPLORE_search_notfound", "Sorry, nothing was found."));
    }
    else
    {
      u_fprintf_u(lx->OUT, FSWF("EXPLORE_search_found", "%d items were found."), totalHits);
      u_fprintf(lx->OUT, " %S\r\n", FSWF("EXPLORE_search_oneperlocale", "Note that only the first match in each locale is shown."));
    }

    usearch_close(search);
    
    u_fprintf(lx->OUT, "<hr />\r\nThis searching is an experimental service. Please write srl@jtcsv.com "
            "with any comments or complaints you might have!"
	      "Results brought to you by the ICU Unicode Search Capability.<p>\r\n");
    
}
