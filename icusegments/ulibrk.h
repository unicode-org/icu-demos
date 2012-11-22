
#ifndef ULIBRK_H
#define ULIBRK_H

#include <unicode/brkiter.h>
#include <unicode/localpointer.h>
#include <unicode/ucharstrie.h>

U_CAPI void ulibrk_install(UErrorCode &status);
U_CAPI void ulibrk_test(void);



class ULISentenceBreakIterator : public BreakIterator {
public:
    ULISentenceBreakIterator(const Locale& where, const char *jsonSource, UErrorCode &status);
    virtual ~ULISentenceBreakIterator() {}
    ULISentenceBreakIterator(const ULISentenceBreakIterator& other);
private:
    LocalPointer<BreakIterator> fDelegate;
    LocalUTextPointer           fText;
    LocalPointer<UCharsTrie>    fBackwardsTrie;
    const char *fJSONSource;
    
    void                        build(UErrorCode& status);
/* -- subclass interface -- */
public:
    /* -- cloning and other subclass stuff -- */
    virtual BreakIterator *  createBufferClone(void *stackBuffer,
                                               int32_t &BufferSize,
                                               UErrorCode &status) {
        
        // TODO fix.
        status = U_SAFECLONE_ALLOCATED_WARNING;
        // punt.
        return clone();
    }
    virtual BreakIterator* clone(void) const { return new ULISentenceBreakIterator(*this); }
    virtual UClassID getDynamicClassID(void) const { return NULL; }
    virtual UBool operator==(const BreakIterator& o) const { if(*this==o) return true; return false; }
    

    /* -- text modifying -- */
    virtual void setText(UText *text, UErrorCode &status) { fDelegate->setText(text,status); }
    virtual BreakIterator &refreshInputText(UText *input, UErrorCode &status) { fDelegate->refreshInputText(input,status); return *this; }
    virtual void adoptText(CharacterIterator* it) { fDelegate->adoptText(it); }
    virtual void setText(const UnicodeString &text) { fDelegate->setText(text); }
    

    /* -- other functions that are just delegated -- */
    virtual UText *getUText(UText *fillIn, UErrorCode &status) const { return fDelegate->getUText(fillIn,status); }
    virtual CharacterIterator& getText(void) const { return fDelegate->getText(); }
    

    /* -- ITERATION -- */
    virtual int32_t first(void) { return fDelegate->first(); }
    virtual int32_t preceding(int32_t offset) { return fDelegate->preceding(offset); }
    virtual int32_t previous(void) { return fDelegate->previous(); }
    virtual UBool isBoundary(int32_t offset) { return fDelegate->isBoundary(offset); }
    virtual int32_t current(void) const { return fDelegate->current(); }

    virtual int32_t next(void);

    virtual int32_t next(int32_t n) { return fDelegate->next(n); }
    virtual int32_t following(int32_t offset) { return fDelegate->following(offset); }
    virtual int32_t last(void) { return fDelegate->last(); }
 
};



#endif
