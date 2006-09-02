/**********************************************************************
*   Copyright (C) 2000-2006, International Business Machines
*   Corporation and others.  All Rights Reserved.
***********************************************************************/

#include "locexp.h"
#include "unicode/ustdio.h"

/* Patterm string - "MMM yyyy" */
static const UChar sShortPat [] = { 0x004D, 0x004D, 0x004D, 0x0020, 
                                    0x0079, 0x0079, 0x0079, 0x0079, 0x0020, 'G', 0x0000 };
/* Pattern string - "MMMM yyyy" */
static const UChar sLongPat [] = { 0x004D, 0x004D, 0x004D, 0x004D, 0x0020, 
                                   0x0079, 0x0079, 0x0079, 0x0079, 0x0020, 'G', 0x0000 };

#define BUF_SIZE 1024

void printCalendar( LXContext *lx, UCalendar *cal )
{
    UErrorCode status = U_ZERO_ERROR;
    int dayCount = 0;
    int monthCount = 0;
    int fdow;
    
    int32_t patience = 999; /* break if more than 999 days in a month */

    /*int32_t  width, pad;*/
    int32_t i, day, day1;
    /*int32_t lens [10]; */
    int32_t firstday, current;
    UNumberFormat *nfmt;
    UDateFormat *dfmt;
    UDate d;
    UChar s [BUF_SIZE];
    const int useLongNames = 1;
    const UChar *pat = (useLongNames ? sLongPat : sShortPat);
    /*int32_t len = -1;*/
#define BUF_SIZ 500
    UChar buf[BUF_SIZ];
    dayCount = ucal_getLimit(cal, UCAL_DAY_OF_WEEK, UCAL_MAXIMUM, &status);
    monthCount = ucal_getLimit(cal, UCAL_MONTH, UCAL_MAXIMUM, &status);
    
    fdow = ucal_getAttribute(cal, UCAL_FIRST_DAY_OF_WEEK) - 1;
    d = ucal_getMillis(cal, &status);
    day1 = ucal_get(cal, UCAL_DATE, &status);

    #if 0
    u_fprintf(lx->OUT, "%d days, %d months - fdow=%d<P>\r\n", dayCount, monthCount, fdow);
    #endif

    /* Set up the day names */
    /* --get_days(days, useLongNames, fdow, status); --*/
    
    /* Print the calendar for the month */
    /* --        print_month(c, days, useLongNames, fdow, status); -- */
    
    
    /* Open a formatter with a month and year only pattern */
    dfmt = udat_open(UDAT_IGNORE,UDAT_IGNORE,lx->curLocaleName,NULL,0,pat, -1,&status);
    
    udat_format(dfmt, d, s, BUF_SIZE, 0, &status);
    
    u_fprintf(lx->OUT, "<table border=\"3\"><tr><td colspan=\"%d\"><h1>%S</h1></td></tr>\r\n",
        dayCount, s);
    
    u_fprintf(lx->OUT, "<tr>");
    for(i = 0; i < dayCount; i++) {
      UChar aBuf[500];
      int32_t len;
      len = udat_getSymbols(dfmt, UDAT_SHORT_WEEKDAYS, 1+ ((i+fdow)%dayCount), aBuf, 499,&status);
      if(len>0) {
        UChar bBuf[500];
        len = udat_getSymbols(dfmt, UDAT_WEEKDAYS, 1+ ((i+fdow)%dayCount), bBuf, 499,&status);
        if(len > 0) {
          u_fprintf(lx->OUT, "<td><a href=\"%s#\" title=\"%S\">%S</a></td>", cgi_url(lx),bBuf, aBuf, i,(i+fdow)%dayCount);
        } else {
          u_fprintf(lx->OUT, "<td>%S</td>", aBuf, i,(i+fdow)%dayCount);
        }
      } else {
        u_fprintf(lx->OUT, "<td><font color=\"red\">%d missing- %s</td>", ((i+fdow)%dayCount), u_errorName(status));
      }
    }
    u_fprintf(lx->OUT, "</tr>\r\n");

    if(U_FAILURE(status)) {
      u_fprintf(lx->OUT, "</table>\r\n");
      explainStatus(lx, status, "");
      return;
    }
    
    /* -- now the days -- */

    ucal_set(cal, UCAL_DATE,1);
    firstday = ucal_get(cal, UCAL_DAY_OF_WEEK,&status);

    firstday -= (fdow+1);
    
    nfmt = unum_open(UNUM_DECIMAL, NULL, 0, lx->curLocaleName, NULL, &status);

    if(U_FAILURE(status)) {
      u_fprintf(lx->OUT, "</table>\r\n");
      explainStatus(lx, status, "");
      return;
    }
    
    current = firstday;
    if(current < 0) {
      current += dayCount;
    }
    for(i=0;i<current;++i) {
      u_fprintf(lx->OUT, "<td bgcolor=\"#BBBBBB\"></td>");
    }
    day = ucal_get(cal, UCAL_DATE, &status);
    do {
      if(U_FAILURE(status) || !(--patience)) {
        u_fprintf(lx->OUT, "</tr></table>\r\n");
        explainStatus(lx, status, "");
        return;
      }

      unum_format(nfmt, day, buf, BUF_SIZE,0,&status);
      u_fprintf(lx->OUT, " <td %s>%S</td>",
                (day==day1)?"bgcolor=\"#00DDFF\"":"", /* current day */
                buf);
      
      ++current;
      current %= dayCount;
      if(current == 0) {
        u_fprintf(lx->OUT, "</tr>\r\n<tr>");
      }

      /* Go to the next day */
      ucal_add(cal, UCAL_DATE, 1, &status);
      day = ucal_get(cal, UCAL_DATE, &status);

    } while(day != 1);
    while(current++ < dayCount) {
      u_fprintf(lx->OUT, "<td bgcolor=\"#dddddd\"></td>");
    }
    u_fprintf(lx->OUT,"</tr>\r\n");
    
    /* -- done -- */
    

    if(U_FAILURE(status)) {
      u_fprintf(lx->OUT, "</table>\r\n");
      explainStatus(lx, status, "");
      return;
    }

    u_fprintf(lx->OUT, "</table>\r\n");
    /* set millis back? */
}

void fillFieldNames(const UChar** n)
{
  int32_t i;

  for(i=0;i<UCAL_FIELD_COUNT;i++) {
    n[i] = NULL; 
  }

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
    u_fprintf(lx->OUT, "<td bgcolor=\"#00cc99\" width=\"20%%\" height=9><img align=\"left\" width=\"15\" height=\"30\" src=\"" LDATA_PATH_LOC "tab_aleft.gif\">", lx->dispLocale);
    u_fprintf(lx->OUT, "<a href=\"%s?_=%s&amp;x=cal&amp;EXPLORE_Calendar=%c&NP_DBL=%s\">%S</a>", 
                cgi_url(lx),
              lx->curLocaleName,
              thisType, num, name);
    u_fprintf(lx->OUT, "<img align=\"right\" width=\"15\" height=\"30\" src=\"" LDATA_PATH_LOC "tab_aright.gif\" /></td>", lx->dispLocale);
    
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
  
    u_fprintf(lx->OUT, "<table cellspacing=\"0\" border=\"0\" cellpadding=\"0\"><tr>");
  
    printCalMenuSection(lx, num, type,
                        'f', FSWF("calexp_fieldsTab", "Fields"));
    u_fprintf(lx->OUT, "<td height=\"4\" width=\"3%%\">&nbsp</td>");
    printCalMenuSection(lx, num, type,
                        'c', FSWF("calexp_calTab", "Calendar"));
    u_fprintf(lx->OUT, "<td height=\"2\" colspan=\"3\" width=\"100%%\"><!-- D -->&nbsp;</td></tr>");
    u_fprintf(lx->OUT, "<tr><td colspan=\"10\" height=\"3\" background=\"" LDATA_PATH_LOC "tab_bot.gif\"></td><font size=4>&nbsp;</font></tr></table>\r\n", lx->dispLocale);
}


extern void showExploreCalendar( LXContext *lx)
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

    if((tmp = strstr(lx->queryString, "EXPLORE_Calendar")))
    {
        type = *(tmp + strlen("EXPLORE_Calendar="));
    }

    nf = unum_open(0, FSWF("EXPLORE_DateTimePatterns_dateAsNumber", "#"), -1, lx-> curLocaleName, NULL, &status);
    status = U_ZERO_ERROR; /* ? */

    if ((tmp = strstr(lx->queryString,"NP_DBL"))) /* Double: UDate format input ============= */
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

    {
      const char *f = num;
      u_fprintf(lx->OUT, "<a href=\"%s&amp;x=dat&amp;NP_DBL=%s\">Date Format Demo...", 
                getLXBaseURL(lx,kNO_URL|kNO_SECT), f);
        showExploreButtonPicture( lx );
      u_fprintf(lx->OUT, "</a><br />\r\n");
    }
    
