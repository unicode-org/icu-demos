/**********************************************************************
*   Copyright (C) 2004-2005, International Business Machines
*   Corporation and others.  All Rights Reserved.
***********************************************************************/
#include "locexp.h"
#include <unicode/ustdio.h>
#include "unicode/putil.h"
#include "unicode/unum.h"

static void submitICIR(LXContext* lx);

static void showICIRSubMenu(LXContext *lx, const char *section, const UChar* name)
{
    UBool isSelected;
    
    if(!strcmp(lx->section,section)) {
        isSelected = TRUE;
        u_fprintf(lx->OUT,"<b>");
    } else {
        isSelected = FALSE;
        u_fprintf(lx->OUT,"");
    }
    u_fprintf(lx->OUT,"<a href=\"%s&amp;x=%s\">%S</a>", getLXBaseURL(lx,kNO_SECT), section, name);
   
    if(isSelected) {
        u_fprintf(lx->OUT,"</b>&nbsp;\n");
    } else {
        u_fprintf(lx->OUT,"&nbsp;\n");
    }
}

static void showICIRMenu(LXContext *lx, const char *superSection, const char *subSection, const char *section, const UChar* name)
{
    UBool isSelected;
    
    if(!strcmp(superSection,section)) {
        isSelected = TRUE;
        u_fprintf(lx->OUT,"<td bgcolor=\"#DDDDFF\"><b>");
    } else {
        isSelected = FALSE;
        u_fprintf(lx->OUT,"<td>");
    }
    u_fprintf(lx->OUT,"<a href=\"%s&amp;x=%s\">%S</a>", getLXBaseURL(lx,kNO_SECT), section, name);
   
    if(isSelected) {
        u_fprintf(lx->OUT,"</b>\n");
    } else {
        u_fprintf(lx->OUT,"\n");
    }
	u_fprintf(lx->OUT, "</td>\n");
}

static void showICIRFieldTwoggles(LXContext *lx, const char *part,const char *subpart, UErrorCode *status)
{
	u_fprintf(lx->OUT, "<select name=\"twg_%s_%s\" >", part, subpart);
	u_fprintf(lx->OUT, "<option value=\"\">%S ", FSWF("icir_none","(no action)"));
	u_fprintf(lx->OUT, "<option value=\"v\">%S ", FSWF("icir_v","Verified"));
	u_fprintf(lx->OUT, "<option value=\"c\">%S ", FSWF("icir_c","Fixed"));
	u_fprintf(lx->OUT, "<option value=\"w\">%S ", FSWF("icir_w","Incorrect"));
	u_fprintf(lx->OUT, "</select>");
}

/**
 * @param uChars chars in the user (display) locale
 * @param pChars chars in en/us/Posix
 * @param lChars chars in Localized form
 */
 
static void	showICIRFieldRow(LXContext *lx, 
					const char *part,
					const char *subpart,
					const UChar *name,
					const UChar *uChars,
					const UChar *pChars,
					const UChar *lChars,
					UErrorCode *status) {
	
    u_fprintf(lx->OUT, "<tr><td colspan=4 bgcolor=\"#666666\">&nbsp;</td></tr>\n"
			"<tr><th align=middle bgcolor=\"#DDFFDD\">%S</th>\n", name);
    u_fprintf(lx->OUT, "<td bgcolor=\"#FFFFDF\">%S</td>\n", pChars);
    u_fprintf(lx->OUT, "<td bgcolor=\"#DDDDFF\">%S</td>\n", uChars);
	u_fprintf(lx->OUT, "<td bgcolor=\"#FFFFFF\"><b><input onChange=\"handleMarkChanged('c_%s_%s')\" "
		"  name=\"c_%s_%s\" value=\"%S\" size=60 />"
		"</b></td>\n", part, subpart,part, subpart,lChars);
	u_fprintf(lx->OUT, "<td>");
	showICIRFieldTwoggles(lx,part,subpart,status);
	u_fprintf(lx->OUT, "</td></tr>\n");
}

static void showICIRloc_lng_1(LXContext *lx, const char *loc)
{
	UChar uChars[1024];
	UChar pChars[1024];
	UChar lChars[1024];
	UErrorCode status = U_ZERO_ERROR;

	/* LANGUAGE */
	uChars[0]=lChars[0]=0;
	uloc_getDisplayLanguage(loc,
			lx->dispLocale,
			uChars,
			sizeof(uChars)/sizeof(uChars[0]),
			&status);
	uloc_getDisplayLanguage(loc,
			lx->curLocaleName,
			lChars,
			sizeof(lChars)/sizeof(lChars[0]),
			&status);
	uloc_getDisplayLanguage(loc,
			"en_US_POSIX",
			pChars,
			sizeof(pChars)/sizeof(pChars[0]),
			&status);
	showICIRFieldRow(lx, 
					"language",
					loc,
					FSWF("LocaleCodes_Language", "Language"),
					uChars,
                    pChars,
					lChars,
					&status);
}

static void showICIRloc_rgn_1(LXContext *lx, const char *loc)
{
	UChar uChars[1024];
	UChar pChars[1024];
	UChar lChars[1024];
	UErrorCode status = U_ZERO_ERROR;

	/* REGION */
	uChars[0]=lChars[0]=0;
	uloc_getDisplayCountry(loc,
			lx->dispLocale,
			uChars,
			sizeof(uChars)/sizeof(uChars[0]),
			&status);
	uloc_getDisplayCountry(loc,
			lx->curLocaleName,
			lChars,
			sizeof(lChars)/sizeof(lChars[0]),
			&status);
	uloc_getDisplayCountry(loc,
			"en_US_POSIX",
			pChars,
			sizeof(pChars)/sizeof(pChars[0]),
			&status);
	showICIRFieldRow(lx, 
					"region",
					loc,
					FSWF("LocaleCodes_Region", "Region"),
					uChars,
					pChars,
					lChars,
					&status);
}

#define GXCOUNT 8  /* Gx (G7+) */
static const char   *Gx_rgn[] = { "_DE", "_GB", "_US", "_CA", "_FR", "_IT", "_JP", "_SE",
								NULL};
static const char   *Gx_lng[] = { "de", "en","fr", "it", "ja", "sv",
								NULL};
                                
#define ITEMS_PER_PAGE 25

static void showICIRloc_lng_menu(LXContext *lx)
{
    const char *o;
    UErrorCode status = U_ZERO_ERROR;
    int n;
    int i;
    o=queryField(lx,"o");
    if(o) { n = atoi(o); }
    mySort(lx->locales, &status, TRUE); /* need the whole thing sorted */
    /*showICIRloc_lng_1(lx, lx->curLocaleName);*/
    u_fprintf(lx->OUT, "<form method=post action=\"%s\">", getLXBaseURL(lx, kALL_PARTS));
    u_fprintf(lx->OUT, "<select name=o>");
    for(i=0;i<lx->locales->nSubLocs;i+=ITEMS_PER_PAGE) {
        u_fprintf(lx->OUT, "<option %s value=\"%d\">%S...%S", (i==n)?"SELECTED":"", i, lx->locales->subLocs[i]->ustr, 
            lx->locales->subLocs[((i+ITEMS_PER_PAGE-1)>lx->locales->nSubLocs)
                ?(lx->locales->nSubLocs-1):i+ITEMS_PER_PAGE-1]->ustr );
	}
    u_fprintf(lx->OUT, "</select>\n");
    u_fprintf(lx->OUT,"<input type=submit value=Go /></form>\n");
}

