/**********************************************************************
*   Copyright (C) 2000, International Business Machines
*   Corporation and others.  All Rights Reserved.
***********************************************************************/

/* Define to taste. */
#define DOCPATH "/home/srl/public_html/xlitdocs/"
#define DOCURL "/~srl/xlitdocs"

#include "stdio.h"
#include "stddef.h"
#include "string.h"
#include "unicode/ustdio.h"
#include "unicode/ucnv.h"
#include "unicode/ustring.h"
#include "unicode/udat.h"
#include "unicode/uloc.h"
#include "unicode/ures.h"
#include "unicode/ucol.h"
#include "unicode/ucal.h"
#include "unicode/usort.h"
#include "ctype.h"
#include "unicode/lx_utils.h"
#include "unicode/ures_additions.h"
#include "unicode/decompcb.h"
#include "unicode/uchar.h"
#include "unicode/umsg.h"
#include "unicode/utrnslit.h"

// local includes
#include "dsource.h"
#include "filesource.h"
#include "xlitsource.h"
#include "urlsource.h"

// for directory traversal
#include <sys/types.h>
#include <dirent.h>

/* context structure */
typedef struct
{
  UFILE      *OUT;
  char cLocale[200];     // current program locale
  char outEncoding[200]; // output encoding
  char translit[100];    // transliterator name to use
  char url[500];         // url (or file name)

} CTX;

void showMenu(CTX *c, const char *qs);            // show the top level page
void showList(CTX *c);                            // list of transliterators
void showFiles(CTX *c, const char *qs);           // possible files to xlit
void doTopFrame(CTX *c, const char *qs);          // top level frameset
void doTranslit(CTX *c, const char *qs);          // transliterated subframe
int32_t setLocaleAndEncodingAndOpenUFILE(CTX *c); // set up the file
int main();                                       
bool_t parseParams(CTX *c, const char *qs);       // parse qs into CTX

// add a predefined transliteration pipeline, for convenience
void addCustomItem(USort *mySort, const char *str, int &count);


/*******************************************/

// borrowed from locexp
int32_t setLocaleAndEncodingAndOpenUFILE(CTX *c)
{
  char *pi;
  char *tmp;
  const char *locale = NULL;
  const char *encoding = NULL;
  UErrorCode status = U_ZERO_ERROR;
  char *acceptLanguage;
  char newLocale[100];
  UFILE *f;
  UChar x[2];
  bool_t DSL = false, *didSetLocale = &DSL;
  bool_t DSE = false, *didSetEncoding = &DSE;

  locale = (const char *)c->cLocale;
  encoding = c->outEncoding; 

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
          strcpy(c->cLocale, locale);
	  *didSetLocale = true;
	}

      if(tmp) /* have encoding */
	{
  
	  tmp++; /* skip '/' */
  
	  pi = tmp;

	  tmp = strchr(tmp, '/');
	  if(tmp)
	    *tmp = 0;
  
	  if(*pi) /* don't want 0 length encodings */
	    {
	      encoding = pi;
/*              fprintf(stderr, "DSE1+%s\n", pi); */
	      *didSetEncoding = true;  

	    }
	}
    }

  if(!(*didSetLocale) && (acceptLanguage=getenv("HTTP_ACCEPT_LANGUAGE")) && acceptLanguage[0] )
    {

      /* OK, if they haven't set a locale, maybe their web browser has. */
	if(!(tmp=strchr(acceptLanguage,','))) /* multiple item separator */
      if(!(tmp=strchr(acceptLanguage,'='))) /* strength separator */
	  tmp = acceptLanguage + strlen(acceptLanguage);

      strncpy(newLocale, acceptLanguage, my_min(100,tmp-acceptLanguage));
      newLocale[my_min(100,tmp-acceptLanguage)] = 0;
      /*      fprintf(stderr," NL=[%s], al=[%s]\r\n", newLocale, acceptLanguage);
	      fflush(stderr);*/

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

      /*      if(isSupportedLocale(newLocale, true)) */
      /* DO NOT pick an unsupported locale from the browser's settings! */
        strcpy(c->cLocale, newLocale);

      status = U_ZERO_ERROR;

      /* that might at least get something.. It's better than defaulting to en_US */
    }
  
  if(!(*didSetEncoding))
    {
      const char *accept;
      const char *agent;
            char *newEncoding;

      accept = getenv("HTTP_ACCEPT_CHARSET");

/*      fprintf(stderr, "AC=%s\n", accept);*/
      
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
      strcpy(c->outEncoding, encoding);
    }

  /* now, open the file */
  f = u_finit(stdout, locale, encoding);

  if(!f)
    {
      /* couldNotOpenEncoding = encoding; */
      f = u_finit(stdout, locale, "LATIN_1"); /* this fallback should only happen if the encoding itself is bad */
      if(!f)
      {
          fprintf(stderr, "Could not finit the file.\n");
          fflush(stderr);
          c->OUT = f; 
          return 1; /* :( */
      }
    }


  /* we know that ufile won't muck withthe locale.
     But we are curious what encoding it chose, and we will propagate it. */
  if(encoding == NULL)
    {
      encoding = u_fgetcodepage(f);
      strcpy(c->outEncoding, encoding);
    }

  /* --sigh-- FIX FSWF SO IT TAKES A LOCALE!! */
  uloc_setDefault(c->cLocale, &status );

  c->OUT = f; 
  return 0;
}

// add a predefined transliteration pipeline, for convenience
void addCustomItem(USort *mySort, const char *str, int &count)
{
  str = strdup(str); 
  usort_addLine(mySort, FSWF(/**/str,str), -1, 
                false, (void*)str);
  count++;
}

// list of transliterators
void showList(CTX *c)
{
  int32_t count, i;
  const char *str;
  UErrorCode status = U_ZERO_ERROR;
  USort *mySort;

  count = utrns_countAvailableIDs();

  mySort = usort_open(c->cLocale, UCOL_DEFAULT_STRENGTH, false, &status);

  for(i=0;i<count;i++)
    {
      str = utrns_getAvailableID(i);
      usort_addLine(mySort, FSWF(/**/str,str), -1, false, (void*)str);
    }
  
  addCustomItem(mySort, "Hangul-Jamo;Jamo-Latin", count);
  addCustomItem(mySort, "Kana-Latin;Kanji-English", count);
  addCustomItem(mySort, "Kana-Latin;Kanji-English;Kanji-OnRomaji", count);
  addCustomItem(mySort, "Kanji-English;Han-Pinyin", count);
  
  usort_sort(mySort);

  u_fprintf(c->OUT, "<H3>%d %U:</H3>\r\n<!--<SELECT NAME=\"xlit\">-->\r\n", count,
            FSWF("xlitavail", "Transliterators Available"));

  for(i=0;i<count;i++)
    {
      u_fprintf(c->OUT, "  <INPUT TYPE=SUBMIT NAME=xlit VALUE=\"%s\">\r\n", 
                (const char*)mySort->lines[i].userData,
                mySort->lines[i].chars);
    }
  
  u_fprintf(c->OUT, "<!--</SELECT>-->\r\n");

  usort_close(mySort);
}

int main()
{
  CTX myContext;
  CTX *c = &myContext;

  const char *qs;

  /* set the path for FSWF */
  {
    char newPath[500];    strcpy(newPath, u_getDataDirectory());
    strcat(newPath, "xlitomatic/");
    
    FSWF_setBundlePath(newPath);
  }

  if(setLocaleAndEncodingAndOpenUFILE(c))
    return 0;

  printf("Content-Type: text/html;charset=%s\r\n\r\n", c->outEncoding);
  fflush(stdout);

  u_fprintf(c->OUT, "<BASE HREF=\"http://%s%s/%s/%s/\">\r\n", getenv("SERVER_NAME"), getenv("SCRIPT_NAME"), c->cLocale, c->outEncoding);

  /***********************************/
  
  qs = getenv("QUERY_STRING");
  
  if(!qs)
    qs = "";

  switch(qs[0])
    {
    case 0:
      showMenu(c, qs);
      break;

    case 'F':
      doTopFrame(c, qs);
      break;

    case 'T':
      try
        {
          doTranslit(c, qs);
        }
      catch(const char *s)
        {
          u_fprintf(c->OUT, "Content-type: text/html\r\n\r\nERROR: %s\n", s);
        }
      break;
    }

  u_fclose(c->OUT);
  
  return 0;
}


// list of files to transliterate
void showFiles(CTX *c, const char *qs)
{
  DIR *d;
  struct dirent *ent;

  u_fprintf(c->OUT, "<BR><B>File:</B> <SELECT NAME=url>\r\n");
  
  d = opendir(DOCPATH);
  while(ent = readdir(d))
    {
      if((ent->d_name[0] != '.') && (!strchr(ent->d_name, '~')))
        {
          u_fprintf(c->OUT, "<OPTION VALUE=\"%s\">%s\r\n",
                    ent->d_name, ent->d_name);
        }
    }
  closedir(d);
  u_fprintf(c->OUT, "</SELECT>\r\n");

}

