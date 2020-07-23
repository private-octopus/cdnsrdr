
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <string.h>

#include "CborTest.h"
#include "CdnsTest.h"

enum test_list_enum {
    test_enum_cbor = 0,
    test_enum_cbor_skip,
    test_enum_cdns,
    test_enum_cdns_dump,
    test_enum_max_number
};

cdns_test_class::cdns_test_class()
{
}

cdns_test_class::~cdns_test_class()
{
}

int cdns_test_class::get_number_of_tests()
{
    return test_enum_max_number;
}

char const * cdns_test_class::GetTestName(int number)
{
    switch (number) {
    case test_enum_cbor:
        return("cbor");
    case test_enum_cbor_skip:
        return("cborSkip");
    case test_enum_cdns:
        return("cdns");
    case test_enum_cdns_dump:
        return("cdns_dump");
    default:
        break;
    }
    return NULL;
}

int cdns_test_class::GetTestNumberByName(const char * name)
{
    for (int i = 0; i < test_enum_max_number; i++)
    {
#ifdef _WINDOWS
        if (_strcmpi(name, GetTestName(i)) == 0)
#else
        if (strcasecmp(name, GetTestName(i)) == 0)
#endif
        {
            return i;
        }
    }
    return test_enum_max_number;
}

cdns_test_class * cdns_test_class::TestByNumber(int number)
{
    cdns_test_class * test = NULL;

    switch ((test_list_enum)number) {
    case test_enum_cbor:
        test = new CborTest();
        break;
    case test_enum_cbor_skip:
        test = new CborSkipTest();
        break;
    case test_enum_cdns:
        test = new CdnsTest();
        break;
    case test_enum_cdns_dump:
        test = new CdnsDumpTest();
        break;
    default:
        break;
    }

    return test;
}

static FILE * F_log = NULL;

void SET_LOG_FILE(FILE * F)
{
    F_log = F;
}

FILE * GET_LOG_FILE()
{
    return F_log;
}

void TEST_LOG(const char * fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    if (F_log != NULL)
    {
#ifdef _WINDOWS
        vfprintf_s(F_log, fmt, args);
#else
        vfprintf(F_log, fmt, args);
#endif
    }
    va_end(args);
}

