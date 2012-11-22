
#include "ulibrk.h"
#include <stdio.h>
#include <unicode/ucharstriebuilder.h>
#include <unicode/ustdio.h>
#include "json.hxx"

#define SRCDIR "data/icusegments-ulijson"


static UBool debug = FALSE;

U_CAPI void ulibrk_install(UErrorCode &status) {
    if(U_FAILURE(status)) return;
    
    LocalPointer<ULISentenceBreakIterator> aBreak(new ULISentenceBreakIterator(Locale::getEnglish(),SRCDIR "/" "en" ".js", status));
    
    URegistryKey k = BreakIterator::registerInstance(aBreak.orphan(), Locale("en","","ULI",""), UBRK_SENTENCE, status);

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
        error = json_loadpath(j.getAlias(), fJSONSource);
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
            if(false) u_printf("Added: /%S/\n", ustr.getTerminatedBuffer());
        }
    } else {
        if(debug) fprintf(stderr, "Error %s loading json from %s\n", json_strerror(error), fJSONSource);
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
            if(debug) u_printf("/%C/ ya skip\n", (UChar)uch);
            UStringTrieResult r = USTRINGTRIE_INTERMEDIATE_VALUE;
            while((uch=utext_previous32(fText.getAlias()))!=U_SENTINEL  &&   // more to consume backwards and..
                  USTRINGTRIE_HAS_NEXT(r=fBackwardsTrie->nextForCodePoint(uch))) // more in the trie
                if(debug) u_printf("/%C/ cont?%d\n", (UChar)uch, r);
            
            if(USTRINGTRIE_MATCHES(r)) {  // matched - so,
                if(debug) u_printf("/%C/ matched, skip..%d\n", (UChar)uch, r);
                n = fDelegate->next(); // skip this one.
                if(n==UBRK_DONE) return n;

                continue; // See if the next is another exception.
            } else {
                return n; // No match - so exit. Not an exception.
            }
        } else {
            if(debug) u_printf("Not skipping for uch /%C/\n", (UChar)uch);
            return n; // No leading 'whitespace', so exit.
        }
    } while(n != UBRK_DONE);
    return n;
}

/// TEST

U_CAPI void ulibrk_test(void) {
    debug=TRUE;
    
    UErrorCode status = U_ZERO_ERROR;
    u_printf("Test. status=%s\n", u_errorName(status));
    ulibrk_install(status);
    u_printf("ulibrk_install returns %s\n", u_errorName(status));
    
    //LocalPointer<BreakIterator> brk = BreakIterator::getInstance
    UnicodeString ustr("For Mrs. Loomis, and young Mr. Loomis.","");
    //ustr = UnicodeString("Mr. Weston Mr. Weston");
    LocalPointer<BreakIterator> brk(BreakIterator::createSentenceInstance(Locale("en__ULI"),status));
    brk->setText(ustr);
    //LocalUBreakIteratorPointer brk(ubrk_open(UBRK_SENTENCE, "en__ULI", ustr.getTerminatedBuffer(), ustr.length(), &status));
    u_printf("Opened, status=%s\n", u_errorName(status));
    u_printf("String = /%S/\n", ustr.getTerminatedBuffer());
    
    int32_t prev = 0;
    for(int32_t n = 0; (n=brk->next())!=UBRK_DONE; ) {
        u_printf("%S/", ustr.tempSubString(prev,n-prev).getTerminatedBuffer());
        prev=n;
    }
    u_printf("%S$", ustr.tempSubString(prev,ustr.length()-prev).getTerminatedBuffer());
    u_printf("\n");
    return;
}
