// Copyright (c) 2010-2013 IBM Corporation and Others. All Rights Reserved.
// Population Data source.
// Written inline for ease of sample programs.
#include "../iucsamples.h"
#include <math.h>

class TerritoryEntry {
 public:
  TerritoryEntry(const UnicodeString &countryName,
                 int64_t population) : fTerritoryName(countryName), fPopulation(population) {}
  const UnicodeString &getTerritoryName() const { return fTerritoryName; }
  int64_t getPopulation() const { return fPopulation; }
 private:
  int64_t fPopulation;
  UnicodeString fTerritoryName;
};

static double ldml2d(int32_t n);

/**
 * @param loc display locale
 * @param nTerr number of territories, on exit
 * @param status error code
 * @return array of pointers to TerritoryEntry.  Caller owns the pointers and the obj.
 */
static TerritoryEntry **getTerritoryEntries(const Locale& loc, int32_t &nTerr, UErrorCode &status) {
  // Note: format of supplementalData is NOT STATIC and may change. It is internal to ICU.
  LocalUResourceBundlePointer suppData(ures_openDirect(NULL, "supplementalData", &status));
  LocalUResourceBundlePointer territoryInfo(ures_getByKey(suppData.getAlias(), "territoryInfo", NULL, &status));
  nTerr = ures_getSize(territoryInfo.getAlias());

  if(U_FAILURE(status)) {
    return NULL;
  }

  TerritoryEntry **entries = new TerritoryEntry* [nTerr];
  LocalUResourceBundlePointer entry;
  int n=0;
  for(n=0;ures_hasNext(territoryInfo.getAlias());n++) {
    {
      UResourceBundle *rawEntry = entry.orphan();
      entry.adoptInstead(ures_getNextResource(territoryInfo.getAlias(), rawEntry, &status));
    }
    LocalUResourceBundlePointer territoryF(ures_getByKey(entry.getAlias(), "territoryF", NULL, &status));
    int32_t len;
    const int32_t *vec = ures_getIntVector(territoryF.getAlias(), &len, &status);

    if(n>=nTerr) {
      status = U_BUFFER_OVERFLOW_ERROR;
      return entries;
    } else if(U_FAILURE(status)) {
      return entries;
    }

    // now we have the entry
    // territoryF = { gdp, literacy, population }
    const char *terrID = ures_getKey(entry.getAlias()); // ex - "CD"
    Locale territory("und", terrID);
    UnicodeString territoryName;
    entries[n] = new TerritoryEntry(territory.getDisplayCountry(loc, territoryName), ldml2d(vec[2]));
  }

  return entries;
}

/**
 * Convert LDML2ICUConverter format floating point (territoryF, etc)
 * into double.
 * See: SupplementalMapper.java - function "&exp"
 * @param n input number, such as -48123456
 * @return double value, such as -123.456
 * @internal
 */
static double ldml2d(int32_t n) {
  if(n == 0) {
    return 0.;
  }
  bool neg = false;
  if(n < 0) {
    n = -n;
    neg = true;
  }
  int exp = (n/1000000);
  n -= (exp * 1000000);
  int sexp = exp - 50; // signed exponent
  double d = n;
  d = d * pow(10, (sexp-5));  // -5 because 50 isn't quite right..
  if(neg) {
    d = -d;
  }
  return d;
}

#if 0 // test program
int main()
{
  printf("foo.\n");

  int32_t junk[] = { 
    49990000, // 99%
    48680000,  // 6.8%
    57344400, // ?
    52940000,  // ?
    0,
    -48123456, // gets -0.012346  not -123.456
    -52123456, // this one gets -123.456
    46100000,
    63146600
  };

  for(int i=0;i<sizeof(junk)/sizeof(junk[0]);i++) {
    printf("%ld -> ", junk[i]);
    int32_t n = junk[i];

    double d = ldml2d(n);
    printf(" %.8f", d);
    printf("\n");
  }

  return 0;
}
#endif

