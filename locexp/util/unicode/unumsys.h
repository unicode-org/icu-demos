#ifndef _UNUMSYS
#define _UNUMSYS

#include <unicode/utypes.h>
#include <unicode/uenum.h>

typedef void UNumberingSystem;

U_CAPI UNumberingSystem *unumsys_open(const char *loc, UErrorCode *status);

U_CAPI const char *unumsys_getName(UNumberingSystem *sys);

U_CAPI UEnumeration *unumsys_openAvailableNames(UErrorCode *status);

U_CAPI void unumsys_close(UNumberingSystem *sys);



#endif
