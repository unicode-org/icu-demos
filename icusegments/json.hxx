//
//  json.hxx
//  icusegmentsX
//
//  Created by Steven Loomis on 11/21/12.
//  Copyright (c) 2012 IBM Corporation and Others. All rights reserved.
//

// for including json.h from C++.

#ifndef icusegmentsX_json_hxx
#define icusegmentsX_json_hxx

// for json.h
typedef int _Bool;
extern "C" {
#include "json.h"
}


// ICU
#include <unicode/localpointer.h>
using namespace icu;

U_DEFINE_LOCAL_OPEN_POINTER(LocalJSONPointer, struct json, json_close);


#endif
