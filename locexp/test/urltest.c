/**********************************************************************
*   Copyright (C) 2003, International Business Machines
*   Corporation and others.  All Rights Reserved.
***********************************************************************/

#include "locexp.h"
#include "unicode/ustdio.h"

int good=0;
int fail=0;
LXContext *lx = NULL;

void checkval( const char *fld, const char *val)
{
  const char *rv;
  rv=queryField(lx,fld);
  if(rv==NULL) rv="NULL";
  if(strcmp(rv,val)) {
    printf("FAIL expected %s = %s but got %s\n", fld, val, rv); 
    fail++;
  } else {
    printf("good  %s = %s\n", fld, rv);
    good++;
  }
}

void checknull( const char *fld)
{
  const char *rv;
  rv=queryField(lx,fld);
  if(rv != NULL) {
    printf("FAIL expected %s = NULL but got %s\n", fld, rv); 
    fail++;
  } else {
    printf("good  %s = NULL\n", fld);
    good++;
  }
}

int main()
{
  LXContext mlx;
  initContext(&mlx);
  lx = &mlx;
  lx->OUT = u_finit(stdout, "en_US", "iso-8859-1");

#define U1 "_=ca_ES&EXPLORE_CollationElements=%5Cu0062%5Cu0061%5Cu0064%5Cu000D%5Cu0042%5Cu0061%5Cu0064%5Cu000D%5Cu0042%5Cu0061%5Cu0074%5Cu000D%5Cu0062%5Cu0061%5Cu0074%5Cu000D%5Cu0062%5Cu00E4%5Cu0064%5Cu000D%5Cu0042%5Cu00E4%5Cu0064%5Cu000D%5Cu0062%5Cu00E4%5Cu0074%5Cu000D%5Cu0042%5Cu00E4%5Cu0074%5Cu000D%5Cu0063%5Cu00f4%5Cu0074%5Cu00e9%5Cu000D%5Cu0063%5Cu006F%5Cu0074%5Cu00e9%5Cu000D%5Cu0063%5Cu00f4%5Cu0074%5Cu0065%5Cu000D%5Cu0063%5Cu006F%5Cu0074%5Cu0065&x=27&y=9"

#define U2 "EXPLORE_CollationElements=%5Cu0088%5Cu0061%5Cu0064%5Cu000D%5Cu0042%5Cu0061%5Cu0064%5Cu000D%5Cu0042%5Cu0061%5Cu0074%5Cu000D%5Cu0062%5Cu0061%5Cu0074%5Cu000D%5Cu0062%5Cu00E4%5Cu0064%5Cu000D%5Cu0042%5Cu00E4%5Cu0064%5Cu000D%5Cu0062%5Cu00E4%5Cu0074%5Cu000D%5Cu0042%5Cu00E4%5Cu0074%5Cu000D%5Cu0063%5Cu00f4%5Cu0074%5Cu00e9%5Cu000D%5Cu0063%5Cu006F%5Cu0074%5Cu00e9%5Cu000D%5Cu0063%5Cu00f4%5Cu0074%5Cu0065%5Cu000D%5Cu0063%5Cu006F%5Cu0074%5Cu0065&x=23&foo=bar";

  lx->queryString = U1;

  checkval("_", "ca_ES");
  checknull("__");
  checknull("EXPLO");
  checknull("000D");
  checknull("foo");
  checknull("ca_ES");
  checkval("x", "27");
  checkval("y", "9");
  checkval("EXPLORE_CollationElements","%5Cu0062%5Cu0061%5Cu0064%5Cu000D%5Cu0042%5Cu0061%5Cu0064%5Cu000D%5Cu0042%5Cu0061%5Cu0074%5Cu000D%5Cu0062%5Cu0061%5Cu0074%5Cu000D%5Cu0062%5Cu00E4%5Cu0064%5Cu000D%5Cu0042%5Cu00E4%5Cu0064%5Cu000D%5Cu0062%5Cu00E4%5Cu0074%5Cu000D%5Cu0042%5Cu00E4%5Cu0074%5Cu000D%5Cu0063%5Cu00f4%5Cu0074%5Cu00e9%5Cu000D%5Cu0063%5Cu006F%5Cu0074%5Cu00e9%5Cu000D%5Cu0063%5Cu00f4%5Cu0074%5Cu0065%5Cu000D%5Cu0063%5Cu006F%5Cu0074%5Cu0065");
  
  lx->postData = U2;
  printf(" -- set POST data -- \n");

  checkval("_", "ca_ES");
  checknull("__");
  checknull("EXPLO");
  checknull("000D");
  checknull("ca_ES");
  checkval("x", "23");
  checkval("y", "9");
  checkval("EXPLORE_CollationElements","%5Cu0088%5Cu0061%5Cu0064%5Cu000D%5Cu0042%5Cu0061%5Cu0064%5Cu000D%5Cu0042%5Cu0061%5Cu0074%5Cu000D%5Cu0062%5Cu0061%5Cu0074%5Cu000D%5Cu0062%5Cu00E4%5Cu0064%5Cu000D%5Cu0042%5Cu00E4%5Cu0064%5Cu000D%5Cu0062%5Cu00E4%5Cu0074%5Cu000D%5Cu0042%5Cu00E4%5Cu0074%5Cu000D%5Cu0063%5Cu00f4%5Cu0074%5Cu00e9%5Cu000D%5Cu0063%5Cu006F%5Cu0074%5Cu00e9%5Cu000D%5Cu0063%5Cu00f4%5Cu0074%5Cu0065%5Cu000D%5Cu0063%5Cu006F%5Cu0074%5Cu0065");
  

  printf("\n-------------\ndone.  %d pass %d fail\n", good, fail);

  if(fail) return fail;
  return 0;
}