// Show the top level options menu.
void showMenu(CTX *c, const char *qs)
{

  u_fprintf(c->OUT, "<TITLE>%U</TITLE>\r\n", FSWF("welcome", "xlitomatic: welcome"));
  
  /*******************/
  //  u_fprintf(c->OUT, "<HR>\r\n");
  u_fprintf(c->OUT, "<FORM>\r\n");
  u_fprintf(c->OUT, "<INPUT TYPE=hidden NAME=F VALUE=>\r\n");
  showList(c);
  showFiles(c, qs);
  
  //  u_fprintf(c->OUT, "<INPUT TYPE=Submit VALUE=\"%U\">\r\n", 
  //            FSWF("ok", "OK"));
  u_fprintf(c->OUT, "</FORM>\r\n");

  u_fprintf(c->OUT, "<HR><FORM>\r\n");
  u_fprintf(c->OUT, "<INPUT TYPE=hidden NAME=F VALUE=>\r\n");
  u_fprintf(c->OUT, "%U: <INPUT Name=xlit VALUE=Null SIZE=90><INPUT VALUE=\"%U\" TYPE=SUBMIT>\r\n",
            FSWF("Custom", "custom"),
            FSWF("ok", "OK"));
  showFiles(c, qs);
  u_fprintf(c->OUT, "</FORM>\r\n");

#if 1
  u_fprintf(c->OUT, "<HR>");
  u_fprintf(c->OUT, "<FORM>\r\n");
  u_fprintf(c->OUT, "<INPUT TYPE=hidden NAME=F VALUE=>\r\n");

  u_fprintf(c->OUT, "%U: <INPUT Name=xlit VALUE=Null SIZE=90><INPUT VALUE=\"%U\" TYPE=SUBMIT>\r\n",
            FSWF("Custom", "custom"),
            FSWF("ok", "OK"));
  u_fprintf(c->OUT, "<B>URL:</B> <INPUT NAME=url SIZE=90><P>\r\n");
  u_fprintf(c->OUT, "</FORM>\r\n");
#endif

}

// the top frameset
void doTopFrame(CTX *c, const char *qs)
{
  const char *end;

  if(parseParams(c, qs))
    {
      showMenu(c,qs);
      return;
    }
  
  u_fprintf(c->OUT, "<FRAMESET COLS=\"*,*\">\r\n");
  if(strstr(c->url, "http://"))
    {
      u_fprintf(c->OUT, "<FRAME SRC=\"%s\">\r\n",
                c->url);
    }
  else
    {
      u_fprintf(c->OUT, "<FRAME SRC=\"%s/%s\">\r\n",
                DOCURL, c->url);
    }
  u_fprintf(c->OUT, "<FRAME SRC=\"?T=&%s\">\r\n",
            qs+3);
  //  u_fprintf(c->OUT, "<FRAME SRC=\"?T=&xlit=%s&url=%s&\">\r\n",
  //            c->translit, c->url);
  u_fprintf(c->OUT, "</FRAMESET>\r\n");
}

void doTranslit(CTX *c, const char *qs)
{
  RawSource *fs;                  // File to..
  UnicodeSource *convertsrc;      // Converter to..
  UnicodeSource *us;              // Transliterator

  if(parseParams(c, qs))
    {
      u_fprintf(c->OUT, "<B>%U</B>\r\n", 
                FSWF("error_parse", "Error parsing query parameters."));
      return;
    }

  const char *urlparse;
  if(!strstr(c->url, "http://"))
    {
      char path[500];
      strcpy(path, DOCPATH);
      strcat(path, "/");
      strcat(path, c->url);
      
      fs = new FileSource(path); /* need inEncoding */
      convertsrc = new ToUSource(*fs);
    }
  else
    {
      fs = new URLSource(c->url);
      //      u_fprintf(c->OUT, "Don't support URL's yet [%s]", c->url);
      ((URLSource*)fs)->getEncoding();
      return;
    }

  us = new TranslitUSource(*convertsrc, c->translit);
  int len;
  
  //  u_fprintf(c->OUT, "Got file [%s]<P>\r\n\r\n\r\n<P><P><P><P>", path);
  if(us->eof())
    {
      u_fprintf(c->OUT, "%U: %s. %U<BR><HR>\r\n",
                FSWF("err_couldntcreate", "Error: Couldn't create the transliterator `"),
                c->translit,
                FSWF("err_couldntcreate2", "'. Perhaps it does not exist."));
    }

  while(!(us->eof()))
    {
      const UChar *start, *end;
      if(len = us->read(start,end))
        {
          //          u_fprintf(c->OUT, "\r\n %d :", len);
          while(start<end)
            {
              u_fprintf(c->OUT, "%K", *start);
              //              break;
              start++;
            }
        }
      //      u_fprintf(c->OUT, "***\r\n");
    }
  //  u_fprintf(c->OUT, "&lt;EOF&gt;<P>\r\n");
  
  delete us;
  delete convertsrc;
  delete fs;
}


// parse from qs into the CTX
bool_t parseParams(CTX *c, const char *qs)
{
  const char *end;

  if( (!strstr(qs,"xlit=")||(!strstr(qs,"url="))) )
    {
      return true;
    }

  qs += 8;
  
  end = strchr(qs, '&');
  if(!end)
    {
      return true;
    }
  
  doDecodeQueryField(qs, c->translit, (end-qs));
  //  strncpy(c->translit, qs, (end-qs));
  c->translit[(end-qs)] = 0;
  qs = (end + 5);
  
  end = strchr(qs, '&');
  if(!end)
    {
      end = qs+strlen(qs);
    }

  doDecodeQueryField(qs, c->url, (end-qs));
  //  strncpy(c->url, qs, (end-qs));
  c->url[(end-qs)] = 0;
  
  return false;
}

