/**********************************************************************
*   Copyright (C) 1999-2005, International Business Machines
*   Corporation and others.  All Rights Reserved.
***********************************************************************/

#include "locexp.h"

/* routines having to do with the break iteration sample */

/*****************************************************************************
 *
 * Explorer for #'s
 */

void showExploreBreak(LXContext *lx, const char *locale)
{
    UChar pattern[1024];
    UNumberFormat  *nf = NULL; /* numfmt in the current locale */
    UNumberFormat  *nf_default = NULL; /* numfmt in the default locale */
    UNumberFormat  *nf_spellout = NULL;
    UErrorCode   status = U_ZERO_ERROR;

    const UChar *defaultValueErr = 0;
    const UChar *localValueErr   = 0;

    showKeyAndStartItem(lx, "EXPLORE_Break", FSWF("EXPLORE_Break", "Explore &gt; Numbers"), locale, FALSE, U_ZERO_ERROR);

    u_fprintf(lx->OUT, "%S<p>", FSWF("formatExample_Break_What","This example demonstrates break iteration"));

    exploreFetchNextPattern(lx, pattern, strstr(lx->queryString,"EXPLORE_Break")); 

    /* 
     *
     *
     *   REAL CODE GOES HERE
     *
     *
     *
     */


    showExploreCloseButton(lx, locale, "Break");
    u_fprintf(lx->OUT, "</td><td align=\"left\" valign=\"top\">");
    printHelpTag(lx, "EXPLORE_Break", NULL);
    u_fprintf(lx->OUT, "</td>\r\n");

  
    showKeyAndEndItem(lx, "EXPLORE_Break", locale);
}

