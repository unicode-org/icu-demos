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




/** Blocks & Property names **/

const char *getUCharCategoryName(int8_t type)
{
  switch(type)
    {
    case U_UNASSIGNED: return "Unassigned"; break;
    case U_UPPERCASE_LETTER: return "Uppercase Letter"; break;
    case U_LOWERCASE_LETTER:  return "Lowercase Letter"; break;
    case U_TITLECASE_LETTER: return "Titlecase Letter"; break;
    case U_MODIFIER_LETTER: return "Modifier Letter"; break;
    case U_OTHER_LETTER: return "Other Letter"; break;
    case U_NON_SPACING_MARK: return "Non-Spacing Mark"; break;
    case U_ENCLOSING_MARK: return "Enclosing Mark"; break;
    case U_COMBINING_SPACING_MARK: return "Combining Spacing Mark"; break;
    case U_DECIMAL_DIGIT_NUMBER: return "Decimal Digit Number"; break;
    case U_LETTER_NUMBER: return "Letter Number"; break;
    case U_OTHER_NUMBER: return "Other Number"; break;
    case U_SPACE_SEPARATOR: return "Space Separator"; break;
    case U_LINE_SEPARATOR: return "Line Separator"; break;
    case U_PARAGRAPH_SEPARATOR: return "Paragraph Separator"; break;
    case U_CONTROL_CHAR: return "Control"; break;
    case U_FORMAT_CHAR: return "Format"; break;
    case U_PRIVATE_USE_CHAR: return "Private Use"; break;
    case U_SURROGATE: return "Surrogate"; break;
    case U_DASH_PUNCTUATION: return "Dash Punctuation"; break;
    case U_START_PUNCTUATION: return "Start Punctuation"; break;
    case U_END_PUNCTUATION: return "End Punctuation"; break;
    case U_CONNECTOR_PUNCTUATION: return "Connector Punctuation"; break;
    case U_OTHER_PUNCTUATION: return "Other Punctuation"; break;
    case U_MATH_SYMBOL: return "Math Symbol"; break;
    case U_CURRENCY_SYMBOL: return "Currency Symbol"; break;
    case U_MODIFIER_SYMBOL: return "Modifier Symbol"; break;
    case U_OTHER_SYMBOL: return "Other Symbol"; break;
    case U_INITIAL_PUNCTUATION: return "Initial Punctuation"; break;
    case U_FINAL_PUNCTUATION: return "Final Punctuation"; break;
#define EXPECTED_U_CHAR_CATEGORY_COUNT (U_FINAL_PUNCTUATION+1)

      /* case U_GENERAL_OTHER_TYPES: return "General Other Types"; break; */

    default: return "Unknown type %d"; break;
    }
}


const char *getUBlockCodeName(UBlockCode block)
{
  switch(block)
    {
case UBLOCK_BASIC_LATIN: return "Basic Latin"; break;
case UBLOCK_LATIN_1_SUPPLEMENT: return "Latin-1 Supplement"; break;
case UBLOCK_LATIN_EXTENDED_A: return "Latin Extended A"; break;
case UBLOCK_LATIN_EXTENDED_B: return "Latin Extended B"; break;
case UBLOCK_IPA_EXTENSIONS: return "IPA Extensions"; break;
case UBLOCK_SPACING_MODIFIER_LETTERS: return "Spacing Modifier Letters"; break;
case UBLOCK_COMBINING_DIACRITICAL_MARKS: return "Combining Diacritical Marks"; break;
case UBLOCK_GREEK: return "Greek"; break;
case UBLOCK_CYRILLIC: return "Cyrillic"; break;
case UBLOCK_ARMENIAN: return "Armenian"; break;
case UBLOCK_HEBREW: return "Hebrew"; break;
case UBLOCK_ARABIC: return "Arabic"; break;
case UBLOCK_SYRIAC: return "Syriac"; break;
case UBLOCK_THAANA: return "Thaana"; break;
case UBLOCK_DEVANAGARI: return "Devanagari"; break;
case UBLOCK_BENGALI: return "Bengali"; break;
case UBLOCK_GURMUKHI: return "Gurmukhi"; break;
case UBLOCK_GUJARATI: return "Gujarati"; break;
case UBLOCK_ORIYA: return "Oriya"; break;
case UBLOCK_TAMIL: return "Tamil"; break;
case UBLOCK_TELUGU: return "Telugu"; break;
case UBLOCK_KANNADA: return "Kannada"; break;
case UBLOCK_MALAYALAM: return "Malayalam"; break;
case UBLOCK_SINHALA: return "Sinhala"; break;
case UBLOCK_THAI: return "Thai"; break;
case UBLOCK_LAO: return "Lao"; break;
case UBLOCK_TIBETAN: return "Tibetan"; break;
case UBLOCK_MYANMAR: return "Myanmar"; break;
case UBLOCK_GEORGIAN: return "Georgian"; break;
case UBLOCK_HANGUL_JAMO: return "Hangul-Jamo"; break;
case UBLOCK_ETHIOPIC: return "Ethiopic"; break;
case UBLOCK_CHEROKEE: return "Cherokee"; break;
case UBLOCK_UNIFIED_CANADIAN_ABORIGINAL_SYLLABICS: return "Unified Canadian Aboriginal Syllabics"; break;
case UBLOCK_OGHAM: return "Ogham"; break;
case UBLOCK_RUNIC: return "Runic"; break;
case UBLOCK_KHMER: return "Khmer"; break;
case UBLOCK_MONGOLIAN: return "Mongolian"; break;
case UBLOCK_LATIN_EXTENDED_ADDITIONAL: return "Latin Extended Additional"; break;
case UBLOCK_GREEK_EXTENDED: return "Greek Extended"; break;
case UBLOCK_GENERAL_PUNCTUATION: return "General Punctuation"; break;
case UBLOCK_SUPERSCRIPTS_AND_SUBSCRIPTS: return "Superscripts and Subscripts"; break;
case UBLOCK_CURRENCY_SYMBOLS: return "Currency Symbols"; break;
case UBLOCK_COMBINING_MARKS_FOR_SYMBOLS: return "Combining Marks for Symbols"; break;
case UBLOCK_LETTERLIKE_SYMBOLS: return "Letterlike Symbols"; break;
case UBLOCK_NUMBER_FORMS: return "Number Forms"; break;
case UBLOCK_ARROWS: return "Arrows"; break;
case UBLOCK_MATHEMATICAL_OPERATORS: return "Mathematical Operators"; break;
case UBLOCK_MISCELLANEOUS_TECHNICAL: return "Miscellaneous Technical"; break;
case UBLOCK_CONTROL_PICTURES: return "Control Pictures"; break;
case UBLOCK_OPTICAL_CHARACTER_RECOGNITION: return "Optical Character Recognition"; break;
case UBLOCK_ENCLOSED_ALPHANUMERICS: return "Enclosed Alphanumerics"; break;
case UBLOCK_BOX_DRAWING: return "Box Drawing"; break;
case UBLOCK_BLOCK_ELEMENTS: return "Block Elements"; break;
case UBLOCK_GEOMETRIC_SHAPES: return "Geometric Shapes"; break;
case UBLOCK_MISCELLANEOUS_SYMBOLS: return "Miscellaneous Symbols"; break;
case UBLOCK_DINGBATS: return "Dingbats"; break;
case UBLOCK_BRAILLE_PATTERNS: return "Braille Patterns"; break;
case UBLOCK_CJK_RADICALS_SUPPLEMENT: return "CJK Radicals Supplement"; break;
case UBLOCK_KANGXI_RADICALS: return "KangXi Radicals"; break;
case UBLOCK_IDEOGRAPHIC_DESCRIPTION_CHARACTERS: return "Ideographic Description Characters"; break;
case UBLOCK_CJK_SYMBOLS_AND_PUNCTUATION: return "CJK Symbols and Punctuation"; break;
case UBLOCK_HIRAGANA: return "Hiragana"; break;
case UBLOCK_KATAKANA: return "Katakana"; break;
case UBLOCK_BOPOMOFO: return "Bopomofo"; break;
case UBLOCK_HANGUL_COMPATIBILITY_JAMO: return "Hangul Compatibility Jamo"; break;
case UBLOCK_KANBUN: return "Kanbun"; break;
case UBLOCK_BOPOMOFO_EXTENDED: return "Bopomofo Extended"; break;
case UBLOCK_ENCLOSED_CJK_LETTERS_AND_MONTHS: return "Enclosed CJK Letters and Months"; break;
case UBLOCK_CJK_COMPATIBILITY: return "CJK Compatibility"; break;
case UBLOCK_CJK_UNIFIED_IDEOGRAPHS_EXTENSION_A: return "CJK Unified Ideographs Extension A"; break;
case UBLOCK_CJK_UNIFIED_IDEOGRAPHS: return "CJK Unified Ideographs"; break;
case UBLOCK_YI_SYLLABLES: return "Yi Syllables"; break;
case UBLOCK_YI_RADICALS: return "Yi Radicals"; break;
case UBLOCK_HANGUL_SYLLABLES: return "Hangul Syllables"; break;
case UBLOCK_HIGH_SURROGATES: return "High Surrogates"; break;
case UBLOCK_HIGH_PRIVATE_USE_SURROGATES: return "High Private-Use Surrogates"; break;
case UBLOCK_LOW_SURROGATES: return "Low Surrogates"; break;
case UBLOCK_PRIVATE_USE_AREA /* PRIVATE_USE */: return "Private Use Area"; break;
case UBLOCK_CJK_COMPATIBILITY_IDEOGRAPHS: return "CJK Compatibility Ideographs"; break;
case UBLOCK_ALPHABETIC_PRESENTATION_FORMS: return "Alphabetic Presentation Forms"; break;
case UBLOCK_ARABIC_PRESENTATION_FORMS_A: return "Arabic Presentation Forms A"; break;
case UBLOCK_COMBINING_HALF_MARKS: return "Combining Half Marks"; break;
case UBLOCK_CJK_COMPATIBILITY_FORMS: return "CJK Compatibility Forms"; break;
case UBLOCK_SMALL_FORM_VARIANTS: return "Small Form Variants"; break;
case UBLOCK_ARABIC_PRESENTATION_FORMS_B: return "Arabic Presentation Forms B"; break;
case UBLOCK_SPECIALS: return "Specials"; break;
case UBLOCK_HALFWIDTH_AND_FULLWIDTH_FORMS: return "Halfwidth and Fullwidth Forms"; break;
    case UBLOCK_OLD_ITALIC: return "Old Italic"; break;
    case UBLOCK_GOTHIC: return "Gothic"; break;
    case UBLOCK_DESERET: return "Deseret"; break;
    case UBLOCK_BYZANTINE_MUSICAL_SYMBOLS: return "Byzantine Musical Symbols"; break;
    case UBLOCK_MUSICAL_SYMBOLS: return "Musical Symbols"; break;
    case UBLOCK_MATHEMATICAL_ALPHANUMERIC_SYMBOLS: return "Mathematical Alphanumeric Symbols"; break;
    case UBLOCK_CJK_UNIFIED_IDEOGRAPHS_EXTENSION_B: return "CJK Unified Ideographs, Extension B"; break;
    case UBLOCK_CJK_COMPATIBILITY_IDEOGRAPHS_SUPPLEMENT: return "CJK Compatibility Ideographs, Supplement"; break;
    case UBLOCK_TAGS: return "Tags"; break;
      /* ICU 2.2: (Unicode 3.2) */
    case UBLOCK_CYRILLIC_SUPPLEMENTARY: return "Cyrillic Supplementary"; break;
    case UBLOCK_TAGALOG: return "Tagalog"; break;
    case UBLOCK_HANUNOO: return "Hanunoo"; break;
    case UBLOCK_BUHID: return "Buhid"; break;
    case UBLOCK_TAGBANWA: return "Tagbanwa"; break;
    case UBLOCK_MISCELLANEOUS_MATHEMATICAL_SYMBOLS_A: return "Miscellaneous Mathematical Symbols A"; break;
    case UBLOCK_SUPPLEMENTAL_ARROWS_A: return "Supplemental Arrows A"; break;
    case UBLOCK_SUPPLEMENTAL_ARROWS_B: return "Supplemental Arrows B"; break;
    case UBLOCK_MISCELLANEOUS_MATHEMATICAL_SYMBOLS_B: return "Miscellaneous Mathematical Symbols B"; break;
    case UBLOCK_SUPPLEMENTAL_MATHEMATICAL_OPERATORS: return "Supplemental Mathematical Operators"; break;
    case UBLOCK_KATAKANA_PHONETIC_EXTENSIONS: return "Katakana Phonetic Extensions"; break;
    case UBLOCK_VARIATION_SELECTORS: return "Variation Selectors"; break;
    case UBLOCK_SUPPLEMENTARY_PRIVATE_USE_AREA_A: return "Supplementary Private Use Area A"; break;
    case UBLOCK_SUPPLEMENTARY_PRIVATE_USE_AREA_B: return "Supplementary Private Use Area B"; break;

#define EXPECTED_UBLOCK_COUNT (UBLOCK_SUPPLEMENTARY_PRIVATE_USE_AREA_B+1)

    default: return "Unknown block "; break;
    }
}

const char *getUPropertyName(UProperty u)
{
  switch(u)
  {
    /** Binary property Alphabetic. Same as u_isUAlphabetic, different from u_isalpha.
        Lu+Ll+Lt+Lm+Lo+Nl+Other_Alphabetic @draft ICU 2.1 */
    case UCHAR_ALPHABETIC: return "Alphabetic";
    /** Binary property ASCII_Hex_Digit. 0-9 A-F a-f @draft ICU 2.1 */
    case UCHAR_ASCII_HEX_DIGIT: return "ASCII Hex Digit";
    /** Binary property Bidi_Control.
        Format controls which have specific functions
        in the Bidi Algorithm. @draft ICU 2.1 */
    case UCHAR_BIDI_CONTROL: return "BiDi Control";
    /** Binary property Bidi_Mirrored.
        Characters that may change display in RTL text.
        Same as u_isMirrored.
        See Bidi Algorithm, UTR 9. @draft ICU 2.1 */
    case UCHAR_BIDI_MIRRORED: return "BiDi Mirrored";
    /** Binary property Dash. Variations of dashes. @draft ICU 2.1 */
    case UCHAR_DASH: return "Dash";
    /** Binary property Default_Ignorable_Code_Point (new in Unicode 3.2).
        Ignorable in most processing.
        <2060..206F, FFF0..FFFB, E0000..E0FFF>+Other_Default_Ignorable_Code_Point+(Cf+Cc+Cs-White_Space) @draft ICU 2.1 */
    case UCHAR_DEFAULT_IGNORABLE_CODE_POINT: return "Default Ignorable Code Point";
    /** Binary property Deprecated (new in Unicode 3.2).
        The usage of deprecated characters is strongly discouraged. @draft ICU 2.1 */
    case UCHAR_DEPRECATED: return "Deprecated";
    /** Binary property Diacritic. Characters that linguistically modify
        the meaning of another character to which they apply. @draft ICU 2.1 */
    case UCHAR_DIACRITIC: return "Diacritic";
    /** Binary property Extender.
        Extend the value or shape of a preceding alphabetic character,
        e.g., length and iteration marks. @draft ICU 2.1 */
    case UCHAR_EXTENDER: return "Extender";
    /** Binary property Full_Composition_Exclusion.
        CompositionExclusions.txt+Singleton Decompositions+
        Non-Starter Decompositions. @draft ICU 2.1 */
    case UCHAR_FULL_COMPOSITION_EXCLUSION: return "Full Composition Exclusion";
    /** Binary property Grapheme_Base (new in Unicode 3.2).
        For programmatic determination of grapheme cluster boundaries.
        [0..10FFFF]-Cc-Cf-Cs-Co-Cn-Zl-Zp-Grapheme_Link-Grapheme_Extend-CGJ @draft ICU 2.1 */
    case UCHAR_GRAPHEME_BASE: return "Grapheme Base";
    /** Binary property Grapheme_Extend (new in Unicode 3.2).
        For programmatic determination of grapheme cluster boundaries.
        Me+Mn+Mc+Other_Grapheme_Extend-Grapheme_Link-CGJ @draft ICU 2.1 */
    case UCHAR_GRAPHEME_EXTEND: return "Grapheme Extend";
    /** Binary property Grapheme_Link (new in Unicode 3.2).
        For programmatic determination of grapheme cluster boundaries. @draft ICU 2.1 */
    case UCHAR_GRAPHEME_LINK: return "Grapheme Link";
    /** Binary property Hex_Digit.
        Characters commonly used for hexadecimal numbers. @draft ICU 2.1 */
    case UCHAR_HEX_DIGIT: return "Hex Digit";
    /** Binary property Hyphen. Dashes used to mark connections
        between pieces of words, plus the Katakana middle dot. @draft ICU 2.1 */
    case UCHAR_HYPHEN: return "Hyphen";
    /** Binary property ID_Continue.
        Characters that can continue an identifier.
        ID_Start+Mn+Mc+Nd+Pc @draft ICU 2.1 */
    case UCHAR_ID_CONTINUE: return "ID Continue";
    /** Binary property ID_Start.
        Characters that can start an identifier.
        Lu+Ll+Lt+Lm+Lo+Nl @draft ICU 2.1 */
    case UCHAR_ID_START: return "ID Start";
    /** Binary property Ideographic.
        CJKV ideographs. @draft ICU 2.1 */
    case UCHAR_IDEOGRAPHIC: return "Ideographic";
    /** Binary property IDS_Binary_Operator (new in Unicode 3.2).
        For programmatic determination of
        Ideographic Description Sequences. @draft ICU 2.1 */
    case UCHAR_IDS_BINARY_OPERATOR: return "IDS Binary Operator";
    /** Binary property IDS_Trinary_Operator (new in Unicode 3.2).
        For programmatic determination of
        Ideographic Description Sequences. @draft ICU 2.1 */
    case UCHAR_IDS_TRINARY_OPERATOR: return "IDS Trinary Operator";
    /** Binary property Join_Control.
        Format controls for cursive joining and ligation. @draft ICU 2.1 */
    case UCHAR_JOIN_CONTROL: return "Join Control";
    /** Binary property Logical_Order_Exception (new in Unicode 3.2).
        Characters that do not use logical order and
        require special handling in most processing. @draft ICU 2.1 */
    case UCHAR_LOGICAL_ORDER_EXCEPTION: return "Logical Order Exception";
    /** Binary property Lowercase. Same as u_isULowercase, different from u_islower.
        Ll+Other_Lowercase @draft ICU 2.1 */
    case UCHAR_LOWERCASE: return "Lowercase";
    /** Binary property Math. Sm+Other_Math @draft ICU 2.1 */
    case UCHAR_MATH: return "Math";
    /** Binary property Noncharacter_Code_Point.
        Code points that are explicitly defined as illegal
        for the encoding of characters. @draft ICU 2.1 */
    case UCHAR_NONCHARACTER_CODE_POINT: return "Noncharacter Code Point";
    /** Binary property Quotation_Mark. @draft ICU 2.1 */
    case UCHAR_QUOTATION_MARK: return "Quotation Mark";
    /** Binary property Radical (new in Unicode 3.2).
        For programmatic determination of
        Ideographic Description Sequences. @draft ICU 2.1 */
    case UCHAR_RADICAL: return "Radical";
    /** Binary property Soft_Dotted (new in Unicode 3.2).
        Characters with a "soft dot", like i or j.
        An accent placed on these characters causes
        the dot to disappear. @draft ICU 2.1 */
    case UCHAR_SOFT_DOTTED: return "Soft Dotted";
    /** Binary property Terminal_Punctuation.
        Punctuation characters that generally mark
        the end of textual units. @draft ICU 2.1 */
    case UCHAR_TERMINAL_PUNCTUATION: return "Terminal Punctuation";
    /** Binary property Unified_Ideograph (new in Unicode 3.2).
        For programmatic determination of
        Ideographic Description Sequences. @draft ICU 2.1 */
    case UCHAR_UNIFIED_IDEOGRAPH: return "Unified Ideograph";
    /** Binary property Uppercase. Same as u_isUUppercase, different from u_isupper.
        Lu+Other_Uppercase @draft ICU 2.1 */
    case UCHAR_UPPERCASE: return "Uppercase";
    /** Binary property White_Space.
        Same as u_isUWhiteSpace, different from u_isspace and u_isWhitespace.
        Space characters+TAB+CR+LF-ZWSP-ZWNBSP @draft ICU 2.1 */
    case UCHAR_WHITE_SPACE: return "White Space";
    /** Binary property XID_Continue.
        ID_Continue modified to allow closure under
        normalization forms NFKC and NFKD. @draft ICU 2.1 */
    case UCHAR_XID_CONTINUE: return "XID Continue";
    /** Binary property XID_Start. ID_Start modified to allow
        closure under normalization forms NFKC and NFKD. @draft ICU 2.1 */
    case UCHAR_XID_START: return "XID Start";

    /** One more than the last constant for binary Unicode properties. @draft ICU 2.1 */
    case UCHAR_BINARY_LIMIT: return "Binary Limit";

    /** Enumerated property Bidi_Class.
        Same as u_charDirection, returns UCharDirection values. @draft ICU 2.2 */

    /** First constant for enumerated/integer Unicode properties. @draft ICU 2.2 */
    case UCHAR_BIDI_CLASS: return "BiDi Class";
    /** Enumerated property Block.
        Same as ublock_getCode, returns UBlockCode values. @draft ICU 2.2 */
    case UCHAR_BLOCK: return "Block";
    /** Enumerated property Canonical_Combining_Class.
        Same as u_getCombiningClass, returns 8-bit numeric values. @draft ICU 2.2 */
    case UCHAR_CANONICAL_COMBINING_CLASS: return "Canonical Combining Class";
    /** Enumerated property Decomposition_Type.
        Returns UDecompositionType values. @draft ICU 2.2 */
    case UCHAR_DECOMPOSITION_TYPE: return "Decomposition Type";
    /** Enumerated property East_Asian_Width.
        See http://www.unicode.org/reports/tr11/
        Returns UEastAsianWidth values. @draft ICU 2.2 */
    case UCHAR_EAST_ASIAN_WIDTH: return "East Asian Width";
    /** Enumerated property General_Category.
        Same as u_charType, returns UCharCategory values. @draft ICU 2.2 */
    case UCHAR_GENERAL_CATEGORY: return "General Category";
    /** Enumerated property Joining_Group.
        Returns UJoiningGroup values. @draft ICU 2.2 */
    case UCHAR_JOINING_GROUP: return "Joining Group";
    /** Enumerated property Joining_Type.
        Returns UJoiningType values. @draft ICU 2.2 */
    case UCHAR_JOINING_TYPE: return "Joining Type";
    /** Enumerated property Line_Break.
        Returns ULineBreak values. @draft ICU 2.2 */
    case UCHAR_LINE_BREAK: return "Line Break";
    /** Enumerated property Numeric_Type.
        Returns UNumericType values. @draft ICU 2.2 */
    case UCHAR_NUMERIC_TYPE: return "Numeric Type";
    /** Enumerated property Script.
        Same as uscript_getScript, returns UScriptCode values. @draft ICU 2.2 */
    case UCHAR_SCRIPT: return "Script";

    case UCHAR_INT_LIMIT: return "Int Limit";
    default: return "Unknown UProperty value";
  }
}

int lxu_validate_property_sanity()
{
  int insane = 0;

  if(U_CHAR_CATEGORY_COUNT != EXPECTED_U_CHAR_CATEGORY_COUNT) {
#if 0
    u_fprintf(gOut, "<B>U_CHAR_CATEGORY_COUNT=%d, expected %d</B><BR>\r\n", 
	      U_CHAR_CATEGORY_COUNT, EXPECTED_U_CHAR_CATEGORY_COUNT);
#endif
    insane=1;
  }

  if(UBLOCK_COUNT != EXPECTED_UBLOCK_COUNT) {
#if 0
    u_fprintf(gOut, "<B>UBLOCK_COUNT=%d, expected %d</B><BR>\r\n", 
	      UBLOCK_COUNT, EXPECTED_UBLOCK_COUNT);
#endif
    insane=1;
  }
  if(insane>0) {
#if 0
    u_fprintf(gOut, "<H1>ERROR! utypes.h has changed since ubrowse.c was updated! Some values will be out of date!</H1>\r\n");
#endif
  }
  return insane;
}


/* Just stuff */
typedef struct {
  const char *n; /* name */
  int i;         /* original index */
} EnumVal;

EnumVal gUCharCategoryNames[U_CHAR_CATEGORY_COUNT];
EnumVal gUBlockCodeNames[UBLOCK_COUNT];
int sortedNames = 0;

void setup_sortedNames();

static int compnames_proc(const void *aa, const void *bb)
{
  EnumVal *a = (EnumVal*)aa;
  EnumVal *b = (EnumVal*)bb;
  return strcmp(a->n, b->n);
}

void setup_sortedNames()
{
  int i;
  /* first, the block names */
  for(i=0;i<UBLOCK_COUNT;i++) {
    gUBlockCodeNames[i].n = getUBlockCodeName(i);
    gUBlockCodeNames[i].i = i;
  }
  qsort(gUBlockCodeNames, UBLOCK_COUNT, sizeof(gUBlockCodeNames[0]), &compnames_proc);

  for(i=0;i<U_CHAR_CATEGORY_COUNT;i++) {
    gUCharCategoryNames[i].n = getUCharCategoryName(i);
    gUCharCategoryNames[i].i = i;
  }

  qsort(gUCharCategoryNames, U_CHAR_CATEGORY_COUNT, sizeof(gUCharCategoryNames[0]), &compnames_proc);
  sortedNames=1;
}

const char *getUCharCategorySortedName(int32_t n)
{
  if(sortedNames==0) setup_sortedNames();
  return gUCharCategoryNames[n].n;
}

const char *getUBlockCodeSortedName(int32_t n)
{
  if(sortedNames==0) setup_sortedNames();
  return gUBlockCodeNames[n].n;
}

UCharCategory getUCharCategorySorted(int32_t n)
{
  if(sortedNames==0) setup_sortedNames();
  return gUCharCategoryNames[n].i;
}

UBlockCode    getUBlockCodeSorted(int32_t n)
{
  if(sortedNames==0) setup_sortedNames();
  return gUBlockCodeNames[n].i;
}