static void showICIRloc_lng(LXContext *lx)
{
    const char *s;
    int o=0;
    int i;
    int limit;
    if((s=queryField(lx,"o"))) {
        o = atoi(s);
        if((o>=lx->locales->nSubLocs) || (o<0)) {
            o = 0;
        }
    }
    if(o==0) {
        showICIRloc_lng_1(lx, lx->curLocaleName);
    }
    limit=o+ITEMS_PER_PAGE;
    if(limit>lx->locales->nSubLocs) {
        limit = lx->locales->nSubLocs;
    }
    for(i=o;i<limit;i++) {
        showICIRloc_lng_1(lx, lx->locales->subLocs[i]->str);
    }
}

static void showICIRloc_rgn(LXContext *lx)
{
	int i;
	for(i=0;Gx_rgn[i];i++) {
		showICIRloc_rgn_1(lx, Gx_rgn[i]);
	}
}

static void showICIRloc(LXContext *lx)
{
	UErrorCode resStatus = U_ZERO_ERROR;
    UResourceBundle *res;

    res = getCurrentBundle(lx, &resStatus);
    if(U_FAILURE(resStatus)) {
        explainStatus(lx,resStatus,NULL);
        u_fprintf(lx->OUT, "<tr><i>error loading bundle</i></tr>"); /* TODO: localize */
        return;
    }
}

static void showICIRnumExample(LXContext *lx, UNumberFormat *unf, UNumberFormat *urbnf, UNumberFormat *nf, double d)
{
    UErrorCode status = U_ZERO_ERROR;
    UChar u1[1024];
    UChar u2[1024];
    UChar u3[1024];
    const char *part="";
    char subpart[50];

    unum_formatDouble(urbnf,d,u1, 1024, 0, &status);
    unum_formatDouble(unf,d,u2, 1024, 0, &status);
    if(U_FAILURE(status)) {
        explainStatus(lx,status,NULL);
        u_fprintf(lx->OUT, "<i>error formatting</i>"); /* TODO: localize */
        return;
    }
    u_fprintf(lx->OUT, "<tr><td bgcolor=\"#666666\">&nbsp;</td></tr>\n<tr><th align=middle bgcolor=\"#DDFFDD\">");
    u_fprintf(lx->OUT, "%S<br />%S", u1, u2);
    u_fprintf(lx->OUT, "</th></tr><tr><td align=middle>");
    unum_formatDouble(nf,d,u3, 1024, 0, &status);
    if(U_FAILURE(status)) {
        explainStatus(lx,status,NULL);
        u_fprintf(lx->OUT, "<i>error formatting</i>"); /* TODO: localize */
    }
	sprintf(subpart,"%f",d);
    u_fprintf(lx->OUT, "<input onChange=\"handleMarkChanged('c_%s_%s')\" "
		"  name=\"c_%s_%s\" value=\"%S\" size=60 />"
		"\n", part, subpart,part, subpart,u3);
		
	showICIRFieldTwoggles(lx,part,subpart,&status);

    u_fprintf(lx->OUT, "</td></tr>");
}

static void showICIRnum(LXContext *lx)
{
    UNumberFormat *unf = NULL;
    UNumberFormat *urbnf = NULL;
    UNumberFormat *nf = NULL;
    UErrorCode status = U_ZERO_ERROR;

    urbnf = unum_open(UNUM_SPELLOUT,NULL, 0, lx->dispLocale, NULL, &status);
    unf = unum_open(UNUM_DEFAULT,NULL, 0, lx->dispLocale, NULL, &status);
    nf = unum_open(UNUM_DEFAULT,NULL, 0, lx->curLocaleName, NULL, &status);
    if(U_FAILURE(status)) {
        u_fprintf(lx->OUT, "<tr><th>");
        explainStatus(lx,status,NULL);
        u_fprintf(lx->OUT, "- error loading bundle</th></tr>\n"); /* TODO: localize */
        return;
    }

    showICIRnumExample(lx, unf, urbnf, nf,  1234.56789);
    showICIRnumExample(lx, unf, urbnf, nf,  -1234.56789);
    showICIRnumExample(lx, unf, urbnf, nf,  0.);
    showICIRnumExample(lx, unf, urbnf, nf,  -.001);
    showICIRnumExample(lx, unf, urbnf, nf,  100.);
    showICIRnumExample(lx, unf, urbnf, nf,  -100.);

    unum_close(nf);
    unum_close(urbnf);
    unum_close(unf);
}

static void showICIRdatExample(LXContext *lx, UDateFormat *udf, UDateFormat *df, UDate d)
{
    UErrorCode status = U_ZERO_ERROR;
    UChar u1[1024];
    UChar u2[1024];
	const char *part = "x";
	char subpart[1024];
        
    udat_format(udf,d,u1, 1024, 0, &status);
    udat_format(df,d,u2, 1024, 0, &status);
    if(U_FAILURE(status)) {
        explainStatus(lx,status,NULL);
        u_fprintf(lx->OUT, "<i>error formatting</i>"); /* TODO: localize */
        return;
    }
    u_fprintf(lx->OUT, "<tr><td bgcolor=\"#666666\">&nbsp;</td></tr>\n<tr><th align=middle bgcolor=\"#DDFFDD\">");
    u_fprintf(lx->OUT, "%S", u1);
    u_fprintf(lx->OUT, "</th></tr><tr><td align=middle>");

	sprintf(subpart, "%.0f", d);
    u_fprintf(lx->OUT, "<input onChange=\"handleMarkChanged('c_%s_%s')\" "
		"  name=\"c_%s_%s\" value=\"%S\" size=60 />"
		"\n", part, subpart,part, subpart,u2);
		
	showICIRFieldTwoggles(lx,part,subpart,&status);

    u_fprintf(lx->OUT, "</td></tr>");
}

static void showICIRtime(LXContext *lx, UDate d)
{
    UErrorCode status = U_ZERO_ERROR;
    UDateFormat *df, *udf;
    
    udf = udat_open(UDAT_FULL, UDAT_NONE, lx->dispLocale, NULL, -1, NULL, 0, &status);
    df = udat_open(UDAT_FULL, UDAT_NONE, lx->curLocaleName, NULL, -1, NULL, 0, &status);
    
     showICIRdatExample(lx,udf,df,d);
    
    udat_close(df);
    udat_close(udf);
}

static void showICIRdate(LXContext *lx, UDate d)
{
    UErrorCode status = U_ZERO_ERROR;
    UDateFormat *df, *udf;
    
    udf = udat_open(UDAT_NONE, UDAT_FULL, lx->dispLocale, NULL, -1, NULL, 0, &status);
    df = udat_open(UDAT_NONE, UDAT_FULL, lx->curLocaleName, NULL, -1, NULL, 0, &status);
    
     showICIRdatExample(lx,udf,df,d);
    
    udat_close(df);
    udat_close(udf);
    
}
static void showICIRdateTime(LXContext *lx, UDate d)
{
    showICIRdate(lx, d);
    showICIRtime(lx, d);
}

static void showICIRdat(LXContext *lx)
{
    showICIRdate(lx, 0.);
    showICIRdate(lx, 424483413.);
    showICIRdate(lx, -49999234.);
    showICIRtime(lx, 0.);
    showICIRtime(lx, 1002442.);
    showICIRtime(lx, 28614.);
    showICIRtime(lx, 3600.*1000.*18.55);
}

