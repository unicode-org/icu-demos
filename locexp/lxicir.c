/**********************************************************************
*   Copyright (C) 2004, International Business Machines
*   Corporation and others.  All Rights Reserved.
***********************************************************************/
#include "locexp.h"
#include <unicode/ustdio.h>
#include "unicode/unum.h"

	static void submitICIR(LXContext* lx);


static void showICIRMenu(LXContext *lx, const char *section, const UChar* name)
{
    UBool isSelected;
    
    if(!strcmp(lx->section,section)) {
        isSelected = TRUE;
        u_fprintf(lx->OUT,"<td bgcolor=\"#DDDDFF\"><b>");
    } else {
        isSelected = FALSE;
        u_fprintf(lx->OUT,"<td>");
    }
    u_fprintf(lx->OUT,"<a href=\"%s&x=%s\">%S</a>", getLXBaseURL(lx,kNO_SECT), section, name);
   
    if(isSelected) {
        u_fprintf(lx->OUT,"</b></td>\n");
    } else {
        u_fprintf(lx->OUT,"</td>\n");
    }
}

static void showICIRFieldTwoggles(LXContext *lx, const char *part,const char *subpart, UErrorCode *status)
{
	u_fprintf(lx->OUT, "<select name=\"twg_%s_%s\" >", part, subpart);
	u_fprintf(lx->OUT, "<option value=\"\">%S ", FSWF("icir_none",""));
	u_fprintf(lx->OUT, "<option value=\"v\">%S ", FSWF("icir_v","Verify"));
	u_fprintf(lx->OUT, "<option value=\"c\">%S ", FSWF("icir_c","Correct"));
	u_fprintf(lx->OUT, "</select>");
}

static void	showICIRFieldRow(LXContext *lx, 
					const char *part,
					const char *subpart,
					const UChar *name,
					const UChar *uChars,
					const UChar *lChars,
					UErrorCode *status) {
	
    u_fprintf(lx->OUT, "<tr><td colspan=4 bgcolor=\"#666666\">&nbsp;</td></tr>\n"
			"<tr><th align=middle bgcolor=\"#DDFFDD\">%S</th>\n", name);
    u_fprintf(lx->OUT, "<td bgcolor=\"#DDDDFF\">%S</td>\n", uChars);
    u_fprintf(lx->OUT, "<td bgcolor=\"#FFFFFF\"><b><input onChange=\"handleMarkChanged('c_%s_%s')\" "
		"  name=\"c_%s_%s\" value=\"%S\" size=60>"
		"</b></td>\n", part, subpart,part, subpart,lChars);
		
	u_fprintf(lx->OUT, "<td>");
	showICIRFieldTwoggles(lx,part,subpart,status);
	u_fprintf(lx->OUT, "</td></tr>\n");
}


