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

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>
#include <string.h>
#include "cbor.h"
#include "cdns.h"
#include "CdnsTest.h"

#ifdef _WINDOWS
#ifndef _WINDOWS64
static char const* cbor_in = "..\\test\\data\\cdns_test_file.cbor";
static char const* cdns_in = "..\\test\\data\\cdns_test_file.cdns";
static char const* text_ref = "..\\test\\data\\cdns_test_ref.txt";
static char const* text_ref_rfc = "..\\test\\data\\cdns_test_ref_rfc.txt";
#else
static char const* cbor_in = "..\\..\\test\\data\\cdns_test_file.cbor";
static char const* cdns_in = "..\\..\\test\\data\\cdns_test_file.cdns";
static char const* text_ref = "..\\..\\test\\data\\cdns_test_ref.txt";
static char const* text_ref_rfc = "..\\..\\test\\data\\cdns_test_ref_rfc.txt";
#endif
#else
static char const* cbor_in = "test/data/cdns_test_file.cbor";
static char const* cdns_in = "test/data/cdns_test_file.cbor";
static char const* text_ref = "test/data/cdns_test_ref.txt";
static char const* text_ref_rfc = "test/data/cdns_test_ref.txt";
#endif
static char const* dump_out = "cdns_dump_file.txt";
static char const* dump_rfc_out = "cdns_dump_rfc_file.txt";
static char const* text_out = "cdns_test_file.txt";
static char const* text_rfc_out = "cdns_test_rfc_file.txt";


CdnsDumpTest::CdnsDumpTest()
{
}

CdnsDumpTest::~CdnsDumpTest()
{
}

bool CdnsDumpTest::DoTest()
{
    cdns cap_cbor;
    bool ret = cap_cbor.open(cbor_in);

    if (ret) {
        ret = cap_cbor.dump(dump_out);
    }

    return ret;
}


CdnsTest::CdnsTest()
{
}

CdnsTest::~CdnsTest()
{
}

void CdnsTest::NamePrint(uint8_t* q_name, size_t q_name_length, FILE* F)
{
    size_t n_index = 0;

    while (n_index < q_name_length) {
        uint8_t l = q_name[n_index++];
        if (n_index > 1) {
            fprintf(F, ".");
        }
        if (l < 64 && n_index + l <= q_name_length) {
            for (size_t i = 0; i < l; i++) {
                uint8_t c = q_name[n_index++];
                if (c == '.' || c=='\\') {
                    fprintf(F, "\\%c", c);
                } else if (c >= 0x20 && c <= 0x7E) {
                    fprintf(F, "%c", c);
                }
                else {
                    fprintf(F, "\\%03d", c);
                }
            }
        }
        else {
            if (l != 0x80) {
                fprintf(F, "L=%02x?", l);
            }
            break;
        }
    }
}

void CdnsTest::SubmitQuery(cdns* cdns_ctx, size_t query_index, FILE * F)
{
    cdns_query* query = &cdns_ctx->block.queries[query_index];
    cdns_query_signature* q_sig = NULL;

    if (query->query_signature_index >= cdns_ctx->index_offset) {
        q_sig = &cdns_ctx->block.tables.q_sigs[(size_t)query->query_signature_index - cdns_ctx->index_offset];
    }

    fprintf(F, "Qsize: %d, rsize:%d",
        query->query_size, query->response_size);

    if (q_sig == NULL) {
        fprintf(F, ", qsig = NULL\n");
    }
    else {
        fprintf(F, ", flags = %x, ", q_sig->qr_sig_flags);

        if ((q_sig->qr_sig_flags & 0x01) != 0) {
            uint64_t query_time_usec = cdns_ctx->block.block_start_us + query->time_offset_usec;

            fprintf(F, "t: %" PRIu64 ", op: %d, r: %d, flags: %x, ",
                query_time_usec, q_sig->query_opcode, q_sig->query_rcode, q_sig->qr_dns_flags);

            if (query->query_name_index > 0) {
                size_t nid = (size_t)query->query_name_index - cdns_ctx->index_offset;
                uint8_t* q_name = cdns_ctx->block.tables.name_rdata[nid].v;
                size_t q_name_length = cdns_ctx->block.tables.name_rdata[nid].l;

                if (q_name_length > 0 && q_name_length < 256) {
                    NamePrint(q_name, q_name_length, F);
                }
                else if (q_name_length == 0) {
                    fprintf(F, ".");
                }
                else {
                    fprintf(F, "name_length %zu", q_name_length);
                }
            }
            else {
                fprintf(F, "name_index %d", query->query_name_index);
            }
            if (q_sig->query_classtype_index > cdns_ctx->index_offset) {
                size_t cid = (size_t)q_sig->query_classtype_index - cdns_ctx->index_offset;
                fprintf(F, ", CL=%d, RR=%d", cdns_ctx->block.tables.class_ids[cid].rr_class, cdns_ctx->block.tables.class_ids[cid].rr_type);
            }
            else if (q_sig->query_classtype_index != cdns_ctx->index_offset) {
                fprintf(F, ", classtype_index = %d", q_sig->query_classtype_index);
            }
        }
        else {
            fprintf(F, "response only.");
        }
        fprintf(F, "\n");
    }
}

FILE* cnds_file_open(char const* file_name, char const* flags);

bool CdnsTest::FileCompare(char const* file_out, char const* file_ref)
{
    bool ret = true;
    int nb_line = 0;
    char buffer1[256];
    char buffer2[256];
    FILE* F1 = cnds_file_open(file_out, "r");
    FILE* F2 = cnds_file_open(file_ref, "r");

    if (F1 == NULL || F2 == NULL) {
        TEST_LOG("Cannot open file %s\n", F1 == NULL ? file_out : file_ref);
        ret = false;
    }
    else {


        while (ret && fgets(buffer1, sizeof(buffer1), F1) != NULL) {
            nb_line++;
            if (fgets(buffer2, sizeof(buffer2), F2) == NULL) {
                /* F2 is too short */
                TEST_LOG("File %s is longer than %s\n", file_out, file_ref);
                TEST_LOG("    Extra line %d: %s", nb_line, buffer1);
                ret = false;
            }
            else {
                if (strcmp(buffer1, buffer2) != 0) {
                    TEST_LOG("File %s differs from %s at line %d\n", file_out, file_ref, nb_line);
                    TEST_LOG("    Got: %s", buffer1);
                    TEST_LOG("    Vs:  %s", buffer2);
                    ret = false;
                }
            }
        }

        if (ret && fgets(buffer2, sizeof(buffer2), F2) != NULL) {
            /* F2 is too long */
            TEST_LOG("File %s is shorter than %s\n", file_out, file_ref);
            TEST_LOG("    Missing line %d: %s", nb_line + 1, buffer2);
            ret = false;
        }
    }
    if (F1 != NULL) {
        fclose(F1);
    }

    if (F2 != NULL) {
        fclose(F2);
    }

    return ret;
}

bool CdnsTest::DoTest(char const * test_in, char const* test_out, char const * test_ref)
{
    cdns cdns_ctx;
    int err;
    int nb_calls = 0;
    bool ret = cdns_ctx.open(test_in);

    if (!ret) {
        TEST_LOG("Could not open file: %s\n", test_in);
    }
    else {
        FILE* F_out = cnds_file_open(test_out, "w");

        if (F_out == NULL) {
            TEST_LOG("Could not open file: %s\n", test_out);
            ret = false;
        }
        else {
            fprintf(F_out, "Block start: %ld.%06ld\n",
                (long)cdns_ctx.block.preamble.earliest_time_sec, (long)cdns_ctx.block.preamble.earliest_time_usec);
            while (ret) {
                nb_calls++;
                ret = cdns_ctx.open_block(&err);
                if (!ret) {
                    break;
                }

                for (size_t i = 0; i < cdns_ctx.block.queries.size(); i++) {
                    SubmitQuery(&cdns_ctx, i, F_out);
                }
            }
        }

        if (!ret && err == CBOR_END_OF_ARRAY && nb_calls > 1) {
            ret = true;
        }
        else {
            TEST_LOG("Open blocks returns err: %d after %d calls\n", err, nb_calls);
        }

        (void)fclose(F_out);
    }

    if (ret) {
        ret = FileCompare(test_out, test_ref);
    }

    return ret;
}


CdnsRfcDumpTest::CdnsRfcDumpTest()
{
}

CdnsRfcDumpTest::~CdnsRfcDumpTest()
{
}

bool CdnsRfcDumpTest::DoTest()
{
    cdns cap_cbor;
    bool ret = cap_cbor.open(cdns_in);

    if (ret) {
        ret = cap_cbor.dump(dump_rfc_out);
    }

    return ret;
}

CdnsTestDraft::CdnsTestDraft()
{
}

CdnsTestDraft::~CdnsTestDraft()
{
}

bool CdnsTestDraft::DoTest()
{
    CdnsTest * test = new CdnsTest();
    bool ret = false;
    
    if (test != NULL) {
        ret = test->DoTest(cbor_in, text_out, text_ref);
        delete test;
    }

    return ret;
}

CdnsTestRfc::CdnsTestRfc()
{
}

CdnsTestRfc::~CdnsTestRfc()
{
}

bool CdnsTestRfc::DoTest()
{
    CdnsTest* test = new CdnsTest();
    bool ret = false;

    if (test != NULL) {
        ret = test->DoTest(cdns_in, text_rfc_out, text_ref_rfc);
        delete test;
    }

    return ret;
}
