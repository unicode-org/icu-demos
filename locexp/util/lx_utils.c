/**********************************************************************
*   Copyright (C) 1999, International Business Machines
*   Corporation and others.  All Rights Reserved.
***********************************************************************/

/*****************************************
   Some random utilities..
*/

#include <stdlib.h>
#include "unicode/lx_utils.h"
#include "unicode/ucnv_cb.h"

/* Realloc broken on linux????? */
void *my_realloc(void *ptr, size_t old, size_t size)
{
  void *newptr;

  newptr = malloc(size);
  if(ptr)
    memcpy(newptr,ptr,old);
  return newptr;
}

/******************************************************** derived from ucnv_err.c */
#define ToOffset(a) ((a)<=9?(0x0030+(a)):(0x0030+(a)+7))

/*Takes a int32_t and fills in  a UChar* string with that number "radix"-based
 * and padded with "pad" zeroes
 */
static void 
  itou (UChar * buffer, int32_t i, int32_t radix, int32_t pad)
{
  int32_t length = 0;
  int32_t num = 0;
  int8_t digit;
  int32_t j;
  UChar temp;

  while (i >= radix)
    {
      num = i / radix;
      digit = (int8_t) (i - num * radix);
      buffer[length++] = (UChar) (ToOffset (digit));
      i = num;
    }

  buffer[length] = (UChar) (ToOffset (i));

  while (length < pad)
    buffer[++length] = (UChar) 0x0030;	/*zero padding */
  buffer[length--] = (UChar) 0x0000;

  /*Reverses the string */
  for (j = 0; j < (pad / 2); j++)
    {
      temp = buffer[length - j];
      buffer[length - j] = buffer[j];
      buffer[j] = temp;
    }

  return;
}

static UBool backslashInit = FALSE;

U_STRING_DECL(backslashBegin,     "\\u"   , 2);
U_STRING_DECL(backslashBeginHTML, "<B>\\u", 5);
U_STRING_DECL(backslashEnd,       ""      , 0);
U_STRING_DECL(backslashEndHTML,   "</B>"  , 4);

void 
UCNV_FROM_U_CALLBACK_BACKSLASH (const void *context,
                                UConverterFromUnicodeArgs *fromUArgs,
                                const UChar* codeUnits,
                                int32_t length,
                                UChar32 codePoint,
                                UConverterCallbackReason reason,
                                UErrorCode *err)
{

  UChar valueString[40];
  int32_t valueStringLength = 0;
  int32_t i = 0;

  const UChar *myValueSource = NULL;
  UErrorCode err2 = U_ZERO_ERROR;
  FromUBackslashContext *ctx;

  UConverterFromUCallback original = NULL;
  const void *originalContext;

  UConverterFromUCallback ignoredCallback = NULL;
  const void *ignoredContext;

  UConverterFromUCallback tCallback = NULL;
  const void *tContext;

  if((reason > UCNV_IRREGULAR) || (length <= 0))
  {
    return;
  }

  if(backslashInit == FALSE)
  {
    U_STRING_INIT(backslashBegin,     "\\u"   , 2);
    U_STRING_INIT(backslashBeginHTML, "<B>\\u", 5);
    U_STRING_INIT(backslashEnd,       ""      , 0);
    U_STRING_INIT(backslashEndHTML,   "</B>"  , 4);
    
    backslashInit = TRUE;
  }

  ctx = (FromUBackslashContext *) context;

  ucnv_getFromUCallBack(fromUArgs->converter, 
                        &tCallback,
                        &tContext);
  

  ucnv_setFromUCallBack (fromUArgs->converter,
                         (UConverterFromUCallback)ctx->subCallback,
                         ctx->subContext,
                         &original,
                         &originalContext,
                         &err2);

  ucnv_getFromUCallBack(fromUArgs->converter, 
                        &tCallback,
                        &tContext);

  if (U_FAILURE (err2))
  {
    *err = err2;
    return;
  }
  
  /*
   * ### TODO:
   * This should actually really work with the codePoint, not with the codeUnits;
   * how do we represent a code point > 0xffff? It should be one single escape, not
   * two for a surrogate pair!
   */
  while (i < length)
  {
    if(ctx->html)
    {
      u_strcpy(valueString+valueStringLength,backslashBeginHTML);
      valueStringLength += u_strlen(backslashBeginHTML);
    }
    else
    {
      u_strcpy(valueString+valueStringLength,backslashBegin);
      valueStringLength += u_strlen(backslashBegin);
    }

    itou (valueString + valueStringLength, codeUnits[i++], 16, 4);
    valueStringLength += 4;

    if(ctx->html)
    {
      u_strcpy(valueString+valueStringLength,backslashEndHTML);
      valueStringLength += u_strlen(backslashEndHTML);
    }
    else
    {
      u_strcpy(valueString+valueStringLength,backslashEnd);
      valueStringLength += u_strlen(backslashEnd);
    }
  }

  myValueSource = valueString;

  /* reset the error */
  *err = U_ZERO_ERROR;

  ucnv_cbFromUWriteUChars(fromUArgs, &myValueSource, myValueSource+valueStringLength, 0, err);



  ucnv_getFromUCallBack(fromUArgs->converter, 
                        &tCallback,
                        &tContext);

  ucnv_setFromUCallBack (fromUArgs->converter,
                         (UConverterFromUCallback)original,
                         originalContext,
                         &ignoredCallback,
                         &ignoredContext,
                         &err2);


  ucnv_getFromUCallBack(fromUArgs->converter, 
                        &tCallback,
                        &tContext);

  if (U_FAILURE (err2))
    {
      *err = err2;
      return;
    }

  return;
}
/*******************************************************end of borrowed code from ucnv_err.c **/


