/**
 * (c) 1999 IBM, Inc.
 *
 * Steven R. Loomis <srl@monkey.sbay.org>
 *
 * Main file for usort. Gives a simple 
 *   command line interface.
 *
 *  1999-oct-07  srl   OK to check in. Needs arg cleanup..
 */

#include "usort.h"

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

  fprintf(stderr, "\n\t-L locale        \n\t\tSet the locale to 'locale'\n");
  fprintf(stderr, "\n\t-1, -2, -3, -I   \n\t\tSet the collation strength to \n\t\tprimary, secondary, tertiary, or identical (respectively)\n");
  fprintf(stderr, "\n");
  fprintf(stderr, "Locale sensitive sort, (c) 1999 IBM, Inc. Uses ICU.\n");
  fprintf(stderr, "http://www.alphaWorks.ibm.com/tech/icu\n");
  exit(-1);
}

int main(int argc, const char *argv[])
{
  char tmp[200];
  /* fetch the options */
  int i;
  UErrorCode status = ZERO_ERROR;
  UConverter *fromConverter = NULL, *toConverter = NULL;
  const char *fromCodepage;
  const char *toCodepage;
  bool_t useDecompose = FALSE, escapeMode = FALSE;
  USort      *list = NULL;
  UCollationStrength strength = UCOL_DEFAULT_STRENGTH;
  fromCodepage = getenv("ICU_ENCODING");
  if(!fromCodepage)
    fromCodepage = "latin-1";
  
  toCodepage = fromCodepage;

  ucnv_setDefaultName(fromCodepage); /* sets OUTPUT codepage */

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
	      useDecompose = TRUE;
	      break;
#endif
	      
	    case 'T':
	      i++;
	      toCodepage = argv[i];
	      break;
	      
	    case 'e':
	      escapeMode = TRUE;
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

	    case 'L':
	      i++;
	      if(i<argc)
		{
		  uloc_setDefault(argv[i], &status);
		  if(FAILURE(status))
		    {
		      fprintf(stderr, "Error %d trying to set the locale to %s\n",
			      status, argv[i]);
		      abort();
		    }
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

  /***   Options loaded. Now, set up some data */


  list = usort_open(NULL, strength, TRUE, &status);

  if(FAILURE(status))
    {
      fprintf(stderr,"Couldn't open sortlist: %d\n", status);
      abort();
    }
  
  fromConverter = ucnv_open(fromCodepage, &status);

  if(FAILURE(status))
    {
      fprintf(stderr,"Couldn't open fromConverter[%s]: %d\n", fromCodepage, status);
      abort();
    }

  toConverter = ucnv_open(toCodepage, &status);

  if(FAILURE(status))
    {
      fprintf(stderr,"Couldn't open toConverter[%s]: %d\n", toCodepage, status);
      abort();
    }
  
  /***     Load the data */

  /* For now only load the data from the stdin */
  usort_addLinesFromFILE( list, NULL, fromConverter, TRUE );

  /*** Now, sort and clean up */

  usort_sort ( list ) ;
  usort_printToFILE(list, NULL, toConverter);

  /*** Clean up          */

  usort_close(list);
  ucnv_close(toConverter);
  ucnv_close(fromConverter);

  return 0;
}

