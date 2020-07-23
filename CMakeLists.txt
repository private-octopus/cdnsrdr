CMAKE_MINIMUM_REQUIRED(VERSION 2.8)
CMAKE_POLICY(SET CMP0003 NEW)

PROJECT(cdnsrdr CXX)

SET(CMAKE_CXX_STANDARD 11)

SET(CMAKE_CXX_FLAGS "-Wall -O2 -g ${CMAKE_CXX_FLAGS}")

INCLUDE_DIRECTORIES(lib test)

SET(CDNS_LIBRARY_FILES
   lib/cbor.cpp
   lib/cdns.cpp
)

add_library(cdnsrdr
    ${CDNS_LIBRARY_FILES}
)

SET(CDNS_TEST_LIBRARY_FILES
   test/CborTest.cpp
   test/CdnsTest.cpp
   test/cdns_test_class.cpp
)

ADD_EXECUTABLE(cdnstest
   test/CdnsTestApp.cpp 
    ${CDNS_TEST_LIBRARY_FILES}
)

target_link_libraries(cdnstest
    cdnsrdr
)

SET_TARGET_PROPERTIES(cdnsrdr PROPERTIES VERSION 1.0)

SET(TEST_EXES cdnstest)

