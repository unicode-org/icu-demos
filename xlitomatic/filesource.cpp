/**********************************************************************
*   Copyright (C) 2000, International Business Machines
*   Corporation and others.  All Rights Reserved.
***********************************************************************/

#include "unicode/ustdio.h"
#include "filesource.h"
#include "unicode/ustring.h"
#include "stdio.h"
#include "unicode/lx_utils.h"

FileSource::FileSource(const char *path)
{
  //  f = u_fopen(fn, "r", NULL, enc);
  f = fopen(path, "r");
}

void FileSource::rewind()
{
  // u_fclose(f);
  //    f = u_fopen(fn, "r", NULL, enc);
  ::rewind(f);
}

int32_t FileSource::read(const uint8_t* &start, const uint8_t* &end)
{
  //  if(!u_fgets(f, sizeof(buffer)/sizeof(UChar), buffer))
  if(feof(f) || fEOF)
    {
      fEOF = TRUE;
      return 0;
    }

  int32_t len;
  char *c;
  c = fgets(buffer, (sizeof(buffer)/sizeof(buffer[0]))-2, f);
  if(!c)
    {
      fEOF = TRUE;
      return 0;
    }

  if(feof(f))
    {
      fEOF = TRUE;
    }

  len = strlen(c);
      
  start = &(buffer[0]);
  end = &(buffer[len]);
  return len;
}

FileSource::~FileSource()
{
  fclose(f);
}

/****************** TOUSOURCE ************/
ToUSource::ToUSource(RawSource& src, const char *enc)
  : u(NULL), source(src)
{
  UErrorCode status = U_ZERO_ERROR;

  fprintf(stderr, "enc=%s\n", enc);
  u = ucnv_open(enc, &status);
  if(U_FAILURE(status))
    {
      fEOF = TRUE;
      if(u) 
        {
          ucnv_close(u);
        }
      u = NULL;
    }
  ucnv_setFromUCallBack(u, UCNV_FROM_U_CALLBACK_BACKSLASH_ESCAPE_HTML, &status);
}

ToUSource::~ToUSource()
{
  if(u)
    {
      ucnv_close(u);
    }
}

void ToUSource::rewind()
{
  // ?
}

int32_t ToUSource::read(const UChar* &start, const UChar* &end)
{
  if(fEOF == TRUE)
    {
      return 0;
    }

  if(source.eof())
    {
      fEOF = TRUE;
      return 0;
    }

  // load in the source data..
  const uint8_t *sourceData, *sourceDataLimit;

  if(source.read(sourceData, sourceDataLimit) == 0)
    {
      fEOF = TRUE;
      return 0;
    }

  UErrorCode status = U_ZERO_ERROR;
  int32_t n;
  UChar *target = buffer;
  const UChar *targetLimit = &buffer[sizeof(buffer)/sizeof(buffer[0])];

  do
    {
      ucnv_toUnicode(u, &target, targetLimit, 
                    (const char **)&sourceData, 
                    (const char *)sourceDataLimit,
                    NULL,
                    source.eof(),
                    &status);
    }
  while(status == U_INDEX_OUTOFBOUNDS_ERROR);

  n = target-buffer;

  start = buffer;
  end   = target;
  return n;
}
