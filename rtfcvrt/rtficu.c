/*  Copyright (c) 2000 IBM, Inc. 
    All Rights Reserved */

/* ICU parts to the RTF converter */

#include "unicode/utypes.h"
#include "unicode/ucnv.h"
#include <stdio.h>

int32_t lastcp = -1; /*  \fcharsetn of last cp run */
UBool ignoreNext = false;

struct
{
  UConverter *conv;
  int32_t     fcharset;
  const char *convid;
} convs[] = 
{

/* list from:

   http://shlimazl.nm.ru/eng/fonts.HTM
  [also thanks for the RTF docs link]
*/

  { NULL, 0,     "cp1252" },
/*   { NULL, 1,     "?"      }, */
/*   { NULL, 2,     "symbol" }, */
/*   { NULL, 77,      "mac"   }, // macroman? */
  { NULL, 128,    "shift-jis" }, /*  sjis? */
/*   { NULL, 129, "hangul" }, // euc-kr? */
/*  { NULL, 130, "hangul-johab" },  */
  { NULL, 134, "gb2312" }, /*  chinese */
  { NULL, 136, "big5" }, /*  chinese */
  { NULL, 161, "cp1253" }, /*  greek */
  { NULL, 162, "cp1254" }, /*  turkish */
  { NULL, 163, "cp1258" }, /*  vietnamese */
  { NULL, 177, "cp1255" }, /*  hebrew */
  { NULL, 178, "cp1256" }, /*  arabic */
  { NULL, 186, "cp1257" }, /*  baltic */
  { NULL, 204, "cp1251" }, /*  cyrillic */
  { NULL, 222, "ibm-874" }, /*  thai  [should be cp874] */
  { NULL, 238, "cp1250" }, /*  CE [latin2] */
/*   { NULL, 255, "dos" }, // OEM/DOS  [??] */
  { NULL, -1, NULL }
};

UConverter *converterForChset(int32_t chset)
{
  int32_t i;
  UErrorCode err = U_ZERO_ERROR;
  UConverter *c;

  for(i=0;convs[i].fcharset != -1;i++) {
    if(convs[i].fcharset == chset) {
      if(convs[i].conv == NULL) {
        c = ucnv_open(convs[i].convid, &err);
        if(U_FAILURE(err)) {
          fprintf(stderr, "Couldn't open %s [\\fcharset%d] - %s\n", 
                  convs[i].convid,
                  chset,
                  u_errorName(err));
          err = U_ZERO_ERROR;
          c = ucnv_open(NULL, &err);
          if(U_FAILURE(err)) {
            fprintf(stderr, "FATAL: can't open default cp! %s\n",
                    u_errorName(err));
            fflush(stderr);
            exit(1);
          }
        }
        convs[i].conv = c;
      }
      return convs[i].conv;
    }
  }
  fprintf(stderr, "FATAL: don't know what to do with \\fcharset%d\n", chset);
  exit(1);
  
  return NULL; 
}

#define BUF 1024
char buf[BUF];
int p = 0;
UChar ubuf[BUF];

char utf8buf[BUF];
UConverter *utf8 = NULL;

void icu_flushit(int final) /*  int for export's sake */
{
  UConverter *c;
  UChar *target;
  UChar *targetLimit;
  const char *source;
  const char *sourceLimit;
  UErrorCode status = U_ZERO_ERROR;
  int32_t n;


  if(utf8 == NULL) {
    UChar bom[] = { 0xFEFF, 0x0000 };

    utf8 = ucnv_open("utf-8", &status);
    
    if(U_FAILURE(status)) {
      fprintf(stderr, "FATAL: can't open utf-8 - %s\n", u_errorName(status));
      exit(3);
    }
    
    n = ucnv_fromUChars(utf8, utf8buf, BUF, bom, 1, &status);
    if(U_FAILURE(status)) {
      fprintf(stderr, "FATAL: can't write UTF8 BOM %s\n", u_errorName(status));
      exit(3);
    }
    
    fwrite(utf8buf, 1, n, stdout);
  }

  if(lastcp == -1) {
    p = 0;
    return; /*  initialization */
  }

  c = converterForChset(lastcp);

  if(p == 0) {
    return; /*  nothing to do */
  }
  
  source = buf;
  sourceLimit = buf + p;
  p = 0;

  do
  {
    target = ubuf;
    targetLimit = ubuf+BUF;

    ucnv_toUnicode(c, &target, targetLimit,
                   &source, sourceLimit, NULL,
                   final,
                   &status);
    
    if(status != U_INDEX_OUTOFBOUNDS_ERROR)
    {
      status = U_ZERO_ERROR;
    }
    else
    {
      if(U_FAILURE(status))
      {
        fprintf(stderr, "FATAL: error in toUnicode, got %s\n", 
                u_errorName(status));
        exit(2);
      }
    }
    
    /*  Write out UTF-8 */
    n = ucnv_fromUChars(utf8, utf8buf, BUF, ubuf, target-ubuf, &status);

    if(U_FAILURE(status)) {
      fprintf(stderr, "FATAL: can't write UTF8 data %s\n", u_errorName(status));
      exit(3);
    }
    fwrite(utf8buf, 1, n, stdout);

  } while (source < sourceLimit);
  if(final) {
    ucnv_reset(c);
  }
  /*  c goes back in the list.. */
} 

void icu_putchar(char ch, int cp)
{
  if(lastcp != cp) {
    icu_flushit(true); /*  flush due to cp change */
    lastcp = cp;
  }

  if(ignoreNext == true) {
    ignoreNext = false;
    return;
  }

  buf[p++] = ch;
  if(p >= BUF) {
    icu_flushit(false); /*  flush due to buffer overrun */
  }
}

void icu_putuchar(int uchar)
{
  int n = 0;
  UErrorCode status = U_ZERO_ERROR;

  icu_flushit(true); /* #1 flush old chars, #2 set up utf8 converter */

  ubuf[0] = uchar;
  
  n = ucnv_fromUChars(utf8, utf8buf, BUF, ubuf, 1, &status);
  
  if(U_FAILURE(status)) {
    fprintf(stderr, "FATAL: can't write UTF8 unicode literal data data %s\n", u_errorName(status));
    exit(3);
  }
  fwrite(utf8buf, 1, n, stdout);
  
  ignoreNext = true; /* get rid of trailing '?' */
}
