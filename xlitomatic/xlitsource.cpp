/**********************************************************************
*   Copyright (C) 2000, International Business Machines
*   Corporation and others.  All Rights Reserved.
***********************************************************************/

#include "unicode/ustdio.h"
#include "xlitsource.h"
#include "unicode/ustring.h"

/****************** TranslitUSource ************/
TranslitUSource::TranslitUSource(UnicodeSource& src, const char *xlit)
  : source(src), trans(NULL), result(),
    inTag(FALSE)
{
  char transname[500];
  sprintf(transname, "HTMLEntities-Unicode;%s", xlit);
  trans = Transliterator::createInstance(transname);
  if(!trans)
    fEOF = true;
}

TranslitUSource::~TranslitUSource()
{
  delete trans;
}

void TranslitUSource::rewind()
{
  source.rewind();
}

int32_t TranslitUSource::read(const UChar* &start, const UChar* &end)
{
  if(fEOF == true) // 'shouldn't happen'
    {
      return 0;
    }

  if(source.eof())
    {
      fEOF = true;
      return 0;
    }

  // load in the source data..
  const UChar *sourceData, *sourceDataLimit;

  if(source.read(sourceData, sourceDataLimit) == 0)
    {
      fEOF = true;
  fprintf(stderr, "****** READ exit ONCE - %d \n", sourceDataLimit-sourceData);

      return 0;
    }
  
  fprintf(stderr, "****** READ CALLED ONCE - %d \n", sourceDataLimit-sourceData);

  result = UnicodeString(sourceData, sourceDataLimit-sourceData);
  
  // now, parse through the thing - watch for <>'s
  int32_t i = 0,j;
  int32_t len = result.length();
  int32_t sublen, oldsublen;
  const UChar kLeft = '<', kRight = '>';
  while(i<len)
    {
      if(inTag) // looking for '>'
        {
          j = result.indexOf(kRight,i);
          if(j == -1)
            {
              i = len;
              break; // rest of the line is in a tag
            }
          i = j+1;
          inTag = FALSE;
        }
      else // looking for '<'
        {
          j = result.indexOf(kLeft,i);
          if(j == -1)
            {
              trans->transliterate(result, i, len);
              i = len; // don't care what the real length is
              break;
            }
          
          // have a substring..
          //  i = start of string
          //  j = pos of '<' (also, limit of substring, exclusive)
          oldsublen = j-i;
          fprintf(stderr, "TRANS: %d/%d @ [%d,%d) -> ", oldsublen, len, i, j);

          j = trans->transliterate(result, i, j);
          sublen = j-i;
          len += (sublen-oldsublen); // update the length
          
          inTag = true;
          i = j; // start after the '<'
          fprintf(stderr, "%d/%d, i@%d\n", sublen, len, i);
        }
    }

  start = result.getUChars();
  end   = start + result.length();
  return result.length();
}
