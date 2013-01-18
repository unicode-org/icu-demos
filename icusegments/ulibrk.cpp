
#include "ulibrk.h"
#include <stdio.h>
#include <unicode/ucharstriebuilder.h>
#include <unicode/ustdio.h>
#include "json.hxx"

#include "demo_config.h"  /* for HAVE_SYS_TYPES */

#if HAVE_SYS_TYPES_H
#include <sys/types.h>
#include <sys/dir.h>
#include <dirent.h>
#include <unicode/stringpiece.h>
#define USE_DIRENT
#endif

#include <string.h>
#include <strings.h>

#define SRCDIR "data/icusegments-ulijson"
#define JSON_SUFFIX ".json"
#define JSON_SUFFIXLEN 5

static UBool debug = FALSE;
static UBool debug2 = FALSE;

U_CAPI void ulibrk_install(UErrorCode &status) {
  if(debug) fprintf(stderr, "Installing ulibrk_install - err=%s\n", u_errorName(status));

    if(U_FAILURE(status)) return;
    
    
#ifdef USE_DIRENT
    // TODO: store key in a map
    DIR *dir = opendir(SRCDIR);
    if(!dir) {
        status = U_MISSING_RESOURCE_ERROR;
        if(debug) fprintf(stderr, "Could not opendir %s\n", SRCDIR);
    } else {
        struct dirent *ent;
        while((ent=readdir(dir))!=NULL) {
		int namlen = strlen(ent->d_name);
            if(!strncmp(ent->d_name+namlen-JSON_SUFFIXLEN,JSON_SUFFIX,JSON_SUFFIXLEN)) {
                char locname[200];
                char pathname[200];
                strcpy(pathname,SRCDIR "/");
                strcat(pathname,ent->d_name);
                strncpy(locname,ent->d_name,namlen-JSON_SUFFIXLEN);
                locname[namlen-JSON_SUFFIXLEN]=0;
                Locale loc(locname);
                if(debug) fprintf(stderr, "About to create loc [%s] from path [%s]\n", loc.getName(), pathname);
                LocalPointer<ULISentenceBreakIterator>
                    aBreak(new ULISentenceBreakIterator(loc,
                                                        pathname, status));
                if(!*(loc.getCountry())) {
                    strcat(locname,"_");
                }
                strcat(locname,"_ULI");
                Locale loc2(locname);
                URegistryKey k = BreakIterator::registerInstance(aBreak.orphan(), loc2, UBRK_SENTENCE, status);
                if(debug) fprintf(stderr, "Registered: %s\n", loc2.getName());
            }
        }
        closedir(dir);
    }
#else
    /* non dirent - for testing. */
    {
        LocalPointer<ULISentenceBreakIterator> aBreak(new ULISentenceBreakIterator(Locale::getEnglish(),SRCDIR "/" "en" JSON_SUFFIX, status));
        URegistryKey k = BreakIterator::registerInstance(aBreak.orphan(), Locale("en","","ULI",""), UBRK_SENTENCE, status);
    }
    {
        LocalPointer<ULISentenceBreakIterator> aBreak(new ULISentenceBreakIterator(Locale("de"),SRCDIR "/" "de" JSON_SUFFIX, status));
        URegistryKey k = BreakIterator::registerInstance(aBreak.orphan(), Locale("de","","ULI",""), UBRK_SENTENCE, status);
    }
 #endif
    // fprintf(stderr, "Installed w/ registrykey %x\n", (long)k);
    
}

ULISentenceBreakIterator::ULISentenceBreakIterator(const ULISentenceBreakIterator& other)
: BreakIterator(other),fDelegate(other.fDelegate->clone()),fBackwardsTrie(), fJSONSource(other.fJSONSource) {
    UErrorCode amIBogus = U_ZERO_ERROR;
    build(amIBogus);
}

ULISentenceBreakIterator::ULISentenceBreakIterator(const Locale& where, const char *jsonSource, UErrorCode &status) :
fDelegate(BreakIterator::createSentenceInstance(where,status)),
fBackwardsTrie(),
fJSONSource(jsonSource) {
    // sit on my hands with the locale.  getLocale is not virtual and no way to set the parent fields. (filed a bug)
    
    build(status);
    
    if(debug) fprintf(stderr, "Builder: built[%s] from %s, status %s\n", where.getName(), jsonSource, u_errorName(status));
    
}

