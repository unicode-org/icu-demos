/*
   usort - library to perform locale-sensitive sorting of text lines
   using the usort library.

   Copyright (c) 1999 IBM, Inc.

   for the IBM Classes for Unicode
   <http://www10.software.ibm.com/developerworks/opensource/icu> 


   Steven R. Loomis <srl@monkey.sbay.org>

   Usage: 
      mySort =  usort_open()..
      usort_addLinesFromFile() or usort_addLine() ..
      usort_sort() ..

        for(i=0;i< mySort->count;i++)
           doSomethingWithLine(mySort->lines[i]);
      OR
        usort_print()..

      usort_close() ..

   Future Features:
      allow use of a custom sort algorithm ?
   
*/

#ifndef USORT_H
#define USORT_H

/*Deals with imports and exports of the dynamic library*/
#ifdef _WIN32
    #define T_USORT_EXPORT __declspec(dllexport)
    #define T_USORT_IMPORT __declspec(dllimport)
#else
    #define T_USORT_EXPORT
    #define T_USORT_IMPORT
#endif

#ifdef __cplusplus
    #define C_USORT_API extern "C"
#else
    #define C_USORT_API
#endif 

#ifdef T_USORT_IMPLEMENTATION
    #define T_USORT_API C_USORT_API  T_USORT_EXPORT
    #define T_USORT_EXPORT_API T_USORT_EXPORT
#else
    #define T_USORT_API C_USORT_API  T_USORT_IMPORT
    #define T_USORT_EXPORT_API T_USORT_IMPORT
#endif




#include <utypes.h>
#include <ucol.h>
#include <ucnv.h>
#include <uloc.h>
#include <stdio.h>  /* for FILE*'s */

/**
 * This structure defines a 'line' (row) of text to be sorted 
 */

typedef struct
{
  uint8_t     *key;     /* Opaque key for this line. 0wned. */
  int32_t      keySize; /* size of above struct */
  const UChar *chars;   /* Null terminated string. Can be 0wned. */
  void        *userData;/* User data */
} USortLine;

/**
 * Structure for a list of lines to be sorted.
 */

typedef struct
{
  USortLine *lines;     /* the list of lines to be sorted. 0wned. */
  int32_t    size;      /* the size of the list */
  int32_t    count;     /* the # of actual lines */
  bool_t     ownsText;     /* True of the lineList owns the chars. */
  UCollator *collator;     /* Collator for text. 0wned.  */
} USort;

/**
 * Open a USort object.
 * @param locale Locale to use. NULL for default
 * @param strength Collation strength. UCOL_DEFAULT is a good default.
 * @param ownText TRUE if this object should own its' text.
 * @param status Error code. [values?]
 */

T_USORT_API USort*
usort_open(const char *locale, UCollationStrength strength, bool_t ownText,
           UErrorCode *status);

/**
 * Close a USort object. If the ownsText was set, then delete the
 * 0wned text.
 * @param usort The USort to be closed.
 */

T_USORT_API void
usort_close(USort *usort);

/**
 * Add a line of text to the list.
 * if ownsText was FALSE, then the text will be copied. 
 * Note: text MUST be null terminated if it's not being copied !
 * @param usort The sort object to add to
 * @param line Text to be added. [Typically, will not end with CR or
 *  LF]
 * @param len Length of text passed in. -1 if null terminated.
 * @param copy TRUE if the text should be copied. Text WILL be copied
 *   if len is not -1. 
 * @param userData Your additional reference data can go here. Pass in NULL by default.
 */

T_USORT_API void
usort_addLine(USort *usort, const UChar *line, int32_t len, bool_t copy, void *userData);

/**
 * Add a bunch of lines to the list from a FILE, converting as 
 * necessary. Break lines at LF.  [todo: use linebreakiterator]
 * [convenience]
 * @param usort the sort object to add to 
 * @param file FILE to read data from. Will read until EOF.
 * @param inConverter Converter to use to convert data in the FILE. If NULL, the converter specified by ucnv_getDefaultName() will be used.
 * @param escapeMode If TRUE, sequences such as \u0308 will be recognized.
 */

T_USORT_API void
usort_addLinesFromFILE( USort *sort, FILE *file, UConverter *inConverter, bool_t escapeMode);

/**
 * Do the final sort. Finally!!
 * @param usort The items to be sorted
 */

T_USORT_API void 
usort_sort(USort *usort);

/**
 * Print out the contents of the sorted list to some FILE
 * [convenience]
 * @param usort The sorted list to be printed
 * @param file FILE to print to. NULL for stdout.
 * @param toConverter Converter to use for the output. NULL to use ucnv_getDefaultName() 
 */

T_USORT_API void
usort_printToFILE(USort *usort, FILE *file, UConverter *toConverter);

#endif /* _USORT */






