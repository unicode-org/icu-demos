/**********************************************************************
*   Copyright (C) 1999-2002, International Business Machines
*   Corporation and others.  All Rights Reserved.
***********************************************************************/

/* Routines having to do with choosing converters */

#include "locexp.h"

/* chooseconverter ----------------------------------------------------------------------------- */

/* Choose from among all converters */
void chooseConverter(LXContext *lx, const char *restored)
{
    int32_t  ncnvs, naliases;
    int32_t i;

    USort *mysort;

    UErrorCode status = U_ZERO_ERROR;

    ncnvs = ucnv_countAvailable();

    mysort = usort_open(NULL, UCOL_DEFAULT_STRENGTH, TRUE, &status);
  
    if(U_FAILURE(status))
    {
        u_fprintf(lx->OUT, "%U<HR>\r\n", FSWF("convERR", "AN error occured trying to sort the converters.\n"));
        explainStatus(lx, status, NULL);
        return;
    }

    for(i=0;i<ncnvs;i++)
    {
        const char *name;
        const char *alias;
        char        dispName[200];
        const char *number;
        UErrorCode  err = U_ZERO_ERROR;

        name = ucnv_getAvailableName(i);

        alias = ucnv_getStandardName(name, "MIME", &err);
        if( (alias == NULL) || U_FAILURE(err))
        {
            err = U_ZERO_ERROR;
            alias = ucnv_getStandardName(name, "MIME", &err);
            if( (alias == NULL) || U_FAILURE(err))
            {
                continue;

                err = U_ZERO_ERROR;
                alias = name;
            }
        }
      
        if((number = strstr(name, "ibm-")))
            number+= 4;      /* ibm-[949] */
        else
            number = name; /* '[fullnameofconverter]' */

#if 0
      
        for(j=0;U_SUCCESS(err);j++)
	{
            alias = ucnv_getAlias(name, j, &err);

            if(!alias)
                break;
#endif
            if(!strstr(alias, "ibm-") || !strstr(alias,name))
                sprintf(dispName, "%s [%s]", alias, number);
            else
                strcpy(dispName, alias);

            if(!strcmp(alias,"fonted"))
                usort_addLine(mysort, uastrdup(dispName), -1, FALSE, (void*)"fonted");
            else
                usort_addLine(mysort, uastrdup(dispName), -1, FALSE, (void*)MIMECharsetName(name));
#if 0
	}

#endif
    }
  

    naliases = (mysort->count);
    u_fprintf_u(lx->OUT, FSWF("convsAvail","%d converters available, %d aliases total."), ncnvs, naliases);


    usort_sort(mysort);
  
    chooseConverterFrom(lx, restored, mysort);

    usort_close(mysort);


}


/* Choose a converter which can properly convert the sample string. */
void chooseConverterMatching(LXContext *lx, const char *restored, UChar *sample)
{
    int32_t  ncnvs, naliases, nmatch = 0;
    int32_t i;
    int32_t j;
    int32_t sampleLen;

    USort *mysort;
  
    int8_t junkChars[1024];

    UErrorCode status = U_ZERO_ERROR;

    sampleLen = u_strlen(sample);

    /* A little bit of sanity. c'mon, FFFD is just the subchar.. */
    for(i=0;i<sampleLen;i++)
    {
        if( (sample[i] == 0xFFFD) ||  /* subchar */
            (sample[i] == 0x221E) ||  /* infinity */
            (sample[i] == 0x2030)     /* permille */
            ) 
            sample[i] = 0x0020;
    }
  
    ncnvs = ucnv_countAvailable();

    mysort = usort_open(NULL, UCOL_DEFAULT_STRENGTH, TRUE, &status);
  
    if(U_FAILURE(status))
    {
        u_fprintf(lx->OUT, "%U<HR>\r\n", FSWF("convERR", "AN error occured trying to sort the converters.\n"));
        return;
    }

    u_fprintf(lx->OUT, "%U<BR>\r\n",
              FSWF("converter_searching", "Searching for converters which match .."));


    for(i=0;i<ncnvs;i++)
    {
        const char *name;
        const char *alias;
        char        dispName[200];
        const char *number;
        UErrorCode  err = U_ZERO_ERROR;

        name = ucnv_getAvailableName(i);

        if(testConverter(name, sample, sampleLen, junkChars, 1023) == FALSE)
            continue; /* Too bad.. */

        nmatch++;
      
        if((number = strstr(name, "ibm-")))
            number+= 4;      /* ibm-[949] */
        else
            number = name; /* '[fullnameofconverter]' */

        for(j=0;U_SUCCESS(err);j++)
	{
            alias = ucnv_getAlias(name,(uint16_t) j, &err);

            if(!alias)
                break;
	  
            if(!strstr(alias, "ibm-") || !strstr(alias,name))
                sprintf(dispName, "%s [%s]", alias, number);
            else
                strcpy(dispName, alias);

            if(!strcmp(alias,"fonted"))
                usort_addLine(mysort, uastrdup(dispName), -1, FALSE, (void*)"fonted");
            else
                usort_addLine(mysort, uastrdup(dispName), -1, FALSE, (void*)name);
	}

    }

    naliases = (mysort->count);
    u_fprintf_u(lx->OUT, FSWF("convsMatch","%d converters match (out of %d), %d aliases total."), nmatch,ncnvs, naliases);
  
    usort_sort(mysort);
  
    chooseConverterFrom(lx, restored, mysort);

    usort_close(mysort);
}

/* Show a list of converters based on the USort passed in */
void chooseConverterFrom(LXContext *lx, const char *restored, USort *list)
{
    int32_t naliases, ncnvs;
    int32_t  i;
    int32_t  COLS = 4; /* number of columns */
    int32_t rows;
    const char *cnvMime, *defaultMime;

    defaultMime = MIMECharsetName(lx->chosenEncoding);
  
    ncnvs = ucnv_countAvailable();

    naliases = list->count;
  
    if(!restored)
	restored = "";

    u_fprintf(lx->OUT,"<A HREF=\"?%s\"><H2>%U%s%U</H2></A>\r\n",
              restored,
              FSWF("encodingOK0", "Click here if the encoding '"),
              lx->chosenEncoding,
              FSWF("encodingOK1", "' is acceptable, or choose one from below."));

    u_fprintf(lx->OUT,"<I>%U</I>\r\n", 
              FSWF("encoding_mime","Note (ICU 1.6 release): This list has been pared down to only show MIME aliases that browsers would understand. I'll add a 'show all' button later.<!--If you translate this, remember it'll go away soon -->"));
    rows = (naliases / COLS) + 1;

    u_fprintf(lx->OUT, "<P><TABLE cellpadding=3 cellspacing=2 >\r\n"
              "<TR>\r\n");

    for(i=0;i<(rows * COLS);i++)
    {
        int32_t theCell;
        UBool hit;
        const char *cnv = NULL; 
      

        u_fprintf(lx->OUT, "<!-- %d -->", i);

        theCell=(rows * (i%COLS)) + (i/COLS); 
        if(theCell >= naliases)
	{
            u_fprintf(lx->OUT,"<td><!-- Overflow %d --></td>", theCell);
            if(((i+1)%COLS) == 0)
                u_fprintf(lx->OUT,"</TR>\n<TR>");
            continue;
	}

        cnv = (const char*)list->lines[theCell].userData;

        if(cnv < (const char *)0x100)
            return;

        if(!cnv)
            continue;

        cnvMime = MIMECharsetName(cnv);

        hit = !strcmp(cnvMime, defaultMime);

        if(hit)
            u_fprintf(lx->OUT,"<TD BGCOLOR=\"#FFDDDD\">");
        else
            u_fprintf(lx->OUT,"<TD>");

        u_fprintf(lx->OUT, "<FONT SIZE=-1>");

        if(hit)  
            u_fprintf(lx->OUT, "<b>");
      
        u_fprintf(lx->OUT, "<A HREF=\"");


        u_fprintf(lx->OUT, "%s/%s/",
                  getenv("SCRIPT_NAME"),
                  lx->cLocale);
        u_fprintf(lx->OUT,"%s/", cnv);
        if(restored)
            u_fprintf(lx->OUT, "?%s", restored); 
      
        u_fprintf(lx->OUT,"\">");
        u_fprintf(lx->OUT, "%U", list->lines[theCell].chars);
        /*       theCnvale.getDisplayName(o.GetLocale(),tmp); */
        u_fprintf(lx->OUT,"</A>\n");
      
        if(hit)
            u_fprintf(lx->OUT, "</b>");
      
        u_fprintf(lx->OUT, "</FONT>");
        u_fprintf(lx->OUT, "</FONT>");
        u_fprintf(lx->OUT, "</TD>");
        if(((i+1)%COLS) == 0)
            u_fprintf(lx->OUT, "</TR>\n<TR>");
    }
    u_fprintf(lx->OUT,"</TABLE>\r\n");
 

    { /* Todo: localize */
        const char *ts = "??";
        UErrorCode status;
        char tmp[100];
        UConverter *u = u_fgetConverter(lx->OUT);
        const char *tmp2;
    

        status = U_ZERO_ERROR;

        u_fprintf(lx->OUT,"<HR>");
        u_fprintf(lx->OUT,"<H3>Information about <B><TT>%s</TT></B></H3>\r\n",
                  lx->chosenEncoding);
        u_fprintf(lx->OUT,"<UL>");
    
        u_fprintf(lx->OUT,"  <LI>ID = %d, platform=%s, name=%s\n",
                  ucnv_getCCSID(u,&status),
                  (ucnv_getPlatform(u,&status) == UCNV_IBM) ? "IBM" : "other",
                  ucnv_getName(u, &status) );
    
        /* TODO: iterate over std names */
        status = U_ZERO_ERROR;
        tmp2 = ucnv_getStandardName(lx->chosenEncoding, "MIME", &status);
        if(tmp2 && U_SUCCESS(status)) {
            u_fprintf(lx->OUT, "  <LI>MIME: %s\n", tmp2);
        }

        status = U_ZERO_ERROR;
        tmp2 = ucnv_getStandardName(lx->chosenEncoding, "IANA", &status);
        if(tmp2 && U_SUCCESS(status)) {
            u_fprintf(lx->OUT, "  <LI>IANA: %s\n", tmp2);
        }
        status = U_ZERO_ERROR;
	      
        u_fprintf(lx->OUT,"  <LI>min/max chars: %d to %d\n",
                  ucnv_getMinCharSize(u),
                  ucnv_getMaxCharSize(u));

        u_fprintf(lx->OUT,"  <LI>Type=");
        switch(ucnv_getType(u))
        {
        case UCNV_UNSUPPORTED_CONVERTER:  ts = "Unsupported"; break;
        case UCNV_SBCS: ts = "Single Byte Character Set"; break;
        case UCNV_DBCS: ts = "Double Byte Character Set"; break;
        case UCNV_MBCS: ts = "Multiple Byte Character Set (variable)"; break;
        case UCNV_LATIN_1: ts = "Latin-1"; break;
        case UCNV_UTF8: ts = "UTF-8 (8 bit unicode)"; break;
        case UCNV_UTF7: ts = "UTF-7 (7 bit unicode transformation format)"; break;
        case UCNV_UTF16_BigEndian: ts = "UTF-16 Big Endian"; break;
        case UCNV_UTF16_LittleEndian: ts = "UTF-16 Little Endian"; break;
        case UCNV_EBCDIC_STATEFUL: ts = "EBCDIC Stateful"; break;
        case UCNV_ISO_2022: ts = "iso-2022 meta-converter"; break;
        case UCNV_LMBCS_1: ts="UCNV_LMBCS_1"; break;
        case UCNV_LMBCS_2: ts="UCNV_LMBCS_2"; break; 
        case UCNV_LMBCS_3: ts="UCNV_LMBCS_3"; break;		
        case UCNV_LMBCS_4: ts="UCNV_LMBCS_4"; break;
        case UCNV_LMBCS_5: ts="UCNV_LMBCS_5"; break;
        case UCNV_LMBCS_6: ts="UCNV_LMBCS_6"; break;
        case UCNV_LMBCS_8: ts="UCNV_LMBCS_8"; break;
        case UCNV_LMBCS_11: ts="UCNV_LMBCS_11"; break;
        case UCNV_LMBCS_16: ts="UCNV_LMBCS_16"; break;
        case UCNV_LMBCS_17: ts="UCNV_LMBCS_17"; break;
        case UCNV_LMBCS_18: ts="UCNV_LMBCS_18"; break;
        case UCNV_LMBCS_19: ts="UCNV_LMBCS_19"; break;

        case UCNV_HZ: ts = "HZ Encoding"; break;
        case UCNV_SCSU: ts = "Standard Compression Scheme for Unicode"; break; /* ? */
        case UCNV_US_ASCII: ts = "7-bit ASCII"; break; /* ? */

        default: ts = tmp; sprintf(tmp, "Unknown type %d", ucnv_getType(u));
        }
        u_fprintf(lx->OUT, "%s\n", ts);

#if defined(LX_UBROWSE_PATH)
        u_fprintf(lx->OUT, "<A TARGET=unibrowse HREF=\"%U/%s/\">%U</A>\r\n", getenv("SERVER_NAME"), getenv("SCRIPT_NAME"), 
                  FSWF( /* NOEXTRACT */ "ubrowse_path", LX_UBROWSE_PATH),
                  defaultMime,
                  FSWF("ubrowse", "Browse Unicode in this codepage"));
#endif
    
        u_fprintf(lx->OUT, "<LI>Aliases:<OL>\r\n");
        {
            int i;
            UErrorCode status = U_ZERO_ERROR;
            const char *name;
            const char *alias;

            name = ucnv_getName(u, &status);

            for(i=0;U_SUCCESS(status);i++)
            {
                alias = ucnv_getAlias(name, (uint16_t)i, &status);

                if(!alias)
                    break;

                u_fprintf(lx->OUT, "  <LI>%s\r\n", alias);
            }
        }
        u_fprintf(lx->OUT, "</OL>\r\n");


        u_fprintf(lx->OUT, "</UL>\r\n");
    }
  
	      
}


/* ----------------- lx->setEncoding */
UFILE *setLocaleAndEncodingAndOpenUFILE(LXContext *lx, char *chosenEncoding, UBool *didSetLocale, UBool *didsetEncoding, const char **fileObject)
{
    char *pi;
    char *tmp;
    const char *locale = NULL;
    const char *encoding = NULL;
    UErrorCode status = U_ZERO_ERROR;
    char *acceptLanguage;
    char newLocale[100];
    UFILE *f;



    locale = (const char *)lx->cLocale;
    encoding = lx->chosenEncoding; 

    pi = getenv("PATH_INFO");
    if( (pi) && (*pi && '/') )
    {
        pi++; /* eat first slash */
        tmp = strchr(pi, '/');
      
        if(tmp)
            *tmp = 0; /* tie off at the slash */

        status = U_ZERO_ERROR;
        locale = pi;

        if ( *locale != 0) /* don't want 0-length locales */
	{
            strcpy(lx->cLocale, locale);
            *didSetLocale = TRUE;
	}

        if(tmp) /* have encoding */
	{
  
            tmp++; /* skip '/' */
  
            pi = tmp;
  
            if(*pi) /* don't want 0 length encodings */
	    {
                encoding = pi;

                tmp = strchr(tmp, '/');
                if(tmp)
                {
                    pi = tmp + 1;
                    *tmp = 0;
                }

                *didsetEncoding = TRUE; 
	    }
	}
    }

    /* Now, did we get a file object? */
    if(*didsetEncoding && pi && !strcmp(encoding, "_"))
    {
        strcpy(lx->chosenEncoding, "us-ascii");
        *didsetEncoding = FALSE;
        *fileObject = pi;
        return NULL;
    }

    if(*didsetEncoding && pi && !strcmp(encoding, "_icudata"))
    {
        *didsetEncoding = FALSE;
        *fileObject = pi;
        strcpy(lx->chosenEncoding, "icudata");
        return NULL;
    }

    if(!(*didSetLocale) && (acceptLanguage=getenv("HTTP_ACCEPT_LANGUAGE")) && acceptLanguage[0] )
    {

        /* OK, if they haven't set a locale, maybe their web browser has. */
	if(!(tmp=strchr(acceptLanguage,','))) /* multiple item separator */
            if(!(tmp=strchr(acceptLanguage,'='))) /* strength separator */
                tmp = acceptLanguage + strlen(acceptLanguage);

        strncpy(newLocale, acceptLanguage, my_min(100,tmp-acceptLanguage));
        newLocale[my_min(100,tmp-acceptLanguage)] = 0;

        /* Note we don't do the PROPER thing here, which is to sort the possible languages by weight. Oh well. */
      
        status = U_ZERO_ERROR;

        /* half hearted attempt at canonicalizing the locale string. */
        newLocale[0] = tolower(newLocale[0]);
        newLocale[1] = tolower(newLocale[1]);
        if(newLocale[2] == '-')
            newLocale[2] = '_';
        if(newLocale[5] == '-')
            newLocale[5] = '_';

        newLocale[3] = toupper(newLocale[3]);
        newLocale[4] = toupper(newLocale[4]);

        if(isSupportedLocale(newLocale, TRUE)) /* DO NOT pick an unsupported locale from the browser's settings! */
            strcpy(lx->cLocale, newLocale);

        status = U_ZERO_ERROR;

        /* that might at least get something.. It's better than defaulting to en_US */
    }
  
    if(!(*didsetEncoding))
    {
        const char *accept;
        const char *agent;

        accept = getenv("HTTP_ACCEPT_CHARSET");

        if(accept && strstr(accept, "utf-8"))
	{
            encoding = "utf-8"; /* use UTF8 if they have it ! */
	}
        else if( (agent = (const char *)getenv("HTTP_USER_AGENT")) &&
                 (strstr(agent, "MSIE 4") || strstr(agent, "MSIE 5")) &&
                 (strstr(agent, "Windows NT")))
	{
            encoding = "utf-8"; /* MSIE can handle utf8 but doesn't request it. */
	}
    }


    if(encoding)
    {
        strcpy(lx->chosenEncoding, encoding);
    }

    /* Map transliterated/fonted : */
    if(0==strcmp(encoding, "fonted"))
    {
        encoding = "usascii";
    }
    if(0==strcmp(encoding, "transliterated"))
    {
        encoding = "utf-8";
    }

    /* now, open the file */
    f = u_finit(lx->fOUT, locale, encoding);

    if(!f)
    {
        lx->couldNotOpenEncoding = encoding;
        f = u_finit(lx->fOUT, locale, "LATIN_1"); /* this fallback should only happen if the encoding itself is bad */
        if(!f)
        {
            fprintf(stderr, "Could not finit the file.\n");
            fflush(stderr);
            return f; /* :( */
        }    
    }

    /* we know that ufile won't muck withthe locale.
       But we are curious what encoding it chose, and we will propagate it. */
    if(encoding == NULL)
    {
        encoding = u_fgetcodepage(f);
        strcpy(lx->chosenEncoding, encoding);
    }

    FSWF_setLocale(lx->cLocale);
    return f;
}