static void showICIRloc(LXContext *lx)
{
	UErrorCode resStatus = U_ZERO_ERROR;
	UChar uChars[1024];
	UChar lChars[1024];
    UResourceBundle *res;
	UErrorCode status = U_ZERO_ERROR;
    
    res = getCurrentBundle(lx, &resStatus);
    if(U_FAILURE(resStatus)) {
        explainStatus(lx,resStatus,NULL);
        u_fprintf(lx->OUT, "<tr><i>error loading bundle</i></tr>"); /* TODO: localize */
        return;
    }
	#if 0
	u_fprintf(lx->OUT, "<tr>");
	showLocaleCodes(lx,  res, lx->curLocaleName);
	u_fprintf(lx->OUT, "</tr>\n");
	#endif
	
	/* LANGUAGE */
	uChars[0]=lChars[0]=0;
	uloc_getDisplayLanguage(lx->curLocaleName,
			lx->dispLocale,
			uChars,
			sizeof(uChars)/sizeof(uChars[0]),
			&status);
	uloc_getDisplayLanguage(lx->curLocaleName,
			lx->curLocaleName,
			lChars,
			sizeof(lChars)/sizeof(lChars[0]),
			&status);
	showICIRFieldRow(lx, 
					"language",
					lx->curLocaleName,
					FSWF("LocaleCodes_Language", "Language"),
					uChars,
					lChars,
					&status);
	/* REGION */
	uChars[0]=lChars[0]=0;
	uloc_getDisplayCountry(lx->curLocaleName,
			lx->dispLocale,
			uChars,
			sizeof(uChars)/sizeof(uChars[0]),
			&status);
	uloc_getDisplayCountry(lx->curLocaleName,
			lx->curLocaleName,
			lChars,
			sizeof(lChars)/sizeof(lChars[0]),
			&status);
	showICIRFieldRow(lx, 
					"region",
					lx->curLocaleName,
					FSWF("LocaleCodes_Region", "Region"),
					uChars,
					lChars,
					&status);

	/* VARIANT */
	uChars[0]=lChars[0]=0;
	uloc_getDisplayVariant(lx->curLocaleName,
			lx->dispLocale,
			uChars,
			sizeof(uChars)/sizeof(uChars[0]),
			&uloc_getDisplayVariant);
	uloc_getDisplayVariant(lx->curLocaleName,
			lx->curLocaleName,
			lChars,
			sizeof(lChars)/sizeof(lChars[0]),
			&status);
	showICIRFieldRow(lx, 
					"region",
					lx->curLocaleName,
					FSWF("LocaleCodes_Variant", "Variant"),
					uChars,
					lChars,
					&status);
	
	#if 0
	              FSWF("LocaleCodes_Language", "Language"),
              FSWF("LocaleCodes_Country", "Region"),
              FSWF("LocaleCodes_Variant", "Variant"));
	#endif
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
    u_fprintf(lx->OUT, "%S<br>%S", u1, u2);
    u_fprintf(lx->OUT, "</th></tr><tr><td align=middle>");
    unum_formatDouble(nf,d,u3, 1024, 0, &status);
    if(U_FAILURE(status)) {
        explainStatus(lx,status,NULL);
        u_fprintf(lx->OUT, "<i>error formatting</i>"); /* TODO: localize */
    }
	sprintf(subpart,"%f",d);
    u_fprintf(lx->OUT, "<input onChange=\"handleMarkChanged('c_%s_%s')\" "
		"  name=\"c_%s_%s\" value=\"%S\" size=60>"
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

    showICIRnumExample(lx, unf, urbnf, nf,  123456.789);
    showICIRnumExample(lx, unf, urbnf, nf,  -123456.789);
    showICIRnumExample(lx, unf, urbnf, nf,  .0000001);
    showICIRnumExample(lx, unf, urbnf, nf,  -.0000001);
    showICIRnumExample(lx, unf, urbnf, nf,  10000000.);
    showICIRnumExample(lx, unf, urbnf, nf,  -10000000.);

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
		"  name=\"c_%s_%s\" value=\"%S\" size=60>"
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

void showICIR(LXContext* lx)
{
	
    UErrorCode resStatus = U_ZERO_ERROR;
    UResourceBundle *res;
	
    u_fprintf(lx->OUT, "<hr><table><tr>\n");
    u_fprintf(lx->OUT, "<th>%S</th>\n", FSWF("icir_menu", "Section: "));
    /* Note: could use other strings here, if it's translated better elsewhere in LX. */
    showICIRMenu(lx, "iloc", FSWF("iloc", "Locale Names"));
    showICIRMenu(lx, "inum", FSWF("inum", "Numbers"));
    showICIRMenu(lx, "idat", FSWF("idat", "Date and Time"));
    showICIRMenu(lx, "", FSWF(/*NOTRANSLATE*/ "icir_imain", "Return to Normal View"));
    u_fprintf(lx->OUT, "</tr></table><hr>\n");

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
    u_fprintf(lx->OUT, "<form method=POST action=\"?\">");
	u_fprintf(lx->OUT, "<input type=hidden name=x value=\"isubmit\">", lx->section);
	u_fprintf(lx->OUT, "<input type=hidden name=ox value=\"%s\">", lx->section);
	u_fprintf(lx->OUT, "<input type=hidden name=_ value=\"%s\">", lx->curLocaleBlob.base);
	u_fprintf(lx->OUT, "<input type=hidden name=calendar value=\"%s\">", lx->curLocaleBlob.calendar);
	u_fprintf(lx->OUT, "<input type=hidden name=currency value=\"%s\">", lx->curLocaleBlob.currency);
	u_fprintf(lx->OUT, "<input type=hidden name=collator value=\"%s\">", lx->curLocaleBlob.collation);
	u_fprintf(lx->OUT, "<input type=hidden name=d_ value=\"%s\">", lx->dispLocale);
    u_fprintf(lx->OUT, "<h1>%S</h1>\n", FSWF(/**/lx->section, "Unknown Section"));
    u_fprintf(lx->OUT, "<i>");
    u_fprintf_u(lx->OUT, FSWF("icir_intro","Each section below has an example in your language (as determined by the 'Display Locale' setting at the bottom of the page), followed by the translation in %S."),
        lx->curLocale->ustr);
    u_fprintf(lx->OUT, "</i>\n");
    u_fprintf(lx->OUT, "<table border=0>\n");
    if(!strcmp(lx->section+1,"num")) {
        showICIRnum(lx);
    } else if(!strcmp(lx->section+1,"dat")) {
        showICIRdat(lx);
    } else if(!strcmp(lx->section+1,"etc")) {
        showICIRnum(lx);
    } else if(!strcmp(lx->section+1,"loc")) {
        showICIRloc(lx);
    } else {
        u_fprintf(lx->OUT, "<B>Unknown section. Please click one of the items above.</b>"); /* internal error, someone was playing with the URL. */
    }
    u_fprintf(lx->OUT, "</table>\n");
    u_fprintf(lx->OUT, "<hr>");
#if 0
    u_fprintf(lx->OUT, "<b>%S:<input name=email value=\"\">", FSWF("icir_email","Email Address: (required for submissions)"));
#endif
    u_fprintf(lx->OUT, "<input type=submit value=\"%S\">\n", FSWF("icir_submit","Submit"));
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
				case 'c':
					u_fprintf(lx->OUT,"Change: %s to ", field+4);
					u_fprintf(lx->OUT, "\"%S\"\n", uch);
					break;
			}
		}
	}
	uurl_close(u);
 	u_fprintf(lx->OUT, "\n\n\nNOTE: doesn't actually submit anywhere yet. --srl\n");
	u_fprintf(lx->OUT, "</textarea>");
}