void ULISentenceBreakIterator::build(UErrorCode &status) {
    LocalPointer<UCharsTrieBuilder> builder(new UCharsTrieBuilder(status));
    
#if 0 /* testing */
    builder->add(UnicodeString(".rM",""), 1, status);
    builder->add(UnicodeString(".srM",""), 1, status);
#endif

    int error = 0;
    LocalJSONPointer j(json_open(0, &error));

    if(!error) {
        error = json_loadpath(j.getAlias(), fJSONSource.c_str());
    }
    
    if(!error) {
        int subCount = json_count(j.getAlias(),"data.abbrs");
        if(false) fprintf(stderr, "Root count: %d  [0] count: %d\n", json_count(j.getAlias(),""),  subCount);
        for(int i=0;i<subCount;i++) {
            const char *abbr = json_string(j.getAlias(), "data.abbrs[#]", i);
            if(false) fprintf(stderr, "[%d] = '%s'\n", i, abbr);
            UnicodeString ustr(abbr,"utf-8");
            // add to forward table
            
            ustr.reverse();
            builder->add(ustr,1,status);
            //if(debug) u_printf("Added: /%S/\n", ustr.getTerminatedBuffer());
        }
        if(debug) u_printf(" %s has %d abbrs.\n", fJSONSource.c_str(), subCount);
    } else {
        if(debug) fprintf(stderr, "Error %s loading json from %s\n", json_strerror(error), fJSONSource.c_str());
    }
    
    fBackwardsTrie.adoptInstead(builder->build(USTRINGTRIE_BUILD_FAST, status));
}

int32_t ULISentenceBreakIterator::next() {
    int32_t n = fDelegate->next();
    if(n == UBRK_DONE) return n;
    // OK, do we need to break here?
    UErrorCode status = U_ZERO_ERROR;
    // refresh text
    fText.adoptInstead(fDelegate->getUText(fText.orphan(), status));
    do {
        utext_setNativeIndex(fText.getAlias(), n); // from n..
        fBackwardsTrie->reset();
        UChar32 uch;
        
        if((uch=utext_previous32(fText.getAlias()))==(UChar32)0x0020) {  // TODO: skip a class of chars here??
            if(debug2) u_printf("/%C/ ya skip\n", (UChar)uch);
            UStringTrieResult r = USTRINGTRIE_INTERMEDIATE_VALUE;
            while((uch=utext_previous32(fText.getAlias()))!=U_SENTINEL  &&   // more to consume backwards and..
                  USTRINGTRIE_HAS_NEXT(r=fBackwardsTrie->nextForCodePoint(uch))) // more in the trie
                if(debug2) u_printf("/%C/ cont?%d\n", (UChar)uch, r);
            
            if(USTRINGTRIE_MATCHES(r)) {  // matched - so,
                if(debug2) u_printf("/%C/ matched, skip..%d\n", (UChar)uch, r);
                n = fDelegate->next(); // skip this one.
                if(n==UBRK_DONE) return n;

                continue; // See if the next is another exception.
            } else {
                return n; // No match - so exit. Not an exception.
            }
        } else {
            if(debug2) u_printf("Not skipping for uch /%C/\n", (UChar)uch);
            return n; // No leading 'whitespace', so exit.
        }
    } while(n != UBRK_DONE);
    return n;
}

/// TEST

static void prtbrks(BreakIterator* brk, UnicodeString &ustr) {
    u_printf("String = /%S/\n", ustr.getTerminatedBuffer());
    
    int32_t prev = 0;
    for(int32_t n = 0; (n=brk->next())!=UBRK_DONE; ) {
        u_printf("%S/", ustr.tempSubString(prev,n-prev).getTerminatedBuffer());
        prev=n;
    }
    u_printf("%S$", ustr.tempSubString(prev,ustr.length()-prev).getTerminatedBuffer());
    u_printf("\n");
}

U_CAPI void ulibrk_test(void) {
    debug=TRUE;
    
    UErrorCode status = U_ZERO_ERROR;
    u_printf("Test. status=%s\n", u_errorName(status));
    ulibrk_install(status);
    u_printf("ulibrk_install returns %s\n", u_errorName(status));
    
    
#if 1
    {
        u_printf("Testing English...\n");
        UnicodeString ustr("For Mrs. Loomis, and young Mr. Loomis. Another sentence.","");
        LocalPointer<BreakIterator> brk(BreakIterator::createSentenceInstance(Locale("en__ULI"),status));
        brk->setText(ustr);
        //LocalUBreakIteratorPointer brk(ubrk_open(UBRK_SENTENCE, "en__ULI", ustr.getTerminatedBuffer(), ustr.length(), &status));
        u_printf("Opened, status=%s\n", u_errorName(status));
        prtbrks(brk.getAlias(),ustr);
    }
    
    {
        u_printf("Testing 'German'...\n");
        UnicodeString ustr("For Hrn. Loomis, und young Hr. Loomis. Another sentence.","");
        LocalPointer<BreakIterator> brk(BreakIterator::createSentenceInstance(Locale("de__ULI"),status));
        brk->setText(ustr);
        u_printf("Opened, status=%s\n", u_errorName(status));
        prtbrks(brk.getAlias(),ustr);
    }
#endif
    {
        u_printf("Testing 'Russian'...\n");
        UChar str[2048];
        u_unescape("Hello \\u043f\\u0440\\u043E\\u0444. Loomis. Another sentence.",str,2048);
        UnicodeString ustr(str);
        LocalPointer<BreakIterator> brk(BreakIterator::createSentenceInstance(Locale("ru__ULI"),status));
        brk->setText(ustr);
        u_printf("Opened, status=%s\n", u_errorName(status));
        prtbrks(brk.getAlias(),ustr);
    }
    
    return;
}
