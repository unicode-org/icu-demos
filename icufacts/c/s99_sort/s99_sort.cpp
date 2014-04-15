#include <ostream>
#include <iostream>
#include <list>
#include <memory> // for auto_ptr
#include <algorithm> // for sort

#include "../iucsamples.h"

/**
 * A comparison object for std::sort which compares two UnicodeStrings
 * according to the Collator given.
 * Note: UnicodeString is handled here, but UChar* could be handled similarly.
 */
class CollatorLessThan : public std::binary_function<UnicodeString, UnicodeString, bool> {
public:
  /**
   * @param coll the Collator to use. Aliased, not adopted.
   * @param status error status from comparison functions. U_SUCCESS(status) should be TRUE on entry. Check the error value on exit from the sort function. NOTE: this reference is retained for the life of the CollatorLessThan object.
   */
  CollatorLessThan(Collator &coll, UErrorCode &status) : fColl(coll), fStatus(status) {}
  inline bool operator()(const UnicodeString& a, const UnicodeString& b) {
    return (fColl.compare(a, b, fStatus) == UCOL_LESS);
  }
private:
  Collator &fColl;
  UErrorCode &fStatus;
};

int main() {
  // Locale to use - Danish.
  Locale loc("da");
  // Input data.
  UnicodeString A[] = { "tirsdag", "aaland", "abel", "zach" };
  const int N = sizeof(A) / sizeof(A[0]);
  UErrorCode errorCode = U_ZERO_ERROR;

  // Setup.
  std::auto_ptr<Collator> coll(Collator::createInstance(loc, errorCode));

  // Check that collator is OK.
  std::cout << u_errorName(errorCode) << std::endl;
  if(U_FAILURE(errorCode)) {
    return 1;
  }

  // The actual sort
  std::sort(A, A + N, CollatorLessThan(*coll, errorCode));

  if(U_FAILURE(errorCode)) {
    std::cout << "Error comparing: " << u_errorName(errorCode) << std::endl;
    return 1;
  }

  // Print the resulting strings
  for(int i=0;i<N;i++) {
    std::cout << A[i];
    std::cout << " ";
  }
  std::cout << std::endl;
  return 0;
}

// Local Variables:
// compile-command: "clang++ -o stl stl.cpp `icu-config --cppflags --ldflags --ldflags-icuio` && ./s99_sort.cpp"
// End:
