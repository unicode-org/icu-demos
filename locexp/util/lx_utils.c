/**********************************************************************
*   Copyright (C) 1999, International Business Machines
*   Corporation and others.  All Rights Reserved.
***********************************************************************/

/*****************************************
   Some random utilities..
*/

#include "lx_utils.h"

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

void 
UCNV_FROM_U_CALLBACK_BACKSLASH_ESCAPE_HTML  (UConverter * _this,
					    char **target,
					    const char *targetLimit,
					    const UChar ** source,
					    const UChar * sourceLimit,
					    int32_t *offsets,
					    bool_t flush,
					    UErrorCode * err)
{
  UChar valueString[100];
  int32_t valueStringLength = 0;
  UChar codepoint[100];
  int32_t i = 0;
  /*Makes a bitwise copy of the converter passwd in */
  UConverter myConverter = *_this;
  char myTarget[100];
  char *myTargetAlias = myTarget;
  const UChar *myValueSource = NULL;
  UErrorCode err2 = U_ZERO_ERROR;


  if (CONVERSION_U_SUCCESS (*err))
    return;

  ucnv_reset (&myConverter);
  ucnv_setFromUCallBack (&myConverter,
			 (UConverterFromUCallback) UCNV_FROM_U_CALLBACK_STOP,
			 &err2);
  if (U_FAILURE (err2))
    {
      *err = err2;
      return;
    }
  u_uastrcpy(codepoint,"<B>\\u");

  while (i < _this->invalidUCharLength)
    {
      itou (codepoint + 5, _this->invalidUCharBuffer[i++], 16, 4);
      u_uastrcpy(codepoint+9, "</B>");
      memcpy (valueString + valueStringLength, codepoint, sizeof (UChar) * (5+4+4) );
      valueStringLength += (5+4+4) ;
    }

  myValueSource = valueString;

  /*converts unicode escape sequence */
  ucnv_fromUnicode (&myConverter,
		    &myTargetAlias,
		    myTargetAlias + 100,
		    &myValueSource,
		    myValueSource + (5+4+4),
		    NULL,
		    TRUE,
		    &err2);

  if (U_FAILURE (err2))
    {
      UCNV_FROM_U_CALLBACK_SUBSTITUTE (_this,
				       target,
				       targetLimit,
				       source,
				       sourceLimit,
				       offsets,
				       flush,
				       err);
      return;
    }

  valueStringLength = myTargetAlias - myTarget;

  /*if we have enough space on the output buffer we just copy
   * the subchar there and update the pointer
   */
  if ((targetLimit - *target) >= valueStringLength)
    {
      memcpy (*target, myTarget, valueStringLength);
      *target += valueStringLength;
      *err = U_ZERO_ERROR;
    }
  else
    {
      /*if we don't have enough space on the output buffer
       *we copy as much as we can to it, update that pointer.
       *copy the rest in the internal buffer, and increase the
       *length marker
       */
      memcpy (*target, valueString, (targetLimit - *target));
      memcpy (_this->charErrorBuffer + _this->charErrorBufferLength,
		  valueString + (targetLimit - *target),
		  valueStringLength - (targetLimit - *target));
      _this->charErrorBufferLength += valueStringLength - (targetLimit - *target);
      *target += (targetLimit - *target);
      *err = U_INDEX_OUTOFBOUNDS_ERROR;
    }

  return;
}

void 
UCNV_FROM_U_CALLBACK_BACKSLASH_ESCAPE (UConverter * _this,
			      char **target,
			      const char *targetLimit,
			      const UChar ** source,
			      const UChar * sourceLimit,
			      int32_t *offsets,
			      bool_t flush,
			      UErrorCode * err)
{
  UChar valueString[100];
  int32_t valueStringLength = 0;
  UChar codepoint[100];
  int32_t i = 0;
  /*Makes a bitwise copy of the converter passwd in */
  UConverter myConverter = *_this;
  char myTarget[100];
  char *myTargetAlias = myTarget;
  const UChar *myValueSource = NULL;
  UErrorCode err2 = U_ZERO_ERROR;


  if (CONVERSION_U_SUCCESS (*err))
    return;

  ucnv_reset (&myConverter);
  ucnv_setFromUCallBack (&myConverter,
			 (UConverterFromUCallback) UCNV_FROM_U_CALLBACK_STOP,
			 &err2);
  if (U_FAILURE (err2))
    {
      *err = err2;
      return;
    }
  u_uastrcpy(codepoint,"\\u");

  while (i < _this->invalidUCharLength)
    {
      itou (codepoint + 2, _this->invalidUCharBuffer[i++], 16, 4);
      memcpy (valueString + valueStringLength, codepoint, sizeof (UChar) * (2+4) );
      valueStringLength += (2+4) ;
    }

  myValueSource = valueString;

  /*converts unicode escape sequence */
  ucnv_fromUnicode (&myConverter,
		    &myTargetAlias,
		    myTargetAlias + 100,
		    &myValueSource,
		    myValueSource + (2+4),
		    NULL,
		    TRUE,
		    &err2);

  if (U_FAILURE (err2))
    {
      UCNV_FROM_U_CALLBACK_SUBSTITUTE (_this,
				       target,
				       targetLimit,
				       source,
				       sourceLimit,
				       offsets,
				       flush,
				       err);
      return;
    }

  valueStringLength = myTargetAlias - myTarget;

  /*if we have enough space on the output buffer we just copy
   * the subchar there and update the pointer
   */
  if ((targetLimit - *target) >= valueStringLength)
    {
      memcpy (*target, myTarget, valueStringLength);
      *target += valueStringLength;
      *err = U_ZERO_ERROR;
    }
  else
    {
      /*if we don't have enough space on the output buffer
       *we copy as much as we can to it, update that pointer.
       *copy the rest in the internal buffer, and increase the
       *length marker
       */
      memcpy (*target, valueString, (targetLimit - *target));
      memcpy (_this->charErrorBuffer + _this->charErrorBufferLength,
		  valueString + (targetLimit - *target),
		  valueStringLength - (targetLimit - *target));
      _this->charErrorBufferLength += valueStringLength - (targetLimit - *target);
      *target += (targetLimit - *target);
      *err = U_INDEX_OUTOFBOUNDS_ERROR;
    }

  return;
}
/*******************************************************end of borrowed code from ucnv_err.c **/



void doDecodeQueryField(const char *in, char *out, int32_t length)
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
     origLen == u_strlen(src); /* but we don't null terminate still */

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
 * TODO: limit displayname to just a part (default -> English -> United States -> California, instead of   .... -> English (United States) -> ... 
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
      s->ustr = malloc((siz+1) * sizeof(UChar));
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
	  s->ustr = malloc((siz+1) * sizeof(UChar));
	  ((UChar*)(s->ustr))[0] = 0;
	  status = U_ZERO_ERROR;
	  uloc_getDisplayCountry( s->str, inLocale, (UChar*)(s->ustr), siz, &status );
	}
      else
	{
	  status = U_ZERO_ERROR;
	  if( ( siz = uloc_getDisplayLanguage( s->str, inLocale, NULL, 0, &status)) &&
	      (siz > 1) )
	    {
	      s->ustr = malloc((siz+1) * sizeof(UChar));
	      ((UChar*)(s->ustr))[0] = 0;
	      status = U_ZERO_ERROR;
	      uloc_getDisplayLanguage( s->str, inLocale, (UChar*)(s->ustr), siz, &status );
	    }
	  else
	    s->ustr = 0;
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
  int32_t n;

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
 * default -> en, ja, de, ... -> en_US, en_CA, .. -> en_US_CALIFORNIA
 * @param inLocale the Locale the tree should be created in
 * @param localeCount [return] total # of items in the tree
 * @return a new MySortable tree, owned by the caller.  Not sorted.
 */

MySortable *createLocaleTree(const char *inLocale, int32_t *localeCount)
{
  MySortable *root;

  int32_t     nlocs;  /* ICU-specified total locales */
  
  /* temps */
  int32_t     i,j;

  root = malloc(sizeof(MySortable));

  /* get things rolling... */
  initSortable(root, "default", inLocale, NULL);
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
      if(a->keySize == minSize)
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
void mySort(MySortable *s, UErrorCode *status, bool_t recurse)
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
  int i;

static  struct
  {
    char old[20];
    char new[20];
  } mapping[] = 
    {
      /* NOTA BENE:   the 'mime' standard (or nonstandard) names for encoders
	 seems like a useful property. We really should have API for dealing with
	 these names.  -srl
      */

      { "UTF8",     "utf-8" },
      { "LATIN_1",  "iso-8859-1" },
      { "ISO_2022", "iso-2022" },
      { "ibm-1089", "iso-8859-6" },
      { "ibm-1252", "windows-1252" },
      { "ibm-1386", "gb" },
      { "ibm-813",  "iso-8859-7" },
      { "ibm-912",  "iso-8859-2" },
      { "ibm-913",  "iso-8859-3" }, 
      { "ibm-914",  "iso-8859-4" },
      { "ibm-915",  "iso-8859-5" },
      { "ibm-916",  "iso-8859-8" },
      { "ibm-920",  "iso-8859-9" },
      { "ibm-923",  "iso-8859-15" },
      { "ibm-943",  "shift_jis" },
      { "ibm-949",  "ksc-5601" },
      { "ibm-954",  "euc-jp" },
      { "ibm-964",  "euc-tw" },
      { "ibm-970",  "euc-kr" },
      { "ibm-1361", "ksc" },
      { "ibm-1383", "euc-cn" },
      { "ibm-874",  "windows-874" },
      { "ibm-878",  "koi8-r" },
      { "ibm-1250", "windows-1250" },
      { "ibm-1251", "windows-1251" },
      { "ibm-1252", "windows-1252" },
      { "ibm-1253", "windows-1253" },
      { "ibm-1254", "windows-1254" },
      { "ibm-1255", "windows-1255" },
      { "ibm-1256", "windows-1256" },
      { "ibm-1257", "windows-1257" },
      { "ibm-1258", "windows-1258" },
      { "ibm-1275", "mac" },
      { "ibm-860",  "cp860" },
      { "ibm-861",  "cp861" },
      { "ibm-862",  "cp862" },
      { "ibm-863",  "cp863" },
      { "ibm-864",  "cp864" },
      { "ibm-865",  "cp865" },
      { "ibm-866",  "cp866" },
      { "ibm-867",  "cp867" },
      { "ibm-868",  "cp868" },
      { "ibm-869",  "cp869" },
      { "\0", "\0" }
    };

  for(i=0;mapping[i].old[0];i++)
    {
      if(!strcmp(n,mapping[i].old))
	return mapping[i].new;
    }
  return n;
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
	*(urlQuery++) = *src;

      src++;
    }
  *urlQuery = 0; 
}


/* date [from udate by Stephen F. Booth] ----------------------------------------------------- */
/* Format the date */




UChar *
date(const UChar *tz,
     UDateFormatStyle style,
     UErrorCode *status)
{
  return dateAt(ucal_getNow(), tz, style, status);
}

UChar *dateAt(UDate adate, const UChar *tz, UDateFormatStyle style, UErrorCode *status)
{
  UChar *s = 0;
  int32_t len = 0;
  UDateFormat *fmt;

  fmt = udat_open(style, style, NULL, tz, -1, status);
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

bool_t testConverter(const char *converter, 
                     const UChar *sample,
		     int32_t sampleLen, 
		     int8_t *buffer,
		     int32_t bufLen)
{
  UErrorCode  status = U_ZERO_ERROR;
  UConverter *cnv;
  bool_t      worked = FALSE;  
  int8_t     *target;

  cnv = ucnv_open(converter, &status);
  if(U_FAILURE(status))
    return FALSE;

  target = buffer;
  
  ucnv_setFromUCallBack(cnv, UCNV_FROM_U_CALLBACK_STOP, &status);

  ucnv_fromUnicode (cnv,
		    &target,
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

UResourceBundle *FSWF_getBundle()
{
  static UResourceBundle *gRB = NULL;

  if(gRB == 0)
    {
      FSWF_bundleErr = U_ZERO_ERROR;
      gRB = ures_open(  FSWF_bundlePath(), NULL, &FSWF_bundleErr);
      if(U_FAILURE(FSWF_bundleErr))
	{
	  gRB = 0;
	}
    }
  return gRB;

}


/*** fetch string with fallback -------------------------------------------------*/
const UChar *FSWF(const char *key, const char *fallback)
{
  UErrorCode status = U_ZERO_ERROR;
  const UChar *str = 0;
  static UChar   gFSWFTempChars[1024];
  UChar *fswfTempChars = gFSWFTempChars;
  UResourceBundle *rb;

  if(strlen(fallback) >  1020)
    fswfTempChars = malloc(sizeof(UChar)*(strlen(fallback)+1));
      
  rb = FSWF_getBundle();

  status = U_ZERO_ERROR;

  if(rb != 0)
    str = ures_get( rb, key, &status);

  if(str == 0) /* failed to get a str */
    {
      /* fallback: use our temp buffer [NON MT safe] and do a strcpy.. */
      u_uastrcpy(gFSWFTempChars, fallback);
      str = malloc((u_strlen(gFSWFTempChars)+1) * sizeof(UChar)); /* LEAK but who cares, it's an error case */
      u_strcpy((UChar*)str,gFSWFTempChars);
    }

  return str;
}


void FSWF_close()
{
  ures_close(FSWF_getBundle());
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




