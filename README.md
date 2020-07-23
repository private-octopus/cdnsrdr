# CDNSRDR
C++ parser for the CDNS format (RFC 8618)

This project builds a small library including:

* a simple parser for the Concise Binary Object Representation (CBOR) defined in [RFC 7049](https://tools.ietf.org/html/rfc7049).
* a parser for the Compacted-DNS format (C-DNS).

The standard version of C-DNS is defined in [RFC 8618](https://tools.ietf.org/html/rfc8618). 
For historical reasons, the current
CDNSRDR code is based on [C-DNS draft-04](https://datatracker.ietf.org/doc/draft-ietf-dnsop-dns-capture-format/04/).
We plan on supporting the final RFC 8618 format very soon.

In a typical case, the application will open a CDNS file, and then successively read the blocks contained in the file,
so that only one CDNS block is loaded in memory at a given time. Once a block is read in memory, the application
can process its elements. For example, an application that processes the recorded DNS queries might look like:
~~~
    #include <cdns.h>

    ...
    cdns cdns_ctx;
    int err;
    bool ret = cdns_ctx.open(fileName);

    while (ret) {
        if (!cdns_ctx.open_block(&err)) {
            ret = (err == CBOR_END_OF_ARRAY);
            break;
        }

        for (size_t i = 0; i < cdns_ctx.block.queries.size(); i++) {
            // Process each query in turn
            ...
        }
    }
~~~

The CDNSRDR library was initially developed as part of the [ITHITOOLS project](https://github.com/private-octopus/ithitools/).

# Building CDNSRDR

CDNSRDR was developed in C++, and can be built under Windows or Linux.

## CDNSRDR on Windows

To build CDNSRDR on Windows, you need to:

 * Have a version of Visual Studio 2017 or later installed. The freely available
   "Community" version will work.

 * Clone CDNSRDR:
~~~
   git clone https://github.com/private-octopus/cdnsrdr/
~~~
 * Compile CDNSRDR, using the Visual Studio 2017 solution 
   cdnsrdr.sln included in the sources.

 * You can use the test program `cdnstest.exe` to 
   verify the port.

The library will be available as `cdnsrdr.lib`.

## CDNSRDR on Linux

To build ITHITOOLS on Linux, you need to:

 * Clone and compile CDNSRDR:
~~~
   git clone https://github.com/private-octopus/cdnsrdr/
   cd cdnsrdr
   cmake .
   make
~~~
 * Run the test program "cdnstest" to verify the port.

Of course, if you want to just update to the latest release, you don't need to install
again. You will do something like:
~~~
   cd cdnsrdr
   git pull --all
   cmake .
   make
~~~

The library will be available as `libcdnsrdr.a`.