/*    u_fprintf(lx->OUT, "[%D %T]<p>", now, now ); */

    if(U_FAILURE(status))
    {
        u_fprintf(lx->OUT, "%S -prep- ", FSWF("calexp_errGet", "Can't get initial date: "));
        explainStatus(lx, status, "");
        return;
    }

    cal = ucal_open(lx->timeZone, -1, lx->curLocaleName, UCAL_TRADITIONAL, &status);
    if(U_FAILURE(status))
    {
        u_fprintf(lx->OUT, "%S -calopen- ", FSWF("calexp_errGet", "Can't get initial date: "));
        explainStatus(lx, status, "");
        return;
    }

    ucal_setMillis(cal, now, &status);
    if(U_FAILURE(status))
    {
        u_fprintf(lx->OUT, "%S -set- ", FSWF("calexp_errGet", "Can't get initial date: "));
        explainStatus(lx, status, "");
        return;
    }



    status = U_ZERO_ERROR;

    /* printCalMenuBar( lx, num, type ); */
    u_fprintf(lx->OUT, "<table border=\"0\"><tr><td>\r\n");


        /**************** calendar fields **********************/
    {
#define  NR_ITEMS 4
        static const  UBool   roll[NR_ITEMS] =  {   TRUE, FALSE, FALSE, TRUE };
        static const  int32_t amt [NR_ITEMS] =  {    -1 ,  -1  ,  1   ,  1   };
        static const char    *nam [NR_ITEMS] =  { "&lt;", "-"  , "+"  , "&gt;"};

        u_fprintf(lx->OUT, "<table border=\"2\">\r\n");
        u_fprintf(lx->OUT, "<tr><td><b>val</b></td><td><b>chg</b></td><td><b>name</b></td><td><b>range</b></td></tr>\r\n");
        for (i=0;i<UCAL_FIELD_COUNT;i++)
        {
            int32_t  val;
            status = U_ZERO_ERROR;
            val=ucal_get(cal, (UCalendarDateFields)i, &status);

            u_fprintf(lx->OUT, "  <tr>\r\n");
      
            u_fprintf(lx->OUT, "    <td %s>%ld</td>\r\n", 
                      (ucal_isSet(cal, (UCalendarDateFields)i)?
                       (U_FAILURE(status)?"BGCOLOR=#FF0000":""):"BGCOLOR=#999999"),
                      val);
      
            /* roll/set table */
            u_fprintf(lx->OUT, "    <td>\r\n");
            status = U_ZERO_ERROR;
            for(j=0;j<NR_ITEMS;j++)
            {
                cal2 = ucal_open(lx->timeZone, -1, lx->curLocaleName, UCAL_TRADITIONAL, &status);
                ucal_setMillis(cal2, now, &status);
                if(U_SUCCESS(status))
                {
                  if(roll[j]) {
                    ucal_roll(cal2, (UCalendarDateFields)i, amt[j], &status);
                  } else {
                    ucal_set(cal2, (UCalendarDateFields)i,
                             ucal_get(cal2, (UCalendarDateFields)i, &status) + amt[j]);
                  }
                }
        
                /* Whew! */
                if(U_SUCCESS(status))
                {
                    char num[100];

                    sprintf(num, "%f", ucal_getMillis(cal2, &status));
                    u_fprintf(lx->OUT, "<a href=\"%s?_=%s&amp;x=cal&amp;EXPLORE_Calendar&NP_DBL=%s\">",
                                cgi_url(lx),
                              lx->curLocaleName,
                              num);
                    u_fprintf(lx->OUT, "%s", nam[j]);
                    u_fprintf(lx->OUT, "</a>");
                }
                else
                {
                    u_fprintf(lx->OUT, "<font color=\"#99D999\">");
                    u_fprintf(lx->OUT, "%s", nam[j]);
                    u_fprintf(lx->OUT, "</font>");
          
                }
        
                ucal_close(cal2);
            }
            u_fprintf(lx->OUT, "    </td>\r\n");
      
            if(fieldNames[i]) {
              u_fprintf(lx->OUT, "    <td>%S</td>\r\n", fieldNames[i]);
            } else {
              u_fprintf(lx->OUT, "    <td>#%d</td>\r\n", i);
            }
            
            {
              int32_t fMin = -1;
              int32_t fMax = -1;
              
              fMin = ucal_getLimit(cal, i, UCAL_MINIMUM, &status);
              fMax = ucal_getLimit(cal, i, UCAL_MAXIMUM, &status);

              u_fprintf(lx->OUT, "<td>%d..%d</td>\r\n", fMin, fMax);
            }
            
            u_fprintf(lx->OUT, "  </tr>\r\n");
        }
        if(U_FAILURE(status))
        {
            u_fprintf(lx->OUT, "  <tr><td colspan=\"2\">ERR: %s</td></tr>\r\n",
                      u_errorName(status));
        }
        
        u_fprintf(lx->OUT, "</table>\r\n");
        
        u_fprintf(lx->OUT, "<p><b>%S</b><br />&lt; %S<br />- %S<br />+ %S<br />&gt; %S<br /><p>\r\n",
                  FSWF("calexp_chg","Change buttons:"),
                  FSWF("calexp_chg_roll_decr", "Roll down"),
                  FSWF("calexp_chg_set_decr", "Decrement"),
                  FSWF("calexp_chg_set_incr", "Increment"),
                  FSWF("calexp_chg_roll_incr", "Roll up"));

    } /* end calendar table [f] */
    
    u_fprintf(lx->OUT, "</td><td valign=\"top\">\r\n");

    printCalendar( lx, cal );

    u_fprintf(lx->OUT, "</tr></table>\r\n");

}



