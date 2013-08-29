#include "../iucsamples.h"
#include "popdata_inc.h"

// this is just a test program for the population data class

int main() {
  UErrorCode status = U_ZERO_ERROR;

  int32_t nTerr;
  TerritoryEntry **ent = getTerritoryEntries(Locale::getDefault(), nTerr, status);
  ASSERT_OK(status);

  u_printf("count=%d\n", nTerr);
  for(int i=0;i<nTerr;i++) {
    u_printf("%20d %.*S\n", ent[i]->getPopulation(),
             ent[i]->getTerritoryName().length(), ent[i]->getTerritoryName().getBuffer() );
    delete ent[i];
  }
  delete [] ent;
}

// Local Variables:
// compile-command: "make check"
// End:
