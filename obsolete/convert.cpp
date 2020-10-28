/*
**********************************************************************
*   Copyright (C) 1998-2006, International Business Machines
*   Corporation and others.  All Rights Reserved.
**********************************************************************
*/
/* C++ wrappers for the ICUs Codeset Conversion Routines*/

#include "unicode/utypes.h"

U_NAMESPACE_BEGIN

class Locale;
class UnicodeString;
class Mutex;

U_NAMESPACE_END

#include "unicode/resbund.h"
#include "cmemory.h"
#include "cstring.h"
#include "mutex.h"
#include "ucnv_io.h"
#include "unicode/ucnv_err.h"
#include "ucnv_bld.h"
#include "unicode/ucnv.h"
#include "unicode/convert.h"
#include "ucln_cmn.h"


/* list of converter and alias names */
static const char **availableConverterNames=NULL;
static int32_t availableConverterNamesCount=0;

UBool UnicodeConverter_cleanup()
{
    if (availableConverterNames)
    {
        for (int32_t i=0; i<availableConverterNamesCount; ++i) {
            uprv_free((char*) availableConverterNames[i]);
        }
        uprv_free(availableConverterNames);
        availableConverterNames = NULL;
    }
    availableConverterNamesCount = 0;
    return true;
}

U_NAMESPACE_BEGIN

const char UnicodeConverter::fgClassID=0;

UnicodeConverter::UnicodeConverter()
    : UObject()
{
    UErrorCode err = U_ZERO_ERROR;
    myUnicodeConverter = ucnv_open(NULL, &err);
}
UnicodeConverter::UnicodeConverter(const char* name, UErrorCode& err)
    : UObject()
{
    myUnicodeConverter = ucnv_open(name, &err);
}

UnicodeConverter::UnicodeConverter(const UnicodeString& name, UErrorCode& err)
    : UObject()
{
    char myName[UCNV_MAX_CONVERTER_NAME_LENGTH];
    name.extract(myName, sizeof(myName), 0, err);
    myUnicodeConverter = ucnv_open(myName, &err);
}


UnicodeConverter::UnicodeConverter(int32_t codepageNumber,
                                         UConverterPlatform platform,
                                         UErrorCode& err)
    : UObject()
{
    myUnicodeConverter = ucnv_openCCSID(codepageNumber,
                                   platform,
                                   &err);
}

UnicodeConverter&   UnicodeConverter::operator=(const UnicodeConverter&  that)
{
    {
        /*Decrements the overwritten converter's ref count
         *Increments the assigner converter's ref count
         */
        Mutex updateReferenceCounters;
        if (myUnicodeConverter->sharedData->referenceCounter != 0 && myUnicodeConverter->sharedData->referenceCounter != (uint32_t) ~0) {
            myUnicodeConverter->sharedData->referenceCounter--;
        }
        if (that.myUnicodeConverter->sharedData->referenceCounter != (uint32_t) ~0) {
            that.myUnicodeConverter->sharedData->referenceCounter++;
        }
    }

    *myUnicodeConverter = *(that.myUnicodeConverter);
    return *this;
}

UBool UnicodeConverter::operator==(const UnicodeConverter& that) const
{
  return
      (myUnicodeConverter->sharedData == that.myUnicodeConverter->sharedData) &&
      (myUnicodeConverter->fromCharErrorBehaviour == that.myUnicodeConverter->fromCharErrorBehaviour) &&
      (myUnicodeConverter->toUContext == that.myUnicodeConverter->toUContext) &&
      (myUnicodeConverter->toUnicodeStatus == that.myUnicodeConverter->toUnicodeStatus) &&
      (myUnicodeConverter->subChar1 == that.myUnicodeConverter->subChar1) &&
      (myUnicodeConverter->subCharLen == that.myUnicodeConverter->subCharLen) &&
      //(uprv_memcmp(myUnicodeConverter->subChar, that.myUnicodeConverter->subChar, myUnicodeConverter->subCharLen) == 0) &&
      (myUnicodeConverter->UCharErrorBufferLength == that.myUnicodeConverter->UCharErrorBufferLength) &&
      (myUnicodeConverter->charErrorBufferLength == that.myUnicodeConverter->charErrorBufferLength) &&
      (uprv_memcmp(myUnicodeConverter->UCharErrorBuffer, that.myUnicodeConverter->UCharErrorBuffer, myUnicodeConverter->UCharErrorBufferLength) == 0) &&
      (uprv_memcmp(myUnicodeConverter->charErrorBuffer, that.myUnicodeConverter->charErrorBuffer, myUnicodeConverter->charErrorBufferLength) == 0) &&
      (myUnicodeConverter->fromUCharErrorBehaviour == that.myUnicodeConverter->fromUCharErrorBehaviour) &&
      (myUnicodeConverter->fromUContext == that.myUnicodeConverter->fromUContext);
}

UBool UnicodeConverter::operator!=(const UnicodeConverter& that) const
{
    return !(*this == that);
}

UnicodeConverter::UnicodeConverter(const UnicodeConverter&  that)
    : UObject(that)
{
    /*increments the referenceCounter to let the static table know
     *it has one more client
     */
    myUnicodeConverter = (UConverter *)uprv_malloc(sizeof(UConverter)); //new UConverter;
    {
        Mutex updateReferenceCounter;
        if (that.myUnicodeConverter->sharedData->referenceCounter != (uint32_t) ~0) {
            that.myUnicodeConverter->sharedData->referenceCounter++;
        }
    }
    *myUnicodeConverter = *(that.myUnicodeConverter);
}


UnicodeConverter::~UnicodeConverter()
{
    ucnv_close(myUnicodeConverter);
}

 void
UnicodeConverter::fromUnicodeString(char*                    target,
                                       int32_t&                 targetSize,
                                       const UnicodeString&     source,
                                       UErrorCode&               err) const
{
    const UChar* mySource = NULL;
    int32_t mySourceLength = 0;
    UConverter myConverter;
    char *myTarget = NULL;

    if (U_FAILURE(err))
        return;

    if ((myUnicodeConverter == NULL) || source.isBogus() || (targetSize <= 0))
    {
        err = U_ILLEGAL_ARGUMENT_ERROR;
        return;
    }

    /*makes a local copy of the UnicodeConverter*/
    myConverter = *myUnicodeConverter;

    /*Removes all state info on the UnicodeConverter*/
    ucnv_reset(&myConverter);


    mySourceLength = source.length();
    mySource = source.getBuffer();
    myTarget = target;
    ucnv_fromUnicode(&myConverter,
                 &myTarget,
                 target + targetSize,
                 &mySource,
                 mySource + mySourceLength,
                 NULL,
                 true,
                 &err);
    targetSize = myTarget - target;
}

 void
UnicodeConverter::toUnicodeString(UnicodeString&         target,
                                     const char*            source,
                                     int32_t                sourceSize,
                                     UErrorCode&             err) const
{
    const char* mySource = source;
    const char* mySourceLimit = source + sourceSize;
    UChar* myTargetUChars = NULL;
    UChar* myTargetUCharsAlias = NULL;
    int32_t myTargetUCharsLength = 0;
    UConverter myConverter;

    if (U_FAILURE(err))
        return;
    if ((myUnicodeConverter == NULL) || target.isBogus() || (sourceSize <= 0))
    {
        err = U_ILLEGAL_ARGUMENT_ERROR;
        return;
    }

    /*makes a local bitwise copy of the UnicodeConverter*/
    myConverter = *myUnicodeConverter;

    /*Removes all state info on the UnicodeConverter*/
    ucnv_reset(&myConverter);
    /*Allocates the theoritically (Not counting added bytes from the error functions) max buffer
    *on a "normal" call, only one iteration will be necessary.
    */
    myTargetUChars =
        (UChar*)uprv_malloc(sizeof(UChar)*(myTargetUCharsLength = (sourceSize/(int32_t)getMinBytesPerChar())));

    if (myTargetUChars == NULL)
    {
        err = U_MEMORY_ALLOCATION_ERROR;
        return;
    }
    /*renders the target clean*/
    target.remove();

    /*Will loop until (re-use the same buffer) until no more memory is requested
     *or an error (other than INDEX_OUTOF_BOUNDS) is encountered
     */
    do
    {
        err = U_ZERO_ERROR;
        myTargetUCharsAlias = myTargetUChars;
        ucnv_toUnicode(&myConverter,
                    &myTargetUCharsAlias,
                    myTargetUChars + myTargetUCharsLength,
                    &mySource,
                    mySourceLimit,
                    NULL,
                    true,
                    &err);

        /*appends what we got thus far to the UnicodeString*/
        target.replace((int32_t)target.length(),
            myTargetUCharsAlias - myTargetUChars,
            myTargetUChars,
            myTargetUCharsAlias - myTargetUChars);
        /*Checks for the integrity of target (UnicodeString) as it adds data to it*/
        if (target.isBogus())
            err = U_MEMORY_ALLOCATION_ERROR;
    } while (err == U_BUFFER_OVERFLOW_ERROR);


    uprv_free(myTargetUChars);
}



void
UnicodeConverter::fromUnicode(char*&                 target,
                              const char*            targetLimit,
                              const UChar*&        source,
                              const UChar*         sourceLimit,
                              int32_t *offsets,
                              UBool                 flush,
                              UErrorCode&             err)
{
    ucnv_fromUnicode(myUnicodeConverter,
                    &target,
                    targetLimit,
                    &source,
                    sourceLimit,
                    offsets,
                    flush,
                    &err);
}



void
UnicodeConverter::toUnicode(UChar*&           target,
                   const UChar*      targetLimit,
                   const char*&        source,
                   const char*         sourceLimit,
                   int32_t* offsets,
                   UBool              flush,
                   UErrorCode&          err)
{
    ucnv_toUnicode(myUnicodeConverter,
                 &target,
                 targetLimit,
                 &source,
                 sourceLimit,
                 offsets,
                 flush,
                 &err);
}

const char*
UnicodeConverter::getName(UErrorCode&  err) const
{
    return ucnv_getName(myUnicodeConverter, &err);
}

 int8_t
UnicodeConverter::getMaxBytesPerChar() const
{
    return ucnv_getMaxCharSize(myUnicodeConverter);
}

int8_t
UnicodeConverter::getMinBytesPerChar() const
{
    return ucnv_getMinCharSize(myUnicodeConverter);
}

void
UnicodeConverter::getSubstitutionChars(char*             subChars,
                                          int8_t&           len,
                                          UErrorCode&        err) const
{
    ucnv_getSubstChars(myUnicodeConverter,
                        subChars,
                        &len,
                        &err);
}

void
UnicodeConverter::setSubstitutionChars(const char*       subChars,
                                          int8_t            len,
                                          UErrorCode&        err)
{
    ucnv_setSubstChars(myUnicodeConverter,
                        subChars,
                        len,
                        &err);
}


void
UnicodeConverter::resetState()
{
    ucnv_reset(myUnicodeConverter);
}


int32_t
UnicodeConverter::getCodepage(UErrorCode& err) const
{
    return ucnv_getCCSID(myUnicodeConverter, &err);
}

void
UnicodeConverter::getMissingCharAction(UConverterToUCallback *action,
                                       const void **context) const
{
    ucnv_getToUCallBack(myUnicodeConverter, action, context);
}

void
UnicodeConverter::getMissingUnicodeAction(UConverterFromUCallback *action,
                                          const void **context) const
{
    ucnv_getFromUCallBack(myUnicodeConverter, action, context);
}


void
UnicodeConverter::setMissingCharAction(UConverterToUCallback  newAction,
                                       const void *newContext,
                                       UConverterToUCallback *oldAction,
                                       const void **oldContext,
                                       UErrorCode&  err)
{
    ucnv_setToUCallBack(myUnicodeConverter, newAction, newContext, oldAction, oldContext, &err);
}

void
UnicodeConverter::setMissingUnicodeAction(UConverterFromUCallback   newAction,
                                          const void* newContext,
                                          UConverterFromUCallback   *oldAction,
                                          const void** oldContext,
                                          UErrorCode&  err)
{
    ucnv_setFromUCallBack(myUnicodeConverter, newAction, newContext, oldAction, oldContext, &err);
}


void
UnicodeConverter::getDisplayName(const Locale&   displayLocale,
                                    UnicodeString&  displayName) const
{
    UErrorCode err = U_ZERO_ERROR;
    UChar name[UCNV_MAX_CONVERTER_NAME_LENGTH];
    int32_t length = ucnv_getDisplayName(myUnicodeConverter,
                                         displayLocale.getName(),
                                         name,
                                         UCNV_MAX_CONVERTER_NAME_LENGTH,
                                         &err);
    if (U_SUCCESS(err))
    {
        displayName.replace(0, 0x7fffffff, name, length);
    }
    else
    {
        /*Error While creating the resource bundle use the internal name instead*/
        displayName.remove();
        displayName = getName(err); /*Get the raw ASCII name*/
    }
}


UConverterPlatform
UnicodeConverter::getCodepagePlatform(UErrorCode &err) const
{
    return ucnv_getPlatform(myUnicodeConverter, &err);
}

UConverterType UnicodeConverter::getType() const
{
    return ucnv_getType(myUnicodeConverter);
}

void UnicodeConverter::getStarters(UBool starters[256],
                 UErrorCode& err) const
{
    ucnv_getStarters(myUnicodeConverter,
        starters,
        &err);
}

const char* const*
UnicodeConverter::getAvailableNames(int32_t& num, UErrorCode& err)
{
    char** names = NULL;
    int32_t count = 0;
    int32_t i;

    if(U_FAILURE(err)) {
        num = 0;
        return NULL;
    }
    if (availableConverterNames==NULL) {
//% Rewritten to not depend on internal API:
//%        count = ucnv_io_countAvailableConverters(&err);
        count = ucnv_countAvailable();
        if (count > 0) {
            names = (char **) uprv_malloc( sizeof(const char*) * count );
            if (names != NULL) {
//% Rewritten to not depend on internal API:
//%                ucnv_io_fillAvailableConverters(names, &err);
                for (i=0; i<count; ++i) {
                    names[i] = NULL;
                }
                for (i=0; i<count; ++i) {
                    const char* p = ucnv_getAvailableName(i);
                    if (p == NULL) {
                        goto ERR;
                    }
                    names[i] = (char*) uprv_malloc(uprv_strlen(p) + 1);
                    if (names[i] == NULL) {
                        goto ERR;
                    }
                    uprv_strcpy(names[i], p);
                }

                /* in the mutex block, set the data for this process */
                umtx_lock(0);
                if (availableConverterNames == NULL) {
                    availableConverterNamesCount = count;
                    availableConverterNames = (const char**) names;
                    names = 0;
                }
                umtx_unlock(0);

                /* if a different thread set it first, then delete the extra data */
                if (names != 0) {
                    for (i=0; i<count; ++i) {
                        uprv_free(names[i]);
                    }
                    uprv_free(names);
                }
            } else {
                num = 0;
                err = U_MEMORY_ALLOCATION_ERROR;
                return NULL;
            }
        }
    }
    num = availableConverterNamesCount;
    return availableConverterNames;

 ERR:
    for (i=0; i<count; ++i) {
        uprv_free(names[i]);
    }
    uprv_free(names);
    num = 0;
    if (U_SUCCESS(err)) {
        err = U_MEMORY_ALLOCATION_ERROR;
    }
    return NULL;
}

int32_t  UnicodeConverter::flushCache()
{
    return ucnv_flushCache();
}

/* TODO: To be cleaned up.  The usage of UChar* and UnicodeString in
the C++ APIs need to be revisited. */
void UnicodeConverter::fixFileSeparator(UnicodeString& source) const {
    if(this==NULL || &source==NULL || source.length()<=0) {
        return;
    }

    int32_t length=source.length();
    ucnv_fixFileSeparator(myUnicodeConverter, source.getBuffer(-1), length);
    source.releaseBuffer(length);
}

UBool UnicodeConverter::isAmbiguous(void) const
{
    return ucnv_isAmbiguous(myUnicodeConverter);
}

U_NAMESPACE_END
