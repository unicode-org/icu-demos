The files in this directory are copied from

 icu/source/test/testdata/

One of the files, out/testdata.dat, is a built file (not a CVS
checked-in file).  It is copied from the corresponding icu directory,
which means icu must be built first.  We assume the icu directory is a
sibling of the icuapps directory.

In the future, it may be desirable to add the build structure to
icuapps/test to create these files during each build, but for now,
this is a much simpler way to do things.

If problems arise, make sure that the data here mirrors the latest
from the corresponding icu build.