void doDecodeQueryField(const char *in, char *out, uint32_t length)
{
  unsigned int i;
  char *anchor;

  anchor = out;
  if(length == -1)
    length = strlen(in);

  for(i=0;i<length;i++)
    {
      if(in[i] == '+')
	{
	  *(out++) = ' ';
	  continue;
	}
      else if(in[i] == '%')
	{
	  if(in[i+1] == '%') /* & */
	    {
	      *(out++) = '%';
	      continue;
	    }
	  if((length-i) < 2)
	    {
	      break;
	    }
	  {
	    char tmp2[3];
	    unsigned int asciiValue;
	    tmp2[(unsigned int)0] = in[i+1];
	    tmp2[(unsigned int)1] = in[i+2];
	    tmp2[(unsigned int)2] = 0;
	    
	    
	    /* & */
	    sscanf(tmp2,"%x",&asciiValue);
	    
	    *(out++) = (unsigned char)asciiValue;
	    
	    i += 2;
	  }
	}
      else
	*(out++) = in[i];
    }
  /* & */
  *(out++) = 0;
}

/**
 * Unescape [changes \uXXXX -> U+XXXX, \\ -> U+005C (\)]
 * DOES NOT null terminate.
 * @param chars  target chars. must be big enough.
 * @param src    source chars. 
 * @param origLen # of chars to copy from the source.  If -1, src is assumed to be null terminated
 * @return number of chars copied into 'chars'. 
 */
 
int32_t copyWithUnescaping( UChar* chars, const UChar* src, int32_t origLen)
{
  /* if we wanted to we could do diacrits and junk here. Or whatever. */
  UChar *dst = chars;

  if(origLen == -1)
     origLen = u_strlen(src); /* but we don't null terminate still */

  while(origLen--)
    {
      if((src[0] == '\\') && (src[1] == 'u'))
	{
	  static const int pv[] = { 0x0000, 0x0001, 0x0010, 0x0100, 0x1000 }; /* place value */
	  UChar out = 0;
	  int32_t max;
	  
	  src += 2;
	  origLen -= 2;
	  for(max=4;*src && (max>0);max--)
	    {
	      char from2 = tolower(*src);
	      if((from2 >= '0') && (from2 <= '9'))
		{
		  int32_t digitval = 0x0 + from2 - '0';
		  out += digitval * pv[max];
		  /* printf("** multiplying %c@%d %x*%x\n",from2,max,digitval,pv[max]); */
		}
	      else if((from2 >= 'a') && (from2 <= 'f'))
		{
		  int32_t digitval = 0xa + from2 - 'a';		
		  out += digitval * pv[max];
		}
	      src++;
	      origLen--;
	    }
	  src--;
	  origLen++;

	  *(dst++) = out;
	}
      else
	{
	  *(dst++) = *src;
	}
	
      src++;
    }

  return dst-chars;
}

/*
   ===================================== MySortable stuff follows ==========
*/

/** 
 * Initialize a MySortable with the specified locale. Will reset all fields.
 * TODO: limit displayname to just a part (root -> English -> United States -> California, instead of   .... -> English (United States) -> ... 
 * @param s The MySortable struct to initialize
 * @param locid The Locale that the MySortable should refer to
 * @param inLocale The Locale which the MySortable's display name should be displayed 
 * @param parent   Parent MySortable
 */

void initSortable(MySortable *s, const char *locid, const char *inLocale, MySortable *parent)
{
  UErrorCode status = U_ZERO_ERROR;
  int32_t siz;
  s->str = strdup(locid);

  /*
   * OLD CODE:

     siz = uloc_getDisplayName(s->str,NULL, NULL, 0, &status);
     s->ustr = malloc((siz+1) * sizeof(UChar));
     ((UChar*)(s->ustr)) [0] = 0;
     status = U_ZERO_ERROR;
     uloc_getDisplayName( (s->str) , inLocale, (UChar*)(s->ustr), siz, &status);
  * Now, we want to print the 'most leaf' string.. 
  */
   
  status = U_ZERO_ERROR; /* check for variant */

  if( (siz = uloc_getDisplayVariant( s->str, inLocale, NULL, 0, &status)) &&
      (siz > 1) )
    {
      s->ustr = calloc((siz+2) , sizeof(UChar));
      ((UChar*)(s->ustr))[0] = 0;
      status = U_ZERO_ERROR;
      uloc_getDisplayVariant( s->str, inLocale, (UChar*)(s->ustr), siz, &status );
    }
  else
    {
      status = U_ZERO_ERROR;
      if( (siz = uloc_getDisplayCountry( s->str, inLocale, NULL, 0, &status))  &&
	  (siz > 1) )
	{
	  s->ustr = calloc((siz+2) , sizeof(UChar));
	  ((UChar*)(s->ustr))[0] = 0;
	  status = U_ZERO_ERROR;
	  uloc_getDisplayCountry( s->str, inLocale, (UChar*)(s->ustr), siz, &status );
	}
      else
	{
          if((siz == 0) && inLocale[0] == '_')
          {
            s->ustr = calloc(2,1);
            s->ustr[0] = '-';
            s->ustr[1] = 0;
          }
          else
          {
            status = U_ZERO_ERROR;
            if( ( siz = uloc_getDisplayLanguage( s->str, inLocale, NULL, 0, &status)) &&
                (siz > 1) )
	    {
	      s->ustr = calloc((siz+2) , sizeof(UChar));
	      ((UChar*)(s->ustr))[0] = 0;
	      status = U_ZERO_ERROR;
	      uloc_getDisplayLanguage( s->str, inLocale, (UChar*)(s->ustr), siz, &status );
	    }
	  else
	    s->ustr = 0;
          }
        }
    }
  s->subLocs = 0;
  s->nSubLocs = 0;
  s->parent = parent;
  s->subLocsSize = 0;
}


/**
 * Add a locale (realloc if necesary) directly to this level.
 * @param root Node to add to
 * @param thisLocale New locale name
 * @param inLocale Locale for string names
 * @param localeCount (return) global count to increment.
 */
 
MySortable *addSubLocaleTo(MySortable *root, const char *thisLocale, const char *inLocale, int32_t *localeCount)
{

  if( (root->nSubLocs) >= root->subLocsSize)
    {
      if(root->subLocsSize)
	root->subLocsSize *= 2;
      else
	root->subLocsSize = 10;
      
      root->subLocs = 
	my_realloc(root->subLocs,sizeof(MySortable)*(root->nSubLocs), sizeof(MySortable) * (root->subLocsSize));
    }
      
  initSortable(&(root->subLocs[root->nSubLocs]), thisLocale, inLocale, root);

  localeCount++;
  
  root->nSubLocs++;

  return &(root->subLocs[root->nSubLocs-1]);
}

void addLocaleRecursive(MySortable *root, const char *thisLoc, const char *level, const char *inLocale, int32_t *localeCount)
{
  char        curStub[128]; /* current stub (next section to add) */
  const char *curStubLimit; /* ptr to end of the current 'stub' (sublocale section) */
  int32_t     j;

  curStubLimit = strchr(level, '_');
  if(!curStubLimit)
    curStubLimit = thisLoc + strlen(thisLoc);

  strncpy(curStub, thisLoc, curStubLimit-thisLoc);
  curStub[curStubLimit-thisLoc] = 0;

  /* OK, find the stub.. */
  j = findLocaleNonRecursive(root, curStub);
  
  /* advance the root to point to the parent of the next item to search in */
  if(j == -1)
    {
      root = addSubLocaleTo(root, strdup(curStub), inLocale, localeCount);
      *localeCount++;
    }
  else
    {
      root = &(root->subLocs[j]);
    }

  if(*curStubLimit) /* We're not at the end yet. CSL points to _. */
    {
      addLocaleRecursive(root, thisLoc, ++curStubLimit, inLocale, localeCount);
    }
  else
    {
      /*
	if *curStubLimit == 0, that means that curStub==thisLoc
	
	therefore, we just added the original locale we were trying
	
	to add. 
      */
    }
}

/**
 * create a MySortable tree of locales. 
 * root -> en, ja, de, ... -> en_US, en_CA, .. -> en_US_CALIFORNIA
 * @param inLocale the Locale the tree should be created in
 * @param localeCount [return] total # of items in the tree
 * @return a new MySortable tree, owned by the caller.  Not sorted.
 */

MySortable *createLocaleTree(const char *inLocale, int32_t *localeCount)
{
  MySortable *root;

  int32_t     nlocs;  /* ICU-specified total locales */
  
  /* temps */
  int32_t     i;

  root = malloc(sizeof(MySortable));

  /* get things rolling... */
  initSortable(root, "root", inLocale, NULL);
  *localeCount = 1;


  /* load the list of locales from the ICU and process them */
  nlocs = uloc_countAvailable();
  
  for(i=0;i<nlocs;i++)
    {
      const char *thisLoc;

      thisLoc = uloc_getAvailable(i);


      addLocaleRecursive(root, thisLoc, thisLoc, inLocale, localeCount);

    }
  
  return root;
}

void destroyLocaleTree(MySortable *d)
{
  int i;

  if(d == NULL) return;

  for(i=0;i<d->nSubLocs;i++)
    {
      destroyLocaleTree(&d->subLocs[i]);
    }
  d->nSubLocs = 0;
  d->subLocsSize = 0;
  free(d->subLocs);
  d->subLocs = NULL;
  
  free(d->ustr);
  free(d->str);
  if(d->parent == NULL) /* root */
  {
    free(d);
  }
}


/* Helper function for mySort --------------------------------------------------------*/
int myCompare(const void *aa, const void *bb)
{
  int cmp;
  int minSize;
  const MySortable *a, *b;

  a = (const MySortable*) aa;
  b = (const MySortable*) bb;

  if(a->keySize == b->keySize)
    return memcmp(a->key,b->key,a->keySize);
  else
    {
      if(a->keySize > b->keySize)
	minSize = b->keySize;
      else
	minSize = a->keySize;

      cmp = memcmp(a->key,b->key, minSize);

      if(cmp != 0) /* if dissimilar, just return */
	return cmp;
      if(a->keySize == (uint32_t)minSize)
	return -1;
      else
	return 1;
    }

}

/**
 * NON recursive exact match search
 * @param toSort  the list of locales to search
 * @param locale  locale to search for
 * @return index into toSort.subLocs[], or -1 if not found
 */

