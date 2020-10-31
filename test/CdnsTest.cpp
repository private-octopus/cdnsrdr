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
static char const* gold_in = "..\\test\\data\\gold.cbor";
static char const* text_ref = "..\\test\\data\\cdns_test_ref.txt";
static char const* text_ref_rfc = "..\\test\\data\\cdns_test_ref_rfc.txt";
static char const* text_ref_gold = "..\\test\\data\\cdns_test_ref_gold.txt";
#else
static char const* cbor_in = "..\\..\\test\\data\\cdns_test_file.cbor";
static char const* cdns_in = "..\\..\\test\\data\\cdns_test_file.cdns";
static char const* gold_in = "..\\..\\test\\data\\gold.cbor";
static char const* text_ref = "..\\..\\test\\data\\cdns_test_ref.txt";
static char const* text_ref_rfc = "..\\..\\test\\data\\cdns_test_ref_rfc.txt";
static char const* text_ref_gold = "..\\..\\test\\data\\cdns_test_ref_gold.txt";
#endif
#else
static char const* cbor_in = "test/data/cdns_test_file.cbor";
static char const* cdns_in = "test/data/cdns_test_file.cdns";
static char const* gold_in = "test/data/gold.cbor";
static char const* text_ref = "test/data/cdns_test_ref.txt";
static char const* text_ref_rfc = "test/data/cdns_test_ref_rfc.txt";
static char const* text_ref_gold = "test/data/cdns_test_ref_gold.txt";
#endif
static char const* dump_out = "cdns_dump_file.txt";
static char const* dump_rfc_out = "cdns_dump_rfc_file.txt";
static char const* dump_gold_out = "gold_dump_file.txt";
static char const* text_out = "cdns_test_file.txt";
static char const* text_rfc_out = "cdns_test_rfc_file.txt";
static char const* text_gold_out = "cdns_test_gold_file.txt";


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

            if (query->query_name_index >= cdns_ctx->index_offset) {
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
            if (q_sig->query_classtype_index >= cdns_ctx->index_offset) {
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

void CdnsTest::SubmitPreamble(FILE* F_out, cdns* cdns_ctx)
{
    fprintf(F_out, "Version: %" PRId64 ".%" PRId64 ".%" PRId64 "\n",
        cdns_ctx->preamble.cdns_version_major,
        cdns_ctx->preamble.cdns_version_minor,
        cdns_ctx->preamble.cdns_version_private);
    if (cdns_ctx->preamble.cdns_version_major >= 1) {
        for (size_t i = 0; i < cdns_ctx->preamble.block_parameters.size(); i++) {
            fprintf(F_out, "Block parameters[%zu]\n", i);
            SubmitStorageParameter(F_out, &cdns_ctx->preamble.block_parameters[i].storage);
            SubmitCollectionParameters(F_out, &cdns_ctx->preamble.block_parameters[i].collection);
        }
    }
    else {
        if (cdns_ctx->preamble.old_generator_id.l > 0) {
            fprintf(F_out, "GeneratorId: ");
            PrintText(F_out, &cdns_ctx->preamble.old_generator_id);
            fprintf(F_out, "\n");
        }
        if (cdns_ctx->preamble.old_host_id.l > 0) {
            fprintf(F_out, "HostId: ");
            PrintText(F_out, &cdns_ctx->preamble.old_host_id);
            fprintf(F_out, "\n");
        }
    }
}

void CdnsTest::SubmitStorageParameter(FILE* F_out, cdnsStorageParameter* storage)
{
    fprintf(F_out, "    Storage parameters:\n");
    fprintf(F_out, "        TicksPerSecond: %" PRId64 "\n", storage->ticks_per_second);
    fprintf(F_out, "        MaxBlockItems: %" PRId64 "\n", storage->max_block_items);
    fprintf(F_out, "        StorageHints:\n");
    fprintf(F_out, "            QRHints: %" PRId64 "\n", storage->storage_hints.query_response_hints);
    fprintf(F_out, "            QRSignatureHints: % " PRId64 "\n", storage->storage_hints.query_response_signature_hints);
    fprintf(F_out, "            RRHints: %" PRId64 "\n", storage->storage_hints.rr_hints);
    fprintf(F_out, "            OtherDataHints: %" PRId64 "\n", storage->storage_hints.other_data_hints);
    fprintf(F_out, "        MaxBlockItems: % " PRId64 "\n", storage->max_block_items);
    if (storage->opcodes.size() > 0) {
        fprintf(F_out, "        OpCodes: ");
        PrintIntVector(F_out, &storage->opcodes);
        fprintf(F_out, "\n");
    }
    if (storage->rr_types.size() > 0) {
        fprintf(F_out, "        RRTypes: ");
        PrintIntVector(F_out, &storage->rr_types);
        fprintf(F_out, "\n");
    }
    if (storage->storage_flags != 0) {
        fprintf(F_out, "        StorageFlags: 0x%" PRIx64 "\n", storage->storage_flags);
    }
    if (storage->client_address_prefix_ipv4 != 0) {
        fprintf(F_out, "        ClientAddressPrefixIpv4: %" PRId64 "\n", storage->client_address_prefix_ipv4);
    }
    if (storage->client_address_prefix_ipv6 != 0) {
        fprintf(F_out, "        ClientAddressPrefixIpv6: %" PRId64 "\n", storage->client_address_prefix_ipv6);
    }
    if (storage->server_address_prefix_ipv4 != 0) {
        fprintf(F_out, "        ServerAddressPrefixIpv4: %" PRId64 "\n", storage->server_address_prefix_ipv4);
    }
    if (storage->server_address_prefix_ipv6 != 0) {
        fprintf(F_out, "        ServerAddressPrefixIpv6: %" PRId64 "\n", storage->server_address_prefix_ipv6);
    }
    if (storage->sampling_method.l > 0) {
        fprintf(F_out, "        SamplingMethod: ");
        PrintText(F_out, &storage->sampling_method);
        fprintf(F_out, "\n");
    }
    if (storage->anonymization_method.l > 0) {
        fprintf(F_out, "        AnonymizationMethod: ");
        PrintText(F_out, &storage->anonymization_method);
        fprintf(F_out, "\n");
    }
}

void CdnsTest::SubmitCollectionParameters(FILE* F_out, cdnsCollectionParameters* collection)
{
    fprintf(F_out, "    Collection parameters:\n");
    fprintf(F_out, "        QueryTimeout: %" PRId64 "\n", collection->query_timeout);
    fprintf(F_out, "        SkewTimeout: %" PRId64 "\n", collection->skew_timeout);
    fprintf(F_out, "        SnapLen: %" PRId64 "\n", collection->snaplen);
    fprintf(F_out, "        Promisc: %s\n", (collection->promisc) ? "true" : "false");
    if (collection->interfaces.size() > 0) {
        fprintf(F_out, "        Interfaces: ");
        PrintTextVector(F_out, &collection->interfaces);
        fprintf(F_out, "\n");
    }
    if (collection->server_addresses.size() > 0) {
        fprintf(F_out, "        ServerAddresses: ");
        PrintBytesVector(F_out, &collection->server_addresses);
        fprintf(F_out, "\n");
    }
    if (collection->vlan_id.size() > 0) {
        fprintf(F_out, "        Vlan_id: ");
        PrintIntVector(F_out, &collection->vlan_id);
        fprintf(F_out, "\n");
    }
    if (collection->filter.l > 0) {
        fprintf(F_out, "        Filter: ");
        PrintText(F_out, &collection->filter);
        fprintf(F_out, "\n");
    }
    if (collection->query_options != 0) {
        fprintf(F_out, "        QueryOptions: 0x%" PRIx64 "\n", collection->query_options);
    }
    if (collection->response_options != 0) {
        fprintf(F_out, "        ResponseOptions: 0x%" PRIx64 "\n", collection->response_options);
    }
    if (collection->generator_id.l > 0) {
        fprintf(F_out, "        GeneratorId: ");
        PrintText(F_out, &collection->generator_id);
        fprintf(F_out, "\n");
    }
    if (collection->host_id.l > 0) {
        fprintf(F_out, "        HostId: ");
        PrintText(F_out, &collection->host_id);
        fprintf(F_out, "\n");
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
            ret = cdns_ctx.read_preamble(&err);

            SubmitPreamble(F_out, &cdns_ctx);

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

void CdnsTest::PrintIntVector(FILE* F_out, std::vector<int>* v_int)
{
    fprintf(F_out, "[");
    for (size_t i = 0; i < v_int->size(); i++) {
        if (i != 0) {
            fprintf(F_out, ", ");
        }
        fprintf(F_out, "%d", (*v_int)[i]);
    }
    fprintf(F_out, "]");
}

void CdnsTest::PrintTextVector(FILE* F_out, std::vector<cbor_text>* v_text)
{
    fprintf(F_out, "[");
    for (size_t i = 0; i < v_text->size(); i++) {
        if (i != 0) {
            fprintf(F_out, ", ");
        }
        CdnsTest::PrintText(F_out, &(*v_text)[i]);
    }
    fprintf(F_out, "]");
}

void CdnsTest::PrintBytesVector(FILE* F_out, std::vector<cbor_bytes>* v_bytes)
{
    fprintf(F_out, "[");
    for (size_t i = 0; i < v_bytes->size(); i++) {
        if (i != 0) {
            fprintf(F_out, ", ");
        }
        CdnsTest::PrintBytes(F_out, &(*v_bytes)[i]);
    }
    fprintf(F_out, "]");
}

void CdnsTest::PrintText(FILE* F_out, cbor_text* p_text)
{
    for (size_t i = 0; i < p_text->l; i++) {
        fputc(p_text->v[i], F_out);
    }
}

void CdnsTest::PrintBytes(FILE* F_out, cbor_bytes* p_bytes)
{
    fprintf(F_out, "0x");
    for (size_t i = 0; i < p_bytes->l; i++) {
        fprintf(F_out, "%02x", p_bytes->v[i]);
    }
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

CdnsGoldDumpTest::CdnsGoldDumpTest()
{
}

CdnsGoldDumpTest::~CdnsGoldDumpTest()
{
}

bool CdnsGoldDumpTest::DoTest()
{
    cdns cap_cbor;
    bool ret = cap_cbor.open(gold_in);

    if (ret) {
        ret = cap_cbor.dump(dump_gold_out);
    }

    return ret;
}

CdnsTestGold::CdnsTestGold()
{
}

CdnsTestGold::~CdnsTestGold()
{
}

bool CdnsTestGold::DoTest()
{
    CdnsTest* test = new CdnsTest();
    bool ret = false;

    if (test != NULL) {
        ret = test->DoTest(gold_in, text_gold_out, text_ref_gold);
        delete test;
    }

    return ret;
}
