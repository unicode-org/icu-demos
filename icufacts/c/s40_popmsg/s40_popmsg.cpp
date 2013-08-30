#include "../iucsamples.h"
#include "../popdata/popdata_inc.h"

// Now we're starting to put things together.

int main() {
  UErrorCode status = U_ZERO_ERROR;
  Locale locale = Locale::getDefault();
  u_setDataDirectory("out"); // this is where the data is generated, see Makefile
  int32_t territoryCount;
  TerritoryEntry **territoryList = getTerritoryEntries(locale, territoryCount, status);
  ASSERT_OK(status);

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
    std::cout << result << std::endl;
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
      std::cout << result << std::endl;
      delete territoryList[i]; // cleanup.
    }
  }
  delete [] territoryList;
}

// Local Variables:
// compile-command: "make check"
// End:
