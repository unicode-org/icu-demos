# ICU Demos

This package contains sample applications built using the
International Components for Unicode (ICU) C++ library ICU4C. For
more information on ICU, see [icu-project.org](http://site.icu-project.org/)

If you have an application that is written using ICU that you
would like to contribute, join the ICU mailing list **icu-support** on [ICU contacts]
(http://site.icu-project.org/contacts).

Note: [The "iucsamples" directory](https://github.com/unicode-org/icu-demos/tree/master/iucsamples) contains
sample applications for the (International Unicode Conference (IUC) 43)
[https://www.unicodeconference.org/iuc43/Conference_Program.pdf].  Each sample has its own
build instructions.

# Building ICU Demos

## UNIX (or other command line platforms):

  1. Build and install the ICU ('make install'). Make note of the
      prefix used to build the ICU, which can be set with the
     '--prefix=' option to it's ./configure. The example directory /var/local/ is used in the samples below.

  2. icu-config doesn't need to be on your PATH - just make sure the same --prefix is used as that which built ICU.

  3. Run the ./configure script in the icu-demos directory.  You will need
     to supply the same '--prefix=XXX' argument that was passed to the
     ICU.

  4. Type 'make' in the icu-demos, or 'make install' if you wish.

Example commands:
```
  icu4c:
     cd ...somewhere/icu4c
     ./configure --prefix=/var/local
     make install

  icu-demos:
     cd ...somewhere/icu-demos         (THIS directory)
     ./configure --prefix=/var/local
     make
     make install                    (optional)
```

## Win32 Instructions
  1. Build ICU in an 'icu' directory
  2. Build icu-demos in an 'icuicu-demos' directory at the same level as 'icu'

# RUNNING CGIS

## Windows systems:
    Put the CGIs  and ICU DLLs in the CGI-BIN directory.

## UNIX type systems:
    You will need to make sure ICU libraries are on the path, if ICU was not built statically.
    One option is to populate the cgi-bin directory with 'wrapper' scripts.  Create copies or
    symlinks of this script with the name of the cgis (locexp, ubrowse, etc).  Change the 'ICU'
    variable to point at the base of the installed ICU. (the prefix).

```
#!/bin/sh
# wrapper script.

ME=`basename $0`
ICU=/installed/icu
cd ${ICU}/bin
export DYLD_LIBRARY_PATH=${ICU}/lib:${DYLD_LIBRARY_PATH}
exec ${ICU}/bin/${ME}
```

# The Demonstration Applications

- uconv    From : Jonas Utterstrom
                  <jonas.utterstrom@vittran.norrnod.se>
           Uses : Conversion

   This program will convert data file(s) from one encoding to another
   via Unicode.

   As of ICU 1.9, this program is now part of the main ICU build. Though
   it is still not supported it is more convenient there.

- usort    From : Steven R. Loomis
                  <srl [at] icu-project.org>

    Uses : Conversion, Collation

    There are two parts here.

    * a shared library (libusort) which makes it simple to sort lines
      of text.

    * a command line example program (usort) which will sort lines
      from the console or the file, similar to UNIX 'sort'.

- locexp   From : Steven R. Loomis
                  <srl [at] icu-project.org>

    Uses : Conversion, Collation, Chartypes, Formatting,
           Resource Bundles,  ...
             uconv & usort [above]

    * Shows off a lot of things.

- ubrowse   From : Steven R. Loomis
                 <srl [at] icu-project.org>

     Uses : Conversion, Chartypes..
         Depends on locexp/util.

    * Browse the Unicode UCS-2 code space.


- dumpdat     From : Steven R. Loomis
                  <srl [at] icu-project.org>

     Uses : UData

    * A way to look at the version and info headers on a udata
      file


- udata    *OBSOLETE - now part of udata in ICU*

 - xlitomatic From: Steven R. Loomis
                   <srl [at] icu-project.org>

    Uses: Transliterator

### License

Please see [./LICENSE](./LICENSE)ation
      Depends on: locexp/util, etc.

    * Transliterates HTML files using any transliterator. C++.

- calexpo   From: Steven R. Loomis        [NOT CHECKED IN]
                  <srl [at] icu-project.org>

    Uses: Date/Time formatting, Calendar

    * Demonstrates the flexibility of the Calendar class.

### Copyright & Licenses

Copyright © 2016-2024 Unicode, Inc. Unicode and the Unicode Logo are registered trademarks of Unicode, Inc. in the United States and other countries.

The project is released under [LICENSE](./LICENSE).

A CLA is required to contribute to this project - please refer to the [CONTRIBUTING.md](https://github.com/unicode-org/.github/blob/main/.github/CONTRIBUTING.md) file (or start a Pull Request) for more information.