/* Show a resource that has a short (*Abbreviations) and long (*Names) version ---------------- */
/* modified showArray */
static void showICIRShortLongCalType( LXContext *lx, UResourceBundle *rb, 
										const char *keyStem, const char *type )
{
    UErrorCode status = U_ZERO_ERROR;
	char part[120];
	char subPart[120];
    /*UErrorCode shortStatus = U_ZERO_ERROR, longStatus = U_ZERO_ERROR;*/
    /*char       shortKey[100], longKey[100];*/
    /*UResourceBundle *item = NULL;*/
    /*int32_t len;*/
    /*const UChar *s  = 0;*/
    int i,j;
    int stuffCount;
    int maxCount = 0;
    struct {
      const char *style;
      const UChar *title;
      int32_t count;
      UResourceBundle *bund;
	  UResourceBundle *pbund;
	  UResourceBundle *dbund;
      UBool isDefault;
    } stuff[] = { {"narrow", NULL, -1, NULL, NULL,NULL,FALSE},
                  {"abbreviated", NULL, -1, NULL, NULL,NULL,FALSE},
                  {"wide", NULL, -1, NULL, NULL,NULL,FALSE} };
 
    sprintf(part,"%s_%s", type, keyStem);
    stuffCount = sizeof(stuff)/sizeof(stuff[0]);
    stuff[0].title = FSWF("DayNarrow", "Narrow Names");
    stuff[1].title = FSWF("DayAbbreviations", "Short Names");
    stuff[2].title = FSWF("DayNames", "Long Names");

/* FSWF("MonthAbbreviations", " - NOT USED - see DayAbbreviations ") */
/* FSWF("MonthNames", "  - NOT USED - see DayNames ") */

    for(i=0;i<stuffCount;i++) {
	 UBool foo;
      stuff[i].bund = loadCalRes3(lx, keyStem, type, stuff[i].style, &stuff[i].isDefault, &status);
      stuff[i].pbund = loadCalRes3x(lx, keyStem, "format", "abbreviated", &foo, lx->calPosixBundle, &status);
      stuff[i].dbund = loadCalRes3x(lx, keyStem, type, stuff[i].style, &foo, lx->calDisplayBundle, &status);
      stuff[i].count = ures_getSize(stuff[i].bund);
      if(stuff[i].count > maxCount) {
        maxCount = stuff[i].count;
      }
    }

    if(U_FAILURE(status)) {
      explainStatus(lx, status, keyStem);
    } else { 
	
      maxCount =0; /* recount max */
      for(i=0;i<stuffCount;i++) {
        u_fprintf(lx->OUT, "<h4>%S", stuff[i].title);
        if((strcmp(type,"format")||(i==0)) &&
           !strcmp(ures_getLocaleByType(stuff[i].bund,ULOC_ACTUAL_LOCALE,&status),"root") &&
           (!lx->curLocaleName[0]||strcmp(lx->curLocaleName,"root"))) {
          UChar tmp[2048]; /* FSWF is not threadsafe. Use a buffer */
          if(strcmp(type,"format")) {
            u_sprintf(tmp, "%S type",  FSWF("Calendar_type_format", "Formatting"));
          } else if(i==0) {
            /* narrow (0) inherits from abbreviated (1) */
            u_strcpy(tmp, stuff[1].title);
          }
          u_fprintf_u(lx->OUT, FSWF(/**/"inherited_from", "from: %S"), tmp);
          stuff[i].count=0;
        } if(stuff[i].isDefault) {
          u_fprintf(lx->OUT, " ");
          calPrintDefaultWarning(lx);
        }
		u_fprintf(lx->OUT, "</h4>\n");
        if(stuff[i].count > maxCount) {
          maxCount = stuff[i].count;
        }
		
		u_fprintf(lx->OUT, "<table border=1 width=\"100%%\">");

        for(j=0;j<stuff[i].count;j++) {
            const UChar *s;
            const UChar *u;
            const UChar *d;
            int32_t len;
            UErrorCode subStatus = U_ZERO_ERROR;
            s = ures_getStringByIndex(stuff[i].bund, j, &len, &subStatus);
            u = ures_getStringByIndex(stuff[i].pbund, j, &len, &subStatus);
            d = ures_getStringByIndex(stuff[i].dbund, j, &len, &subStatus);
            if(U_SUCCESS(subStatus) && len) {
				int spl;
			    sprintf(subPart,"%s_%d_",stuff[i].style,j);
				spl = strlen(subPart);
				u_UCharsToChars(u,subPart+spl,u_strlen(u));
				subPart[spl+u_strlen(u)]=0;
				showICIRFieldRow(lx, 
					part,
					subPart,
					u, /*n - FIXME - # */
					d, /* d */
					u, /* n */
					s,
					&subStatus);
            } else {
              u_fprintf(lx->OUT, "<tr>");
              explainStatus(lx, subStatus, NULL);
              u_fprintf(lx->OUT, "</tr>");
            }
          }
		  u_fprintf(lx->OUT, "</table>\n");
        }
	}

    if(U_SUCCESS(status)) {
      for(i=0;i<stuffCount;i++) {
        ures_close(stuff[i].bund);
      }
    }
}

static void showICIRShortLongCal( LXContext *lx, UResourceBundle *rb, const char *keyStem)
{
  char aKeyStem[400];
  char *q;
  strcpy(aKeyStem, keyStem);
  aKeyStem[0]=toupper(aKeyStem[0]);
  if((q = strstr(aKeyStem, "Names"))) {
    *q = 0;
  }
  /* dayNames -> Day,  monthNames -> Month 
     for legacy translations */
   u_fprintf(lx->OUT, "<h3>%S</h3>\n", FSWF(/**/aKeyStem, aKeyStem));
   
   u_fprintf(lx->OUT, "<h4>%S</h4>\n", FSWF("Calendar_type_format", "Formatting"));
   showICIRShortLongCalType(lx, rb, keyStem, "format");
   u_fprintf(lx->OUT, "<h4>%S</h4>\n", FSWF("Calendar_type_stand-alone", "Stand-alone"));
   showICIRShortLongCalType(lx, rb, keyStem, "stand-alone");
}

static void showICIRdat_day(LXContext *lx)
{
    UErrorCode resStatus = U_ZERO_ERROR;
    UResourceBundle *res;
    res = getCurrentBundle(lx, &resStatus);
    if(U_FAILURE(resStatus)) {
        explainStatus(lx,resStatus,NULL);
        u_fprintf(lx->OUT, "<tr><i>error loading bundle</i></tr>"); /* TODO: localize */
        return;
    }
	u_fprintf(lx->OUT, "<tr>");
	showICIRShortLongCal( lx, res, "dayNames");
	u_fprintf(lx->OUT, "</tr>\n");
}

static void showICIRdat_mon(LXContext *lx)
{
    UErrorCode resStatus = U_ZERO_ERROR;
    UResourceBundle *res;
    res = getCurrentBundle(lx, &resStatus);
    if(U_FAILURE(resStatus)) {
        explainStatus(lx,resStatus,NULL);
        u_fprintf(lx->OUT, "<tr><i>error loading bundle</i></tr>"); /* TODO: localize */
        return;
    }
	u_fprintf(lx->OUT, "<tr>");
	showICIRShortLongCal( lx, res, "monthNames");
	u_fprintf(lx->OUT, "</tr>\n");
}


