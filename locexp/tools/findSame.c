/* Copyright (c) 2000 IBM, Inc.

   A tool that tells whether a given locale in locale explorer, has tags that are duplicates of the parent (root).
 
 This was designed to help in the analysis of local explorer translation. But maybe it'll be useful as a general resource bundle analyzer.

   These duplicates might be removed, or marked for translation.

   Example:

Couldn't find Version in root, only en      <-- Version was translated in en, not in root.. Reason: it wasn't exported to Root.

DUP: helpPrefix (same txt in root and en)  <-- Yes, helpPrefix will be the same. it's a 'marker' that proves that a locale is considered stable enough for locale explorer to use.

Example 2:
  change 'en' to 'fr'.. many strings are simply copied from the root, not translated. these should be either commented out or translated.

Example 3;
  change 'en' to 'sr'.. I've eliminated a few tags, the "Couldn't find XXX in root" tells you which ones have been removed from root..

TODO: write real command line opitons for this thing :)

*/

#include <stdio.h>
#include <ctype.h>            /* for isspace, etc.    */
#include <assert.h>
#include <string.h>


#include "unicode/utypes.h"
#include "unicode/ures.h"
#include "unicode/ustring.h"
#include "unicode/uchar.h"

#define U_ASSERT(x)  { if(U_FAILURE(x)) {fflush(stdout);fflush(stderr); fprintf(stderr, #x " == %s\n", u_errorName(x)); assert(U_SUCCESS(x)); }}

/* Print a UChar if possible, in seven characters. */
void prettyPrintUChar(UChar c)
{
  if(  (c <= 0x007F) &&
       (isgraph(c))  ) {
    printf("  '%c'  ", (char)(0x00FF&c));
  } else if ( c > 0x007F ) {
    char buf[1000];
    UErrorCode status = U_ZERO_ERROR;
    int32_t o;
    
    o = u_charName(c, U_UNICODE_CHAR_NAME, buf, 1000, &status);
    if(U_SUCCESS(status) && (o>0) ) {
      buf[6] = 0;
      printf("%- 7s", buf);
    } else {
      o = u_charName(c, U_UNICODE_10_CHAR_NAME, buf, 1000, &status);
      if(U_SUCCESS(status) && (o>0)) {
        buf[5] = 0;
        printf("~%- 6s", buf);
      }
      else {
        printf("??????? ");
      }
    }
  } else {
    switch((char)(c & 0x007F)) {
    case ' ':
      printf("  ' '  ");
      break;
    case '\t':
      printf("  \\t   ");
      break;
    case '\n':
      printf("  \\n   ");
      break;
    default:
      printf("   _   ");
      break;
    }
  }
}


void printUChars(const char  *name,
                 const UChar *uch,
                 int32_t     len)
{
  int32_t i;

  if( (len == -1) && (uch) ) {
    len = u_strlen(uch);
  }

  printf("% 5s:", name);
  for( i = 0; i <len; i++) {
    printf("%- 6d ", i);
  }
  printf("\n");

  printf("% 5s: ", "uni");
  for( i = 0; i <len; i++) {
    printf("\\u%04X ", (int)uch[i]);
  }
  printf("\n");

  printf("% 5s: ", "ch");
  for( i = 0; i <len; i++) {
    prettyPrintUChar(uch[i]);
  }
  printf("\n");
}

main()
{
  UErrorCode  err = U_ZERO_ERROR;
  
  UResourceBundle *root;
  UResourceBundle *x;
  const char *loc  = "en";
  const char *rootName = "root";
  int  n;
  int t;
  const UChar *s1, *s2;
  const char path[200];
  int len;
  UResourceBundle *sub = NULL;

  sprintf(path, "%s/locexp/", u_getDataDirectory());

  printf("Datapath=%s\n",path);
  printf("Comparing '%s' to '%s':\n", rootName, loc);

  root = ures_open(path, rootName, &err);
  U_ASSERT(err);

  err = U_ZERO_ERROR;
  x =  ures_open(path, loc, &err);
  U_ASSERT(err);
  
  if(err != U_ZERO_ERROR) 
    {
      printf("** warning: opened %s with %s!\n", loc, u_errorName(err));
    }
  
  t = ures_getSize(root);
  printf("root size=%d\n", t);
  t = ures_getSize(x);
  printf("%s size=%d\n", loc, t);

  ures_resetIterator(x);
  while(ures_hasNext(x))
    {
      sub = ures_getNextResource(x, sub, &err);
      U_ASSERT(err);
      
/*      printf("== %s\n", ures_getKey(sub)); */
      if(ures_getType(sub)==RES_STRING)
        {
          s1 = ures_getString(sub, &len, &err);
          U_ASSERT(err);
          s2 = ures_getStringByKey(root,ures_getKey(sub),&len,&err);
          if(U_FAILURE(err))
            {
              printf("Couldn't find %s in %s, only %s\n", ures_getKey(sub),
                     rootName, loc);
              err = U_ZERO_ERROR;
              continue;
            }
          U_ASSERT(err);

          if(u_strcmp(s1,s2)==0)
            {
              printf("DUP: %s (same txt in %s and %s)\n", ures_getKey(sub),
                     rootName, loc);
            }
        }
      U_ASSERT(err);
    }

  ures_close(x);
  ures_close(root);
  ures_close(sub);
}
