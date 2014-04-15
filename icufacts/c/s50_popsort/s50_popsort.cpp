#include "../iucsamples.h"
#include "../popdata/popdata_inc.h"
#include <list>
#include <algorithm>

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
  Locale defaultLocaleId = Locale::getDefault();
  int32_t territoryCount; // == number of territories
  u_setDataDirectory("out"); // this is where the data is generated, see Makefile

  TerritoryEntry **territoryList = TerritoryEntry::getTerritoryEntries(defaultLocaleId, territoryCount, status);
  ASSERT_OK(status);

  // Create collator and sort the territories
  LocalPointer<Collator> coll(Collator::createInstance(defaultLocaleId, status));
  std::sort(territoryList, territoryList+territoryCount, CollatorLessThan(*coll, status));

  // First, read the messages.
  ResourceBundle resourceBundle("popmsg", defaultLocaleId, status);
  ASSERT_OK(status);

  // Print the welcome message.
  {
    LocalPointer<LocaleDisplayNames>
      ldn(LocaleDisplayNames::createInstance(defaultLocaleId,
                                             ULDN_DIALECT_NAMES));

    UnicodeString myLanguage;
    ldn->localeDisplayName(defaultLocaleId, myLanguage);
    // "American English"

    UnicodeString world;
    ldn->regionDisplayName("001",world);
    // "World"

    UnicodeString welcome = resourceBundle.getStringEx("welcome", status);
    UnicodeString names[] = {
      "myLanguage", "today", "territoryCount"
    };
    Formattable args[] = {
      myLanguage, Calendar::getNow(), territoryCount
    };
    MessageFormat fmt(welcome, defaultLocaleId, status);
    UnicodeString result;
    fmt.format(names, args, 3, result, status);
    ASSERT_OK(status);
    std::cout << result << std::endl;
  }

  {
    // set up the 'info' messageformat.
    UnicodeString info = resourceBundle.getStringEx("info", status);
    MessageFormat fmt(info, defaultLocaleId, status);
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
      std::cout << result << std::endl;
      delete territoryList[i]; // cleanup.
    }
  }
  delete [] territoryList;
}

// Local Variables:
// compile-command: "make check"
// End:
