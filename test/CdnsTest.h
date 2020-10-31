/*
* Author: Christian Huitema
* Copyright (c) 2019, Private Octopus, Inc.
* All rights reserved.
*
* Permission to use, copy, modify, and distribute this software for any
* purpose with or without fee is hereby granted, provided that the above
* copyright notice and this permission notice appear in all copies.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
* ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL Private Octopus, Inc. BE LIABLE FOR ANY
* DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
* ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
* SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef CDNS_TEST_H
#define CDNS_TEST_H
#include "cdns.h"
#include "cdns_test_class.h"

class CdnsTest
{
public:
    CdnsTest();
    ~CdnsTest();

    static void NamePrint(uint8_t* q_name, size_t q_name_length, FILE* F);

    static void SubmitQuery(cdns* cdns_ctx, size_t query_index, FILE* F);

    static void SubmitPreamble(FILE* F_out, cdns* cdns_ctx);
    static void SubmitStorageParameter(FILE* F_out, cdnsStorageParameter* storage);
    static void SubmitCollectionParameters(FILE* F_out, cdnsCollectionParameters* collection);

    static bool FileCompare(char const* file_out, char const* file_ref);

    bool DoTest(char const* test_in, char const* test_out, char const* test_ref);

    static void  PrintIntVector(FILE* F_out, std::vector<int>* v_int);
    static void  PrintTextVector(FILE* F_out, std::vector<cbor_text>* v_int);
    static void  PrintBytesVector(FILE* F_out, std::vector<cbor_bytes>* v_bytes);
    static void  PrintText(FILE* F_out, cbor_text* p_text);
    static void  PrintBytes(FILE* F_out, cbor_bytes* p_bytes);

};

class CdnsTestDraft : public cdns_test_class
{
public:
    CdnsTestDraft();
    ~CdnsTestDraft();

    bool DoTest() override;
};

class CdnsTestRfc : public cdns_test_class
{
public:
    CdnsTestRfc();
    ~CdnsTestRfc();

    bool DoTest() override;
};

class CdnsDumpTest : public cdns_test_class
{
public:
    CdnsDumpTest();
    ~CdnsDumpTest();

    bool DoTest() override;
};

class CdnsRfcDumpTest : public cdns_test_class
{
public:
    CdnsRfcDumpTest();
    ~CdnsRfcDumpTest();

    bool DoTest() override;
};

class CdnsGoldDumpTest : public cdns_test_class
{
public:
    CdnsGoldDumpTest();
    ~CdnsGoldDumpTest();

    bool DoTest() override;
};

class CdnsTestGold : public cdns_test_class
{
public:
    CdnsTestGold();
    ~CdnsTestGold();

    bool DoTest() override;
};


#endif