/******************************************************************************
*
*   Copyright (C) 1999-2012, International Business Machines
*   Corporation and others.  All Rights Reserved.
*
*******************************************************************************
 * Steven R. Loomis <srl@jtcsv.com>
 *
 * Main file for usort. Gives a simple 
 *   command line interface.
 *
 *  1999-oct-07  srl   Needs arg cleanup..
 */

#include "unicode/usort.h"


#include <stdlib.h>


void usage(const char *pname, const char *msg)
{
  fprintf(stderr, "[ Note: this list isn't current. Go bug srl. ] \r\n");
  if(msg)
    fprintf(stderr,"%s: %s\n", pname,msg);
  fprintf(stderr, "Usage: %s %s [ -{123I} ] [ -L locale ]\n", pname,
#ifdef DECOMPOSE
	  " [ -D ] "
#else
	  ""
#endif
	  );

#ifdef DECOMPOSE
  fprintf(stderr, "\n\t-D               \n\t\tUse Decomposition iterator\n");
#endif

  fprintf(stderr, "\n\t-F codepage      \n\t\tSet the input codepage\n");
  fprintf(stderr, "\n\t-T codepage      \n\t\tSet the output codepage\n");
  fprintf(stderr, "\n\t-L locale        \n\t\tSet the locale to 'locale'\n");
  fprintf(stderr, "\n\t-c rules         \n\t\tUse 'rules' as custom tailoring rules\n");
  /*  fprintf(stderr, "\n\t-C rulefile      \n\t\tRead from file 'rules' for custom tailoring rules, in the input codepage\n"); */
  fprintf(stderr, "\n\t-1, -2, -3, -I   \n\t\tSet the collation strength to \n\t\tprimary, secondary, tertiary, or identical (respectively)\n");
  fprintf(stderr, "\n\t-n               \n\t\tTurn the normalization on\n");
  fprintf(stderr, "\n\t-t               \n\t\tTrim the trailing spaces from the strings\n");
  fprintf(stderr, "\n");
  fprintf(stderr, "Locale sensitive sort, (c) 1999-2012 IBM, Inc. Uses ICU.\n");
  fprintf(stderr, "http://www.icu-project.org/\n");
  exit(-1);
}

