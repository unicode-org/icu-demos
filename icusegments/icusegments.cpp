/*
   Copyright (C) 1999-2014, International Business Machines
   Corporation and others.  All Rights Reserved.

    ICU Segments Demo
 */

#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

#include "unicode/utypes.h"
#include "unicode/ubrk.h"
#include "unicode/brkiter.h"
#include "unicode/ustring.h"
#include "unicode/uloc.h"
#include "unicode/localpointer.h"
#include "unicode/filteredbrk.h"

#include "demo_settings.h"
#include "demoutil.h"

//#include "ulibrk.h"


#include "json.hxx"

static const char *htmlHeader=
  "Content-Type: text/html; charset=utf-8\n"
  "\n"
  "<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Transitional//EN\" \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd\">\n"
  "<html xmlns=\"http://www.w3.org/1999/xhtml\" xml:lang=\"en\" lang=\"en\">\n"
  "<head>\n";

static const char endHeaderBeginBody[] =
  "</head>\n"
  "<body>\n";

static const char breadCrumbMainHeader[]=
  DEMO_BREAD_CRUMB_BAR
  "<h1>ICU Segments</h1>\n";

static const char defaultHeader[]=
  "<meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\">\n"
  "<title>ICU Segments (Error: can't load icusegments-header.html!)</title>\n";

static const char *htmlFooter=
  "</body>\n"
  "</html>";

/**
 *  path,  mimetype,
 *  path,  mimetype,
 *  NULL,  NULL
 */
const char *statics[] = {
  "/icusegments.js", "application/javascript",
  "/icusegments.css", "text/css",
  "/icusegments-dojo.js", "application/javascript",
  NULL, NULL
};

static void doJSON(const char *pi);
UErrorCode uliStatus = U_BRK_INTERNAL_ERROR;

int main(void)
{


  const char *rm = getenv("REQUEST_METHOD");
  // static files
  const char *pi = getenv("PATH_INFO");

  if(!rm) rm = "GET";

  if(!strcmp(rm,"GET") && pi && *pi) { /* static */
    if(!strcmp(pi, "/ulitest")) {
        printf("Content-type: text/plain\n\n");
        printf("N/A");
        //ulibrk_test();
        return 0;
    } else {
        if(serveStaticFile(statics, pi)) return 0;
        printf("Status: 404 Not Found\nContent-type: text/plain\n\nError: no file at this location.\n");
        return 0;
    }
  } else if(strcmp(rm,"POST")) { /* homepage */
    //const char *script=getenv("SCRIPT_NAME"); //"/cgi-bin/nbrowser"
    puts(htmlHeader);
    if (FALSE || !printHTMLFragment(NULL, NULL, DEMO_COMMON_DIR "icusegments-header.html")) {
      puts(defaultHeader);
    }
    puts(endHeaderBeginBody);
    if (printHTMLFragment(NULL, NULL, DEMO_COMMON_MASTHEAD)) {
      puts(DEMO_BEGIN_LEFT_NAV);
      printHTMLFragment(NULL, NULL, DEMO_COMMON_LEFTNAV "2");
      puts(DEMO_END_LEFT_NAV);
      puts(DEMO_BEGIN_CONTENT);
    }
    puts(breadCrumbMainHeader);

    if (!printHTMLFragment(NULL, NULL, DEMO_COMMON_DIR "icusegments-body.html")) {
      puts("<h1>Error: Could not load icusegments-body.html</h1>");
    }

    puts(DEMO_END_CONTENT);
    printHTMLFragment(NULL, NULL, DEMO_COMMON_FOOTER);
    puts(htmlFooter);

  } else {
    // POST.
    //uliStatus = U_ZERO_ERROR;
    //ulibrk_install(uliStatus);
    doJSON(pi);
  }

  return 0;
}


static int json_setUString(struct json *J, const UChar* s, const char *path) {
  char tmp[2048];
  UErrorCode status = U_ZERO_ERROR; // TODO check
  int err = json_setstring(J, u_strToUTF8(tmp, 2048, NULL, s, -1, &status), path);
    if(U_FAILURE(status)) return -1;
    return err;
}


static void doJSON(const char *pi) {
  int error = 0;
  UErrorCode status = U_ZERO_ERROR;

  // read input JSON
  json *ji = json_open(0, &error); // input
  json *j = json_open(0, &error); // output
  // for now - assume processes utf-8 ok?
  {
    char     inbuf[4096];
    int  inSize=-1;
    const char *contentLength = getenv("CONTENT_LENGTH");
    if(!contentLength||!*contentLength ||
       !sscanf(contentLength, "%d", &inSize) || inSize==0) {
      // no contentlength?!
      json_setnumber(ji, 0, "err.contentLength");
      json_setstring(j, "No ContentLength", "err.message");
    } else {
      int sizeLeft=inSize;
      int thisSize=-1;
        json_setnumber(j, inSize, "debug.contentLength");
      while(sizeLeft>0&&!feof(stdin)&&(thisSize=fread(inbuf,1,sizeof(inbuf)/sizeof(inbuf[0]),stdin))) {
        sizeLeft-=thisSize;
          json_setnumber(j, thisSize, "debug.lastReadOk");
          //if((error=json_parse(ji,inbuf,thisSize))) {   TODO: why doesn't json_parse work?
          if((error=json_loadlstring(ji,inbuf,thisSize))) {
            json_setnumber(ji, error, "err.reading");
            json_setnumber(j, error, "err.reading");
            char tmp[200];
            sprintf(tmp,"Error reading: %s at byte %d", json_strerror(error), inSize-sizeLeft);
            json_setstring(j, tmp, "err.message");
            json_setstring(ji, tmp, "err.message");
          break;
        }
      }
      if(error==0 && sizeLeft>0) {
          json_setnumber(ji, sizeLeft, "err.shortRead");
          json_setnumber(j, sizeLeft, "err.shortRead");
          json_setboolean(j, feof(stdin), "debug.feof");
        json_setstring(j, "Error short read", "err.message");
      }
    }
  }

  // now, the reply
  if(!strcmp(pi,"/version")) {
    json_setstring(j, U_ICU_VERSION, "icu.version");
    //json_setstring(j, u_errorName(uliStatus), "icu.ulistatus");
    // put a list of all break iterators available
    //int32_t a = ubrk_countAvailable();
    LocalPointer<StringEnumeration> availLocs(BreakIterator::getAvailableLocales());
    const char *s;
    for(int32_t i=0; (s=availLocs->next(NULL, status)); i++) {
      char tmp[200];

      sprintf(tmp, "brks.%s", s);
      UChar tmp2[2048] = {(UChar)0x0};
      uloc_getDisplayName(s, NULL, tmp2,2048,&status);
      json_setUString(j, tmp2, tmp);
    }
    // HACK - need a way to list these
    const char *uliLocs[] = { "de",
                              "en",
                              "es",
                              "fr",
                              "it",
                              "pt",
                              "ru",
                              NULL
    };
    for(int32_t i =0;uliLocs[i];i++) {
      char tmp[200];
      char s[200];
      sprintf(s, "%s__ULI", uliLocs[i]);
      sprintf(tmp, "brks.%s", s);
      UChar tmp2[2048] = {(UChar)0x0};
      uloc_getDisplayName(s, NULL, tmp2,2048,&status);
      json_setUString(j, tmp2, tmp);
    }


    // add types
    {
#define ADD_TYPE(x)   json_setstring(j, #x, "types.#", (int)  UBRK_ ## x)
      ADD_TYPE(CHARACTER);
      ADD_TYPE(WORD);
      ADD_TYPE(LINE);
      ADD_TYPE(SENTENCE);
#undef ADD_TYPE
    }
  } else if(!strcmp(pi,"/break")) {
      const char *bLocale = json_string(ji, "settings.bLocale");
      //const char *dLocale = json_string(ji, "settings.dLocale");
      const char *str     = json_string(ji, "param.str");
      const char *tstr =            json_string(ji, "settings.type"); // TODO: json_number failed us here.
      int type =-1;
      if(!sscanf(tstr, "%d", &type)) {
          type=-1;
      }

      //UChar ustr[2048];
      UErrorCode status = U_ZERO_ERROR;
      UnicodeString ustr(str, "utf-8");
      //LocalUTextPointer utxt(utext_openUTF8(NULL,str, -1, &status));

      json_setarray(j, "breaks.idx");
      int n = 0;
      int32_t next;

      //LocalUBreakIteratorPointer brk(ubrk_open((UBreakIteratorType)type, bLocale, NULL, 0, &status));
#if 1
      // Argh. Have to use C++.  Filed #9764
      LocalPointer<BreakIterator> brk;
      Locale bLocId(bLocale);
      // pointless duplication of ubrk.cpp
      switch(type) {
          case UBRK_CHARACTER:
              brk.adoptInstead(BreakIterator::createCharacterInstance(bLocId,status));
              break;
          case UBRK_WORD:
              brk.adoptInstead(BreakIterator::createWordInstance(bLocId,status));
              break;
          case UBRK_SENTENCE:
            brk.adoptInstead(BreakIterator::createSentenceInstance(bLocId,status));
            if(strcmp(bLocId.getVariant(),"ULI") == 0) {
              // TODO: using _ULI is a hack here anyways.
              // should include keywords here.
              // TODO: worse than that. _ULI is a territory..
              Locale locId(bLocId.getLanguage(), bLocId.getCountry(), NULL, NULL);
              LocalPointer<FilteredBreakIteratorBuilder> builder;
              builder.adoptInstead(FilteredBreakIteratorBuilder::createInstance(locId,
                                                                                status));
              fprintf(stderr, "%s -> %s\n", locId.getBaseName(), u_errorName(status));
              json_setstring(j, locId.getBaseName(), "_dbg.locid");
              json_setstring(j, u_errorName(status), "_dbg.status");
              brk.adoptInstead(builder->build(brk.orphan(), status)); // adopt filtered
            } else {
              json_setstring(j, bLocId.getCountry(), "_dbg.variant");
            }
            break;
          case UBRK_LINE:
              brk.adoptInstead(BreakIterator::createLineInstance(bLocId,status));
              break;
      }
      brk->setText(ustr);
      json_setnumber(j, brk->current(), "breaks.idx[#]", n++);
      while((next=brk->next())!=UBRK_DONE) {
          json_setnumber(j, next, "breaks.idx[#]", n++); // "next" would give us the UChar index.
      }
#else
      // Text comes in as UTF-8 - however, JavaScript will operate in UTF-16 (UCS-2?) chunks. So, go via UnicodeString
      LocalUBreakIteratorPointer brk(ubrk_open((UBreakIteratorType)type, bLocale, ustr.getTerminatedBuffer(), ustr.length(), &status));
      //ubrk_setUText(brk.getAlias(), utxt.getAlias(), &status);

      json_setnumber(j, ubrk_current(brk.getAlias()), "breaks.idx[#]", n++);
      //json_setnumber(j, utext_getNativeIndex(utxt.getAlias()), "breaks.idx[#]", n++); // ubrk_current would give us the UChar index.
      while((next=ubrk_next(brk.getAlias()))!=UBRK_DONE) {
          json_setnumber(j, next, "breaks.idx[#]", n++); // "next" would give us the UChar index.
          //json_setnumber(j, utext_getNativeIndex(utxt.getAlias()), "breaks.idx[#]", n++); // "next" would give us the UChar index.
      }
#endif

      //json_setnumber(j, utext_nativeLength(utxt.getAlias()), "debug.nativeLength");
      json_setnumber(j, strlen(str), "debug.strlen");

      json_setstring(j, bLocale, "debug.bLocale");
      json_setnumber(j, type, "debug.type");

      if(U_FAILURE(status)) {
          json_setstring(j, u_errorName(status), "err.message");
      }

  } else {
      json_setstring(j, "Unknown path!","err.message");
  }

  printf("Content-type: application/json\n");
  printf("\n");

  if((error=json_printfile(j, stdout, 0))) {
    printf(" /* ERROR %d */\n", error);
  }
  printf(" /* end JSON for %s */\n", pi);
}
