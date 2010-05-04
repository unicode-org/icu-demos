# Copyright (C) 1999-2010, International Business Machines Corporation and Others. All Rights Reserved.
s%.*FSWF("\([^"]*\)",[ ]*"\([^"]*\)"/\*\([^*][^*]*\).*%    /**\
     * @note \3\
     * @translate yes\
     */\
     \1  {  "\2"  }\
%
s%.*FSWF("\([^"]*\)",[ ]*"\([^"]*\)".*%    /**\
     * @translate yes\
     */\
     \1  {  "\2"  }\
%
