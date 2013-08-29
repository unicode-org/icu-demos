#include "../iucsamples.h"
#include "../popdata/popdata_inc.h"
#include <list>

/**
 * A comparison object for std::sort which compares two TerritoryEntries
 * according to the Collator given.
 * Note: UnicodeString is handled here, but UChar* could be handled similarly.
 */
class CollatorLessThan : public std::binary_function<const TerritoryEntry*, const TerritoryEntry*, bool> {
public:
  /**
   * @param coll the Collator to use. Aliased, not adopted.
   * @param status error status from comparison functions. U_SUCCESS(status) should be TRUE on entry. Check the error value on exit from the sort function. NOTE: this reference is retained for the life of the CollatorLessThan object.
   */
  CollatorLessThan(Collator &coll, UErrorCode &status) : fColl(coll), fStatus(status) {}
  inline bool operator()(const TerritoryEntry* a, const TerritoryEntry* b) {
    return (fColl.compare(a->getTerritoryName(), b->getTerritoryName(), fStatus) == UCOL_LESS);
  }
private:
  Collator &fColl;
  UErrorCode &fStatus;
};


// Sort the list by collator.


int main() {
  UErrorCode status = U_ZERO_ERROR;
  Locale locale = Locale::getDefault();
  u_setDataDirectory("out"); // this is where the data is generated, see Makefile
  int32_t territoryCount;
  TerritoryEntry **territoryList = getTerritoryEntries(Locale::getDefault(), territoryCount, status);
  ASSERT_OK(status);

  // Create collator and sort the territories
  LocalPointer<Collator> coll(Collator::createInstance(locale, status));
  std::sort(territoryList, territoryList+territoryCount, CollatorLessThan(*coll, status));

  // First, read the messages.
  ResourceBundle resourceBundle("popmsg", locale, status);
  ASSERT_OK(status);

  // Print the welcome message.
  {
    UnicodeString welcome = resourceBundle.getStringEx("welcome", status);
    UnicodeString names[] = {
      "territoryCount",
    };
    Formattable args[] = {
      territoryCount
    };
    MessageFormat fmt(welcome, locale, status);
    UnicodeString result;
    fmt.format(names, args, 1, result, status);
    ASSERT_OK(status);
    u_printf("%S\n\n", result.getTerminatedBuffer());
  }

  {
    // set up the 'info' messageformat.
    UnicodeString info = resourceBundle.getStringEx("info", status);
    MessageFormat fmt(info, locale, status);
    UnicodeString names[] = {
      "territory",
      "population"
    };
    for(int i=0;i<territoryCount;i++) {
      UnicodeString result;
      Formattable args[] = {
        territoryList[i]->getTerritoryName(),
        territoryList[i]->getPopulation()
      };
      result = fmt.format(names, args, 2, result, status);
      ASSERT_OK(status);
      u_printf("%S\n", result.getTerminatedBuffer());
      delete territoryList[i]; // cleanup.
    }
  }
  delete [] territoryList;
}

// Local Variables:
// compile-command: "make check"
// End:
