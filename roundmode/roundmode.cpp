/*
*******************************************************************************
*   Copyright (C) 2012, International Business Machines
*   Corporation and others.  All Rights Reserved.
*******************************************************************************
*/

/*
 *  Generates a rounding mode chart.
 *
 *  To build, use  icurun   
 */

#include "unicode/unum.h"
#include "unicode/putil.h"
#include "unicode/uvernum.h"
#include <stdio.h>

#define ASSERTOK() if(U_FAILURE(status)) {fprintf(stderr,"%s:%d: Error: %s\n", __FILE__,__LINE__,u_errorName(status));return 1;}


#define MODE(x)   { x, #x },

typedef struct {
  UNumberFormatRoundingMode mode;
  const char name[25];
} RoundTable;

RoundTable roundModes[] = {
  MODE(UNUM_ROUND_CEILING)
  MODE(UNUM_ROUND_FLOOR)
  MODE(UNUM_ROUND_DOWN)
  MODE(UNUM_ROUND_UP)
  MODE(UNUM_ROUND_HALFEVEN)
  MODE(UNUM_ROUND_HALFDOWN)
  MODE(UNUM_ROUND_HALFUP)
};

#define MODE_COUNT sizeof(roundModes)/sizeof(roundModes[0])

void hdr() {
  int m;
  printf("<div class='roundhdr'>");
  printf("<span class='roundnum'>#</span>");
  for(m=0 ;m<(int)(MODE_COUNT);m++) {
    printf("<span class='roundtype'>%s</span>", roundModes[m].name+11);
  }
  printf("<span class='roundnum'>#</span>");
  printf("</div>\n");
}

int main() {
  UErrorCode status = U_ZERO_ERROR;
  UNumberFormat *nf = unum_open(UNUM_DECIMAL, NULL, -1, "en_CA", NULL, &status);
  int m,n;
  
  ASSERTOK();
  unum_setAttribute(nf, UNUM_DECIMAL_ALWAYS_SHOWN, 1);
  unum_setAttribute(nf, UNUM_MAX_FRACTION_DIGITS, 1);
  unum_setAttribute(nf, UNUM_MIN_FRACTION_DIGITS, 1);

  printf("<!-- %s -->\n", U_COPYRIGHT_STRING);
  printf("<h1>ICU %s Rounding Modes</h1>\n", U_ICU_VERSION);

  printf("    <link rel=\"stylesheet\" type=\"text/css\" href=\"round.css\" />\n");

  printf("<div class='roundtbl'>\n");

  
  printf("<div class='roundrow roundblank'><span class='roundspacer roundnum'>&nbsp;</span></div>\n"); // spacer
  
  for(n=000;n<=100;n++) {

    if((n%10)==0) { 
      hdr();
    }
      double d = n / 100.0;

      printf("<div class='roundrow %s'>",  ((n%10)==5)?"nx5":"");
      printf("<span class='roundnum'>%.2f</span>", d);

      for(m=0 ;m<(int)(MODE_COUNT);m++) {
        UChar result[100];
        char str[100];
        unum_setAttribute(nf, UNUM_ROUNDING_MODE, roundModes[m].mode);
        unum_formatDouble(nf, d, result, 100,0, &status);
        ASSERTOK();
        u_UCharsToChars(result,str,u_strlen(result)+1);
        printf("<span class='n%c%c roundres'>%s</span>",str[0],str[2],str); // class='n06'  0.6
      }
      printf("<span class='roundnum'>%.2f</span>", d);
      printf("</div>\n");
  }
  printf("</div>\n");
  
  unum_close(nf);

  return 0;
}
