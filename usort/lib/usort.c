/******************************************************************************
*
*   Copyright (C) 1999-2000, International Business Machines
*   Corporation and others.  All Rights Reserved.
*
*******************************************************************************
 *
 * <srl@monkey.sbay.org>
 *
 * Implementation for the USort library
 */

#define kInBufSize     30  /* Just the input buffer chunk size. */
#define kOutBufSize    1024 /* expected line length in chars. [mem usage: 2x ] */
#define kPrintBufSize  1024

#define T_USORT_IMPLEMENTATION

#include <stdlib.h>
#include "unicode/usort.h"

#ifdef WIN32
#include <stdlib.h>
#include <fcntl.h>
#include <io.h>
#endif

/** Utility fcn to print out chars. Ripped off badly from date/cal::uprint **/

static void usort_printChars(const UChar *s, FILE *f, UConverter *converter, UErrorCode *status)
{
  /* converter */
  char buf [kPrintBufSize];
  int32_t sourceLen;
  const UChar *mySource;
  const UChar *mySourceEnd;
  char *myTarget;
  int32_t arraySize;

  if(s == 0) return;

  /* set up the conversion parameters */
  sourceLen    = u_strlen(s);
  mySource     = s;
  mySourceEnd  = mySource + sourceLen;
  myTarget     = buf;
  arraySize    = kPrintBufSize;

  /* if we failed, clean up and exit */
  if(U_FAILURE(*status)) goto finish;
  
  /* perform the conversion */
  do {
    /* reset the error code */
    *status = U_ZERO_ERROR;

    /* perform the conversion */
    ucnv_fromUnicode(converter, &myTarget,  myTarget + arraySize,
		     &mySource, mySourceEnd, NULL,
		     TRUE, status);

    /* Write the converted data to the FILE* */
    fwrite(buf, sizeof(char), myTarget - buf, f);

    /* update the conversion parameters*/
    myTarget     = buf;
    arraySize    = kPrintBufSize;
  }
  while(*status == U_INDEX_OUTOFBOUNDS_ERROR); 

 finish:

  ;

  
}

/** Utility fcn to do the low level compare **/
/* TODO: Rewrite so it's not using memcmp!!!!! */
static int usort_sortProc(const void *aa, const void *bb)
{
  int cmp;
  int minSize;
  const USortLine *a, *b;

  a = (const USortLine*) aa;
  b = (const USortLine*) bb;

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


USort*
usort_open(const char *locale, UCollationStrength strength, bool_t ownText,
           UErrorCode *status)
{
  USort *n;
  
  if(U_FAILURE(*status))
    return 0;

  n = malloc(sizeof(USort));

  if(!n)
    {
      *status = U_MEMORY_ALLOCATION_ERROR;
      return n;
    }

  n->lines  = 0;
  n->size =0;
  n->count = 0;
  n->ownsText = ownText;
  n->collator = ucol_open(locale, status);
  n->func = ucol_getSortKey;

  if(U_FAILURE(*status)) /* Failed to open the collator. */
    {
      free(n);
      return 0;
    }
  if(strength != UCOL_DEFAULT) 
  {
    ucol_setStrength(n->collator, strength);
  }

  return n;
}

void
usort_close(USort *usort)
{
  if(!usort)
    return;

  free(usort->collator);
  if(usort->ownsText)
    {
      /* TO DO */
    }

  free(usort->lines);
  
  free(usort);
}

void
usort_addLine(USort *usort, const UChar *line, int32_t len, bool_t copy, void *userData)
{
  UErrorCode status = U_ZERO_ERROR;

  if(len == -1)
    len = u_strlen(line);
  
  /* make sure the list has enough room */
  if(usort->count >= usort->size) /* also true the 1st time */
    {
      USortLine *newList = NULL;
      int32_t newSize;

      newSize = usort->size * 2;

      if(newSize == 0)
	newSize = 32; /* they're not too big */

      newList = realloc(usort->lines, sizeof(USortLine) * newSize);
      if(!newList)
	{
	  fprintf(stderr,"Fatal: could not rallocate %d bytes for linelist\n",
		  sizeof(USortLine) * newSize);
	  abort();
	}
#ifdef SDEBUG
      fprintf(stderr,"realloc linelist from %d to %d lines\n", lineSize, newSize);
#endif
      usort->lines = newList;
      usort->size = newSize;
    }
  if(copy)
      usort->lines[usort->count].chars = malloc(sizeof(UChar)*((len)+1));
  else
    usort->lines[usort->count].chars = (UChar *)line;

  if(!usort->lines[usort->count].chars)
    {
      fprintf(stderr,"could not allocate %d bytes for lineList.chars\n", (len+1)*sizeof(UChar));
      abort();
    }

  if(copy)
    {
      memcpy((UChar*)usort->lines[usort->count].chars, line, (len+1) * sizeof(UChar));

      usort->lines[usort->count].chars[len] = 0;
    }

  /* now,  collate it. Note we do NOT include the null or newline in the collation. */
  /*usort->lines[usort->count].keySize = ucol_getSortKey(usort->collator, usort->lines[usort->count].chars, len ,NULL, 0);*/
  usort->lines[usort->count].keySize = usort->func(usort->collator, usort->lines[usort->count].chars, len ,NULL, 0);
  usort->lines[usort->count].userData = userData;

  usort->lines[usort->count].key = malloc ( usort->lines[usort->count].keySize );
  /*ucol_getSortKey(usort->collator, usort->lines[usort->count].chars, len ,usort->lines[usort->count].key, usort->lines[usort->count].keySize);*/
  usort->func(usort->collator, usort->lines[usort->count].chars, len ,usort->lines[usort->count].key, usort->lines[usort->count].keySize);


#ifdef SDEBUG
  fprintf(stderr, "Line %d added, keysize %d\n", lineCount, lineList[lineCount].keySize);
  uprint(lineList[lineCount].chars, stderr, &status);
  fprintf(stderr, "\n");
#endif
	 
  usort->count++;

}

void
usort_addLinesFromFILE( USort *usort, FILE *f, UConverter *fromConverter, bool_t escapeMode)
{
  UConverter *newConverter = NULL;
  UErrorCode status = U_ZERO_ERROR;

  UChar *outBuf    = NULL; /* buffer for chars being turned into lines */
  UChar *outBufEnd = NULL; /* end of the outbuf - we'll grow if need be ! */
         UChar *target    = NULL; /* points after end of data in outBuf */
  char  *inBuf     = NULL; /* input char buffer */
  const char *inBufEnd  = NULL; /* end of inbuf, just for convenience */
  const  char  *source    = NULL; /* beginning of data to convert */
  const  char  *readData  = NULL; /* end of data in inputBuffer */
	 int32_t readCount;
	 int32_t totalReadCount = 0;
         UChar *p;

  if(fromConverter == NULL)
    {
      newConverter = ucnv_open(NULL, &status);
      fromConverter = newConverter;
    }

  if(f == NULL)
    f = stdin;

/* Setup stuff */
  if(inBuf == NULL)
    {
      inBuf = malloc( (sizeof(char) * kInBufSize));
      inBufEnd = inBuf + kInBufSize;
    }
 
  if(outBuf == NULL)
    {
      outBuf = malloc((sizeof(UChar) * kOutBufSize)
		      + 4 /* termination */
		      );
      outBufEnd = outBuf + kOutBufSize;
    }

  target   = outBuf; /* Only reset this when we hit a CR or eof */

  /* loop over the entire infile */
  while(!feof(f))
    {
#ifdef SDEBUG
      fprintf(stderr, "** reading data...\n");
#endif
      /* task #1: fill the input buffer, for more efficient conversion */
      /* at this point, inBuf has already been processed (or is empty) */
      readData = inBuf;
      source   = inBuf;
      while(readData < inBufEnd) /* more space in the inbuf */
	{
	  readCount = fread((char*)readData,sizeof(char), inBufEnd - readData, f);
	  readData += readCount;
	  if(feof(f))
	    break; /* won't get filled. */
	}
#ifdef SDEBUG
      fprintf(stderr, "%d bytes to consume\n", readData - inBuf);
#endif
      /* Task #2: transcode those bytes into uchars. At each step, stop and look for newlines.
	 we DONT want to transcode too much at a time, because it will mean more copying later.
      */
      p = target;
      while(source < readData) /* however, if we run out of source, go back and get more */
	{
	  status = U_ZERO_ERROR;
#ifdef SDEBUG
	  *readData = 0;
	  fprintf(stderr,"Transcoding %08x--%08x@%08x to %08x--%08x@%08X.\nData IN>%s<",
		  source, readData, inBuf, target, outBufEnd, outBuf, source);
#endif
	  ucnv_toUnicode(fromConverter,
			 &target,
			 outBufEnd, /* always have the rest of the buffer avail */
			 &source, 
			 readData,/* always use all read data */
			 NULL,    /* don't need offsets */
			 feof(f), /* We're always giving it the entire amount of read data.*/
		         &status);/* Therefore, if we're at EOF, [source,readData] is the */
				  /* last chunk so flush.. */
	  
	  /* OK let's break it down.. */
	  if(status == U_INDEX_OUTOFBOUNDS_ERROR)
	    {
	      UChar *newBuf;
	      int32_t newSize = sizeof(UChar)*((outBufEnd-outBuf)*2);
#ifdef SDEBUG
	      fprintf(stderr,"Resizing outbuf from %d to %d bytes\n", sizeof(UChar)*(outBufEnd-outBuf), newSize);
#endif

	      newBuf = realloc(outBuf,newSize
#ifdef SDEBUG
			       + 4 /* terminators */
#endif
			       );
	      if(!newBuf)
		{
		  fprintf(stderr," OH NO!! couldn't realloc to %d bytes.. byebye..\n", newSize);
		  abort();
		}
	      
	      /* Now, move the relevant ptrs... fun.. */
	      target = newBuf + (target-outBuf);
	      outBufEnd = newBuf + newSize;
	      outBuf = newBuf;
	      /* that's it..  OK we've got more memory for the next 'go round. But we'll
		 process what we have first. */
	      status = U_ZERO_ERROR;
	    } /* end U_INDEX_OUTOFBOUNDS_ERROR handler */

	  if(U_FAILURE(status))
	    {
	      /* we have a fairly good calculation of where the error was. totalReadCount 
		 has how many bytes have been successfully converted,
	      fprintf(stderr,"Error %d converting data around byte %d in the inputstream. (%d+ bytes successfully converted)\n", status, totalReadCount + (source-inBuf), totalReadCount);

	      /* Ignore the data?? */
	      fprintf(stderr, "** Aborting work on this file, err was %d.\n", status);
	      return; /* Oh well.. */
	    }

#ifdef SDEBUG
	  *target = 0;
	  fprintf(stderr, "Transcoded data is >");
	  uprint(p, stderr, &status); /* p is the old target */
	  fprintf(stderr, "<");
#endif

	  /* now that all THAT's out of the way, let's look at the data. */
	  for(p=outBuf;p<target;p++)
	    {
	      if( *p == 0x000A ) /* <------- REVISIT should be, LINE_SEPARATOR or something */
		{
		  /* OK! got a line. */
#ifdef SDEBUG		  
		  fprintf(stderr, "** LINE: >");
		  *p = 0; /* term */
		  uprint(outBuf, stderr, &status);		  
		  *target = 0; /* ok ?? */
		  fprintf(stderr, "< rest= >");
		  uprint(p+1, stderr, &status);
		  fprintf(stderr, "<--\n");
#endif

		  /* #1 Append the line  to the list */
		  usort_addLine(usort, outBuf, p-outBuf, TRUE, NULL);

		  /* #2 copy everything back.. */
		  if( (p+1) < target )
		    {
		      memmove(outBuf, p+1, (target-(p+1)) * sizeof(UChar) );

#ifdef SDEBUG
		      outBuf[target-(p+1)]  = 0;
		      fprintf(stderr,"Shifted, outBuf=>");
		      uprint(outBuf, stderr, &status);
		      fprintf(stderr, "<-\n");
#endif
		      
		    }

		  /* always shift other pointers.. */
#ifdef SDEBUG
		  fprintf(stderr, "out=%d, targ=%d, p=%d.. about to shift targ -= %dn",
			  outBuf - outBuf, target - outBuf, p - outBuf, ((p+1)-outBuf));
#endif

		  target -= ((p+1) - outBuf); /* right?? */

		  p = outBuf;

		}
	    }
	  /* done looking for lines.. go back and convert more .. */
	}
      /*done converting this block. Go read more data.. */

      totalReadCount += readData - inBuf; /* update the 'coarse' readCount. */
   }
  /* done with this file */
  

  /* clean up*/
  if(newConverter)
    ucnv_close(newConverter);
}

void 
usort_sort(USort *usort)
{
  /* Too easy! */
  qsort(usort->lines, usort->count, sizeof(USortLine), &usort_sortProc);
}

void
usort_printToFILE(USort *usort, FILE *file, UConverter *toConverter)
{
  UConverter *newConverter = NULL;
  UErrorCode status = U_ZERO_ERROR;
  int32_t i;
  static const UChar kLF[] = { 0x000A, 0x0000 };

  if(file == NULL)
    file = stdout;

#ifdef WIN32
  if( setmode( fileno ( file ), O_BINARY ) == -1 ) {
          perror ( "Cannot set file to binary mode" );
          exit(-1);
  }
#endif

  if(toConverter == NULL)
    {
      newConverter = ucnv_open(NULL, &status); /* TODO: errorcheck.  We're using the default converter */
      toConverter = newConverter;
    }

  for(i=0;i<usort->count;i++)
    {
      usort_printChars(usort->lines[i].chars, file, toConverter, &status);
      usort_printChars(kLF, file, toConverter, &status);
    }

  if(newConverter) /* did we allocate? */
    ucnv_close(newConverter);
}


void
usort_setSortKeyFunction(USort *usort, SortKeyFunction skFunc) {
  usort->func = skFunc;
}







