# CDNSRDR
C++ parser for the CDNS format (RFC 8618)

This project builds a small library including:

* a simple parser for the Concise Binary Object Representation (CBOR) defined in [RFC 7049](https://tools.ietf.org/html/rfc7049).
* a parser for the Compacted-DNS format (C-DNS).

The standard version of C-DNS is defined in [RFC 8618](https://tools.ietf.org/html/rfc8618). 
For historical reasons, the current
CDNSRDR code supports both encodings based on that version and 
those based on [C-DNS draft-04](https://datatracker.ietf.org/doc/draft-ietf-dnsop-dns-capture-format/04/).

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

## API differences between RFC 8618 and draft version

The CDNS format definition evolved between the draft 04 on which the draft version was based and the version published
in RFC 8618. Simple coding differences such as use of different indices in CBOR maps are easy to handle during parsing, but
semantic differences are harder. The main differences are:

* Different way to represent default values in the "file preamble",
* Indexing of tables from 1 to N in the draft version, from 0 to N-1 in the RFC version,
* Different formats for the "transport flags" and "signature flags" in the query signature table.

Applications that use both formats need to be aware of these differences. Developers should:
 
* Use the function `cdns.is_old_version()` to check whether the current object was encoded using the old or the new version,
or alternatively read the properties `cdns.preamble.cdns_version_major` and `cdns.preamble.cdns_version_minor`.
* when accessing indexed table elements, shift the index by the value `cdns.index_offset`. This value is set to 1
for old encodings, 0 for new encodings.
* Use the properties `preamble.old_block_parameters`, `old_generator_id` and `old_host_id` when looking at properties of an old encoding,
instead of looking at `preamble.block_parameters[]` for new encodings.
* Use a set of helper methods in the `cdns_query_signature` object to access the value of ip protocol, transport protocol, the
and the presence of trailing bytes in a version independent manner instead of directly accessing the `qr_transport_flags` property.
* Use another set of helper methods in the `cdns_query_signature` object to access the presence or absence of query, response or OPT
records in a version independent manner instead of directly accessing the `qr_sig_flags` property.

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
