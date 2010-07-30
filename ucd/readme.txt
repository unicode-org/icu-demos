# Copyright (c) 2010 IBM Corporation and Others. All Rights Reserved.

Run 'ucd -h' and 'cud h' for option and command help, respectively.

Examples:
---------

Say you can't remember what U+12345 is

$ ucd u+12345
(U+12345) CUNEIFORM SIGN URU TIMES KI

You can give a UnicodeSet, then see how big it is (?), then print it ℗

$ ucd '[:Ital:]' ? p
Workbuf has 70 code units and 35 code points in it.
𐌀𐌁𐌂𐌃𐌄𐌅𐌆𐌇𐌈𐌉𐌊𐌋𐌌𐌍𐌎𐌏𐌐𐌑𐌒𐌓𐌔𐌕𐌖𐌗𐌘𐌙𐌚𐌛𐌜𐌝𐌞𐌠𐌡𐌢𐌣

Or a more complex one

$ ucd '[[a-z]-[aeiou]]' p
bcdfghjklmnpqrstvwxyz

The slash / command  searches

$ ucd /smiling i p
(U+263A) WHITE SMILING FACE
(U+263B) BLACK SMILING FACE
☺☻

You can use the quote " command to insert a literal: (backslash is a shell escape)

$ ucd \"abcd
(U+0061) LATIN SMALL LETTER A
(U+0062) LATIN SMALL LETTER B
(U+0063) LATIN SMALL LETTER C
(U+0064) LATIN SMALL LETTER D

You can run all of these in interactive mode:

$ ucd
ucd> 

You type "é

ucd> "é
(U+00E9) LATIN SMALL LETTER E WITH ACUTE
