/* Copyright (c) 2000-2002 IBM, Inc. and Others, all rights reserved */

#include "locexp.h"
#include "unicode/ustdio.h"

void fillFieldNames(const UChar** n)
{
    n[UCAL_ERA] =         FSWF("UCAL_ERA", "Era field");
    n[UCAL_YEAR] =        FSWF("UCAL_YEAR", "Year field");
    n[UCAL_MONTH] =       FSWF("UCAL_MONTH", "Month field");
    n[UCAL_WEEK_OF_YEAR]= FSWF("UCAL_WEEK_OF_YEAR", "Week of year field");
    n[UCAL_WEEK_OF_MONTH]=FSWF("UCAL_WEEK_OF_MONTH", "Week of month field");
    n[UCAL_DATE] =        FSWF("UCAL_DATE", "Date field");
    n[UCAL_DAY_OF_YEAR] = FSWF("UCAL_DAY_OF_YEAR", "Day of year field");
    n[UCAL_DAY_OF_WEEK] = FSWF("UCAL_DAY_OF_WEEK", "Day of week field");
    n[UCAL_DAY_OF_WEEK_IN_MONTH] =
        FSWF("UCAL_DAY_OF_WEEK_IN_MONTH", "Day of week in month field");
    n[UCAL_AM_PM] =       FSWF("UCAL_AM_PM", "AM/PM field");
    n[UCAL_HOUR] =        FSWF("UCAL_HOUR", "Hour field");
    n[UCAL_HOUR_OF_DAY] = FSWF("UCAL_HOUR_OF_DAY", "Hour of day field");
    n[UCAL_MINUTE] =      FSWF("UCAL_MINUTE", "Minute field");
    n[UCAL_SECOND] =      FSWF("UCAL_SECOND", "Second field");
    n[UCAL_MILLISECOND] = FSWF("UCAL_MILLISECOND", "Millisecond field");
    n[UCAL_ZONE_OFFSET] = FSWF("UCAL_ZONE_OFFSET", "Zone offset field");
    n[UCAL_DST_OFFSET] =  FSWF("UCAL_DST_OFFSET", "DST offset field");
    n[UCAL_YEAR_WOY] =    FSWF("UCAL_YEAR_WOY", "Year / week of year");
    n[UCAL_DOW_LOCAL] =   FSWF("UCAL_DOW_LOCAL", "Day of week localized (1..7)");
}

void printCalMenuSection( LXContext *lx, const char *num, char type,
                          char thisType,const UChar *name)
{
    /* if(type==thisType) */  /* LEFT tab A */
    {
        u_fprintf(lx->OUT, "<TD BGCOLOR=\"#00cc99\" WIDTH=\"5%%\" HEIGHT=9><IMG ALIGN=LEFT WIDTH=15 HEIGHT=30 ALT=\"\" SRC=\"../_/tab_aleft.gif\">");
        u_fprintf(lx->OUT, "<A HREF=\"?_=x-klingon&EXPLORE_Calendar=%c&NP_DBL=%s\">%U</A>", 
                  thisType, num, name);
        u_fprintf(lx->OUT, "<IMG WIDTH=15 HEIGHT=30 ALT=\"\" SRC=\"../_/tab_aright.gif\"></TD>");

    }
}

void printCalMenuBar( LXContext *lx, const char *num, char type )
{
/*
  tab active color = #00cc99
  tab height = 30
  tablet width=10
  lower bar dim 4x4 [doesn't matter]
*/
    switch(type)
    {
    case 'c':
    case 'f':
        break;
    default:
        type='f';
    }
  
    u_fprintf(lx->OUT, "<TABLE cellspacing=0 border=0 cellpadding=0><TR>");
  
    printCalMenuSection(lx, num, type,
                        'f', FSWF("calexp_fieldsTab", "Fields"));
    u_fprintf(lx->OUT, "<TD HEIGHT=4 WIDTH=\"3%%\">&nbsp</TD>");
    printCalMenuSection(lx, num, type,
                        'c', FSWF("calexp_calTab", "Calendar"));
    u_fprintf(lx->OUT, "<TD HEIGHT=2 COLSPAN=3 WIDTH=100%%>D&nbsp;</TD></TR>");
    u_fprintf(lx->OUT, "<TR><TD COLSPAN=10 HEIGHT=3 BACKGROUND=\"../_/tab_bot.gif\"></TD><FONT SIZE=4>&nbsp;</FONT></TR></TABLE>\r\n");
}


extern void showExploreCalendar( LXContext *lx, const char *qs)
{
    UNumberFormat *nf = NULL; /* for formatting the number */
    char *tmp;
    int32_t parsePos = 0;
    UChar valueString[1024];
    UErrorCode status = U_ZERO_ERROR;
    UDate now;
    const UChar *fieldNames[UCAL_FIELD_COUNT];
    int32_t i, j;
    UCalendar *cal, *cal2;
    char type  = 'x';
    char num[100];

    /* fill out field names */
    fillFieldNames(fieldNames);

    if((tmp = strstr(qs, "EXPLORE_Calendar")))
    {
        type = *(tmp + strlen("EXPLORE_Calendar="));
    }

    nf = unum_open(0, FSWF("EXPLORE_DateTimePatterns_dateAsNumber", "#"), -1, NULL, NULL, &status);
    status = U_ZERO_ERROR; /* ? */

    if ((tmp = strstr(qs,"NP_DBL"))) /* Double: UDate format input ============= */
    {
        /* Localized # */
        tmp += 7;

        unescapeAndDecodeQueryField(valueString, 1000, tmp);
        u_replaceChar(valueString, 0x0020, 0x00A0);

        status = U_ZERO_ERROR;
        now = unum_parseDouble(nf, valueString, -1, &parsePos, &status);
        if(U_FAILURE(status))
        {
            now = ucal_getNow();
        }
    }
    else
    {
        now = ucal_getNow();
    }
  
    sprintf(num, "%f", now);

    u_fprintf(lx->OUT, "[%D %T]<P>", now, now );

    cal = ucal_open(lx->timeZone, -1, lx->cLocale, UCAL_TRADITIONAL, &status);
    ucal_setMillis(cal, now, &status);

    if(U_FAILURE(status))
    {
        u_fprintf(lx->OUT, "%U ", FSWF("calexp_errGet", "Can't get initial date: "));
        explainStatus(lx, status, "");
        return;
    }

    status = U_ZERO_ERROR;

    printCalMenuBar( lx, num, type );

    switch(type)
    {
    case 'c':
        u_fprintf(lx->OUT, "CAL<P>\r\n");
        break;

    case 'f':
    default:

        /**************** calendar fields **********************/
    {
#define  NR_ITEMS 4
        static const  UBool   roll[NR_ITEMS] =  {   TRUE, FALSE, FALSE, TRUE };
        static const  int32_t amt [NR_ITEMS] =  {    -1 ,  -1  ,  1   ,  1   };
        static const char    *nam [NR_ITEMS] =  { "&lt;", "-"  , "+"  , "&gt;"};

        u_fprintf(lx->OUT, "<TABLE BORDER=2>\r\n");
        for (i=0;i<UCAL_FIELD_COUNT;i++)
        {
            int32_t  val;
            status = U_ZERO_ERROR;
            val=ucal_get(cal, (UCalendarDateFields)i, &status);

            u_fprintf(lx->OUT, "  <TR>\r\n");
      
            u_fprintf(lx->OUT, "    <TD %s>%ld</TD>\r\n", 
                      (ucal_isSet(cal, (UCalendarDateFields)i)?
                       (U_FAILURE(status)?"BGCOLOR=#FF0000":""):"BGCOLOR=#999999"),
                      val);
      
            /* roll/set table */
            u_fprintf(lx->OUT, "    <TD>\r\n");
            status = U_ZERO_ERROR;
            for(j=0;j<NR_ITEMS;j++)
            {
                cal2 = ucal_open(lx->timeZone, -1, lx->cLocale, UCAL_TRADITIONAL, &status);
                ucal_setMillis(cal2, now, &status);
                if(U_SUCCESS(status))
                {
                    if(roll[j] == TRUE)
                    {
                        ucal_roll(cal2, (UCalendarDateFields)i, amt[j], &status);
                    }
                    else
                    {
                        ucal_set(cal2, (UCalendarDateFields)i,
                                 ucal_get(cal2, (UCalendarDateFields)i, &status) + amt[j]);
                    }
                }
        
                /* Whew! */
                if(U_SUCCESS(status))
                {
                    char num[100];

                    sprintf(num, "%f", ucal_getMillis(cal2, &status));
                    u_fprintf(lx->OUT, "<A HREF=\"?_=%s&EXPLORE_Calendar&NP_DBL=%s\">",
                              lx->cLocale,
                              num);
                    u_fprintf(lx->OUT, "%s", nam[j]);
                    u_fprintf(lx->OUT, "</A>");
                }
                else
                {
                    u_fprintf(lx->OUT, "<FONT COLOR=\"#99D999\">");
                    u_fprintf(lx->OUT, "%s", nam[j]);
                    u_fprintf(lx->OUT, "</FONT>");
          
                }
        
                ucal_close(cal2);
            }
            u_fprintf(lx->OUT, "    </TD>\r\n");
      
            u_fprintf(lx->OUT, "    <TD>%U</TD>\r\n", fieldNames[i]);
            u_fprintf(lx->OUT, "  </TR>\r\n");
        }
        if(U_FAILURE(status))
        {
            u_fprintf(lx->OUT, "  <TR><TD COLSPAN=2>ERR: %s</TD></TR>\r\n",
                      u_errorName(status));
        }
        u_fprintf(lx->OUT, "</TABLE>\r\n");
    } /* end calendar table [f] */
    break;

    } 
}