int main(int argc, const char *argv[])
{
  char tmp[200];
  /* fetch the options */
  int i;
  int verbose = 0;
  UErrorCode status = U_ZERO_ERROR;
  UConverter *fromConverter = NULL, *toConverter = NULL;
  const char *fromCodepage;
  const char *customRulesStr = NULL;

  const char *toCodepage;
  const char *locale = NULL;
#ifdef DECOMPOSE
  UBool useDecompose = false
#endif
  UBool escapeMode = false;
  USort      *list = NULL;
  UCollationStrength strength = UCOL_DEFAULT_STRENGTH;
  UColAttributeValue normalizationMode = UCOL_OFF;
  UBool trim = false;

  fromCodepage = ucnv_getDefaultName();

  if(!fromCodepage) {
    fromCodepage = "latin-1";
  }
  
  toCodepage = fromCodepage;

  for(i=1;i<argc;i++)
    {
#ifdef SDEBUG
      fprintf(stderr,"Arg[%d]= %s\n", i, argv[i]);
#endif
      if(argv[i][0] == '-')
	{
	  switch(argv[i][1])
	    {
#ifdef DECOMPOSE
	    case 'D':
	      useDecompose = true;
	      break;
#endif
	 
            case 'c':
              i++;
              customRulesStr = argv[i];
              break;
              
 	    case 'T':
	      i++;
	      toCodepage = argv[i];
	      break;

            case 'F':
	      i++;
	      fromCodepage = argv[i];
	      break;

	    case 'e':
	      escapeMode = true;
	      break;

	    case '1':
	      strength = UCOL_PRIMARY;
	      break;
	      
	    case '2':
	      strength = UCOL_SECONDARY;
	      break;

	    case '3':
	      strength = UCOL_TERTIARY;
	      break;
	      
	    case 'I':
	      strength = UCOL_IDENTICAL;
	      break;

        case 'v':
          verbose = 1;
          break;

        case 'n':
          normalizationMode = UCOL_ON;
          break;

        case 't':
          trim = true;
          break;

	    case 'L':
	      i++;
	      if(i<argc)
		{
            locale = argv[i];
		  /*uloc_setDefault(argv[i], &status);
		  if(U_FAILURE(status))
		    {
		      fprintf(stderr, "Error %d trying to set the locale to %s\n",
			      status, argv[i]);
		      abort();
		    }
            */
		}
	      else
		usage(argv[0], "Missing option to -L");
	      break;

	    default:
	      sprintf(tmp,"Unknown option %s", argv[i]);
	      usage(argv[0], tmp);
	    }
	}
    }

  ucnv_setDefaultName(fromCodepage); /* sets OUTPUT codepage */

  /***   Options loaded. Now, set up some data */
  if(verbose) fprintf(stderr, "Opening from converter %s\n",fromCodepage);
  fromConverter = ucnv_open(fromCodepage, &status);

  if(U_FAILURE(status))
    {
      fprintf(stderr,"Couldn't open fromConverter[%s]: %d\n", fromCodepage, status);
      abort();
    }


  if(customRulesStr!=NULL) {
    int32_t rulesMax = 2048;
    int32_t rulesLen = 0;
    const UChar *baseRules = NULL;
    int32_t baseRulesLen =0;
    UCollator *theCollator;
    UChar *rules = (UChar*)malloc(rulesMax*sizeof(UChar));

    theCollator = ucol_open(locale, &status);
    if(U_FAILURE(status))   {
      fprintf(stderr,"Couldn't open base collator:  %s  \n", u_errorName(status));
      abort();
    }
    rulesLen = ucnv_toUChars(fromConverter, rules, rulesMax, customRulesStr, -1, &status);
    if(U_FAILURE(status))   {
      fprintf(stderr,"Couldn't convert custom rules to unicode: %s (max buffer=%d UChars) \n", u_errorName(status), rulesMax);
      abort();
    }
    baseRules=ucol_getRules(theCollator, &baseRulesLen);
    /* ok. Close the collator */
    ucol_close(theCollator);
    rules=realloc(rules, (rulesLen+baseRulesLen)*sizeof(UChar));
    u_strncpy(rules+rulesLen, baseRules, baseRulesLen); /* append base rules */
    rulesLen += baseRulesLen; /* now has base rules following */
    theCollator = ucol_openRules(rules, rulesLen, normalizationMode, strength, NULL /* TODO: catch parseerr*/, &status);
    if(U_FAILURE(status))   {
      fprintf(stderr,"Couldn't open collator with cusotm rules: %s \n", u_errorName(status));
      abort();
    }
    list = usort_openWithCollator(theCollator, true, &status); /* common error check in a few lines.. */
  } else {
    list = usort_open(locale, strength, true, &status);
    ucol_setAttribute(usort_getCollator(list), UCOL_NORMALIZATION_MODE, normalizationMode, &status);
  }

  if(U_FAILURE(status))
    {
      fprintf(stderr,"Couldn't open sortlist: %d\n", status);
      abort();
    }
  list->trim = trim;
  
  if(verbose) fprintf(stderr, "Opening 'to' converter %s\n",toCodepage);
  toConverter = ucnv_open(toCodepage, &status);

  if(U_FAILURE(status))
    {
      fprintf(stderr,"Couldn't open toConverter[%s]: %d\n", toCodepage, status);
      abort();
    }
  
  /***     Load the data */

  /* For now only load the data from the stdin */
  usort_addLinesFromFILE( list, NULL, fromConverter, true );

  /*** Now, sort and clean up */

  usort_sort ( list ) ;
  usort_printToFILE(list, NULL, toConverter);

  /*** Clean up          */

  usort_close(list);
  ucnv_close(toConverter);
  ucnv_close(fromConverter);

  return 0;
}

