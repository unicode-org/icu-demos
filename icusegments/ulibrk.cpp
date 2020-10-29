/**
 * Copyright (C) 2012-2013 IBM Corporation and Others. All Rights Reserved.
 */

#include "ulibrk.h"
#include <stdio.h>
#include <unicode/ucharstriebuilder.h>
#include <unicode/ustdio.h>
#include "json.hxx"

#include <stdlib.h>

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

// values for backwardsTrie.getValue
#define kPARTIAL (1<<0) //< partial - need to run through forward trie
#define kMATCH (1<<1) //< exact match - skip this one.

static UBool debug = false;
static UBool debug2 = false;

U_CAPI void ulibrk_install(UErrorCode &status) {
  if(debug) u_printf("Installing ulibrk_install - err=%s\n", u_errorName(status));

    if(U_FAILURE(status)) return;
    
    
#ifdef USE_DIRENT
    // TODO: store key in a map
    DIR *dir = opendir(SRCDIR);
    if(!dir) {
        status = U_MISSING_RESOURCE_ERROR;
        if(debug) u_printf("Could not opendir %s\n", SRCDIR);
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
                if(debug) u_printf("About to create loc [%s] from path [%s]\n", loc.getName(), pathname);
                LocalPointer<ULISentenceBreakIterator>
                    aBreak(new ULISentenceBreakIterator(loc,
                                                        pathname, status));
                if(!*(loc.getCountry())) {
                    strcat(locname,"_");
                }
                strcat(locname,"_ULI");
                Locale loc2(locname);
                /*URegistryKey k = */ BreakIterator::registerInstance(aBreak.orphan(), loc2, UBRK_SENTENCE, status);
                if(debug) u_printf("Registered: %s\n", loc2.getName());
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
    // u_printf("Installed w/ registrykey %x\n", (long)k);
    
}

ULISentenceBreakIterator::ULISentenceBreakIterator(const ULISentenceBreakIterator& other)
  : BreakIterator(other),fDelegate(other.fDelegate->clone()),fBackwardsTrie(), fForwardsPartialTrie(), fJSONSource(other.fJSONSource) {
    UErrorCode amIBogus = U_ZERO_ERROR;
    build(amIBogus);
}

ULISentenceBreakIterator::ULISentenceBreakIterator(const Locale& where, const char *jsonSource, UErrorCode &status) :
fDelegate(BreakIterator::createSentenceInstance(where,status)),
fBackwardsTrie(),
fForwardsPartialTrie(),
fJSONSource(jsonSource) {
    // sit on my hands with the locale.  getLocale is not virtual and no way to set the parent fields. (filed a bug)
    
    build(status);
    
    if(debug) u_printf("Builder: built[%s] from %s, status %s\n", where.getName(), jsonSource, u_errorName(status));
    
}

void ULISentenceBreakIterator::build(UErrorCode &status) {
    LocalPointer<UCharsTrieBuilder> builder(new UCharsTrieBuilder(status));
    LocalPointer<UCharsTrieBuilder> builder2(new UCharsTrieBuilder(status));
    
    int error = 0;
    LocalJSONPointer j(json_open(0, &error));

    int revCount = 0;
    int fwdCount = 0;

    if(!error) {
        error = json_loadpath(j.getAlias(), fJSONSource.c_str());
    }
    
    if(!error) {
      const int kSuppressInReverse  =  1 << 0;
      const int kAddToForward = 1 << 1;

        int subCount = json_count(j.getAlias(),"data.abbrs");
        if(false) u_printf("Root count: %d  [0] count: %d\n", json_count(j.getAlias(),""),  subCount);
        UnicodeString *ustrs = new UnicodeString[subCount];
        int *partials = new int[subCount];
        for(int i=0;i<subCount;i++) {
            const char *abbr = json_string(j.getAlias(), "data.abbrs[#]", i);
            if(false) u_printf("[%d] = '%s'\n", i, abbr);
            ustrs[i] = UnicodeString(abbr,"utf-8");
            partials[i] = 0; // default: not partial
        }
        // first pass - find partials.
        for(int i=0;i<subCount;i++) {
          int nn = ustrs[i].indexOf('.'); // TODO: U+value
          if(nn>-1 && (nn+1)!=ustrs[i].length()) {
            //if(true) u_printf("Is a partial: /%S/\n", ustrs[i].getTerminatedBuffer());
            // is partial.
            // is it unique?
            int sameAs = -1;
            for(int j=0;j<subCount;j++) {
              if(j==i) continue;
              if(ustrs[i].compare(0,nn+1,ustrs[j],0,nn+1)==0) {
                //if(true) u_printf("Prefix match: /%S/ to %d\n", ustrs[j].getTerminatedBuffer(), nn+1);
                //UBool otherIsPartial = ((nn+1)!=ustrs[j].length());  // true if ustrs[j] doesn't end at nn
                if(partials[j]==0) { // hasn't been processed yet
                  partials[j] = kSuppressInReverse | kAddToForward;
                  //if(true) u_printf("Suppressing: /%S/\n", ustrs[j].getTerminatedBuffer());
                } else if(partials[j] & kSuppressInReverse) {
                  sameAs = j; // the other entry is already in the reverse table.
                }
              }
            }
            if(debug2) u_printf("for partial /%S/ same=%d partials=%d\n", 
                     ustrs[i].getTerminatedBuffer(), sameAs, partials[i]);
            UnicodeString prefix(ustrs[i], 0, nn+1);
            if(sameAs == -1 && partials[i] == 0) {
              // first one - add the prefix to the reverse table. 
              prefix.reverse();
              builder->add(prefix, kPARTIAL, status);
              revCount++;
              if(debug2) u_printf("Added Partial: /%S/ from /%S/ status=%s\n", prefix.getTerminatedBuffer(), ustrs[i].getTerminatedBuffer(), u_errorName(status));
              partials[i] = kSuppressInReverse | kAddToForward;
            } else {
              if(debug2) u_printf(" // not adding partial for /%S/ from /%S/\n", prefix.getTerminatedBuffer(), ustrs[i].getTerminatedBuffer());
            }
          }
        }
        for(int i=0;i<subCount;i++) { 
         if(partials[i]==0) {
            ustrs[i].reverse();
            builder->add(ustrs[i], kMATCH, status);
            revCount++;
            if(debug2) u_printf("Added: /%S/ status=%s\n", ustrs[i].getTerminatedBuffer(), u_errorName(status));
          } else {
           if(debug2) u_printf(" Adding fwd: /%S/\n", ustrs[i].getTerminatedBuffer());

           // an optimization would be to only add the portion after the '.'
           // for example, for "Ph.D." we store ".hP" in the reverse table. We could just store "D." in the forward, 
           // instead of "Ph.D." since we already know the "Ph." part is a match.
           // would need the trie to be able to hold 0-length strings, though.
           builder2->add(ustrs[i], kMATCH, status); // forward
           fwdCount++;
           //ustrs[i].reverse();
           //if(debug2) u_printf("SUPPRESS- not Added(%d):  /%S/ status=%s\n",partials[i], ustrs[i].getTerminatedBuffer(), u_errorName(status));
          }
        }
        if(debug) u_printf(" %s has %d abbrs.\n", fJSONSource.c_str(), subCount);
        delete [] ustrs;
        delete [] partials;
    } else {
        if(debug) u_printf("Error %s loading json from %s\n", json_strerror(error), fJSONSource.c_str());
        return;
    }
    
    if(revCount>0) {
      fBackwardsTrie.adoptInstead(builder->build(USTRINGTRIE_BUILD_FAST, status));
      if(U_FAILURE(status)) {
        if(debug) u_printf("Error %s building backwards\n", u_errorName(status));
        return;
      }
    }

    if(fwdCount>0) { 
      fForwardsPartialTrie.adoptInstead(builder2->build(USTRINGTRIE_BUILD_FAST, status));
      if(U_FAILURE(status)) {
        if(debug) u_printf("Error %s building forwards\n", u_errorName(status));
        return;
      }
    }
}

int32_t ULISentenceBreakIterator::next() {
    int32_t n = fDelegate->next();
    if(n == UBRK_DONE || // at end  or
       fBackwardsTrie.isNull()) { // .. no backwards table loaded == no exceptions
      return n;
    }
    // OK, do we need to break here?
    UErrorCode status = U_ZERO_ERROR;
    // refresh text
    fText.adoptInstead(fDelegate->getUText(fText.orphan(), status));
    if(debug2) u_printf("str, native len=%d\n", utext_nativeLength(fText.getAlias()));
    do { // outer loop runs once per underlying break (from fDelegate).
         // loops while 'n' points to an exception.
        utext_setNativeIndex(fText.getAlias(), n); // from n..
        fBackwardsTrie->reset();
        UChar32 uch;
        if(debug2) u_printf(" n@ %d\n", n);
        // Assume a space is following the '.'  (so we handle the case:  "Mr. /Brown")
        if((uch=utext_previous32(fText.getAlias()))==(UChar32)0x0020) {  // TODO: skip a class of chars here??
          // TODO only do this the 1st time?
          if(debug2) u_printf("skipping prev: |%C| \n", (UChar)uch);
        } else {
          if(debug2) u_printf("not skipping prev: |%C| \n", (UChar)uch);   
          uch = utext_next32(fText.getAlias());
          if(debug2) u_printf(" -> : |%C| \n", (UChar)uch);   
        }
        UStringTrieResult r = USTRINGTRIE_INTERMEDIATE_VALUE;

        int32_t bestPosn = -1;
        int32_t bestValue = -1;

        while((uch=utext_previous32(fText.getAlias()))!=U_SENTINEL  &&   // more to consume backwards and..
              USTRINGTRIE_HAS_NEXT(r=fBackwardsTrie->nextForCodePoint(uch))) {// more in the trie
          if(USTRINGTRIE_HAS_VALUE(r)) { // remember the best match so far
            bestPosn = utext_getNativeIndex(fText.getAlias());
            bestValue = fBackwardsTrie->getValue();
          }
          if(debug2) u_printf("rev< /%C/ cont?%d @%d\n", (UChar)uch, r, utext_getNativeIndex(fText.getAlias()));
        }

        if(USTRINGTRIE_MATCHES(r)) { // exact match?
          if(debug2) u_printf("rev<?/%C/?end of seq.. r=%d, bestPosn=%d, bestValue=%d\n", (UChar)uch, r, bestPosn, bestValue);
          bestValue = fBackwardsTrie->getValue();
          bestPosn = utext_getNativeIndex(fText.getAlias());
          if(debug2) u_printf("rev<+/%C/+end of seq.. r=%d, bestPosn=%d, bestValue=%d\n", (UChar)uch, r, bestPosn, bestValue);
        }

        if(bestPosn>=0) {
          if(debug2) u_printf("rev< /%C/ end of seq.. r=%d, bestPosn=%d, bestValue=%d\n", (UChar)uch, r, bestPosn, bestValue);
          
        //if(USTRINGTRIE_MATCHES(r)) {  // matched - so, now what?
        //int32_t bestValue = fBackwardsTrie->getValue();
        //if(debug2) u_printf("rev< /%C/ matched, skip..%d  bestValue=%d\n", (UChar)uch, r, bestValue);

          if(bestValue == kMATCH) { // exact match!
            if(debug2) u_printf(" exact backward match\n");
            n = fDelegate->next(); // skip this one. Find the next lowerlevel break.
            if(n==UBRK_DONE) return n;
            continue; // See if the next is another exception.
          } else if(bestValue == kPARTIAL 
                    && fForwardsPartialTrie.isValid()) { // make sure there's a forward trie
            if(debug2) u_printf(" partial backward match\n");
            // We matched the "Ph." in "Ph.D." - now we need to run everything through the forwards trie
            // to see if it matches something going forward.
            fForwardsPartialTrie->reset();
            UStringTrieResult rfwd = USTRINGTRIE_INTERMEDIATE_VALUE;
            utext_setNativeIndex(fText.getAlias(), bestPosn); // hope that's close .. 
            if(debug2) u_printf("Retrying at %d\n", bestPosn);
            while((uch=utext_next32(fText.getAlias()))!=U_SENTINEL &&
                  USTRINGTRIE_HAS_NEXT(rfwd=fForwardsPartialTrie->nextForCodePoint(uch))) {
              if(debug2) u_printf("fwd> /%C/ cont?%d @%d\n", (UChar)uch, rfwd, utext_getNativeIndex(fText.getAlias()));
            }
            if(USTRINGTRIE_MATCHES(rfwd)) {
              if(debug2) u_printf("fwd> /%C/ == forward match!\n", (UChar)uch);
              // only full matches here, nothing to check
              // skip the next:
              n = fDelegate->next();
              if(n==UBRK_DONE) return n;
              continue;
            } else {
              if(debug2) u_printf("fwd> /%C/ no match.\n", (UChar)uch);
              // no match (no exception) -return the 'underlying' break
              return n;
            }
          } else {
            return n; // internal error and/or no forwards trie
          }
        } else {
          if(debug2) u_printf("rev< /%C/ .. no match..%d\n", (UChar)uch, r);  // no best match
          return n; // No match - so exit. Not an exception.
        }
    } while(n != UBRK_DONE);
    return n;
}

/// TEST

static const UChar PILCROW=0x00B6, CHSTR=0x3010, CHEND=0x3011; // lenticular brackets

// TODO: does not handle supp chars
static void prtbrks(BreakIterator* brk, UnicodeString &ustr) {
    u_printf("String = /%S/\n", ustr.getTerminatedBuffer());
    
    int32_t *pos = new int32_t[ustr.length()];
    uint32_t posCount = 0;

    // calculate breaks up front, so we can print out 
    // sans any debugging 
    for(int32_t n = 0; (n=brk->next())!=UBRK_DONE; ) {
      pos[posCount++] = n;
      if(posCount>=ustr.length()) {
        puts("brk count exceeds string length!");
        abort();
      }
    }
    
    u_printf("%C", CHSTR);
    int32_t prev = 0;
    for(int32_t i=0;i<posCount;i++) {
      int32_t n=pos[i];
      u_printf("%S%C", ustr.tempSubString(prev,n-prev).getTerminatedBuffer(), PILCROW);
      prev=n;
    }
    u_printf("%S%C", ustr.tempSubString(prev,ustr.length()-prev).getTerminatedBuffer(), CHEND);
    u_printf("\n");
    for(int32_t i=0;i<posCount;i++) {
      u_printf("%d ",pos[i]);
    }
    u_printf("\n");
    delete [] pos;
}

U_CAPI void ulibrk_test(void) {
    debug=true;
    
    UErrorCode status = U_ZERO_ERROR;
    u_printf("Test. status=%s\n", u_errorName(status));
    ulibrk_install(status);
    u_printf("ulibrk_install returns %s\n", u_errorName(status));
    
    
#if 1
    {
        u_printf("Testing English...\n");
        UnicodeString ustr("For Mrs. Loomis, and young Mr. Loomis. Another early 2013 A.D. sentence. The Ph.D. is waiting in room D. And the hinges made this sound: Ph. Not plugh, just Ph.","");
        LocalPointer<BreakIterator> brk(BreakIterator::createSentenceInstance(Locale("en__ULI"),status));
        brk->setText(ustr);
        //LocalUBreakIteratorPointer brk(ubrk_open(UBRK_SENTENCE, "en__ULI", ustr.getTerminatedBuffer(), ustr.length(), &status));
        u_printf("Opened, status=%s\n", u_errorName(status));
        prtbrks(brk.getAlias(),ustr);
    }
#endif

#if 1
    {
        u_printf("Testing 'German'...\n");
        UnicodeString ustr("For Hrn. Loomis, und young Hr. Loomis. Ich sah Frankfurt a.M.","");
        LocalPointer<BreakIterator> brk(BreakIterator::createSentenceInstance(Locale("de__ULI"),status));
        brk->setText(ustr);
        u_printf("Opened, status=%s\n", u_errorName(status));
        prtbrks(brk.getAlias(),ustr);
    }
#endif
#if 1
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
    
#endif
    return;
}