void showICIR(LXContext* lx)
{
    UErrorCode resStatus = U_ZERO_ERROR;
    UResourceBundle *res;
    UChar locName[1024];
    UChar dispName[1024];
	char superSection[5];
	const char *subSection = "";
    UErrorCode stat = U_ZERO_ERROR;
    dispName[0] = 0;
    locName[0] = 0;
    uloc_getDisplayName(lx->curLocaleName, lx->dispLocale, locName, 1024, &stat);
    uloc_getDisplayName(lx->dispLocale, lx->dispLocale, dispName, 1024, &stat);

	
	strncpy(superSection, lx->section, 4);
	superSection[4]=0;
	if(strlen(lx->section)>4) {
		subSection = lx->section + 4;
	}
    
	
    u_fprintf(lx->OUT, "<hr /><table><tr>\n");
    u_fprintf(lx->OUT, "<th>%S</th>\n", FSWF("icir_menu", "Section: "));
    /* Note: could use other strings here, if it's translated better elsewhere in LX. */
    showICIRMenu(lx, superSection, subSection, "iloc", FSWF("iloc", "Display Names"));
    showICIRMenu(lx,  superSection, subSection, "inum", FSWF("inum", "Numbers"));
    showICIRMenu(lx,  superSection, subSection, "idat", FSWF("idat", "Date and Time"));
    showICIRMenu(lx,  superSection, subSection, "", FSWF(/*NOTRANSLATE*/ "icir_imain", "Return to Normal View"));
    u_fprintf(lx->OUT, "</tr></table><hr />\n");

	if(!strcmp(lx->section,"isubmit")) {
		submitICIR(lx);
		return;
	}
    res = getCurrentBundle(lx, &resStatus);
    if(U_FAILURE(resStatus)) {
        explainStatus(lx,resStatus,NULL);
        u_fprintf(lx->OUT, "<i>error loading bundle</i>"); /* TODO: localize */
        return;
    }
	
	loadDefaultCalendar(lx, res, lx->curLocaleName);
	if(lx->curLocaleBlob.calendar[0]) {
		strcpy(lx->defaultCalendar, lx->curLocaleBlob.calendar);
	}
	loadCalendarStuff(lx, res, lx->curLocaleName);
	{
		UResourceBundle *posixBundle = NULL;
		UResourceBundle *dispBundle = NULL;
		posixBundle = ures_open(NULL,"en_US_POSIX",&resStatus);
		lx->calPosixBundle = loadCalendarStuffFor(lx, posixBundle, "en_US_POSIX", "gregorian");
		dispBundle = ures_open(NULL,lx->dispLocale,&resStatus);
		lx->calDisplayBundle = loadCalendarStuffFor(lx, dispBundle, lx->dispLocale, "gregorian");
	}
    
    u_fprintf(lx->OUT, "<h1>%S", FSWF(/**/superSection, "Unknown Section"));
	u_fprintf(lx->OUT, "</h1>\n");
	if(!strcmp(superSection, "iloc")) {
		u_fprintf(lx->OUT, "<ul>");
		showICIRSubMenu(lx, "iloc", FSWF("icir_Misc", "Misc"));
		showICIRSubMenu(lx, "iloc_lng", FSWF(/**/"Languages", "Languages"));
		showICIRSubMenu(lx, "iloc_rgn", FSWF(/**/"Countries", "Regions"));
		u_fprintf(lx->OUT, "</ul>");
	} else 	if(!strcmp(superSection, "idat")) {
		u_fprintf(lx->OUT, "<ul>");
		showICIRSubMenu(lx, "idat", FSWF("icir_Basics", "Basics"));
		showICIRSubMenu(lx, "idat_day", FSWF(/**/"Day", "Day"));
		showICIRSubMenu(lx, "idat_mon", FSWF(/**/"Month", "Month"));
		u_fprintf(lx->OUT, "</ul>");
	}

    if(!strcmp(superSection+1,"loc")) {
		if(!strcmp(subSection,"_lng")) {
			showICIRloc_lng_menu(lx);
		}
    }
	
    u_fprintf(lx->OUT, "<i>");
    u_fprintf_u(lx->OUT, FSWF("icir_intro","Each section below has an example in %S (as determined by the 'Display Locale' setting at the bottom of the page), followed by the translation in %S.<!-- sorry, order of the %%S's is important. -->"), dispName, locName);
    u_fprintf(lx->OUT, "</i>\n");
    u_fprintf(lx->OUT, "<form method=\"post\" action=\"?\">");
	u_fprintf(lx->OUT, "<input type=\"hidden\" name=x value=\"isubmit\" />", lx->section);
	u_fprintf(lx->OUT, "<input type=\"hidden\" name=ox value=\"%s\" />", lx->section);
	u_fprintf(lx->OUT, "<input type=\"hidden\" name=_ value=\"%s\" />", lx->curLocaleBlob.base);
	u_fprintf(lx->OUT, "<input type=\"hidden\" name=calendar value=\"%s\" />", lx->curLocaleBlob.calendar);
	u_fprintf(lx->OUT, "<input type=\"hidden\" name=currency value=\"%s\" />", lx->curLocaleBlob.currency);
	u_fprintf(lx->OUT, "<input type=\"hidden\" name=collator value=\"%s\" />", lx->curLocaleBlob.collation);
	u_fprintf(lx->OUT, "<input type=\"hidden\" name=d_ value=\"%s\" />", lx->dispLocale);

    u_fprintf(lx->OUT, "<table border=\"0\">\n");
    if(!strcmp(superSection+1,"num")) {
        showICIRnum(lx);
    } else if(!strcmp(superSection+1,"dat")) {
		if(!strcmp(subSection,"_day")) {
			showICIRdat_day(lx);
		} else if(!strcmp(subSection,"_mon")) {
			showICIRdat_mon(lx);
		} else {
			showICIRdat(lx);
		}
    } else if(!strcmp(superSection+1,"etc")) {
        showICIRnum(lx);
    } else if(!strcmp(superSection+1,"loc")) {
		if(!strcmp(subSection,"_lng")) {
			showICIRloc_lng(lx);
		} else if(!strcmp(subSection,"_rgn")) {
			showICIRloc_rgn(lx);
		} else {
			showICIRloc(lx);
		}
    } else {
        u_fprintf(lx->OUT, "<B>Unknown section. Please click one of the items above.</b>"); /* internal error, someone was playing with the URL. */
    }
    u_fprintf(lx->OUT, "</table>\n");
    u_fprintf(lx->OUT, "<hr />");
#if 0
    u_fprintf(lx->OUT, "<b>%S:<input name=email value=\"\" />", FSWF("icir_email","Email Address: (required for submissions)"));
#endif
    u_fprintf(lx->OUT, "<input type=submit value=\"%S\" />\n", FSWF("icir_submit","Submit"));
	u_fprintf(lx->OUT, "</form>\n");
}

static void submitICIR(LXContext* lx)
{
	UUrlIter *u;
	UChar uch[1024];
	const char *field;
	const char *val="";
	char *fieldName;
	const char *fieldVal;
	u_fprintf(lx->OUT, "<textarea rows=10 cols=80 name=\"icir_data\">\n");
	u_fprintf(lx->OUT, "Locale: %s\n", lx->curLocaleName);
	u_fprintf(lx->OUT, "DispLocale: %s\n", lx->dispLocale);
	u_fprintf(lx->OUT, "ICU: %s\n", U_ICU_VERSION);
	u_fprintf(lx->OUT, "Section: %s\n", queryField(lx,"ox"));
	u_fprintf(lx->OUT, "calendar: %s\n", lx->defaultCalendar);
	u_fprintf(lx->OUT, "\n");
	u = uurl_open(lx);
	while(field = uurl_next(u)) {
		if(!strncmp(field,"twg_",4) && (val=uurl_value(u))) {
			fieldName = strdup(field+2);
			*fieldName = 'c';
			fieldVal = queryField(lx,fieldName);
			uch[0]=0;
			unescapeAndDecodeQueryField_enc(uch, 1000, fieldVal, lx->convRequested);
			switch(*val) {
				case 'v':
					u_fprintf(lx->OUT,"Mark as Verified: %s \"%S\"\n", field+4, uch);
					break;
				case 'w':
					u_fprintf(lx->OUT,"Incorrect: %s \"%S\"\n", field+4, uch);
					break;
				case 'c':
					u_fprintf(lx->OUT,"Fix: %s to ", field+4);
					u_fprintf(lx->OUT, "\"%S\"\n", uch);
					break;
			}
		}
	}
	uurl_close(u);
 	u_fprintf(lx->OUT, "\n\n\nNOTE: doesn't actually submit anywhere yet. --srl\n");
	u_fprintf(lx->OUT, "</textarea>");
}
