Making Wingstall Unicode Database
=================================

The Wingstall Unicode database is used by the **soulng.util** library that is used by all Wingstall tools and libraries.
It is located in the **%WINGSTALL_ROOT/unicode** directory.
Here are instructions how to generate the Unicode database files:

*   Extract the ucd.all.flat.zip (extracts to ucd.all.flat.xml)

*   Run make\_wingstall\_ucd.exe

*   The make\_wingstall\_ucd program generates two files:

    *   wingstall\_ucd.bin
        contains uncompressed Unicode database

    *   wingstall\_ucd.deflate.bin
        contains Unicode database compressed with zlib

*   Now the Wingstall Unicode database is ready for use by the Wingstall tools and libraries.
