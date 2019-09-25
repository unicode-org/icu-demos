#include "unicode/unumsys.h"
#include <unicode/numsys.h>

using namespace icu; // TODO:

U_CAPI UNumberingSystem *unumsys_open(const char *loc, UErrorCode *status){
  return (UNumberingSystem*)NumberingSystem::createInstance(Locale(loc),*status);
}


U_CAPI const char *unumsys_getName(UNumberingSystem *sys) {
  return ((NumberingSystem*)sys)->getName();
}

U_CAPI UEnumeration *unumsys_openAvailableNames(UErrorCode *status) {
#if 0
  return uenum_openFromStringEnumeration(NumberingSystem::getAvailableNames(*status), status);
#else
  return NULL;
#endif
}

U_CAPI void unumsys_close(UNumberingSystem *sys) {
  delete ((NumberingSystem*)sys);
}