int32_t findLocaleNonRecursive(MySortable *toSort, const char *locale)
{
  int32_t j;
  int32_t numTopLocs = toSort->nSubLocs;
  MySortable *s = toSort->subLocs;

  for(j=0;j<numTopLocs;j++)
    {
      if(!strcmp(s[j].str, locale))
	{
	  return j;
	}
    }
  
  return -1; /* not found */
}

MySortable *findLocale(MySortable *root, const char *locale)
{
  int32_t    n;
  MySortable *found;

  if(!root)
    return NULL;
  
  if(!strcmp(root->str,locale))
    {
      return root;
    }
  
  /* Right now very stupid DFS. Let's forget that we actually know that if we're searching for fr_BF that we should look under the fr node. */
  for(n=0;n<root->nSubLocs;n++)
    {
      if(!strcmp(root->subLocs[n].str,locale))
	{
	  return &(root->subLocs[n]);
	}
      
      if(found = findLocale(&(root->subLocs[n]),locale)) /* if found at a sublevel */
	return found;
    }
  
  return NULL; /* not found */
}

/* Sort an array of sortables --------------------------------------------------------*/
/* terminated with s->str being null */
void mySort(MySortable *s, UErrorCode *status, UBool recurse)
{
  UCollator *coll;
  MySortable *p;
  int32_t num;
  int32_t n = 0;

  num = s->nSubLocs;

  if(num <= 1)
    return; /* nothing to do */

  *status = U_ZERO_ERROR;
  coll = ucol_open(NULL, status);
  if(U_FAILURE(*status))
    return;

  ucol_setStrength(coll, UCOL_PRIMARY);
  /* First, fill in the keys */
  for(p=(s->subLocs);n < num;p++)
    {
      if(recurse)
	mySort(p,status,recurse); /* sub sort */
      
      p->keySize = ucol_getSortKey(coll, p->ustr, -1, p->key, SORTKEYSIZE);
      /*      if(U_FAILURE(*status))
	      return; */
      n++;
    }
  
  ucol_close(coll); /* don't need it now */

  qsort(s->subLocs, n, sizeof(MySortable), &myCompare);

}

/* convert from our internal names to a MIME friendly name .. ----------------------------  */
const char *MIMECharsetName(const char *n)
{
  UErrorCode err = U_ZERO_ERROR;
  const char *mime;

  if(!strcmp(n, "transliterated")) {
    /* transliteration gets you UTF-8 */
    return "utf-8"; 
  }

  mime = ucnv_getStandardName(n, "MIME", &err);
  if(U_FAILURE(err)||!mime) {
    return n;
  } else {
    return mime;
  }
}


/* Kind of crude. we COULD be more lenient and pass more chars through. But it's only for transport to the next form. So, we don't optimize for URL size but for code simplicity.  [Ultra paranoid method = attempt to convert each char into the target CP, and only escape chars that fail a). said conversion or b). if the conversion generates illegal chars for a URL. Just in case you wondered.

 <legal char              -> <legal char>
 <illegal char> -> \uXXXX -> %5cXXXX

 The code that displays this stuff only has to worry about being able to output to the target CP. We'll make the browser do the rest. 
*/

void ucharsToEscapedUrlQuery(char *urlQuery, const UChar *src)
{
  while(*src)
    {
  
      if(  ((*src) > 0x007F)
	|| !( isalnum(*src) || (*src==',') || (*src==':') || (*src == '.')))
	{
	  *(urlQuery++) = '%';
	  *(urlQuery++) = '5';
	  *(urlQuery++) = 'C';
	  *(urlQuery++) = 'u';
	  *(urlQuery++) = ToOffset( ((*src) & 0xF000) >> 12);
	  *(urlQuery++) = ToOffset( ((*src) & 0x0F00) >>  8);
	  *(urlQuery++) = ToOffset( ((*src) & 0x00F0) >>  4);
	  *(urlQuery++) = ToOffset( ((*src) & 0x000F) >>  0);
	}
      else
	*(urlQuery++) = (char)*src;

      src++;
    }
  *urlQuery = 0; 
}


/* date [from udate by Stephen F. Booth] ----------------------------------------------------- */
/* Format the date */




UChar *
date(const UChar *tz,
     UDateFormatStyle style,
     const char *locale,
     UErrorCode *status)
{
  return dateAt(ucal_getNow(), tz, style, locale, status);
}

UChar *dateAt(UDate adate, const UChar *tz, UDateFormatStyle style, const char *locale, UErrorCode *status)
{
  UChar *s = 0;
  int32_t len = 0;
  UDateFormat *fmt;

  fmt = udat_open(style, style, locale, tz, -1, NULL, 0, status);
  len = udat_format(fmt, ucal_getNow(), 0, len, 0, status);
  if(*status == U_BUFFER_OVERFLOW_ERROR) {
    *status = U_ZERO_ERROR;
    s = (UChar*) malloc(sizeof(UChar) * (len+1));
    if(s == 0) goto finish;
    udat_format(fmt, adate, s, len + 1, 0, status);
    if(U_FAILURE(*status)) goto finish;
  }

 finish:
  udat_close(fmt);
  return s;
}

UChar* u_uastrcpy_enc(UChar *ucs1,
          const char *s2, const char *enc )
{
    UErrorCode err = U_ZERO_ERROR;

  UConverter *cnv = ucnv_open(enc, &err);
  if(cnv != NULL) {
    ucnv_toUChars(cnv,
                    ucs1,
                    1000,
                    s2,
                    strlen(s2),
                    &err);
    if(U_FAILURE(err)) {
      *ucs1 = 0;
    }
  } else {
    *ucs1 = 0;
  }
  ucnv_close(cnv);
  return ucs1;
}

int32_t unescapeAndDecodeQueryField(UChar *dst, int32_t dstLen, const char *src)
{
  const char *fieldLimit;
  UChar temp[1024];
  char tmpc[1024];
  int32_t len;

  /* make fieldLimit point to the end of the field data */
  fieldLimit = strchr(src,'&');
  if(!fieldLimit)
    fieldLimit = src + strlen(src);

  /* sanity [safety from buffer overruns]  */
  if( (fieldLimit-src) > dstLen)
    fieldLimit = src + dstLen;

  /********************/

  /* First off, convert the field into an 8 bit string in the user's codepage */
  doDecodeQueryField(src,tmpc,fieldLimit-src);

  /* Now, convert it into Unicode, still escaped.. */
  u_uastrcpy(temp, tmpc);
  
  /* Now, de escape it.. */
  len = u_strlen(temp);
  len = copyWithUnescaping( dst, temp, len);
  dst[len] = 0; /* copy with unescaping DOES NOT terminate the str */
  
  return len;
}

int32_t unescapeAndDecodeQueryField_enc(UChar *dst, int32_t dstLen, const char *src, const char *enc)
{
  const char *fieldLimit;
  UChar temp[1024];
  char tmpc[1024];
  int32_t len;

  /* make fieldLimit point to the end of the field data */
  fieldLimit = strchr(src,'&');
  if(!fieldLimit)
    fieldLimit = src + strlen(src);

  /* sanity [safety from buffer overruns]  */
  if( (fieldLimit-src) > dstLen)
    fieldLimit = src + dstLen;

  /********************/

  /* First off, convert the field into an 8 bit string in the user's codepage */
  doDecodeQueryField(src,tmpc,fieldLimit-src);

  /* Now, convert it into Unicode, still escaped.. */
  if(enc == NULL)
      u_uastrcpy(temp, tmpc);
  else
      u_uastrcpy_enc(temp, tmpc, enc);
  
  /* Now, de escape it.. */
  len = u_strlen(temp);
  len = copyWithUnescaping( dst, temp, len);
  dst[len] = 0; /* copy with unescaping DOES NOT terminate the str */
  
  return len;
}

/**
 * Replace all instances of 'from' with 'to'.
 * @param str the string to modify. Null terminated
 * @param from the char to look for
 * @param to the char to change it to.
 */

void u_replaceChar(UChar *str, UChar from, UChar to)
{
  if(from == 0x0000) /* Don't allow the end condition to change. */
    return;

  do
    {
      if(*str == from)
	*str = to;
    } while( *(str++) );
}

/**
 * Duplicate a string from the host encoding to Unicode 
 * caller owns storage
 * @param hostchars chars in host encoding
 * @return ptr to new unichars, or NULL if allocation failed.
 */

U_CFUNC UChar *uastrdup(const char *hostchars)
{
  UChar *chr;

  if(!hostchars)
    return NULL;
  
  chr = malloc((strlen(hostchars)+1)*sizeof(UChar));

  if(!chr)
    return NULL;

  return u_uastrcpy(chr, hostchars);
}

UBool testConverter(const char *converter, 
                     const UChar *sample,
		     int32_t sampleLen, 
		     int8_t *buffer,
		     int32_t bufLen)
{
  UErrorCode  status = U_ZERO_ERROR;
  UConverter *cnv;
  UBool      worked = FALSE;  
  int8_t     *target;
  void       *oldContext;
  UConverterFromUCallback oldAction;

  cnv = ucnv_open(converter, &status);
  if(U_FAILURE(status))
    return FALSE;

  target = buffer;

  
  
  ucnv_setFromUCallBack(cnv, UCNV_FROM_U_CALLBACK_STOP,  NULL,
                        &oldAction, &oldContext, &status);

  ucnv_fromUnicode (cnv,
		    (char**)&target,
		    buffer + bufLen,
		    &sample,
		    sample + sampleLen,
		    NULL,
		    TRUE,
		    &status);
  
  if(U_FAILURE(status))
    {
      fprintf(stderr, "cnv [%s] failed on [%04X]\r\n", converter, sample[-1]);
    }
  
  ucnv_close(cnv);

  if(U_SUCCESS(status))
    return TRUE;
  else
    return FALSE;
}

UErrorCode  FSWF_bundleErr = U_ZERO_ERROR;

const char *gLocale = NULL;

UResourceBundle *gRB = NULL;

UResourceBundle *FSWF_getBundle()
{

  if(gRB == 0)
    {
      FSWF_bundleErr = U_ZERO_ERROR;
      gRB = ures_open(  FSWF_bundlePath(), gLocale, &FSWF_bundleErr);
      if(U_FAILURE(FSWF_bundleErr))
	{
	  gRB = 0;
	}
    }
  return gRB;

}

void FSWF_close()
{
  if(gRB != NULL)
    {
      ures_close(gRB);
      gRB = NULL;
    }
}

void FSWF_setLocale(const char *locale)
{
  FSWF_close();
  gLocale = locale;
  FSWF_getBundle();
}


/*** fetch string with fallback -------------------------------------------------*/
const UChar *FSWF(const char *key, const char *fallback)
{
  UErrorCode status = U_ZERO_ERROR;
  const UChar *str = 0;
  static UChar   gFSWFTempChars[1024];
  UChar *fswfTempChars = gFSWFTempChars;
  UResourceBundle *rb;
  int32_t len;

  if(strlen(fallback) >  1020)
    fswfTempChars = malloc(sizeof(UChar)*(strlen(fallback)+1));
      
  rb = FSWF_getBundle();

  status = U_ZERO_ERROR;

  if(rb != 0)
    str = ures_getStringByKey( rb, key, &len, &status);

  if(str == 0) /* failed to get a str */
    {
      /* fallback: use our temp buffer [NON MT safe] and do a strcpy.. */
      u_uastrcpy(gFSWFTempChars, fallback);
      str = malloc((u_strlen(gFSWFTempChars)+1) * sizeof(UChar)); /* LEAK but who cares, it's an error case */
      u_strcpy((UChar*)str,gFSWFTempChars);
    }

  return str;
}



static char FSWF_path[500] = "";

/* Set the path for FSWF. Default: icudata/FSWF/ */
U_CAPI void FSWF_setBundlePath(char *newPath)
{
  strcpy(FSWF_path, newPath);
}

const char *FSWF_bundlePath()
{
  if(FSWF_path[0] == 0)
    {
      strcpy(FSWF_path, u_getDataDirectory());
      strcat(FSWF_path, "FSWF/");
    }
  
  return FSWF_path;
}

U_CAPI UErrorCode FSWF_bundleError()
{
  return FSWF_bundleErr;
}





