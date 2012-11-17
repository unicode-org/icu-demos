//
//  ItmlParser.h
//  locexp2
//
//  Copyright 2011 IBM Corporation and Others. All rights reserved.
//

#ifndef _ITMLPARSER
#define _ITMLPARSER

#include "unicode/regex.h"
#include "unicode/utypes.h"
#include "unicode/unistr.h"
#include "unicode/ustdio.h"

#include <list>
#include <vector>
#include <string>
#include <memory>
using namespace std;


class ItmlParser : public UMemory {
public:
    ItmlParser(UErrorCode &status);
    virtual ~ItmlParser();
    
    virtual void addFile(const char *subname, UErrorCode &status);

    virtual int prologue(UFILE *out);
    
    virtual int parse(const char *sub, UnicodeString &tmpl, UFILE *out, UErrorCode &status);
    
    virtual int epilogue(UFILE *out);
    
private:
    void writeLiteral(UFILE *out, UnicodeString &tmpl, int32_t start, int32_t limit, UErrorCode &status);
    
    vector<string> filelist;
    
    int xstr;
    
    RegexPattern *pattern[2];
    auto_ptr<RegexPattern> stringPattern,pagePattern;
};

#endif