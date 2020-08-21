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
#ifndef CDNS_H
#define CDNS_H

#include <vector>
#include "cbor.h"

class cdns; /* Definition here allows for backpointers */
class cdnsBlock;

class cdns_block_preamble_old
{
public:
    cdns_block_preamble_old();
    ~cdns_block_preamble_old();

    uint8_t* parse(uint8_t* in, uint8_t const* in_max, int* err);

    uint8_t* parse_map_item(uint8_t* in, uint8_t const* in_max, int64_t val, int* err);

    int64_t earliest_time_sec;
    int64_t earliest_time_usec;
    bool is_filled;
};

class cdns_block_preamble
{
public:
    cdns_block_preamble();
    ~cdns_block_preamble();

    uint8_t* parse(uint8_t* in, uint8_t const* in_max, int* err, cdnsBlock* current_block);

    uint8_t* parse_map_item(uint8_t* in, uint8_t const* in_max, int64_t val, int* err);

    uint8_t* parse_time_stamp(uint8_t* in, uint8_t const* in_max, int* err);

    void clear();

    cdnsBlock* current_block;
    int64_t earliest_time_sec;
    int64_t earliest_time_usec;
    int64_t block_parameter_index;
    bool is_filled;
};

class cdns_block_statistics
{
public:
    cdns_block_statistics();
    ~cdns_block_statistics();

    uint8_t* parse(uint8_t* in, uint8_t const* in_max, int* err, cdnsBlock* current_block);

    uint8_t* parse_map_item(uint8_t* in, uint8_t const* in_max, int64_t val, int* err);

    void clear();

    cdnsBlock* current_block;
    int processed_messages; // was total_packets;
    int qr_data_items; // was total_pairs;
    int unmatched_queries;
    int unmatched_responses;
    int discarded_opcode; // only availbale after v1.0
    int malformed_items;
    
    bool is_filled;
};

class cdns_response_processing_data
{
public:
    cdns_response_processing_data();
    ~cdns_response_processing_data();

    uint8_t* parse(uint8_t* in, uint8_t const* in_max, int* err);

    uint8_t* parse_map_item(uint8_t* in, uint8_t const* in_max, int64_t val, int* err);

    void clear();

    bool is_present;
    int bailiwick_index;
    int processing_flags;
};

class cdns_qr_extended {
public:
    cdns_qr_extended();
    ~cdns_qr_extended();

    uint8_t* parse(uint8_t* in, uint8_t const* in_max, int* err);

    uint8_t* parse_map_item(uint8_t* in, uint8_t const* in_max, int64_t val, int* err);

    void clear();

    int question_index;
    int answer_index;
    int authority_index;
    int additional_index;
    bool is_filled;
};

class cdns_query {
public:
    cdns_query();
    ~cdns_query();

    uint8_t* parse(uint8_t* in, uint8_t const* in_max, int* err, cdnsBlock* current_block);

    uint8_t* parse_map_item(uint8_t* in, uint8_t const* in_max, int64_t val, int* err);
    uint8_t* parse_map_item_old(uint8_t* in, uint8_t const* in_max, int64_t val, int* err);

    cdnsBlock* current_block;
    int time_offset_usec;
    int client_address_index;
    int client_port;
    int transaction_id;
    int query_signature_index;
    int client_hoplimit;
    int delay_useconds;
    int query_name_index;
    int query_size;
    int response_size;
    cdns_response_processing_data rpd;
    cdns_qr_extended q_extended;
    cdns_qr_extended r_extended;
};

class cdns_address_event_count {
public:
    cdns_address_event_count();
    ~cdns_address_event_count();

    uint8_t* parse(uint8_t* in, uint8_t const* in_max, int* err, cdnsBlock* current_block);

    uint8_t* parse_map_item(uint8_t* in, uint8_t const* in_max, int64_t val, int* err);

    uint8_t* parse_map_item_old(uint8_t* in, uint8_t const* in_max, int64_t val, int* err);

    cdnsBlock* current_block;
    int ae_type;
    int ae_code;
    int ae_transport_flags;
    int ae_address_index;
    int ae_count;
};


class cdns_class_id {
public:
    cdns_class_id();
    ~cdns_class_id();

    uint8_t * parse(uint8_t* in, uint8_t const* in_max, int* err);

    uint8_t* parse_map_item(uint8_t* in, uint8_t const * in_max, int64_t val, int* err);

    int rr_type;
    int rr_class;
};

typedef enum {
    ipv4 = 0,
    ipv6 = 1
} cdns_ip_protocol_enum;

typedef enum {
    udp=0,
    tcp=1,
    tls=2,
    dtls=3,
    https=4,
    non_standard=15
} cdns_transport_protocol_enum;

class cdns_query_signature {
public:
    cdns_query_signature();
    ~cdns_query_signature();

    cdns_ip_protocol_enum ip_protocol();
    cdns_transport_protocol_enum transport_protocol();
    bool has_trailing_bytes();

    bool is_query_present();
    bool is_response_present();
    bool is_query_present_with_OPT();
    bool is_response_present_with_OPT();
    bool is_query_present_with_no_question();
    bool is_response_present_with_no_question();

    uint8_t* parse(uint8_t* in, uint8_t const* in_max, int* err, cdnsBlock* current_block);

    uint8_t* parse_map_item(uint8_t* in, uint8_t const* in_max, int64_t val, int* err);
    uint8_t* parse_map_item_old(uint8_t* in, uint8_t const* in_max, int64_t val, int* err);

    cdnsBlock* current_block;
    int server_address_index;
    int server_port;
    int qr_transport_flags;
    int qr_type;
    int qr_sig_flags;
    int query_opcode;
    int qr_dns_flags;
    int query_rcode;
    int query_classtype_index;
    int query_qd_count;
    int query_an_count;
    int query_ar_count;
    int query_ns_count;
    int edns_version;
    int udp_buf_size;
    int opt_rdata_index;
    int response_rcode;
};

/* The cdns_question class describes the elements in the QRR table */
class cdns_question {
public:
    cdns_question();
    ~cdns_question();

    uint8_t* parse(uint8_t* in, uint8_t const* in_max, int* err);

    uint8_t* parse_map_item(uint8_t* in, uint8_t const* in_max, int64_t val, int* err);

    int name_index;
    int classtype_index;
};

/* The RR_FIELD table describes elements in the RR table */
class cdns_rr_field {
public:
    cdns_rr_field();
    ~cdns_rr_field();

    uint8_t* parse(uint8_t* in, uint8_t const* in_max, int* err);

    uint8_t* parse_map_item(uint8_t* in, uint8_t const* in_max, int64_t val, int* err);

    int name_index;
    int classtype_index;
    int ttl;
    int rdata_index;
};

/* The RR list table */
class cdns_rr_list{
public:
    cdns_rr_list();
    ~cdns_rr_list();

    uint8_t* parse(uint8_t* in, uint8_t const* in_max, int* err);

    std::vector<int> rr_index;
};

/* The Question List table */
class cdns_question_list {
public:
    cdns_question_list();
    ~cdns_question_list();

    uint8_t* parse(uint8_t* in, uint8_t const* in_max, int* err);

    std::vector<int> question_table_index;
};

class cdnsBlockTables
{
public:
    cdnsBlockTables();

    ~cdnsBlockTables();

    uint8_t* parse(uint8_t* in, uint8_t const* in_max, int* err, cdnsBlock* current_block);

    uint8_t* parse_map_item(uint8_t* in, uint8_t const* in_max, int64_t val, int* err);

    void clear();

    cdnsBlock* current_block;
    std::vector<cbor_bytes> addresses;
    std::vector<cdns_class_id> class_ids;
    std::vector<cbor_bytes> name_rdata;
    std::vector<cdns_query_signature> q_sigs;
    std::vector<cdns_question_list> question_list; /* Indexes to question items in the qrr array */
    std::vector<cdns_question> qrr; /* Individual questions -- index to name and class/type */
    std::vector<cdns_rr_list> rr_list; /* Indexes to RR items in RR table */
    std::vector<cdns_rr_field> rrs; /* List of individual RR */
    /* TODO: track malformed record data if there is demand for it */
    bool is_filled;
};

class cdnsBlock
{
public:
    cdnsBlock();

    ~cdnsBlock();

    uint8_t* parse(uint8_t* in, uint8_t const* in_max, int* err, cdns* current_cdns);

    uint8_t* parse_map_item(uint8_t* in, uint8_t const* in_max, int64_t val, int* err);

    void clear();

    cdns * current_cdns;
    cdns_block_preamble preamble;
    cdns_block_statistics statistics;
    cdnsBlockTables tables;
    std::vector<cdns_query> queries; /* TODO -- check difference between V0.5 and V1 */
    std::vector<cdns_address_event_count> address_events; /* TODO -- check difference between V0.5 and V1 */

    int is_filled;
    uint64_t block_start_us;
};

class cdnsStorageHints
{
public:
    cdnsStorageHints();

    ~cdnsStorageHints();

    uint8_t* parse(uint8_t* in, uint8_t const* in_max, int* err);

    uint8_t* parse_map_item(uint8_t* in, uint8_t const* in_max, int64_t val, int* err);

    void clear();

    int64_t query_response_hints;
    int64_t query_response_signature_hints;
    int64_t rr_hints;
    int64_t other_data_hints;
};

class cdnsStorageParameter
{
public:
    cdnsStorageParameter();

    ~cdnsStorageParameter();

    uint8_t* parse(uint8_t* in, uint8_t const* in_max, int* err);
    uint8_t* parse_map_item(uint8_t* in, uint8_t const* in_max, int64_t val, int* err);

    void clear();

    int64_t ticks_per_second;
    int64_t max_block_items;
    cdnsStorageHints storage_hints;
    std::vector<int> opcodes;
    std::vector<int> rr_types;
    int64_t storage_flags;
    int64_t client_address_prefix_ipv4;
    int64_t client_address_prefix_ipv6;
    int64_t server_address_prefix_ipv4;
    int64_t server_address_prefix_ipv6;
    cbor_bytes sampling_method;
    cbor_bytes anonymization_method;
};

class cdnsCollectionParameters
{
public:
    cdnsCollectionParameters();

    ~cdnsCollectionParameters();

    uint8_t* parse(uint8_t* in, uint8_t const* in_max, int* err);
    uint8_t* parse_map_item(uint8_t* in, uint8_t const* in_max, int64_t val, int* err);

    void clear();

    int64_t query_timeout;
    int64_t skew_timeout;
    int64_t snaplen;
    bool promisc;
    std::vector<cbor_bytes> interfaces;
    std::vector<cbor_bytes> server_addresses;
    std::vector<cbor_bytes> vlan_id;
    cbor_bytes filter;
    int64_t query_options;
    int64_t response_options;
    cbor_text generator_id;
    cbor_text host_id;
};

class cdnsBlockParameter
{
public:
    cdnsBlockParameter();

    ~cdnsBlockParameter();

    uint8_t* parse(uint8_t* in, uint8_t const* in_max, int* err);
    uint8_t* parse_map_item(uint8_t* in, uint8_t const* in_max, int64_t val, int* err);

    void clear();

    cdnsStorageParameter storage;
    cdnsCollectionParameters collection;
};

class cdnsBlockParameterOld
{
public:
    cdnsBlockParameterOld();

    ~cdnsBlockParameterOld();

    uint8_t* parse(uint8_t* in, uint8_t const* in_max, int* err);
    uint8_t* parse_map_item(uint8_t* in, uint8_t const* in_max, int64_t val, int* err);

    void clear();

    int64_t query_timeout;
    int64_t skew_timeout;
    int64_t snaplen;
    int64_t promisc;
    std::vector<cbor_bytes> interfaces;
    std::vector<cbor_bytes> server_addresses;
    /* std::vector<cbor_bytes> vlan_id; */ /* This is specified by the spec, but does not seem to be actually encoded */
    cbor_text filter;
    int64_t query_options;
    int64_t response_options;
    std::vector<cbor_text> accept_rr_types;
    std::vector<cbor_text> ignore_rr_types;
    int64_t max_block_qr_items;
    int64_t collect_malformed;
};

class cdnsPreamble
{
public:
    cdnsPreamble();

    ~cdnsPreamble();

    uint8_t* parse(uint8_t* in, uint8_t const* in_max, int* err);

    uint8_t* parse_map_item(uint8_t* in, uint8_t const* in_max, int64_t val, int* err);

    void clear();

    int64_t cdns_version_major;
    int64_t cdns_version_minor;
    int64_t cdns_version_private;
    std::vector<cdnsBlockParameter> block_parameters;
    cdnsBlockParameterOld old_block_parameters;
    cbor_text old_generator_id;
    cbor_text old_host_id;
};


#define CNDS_INDEX_OFFSET 1

class cdns
{
public:
    cdns();

    ~cdns();

    bool open(char const* file_name);

    bool dump(char const* file_out);

    bool open_block(int* err);

    bool is_first_block() {
        return nb_blocks_read == 1;
    }

    bool is_last_block() {
        return nb_blocks_read == nb_blocks_present;
    }

    bool is_old_version() {
        return (preamble_parsed && preamble.cdns_version_major == 0);
    }

    int64_t get_ticks_per_second(int64_t block_id);

    int64_t ticks_to_microseconds(int64_t ticks, int64_t block_id);

    static int get_dns_flags(int q_dns_flags, bool is_response);
    static int get_edns_flags(int q_dns_flags);


    static uint8_t* dump_query(uint8_t* in, const uint8_t* in_max, char* out_buf, char* out_max, int cdns_version, int* err, FILE* F_out);

    cdnsPreamble preamble;
    cdnsBlock block; /* Current block */
    uint64_t first_block_start_us;
    int index_offset;

private:
    FILE* F;
    uint8_t* buf;
    size_t buf_size;
    size_t buf_read;
    size_t buf_parsed;
    bool end_of_file;
    bool preamble_parsed;
    bool file_head_undef;
    bool block_list_undef;
    int64_t nb_blocks_present;
    int64_t nb_blocks_read;

    bool load_entire_file();
    bool read_preamble(int* err); /* Leaves nb_read pointing to the beginning of the 1st block */



    uint8_t* dump_preamble(uint8_t* in, uint8_t* in_max, char* out_buf, char* out_max, int* cdns_version, int* err, FILE* F_out);
    uint8_t* dump_block_parameters(uint8_t* in, uint8_t* in_max, char* out_buf, char* out_max, int cdns_version, int* err, FILE* F_out);
    uint8_t* dump_block_parameters_rfc(uint8_t* in, uint8_t* in_max, char* out_buf, char* out_max, int* err, FILE* F_out);
    uint8_t* dump_block_parameters_storage(uint8_t* in, uint8_t* in_max, char* out_buf, char* out_max, int* err, FILE* F_out);
    uint8_t* dump_block_parameters_collection(uint8_t* in, uint8_t* in_max, char* out_buf, char* out_max, int* err, FILE* F_out);
    uint8_t* dump_block(uint8_t* in, uint8_t* in_max, char* out_buf, char* out_max, int cdns_version, int* err, FILE* F_out);
    uint8_t* dump_block_properties(uint8_t* in, uint8_t* in_max, char* out_buf, char* out_max, int cdns_version, int* err, FILE* F_out);
    uint8_t* dump_block_tables(uint8_t* in, uint8_t* in_max, char* out_buf, char* out_max, int cdns_version, int* err, FILE* F_out);
    uint8_t* dump_queries(uint8_t* in, uint8_t* in_max, char* out_buf, char* out_max, int cdns_version, int* err, FILE* F_out);
    uint8_t* dump_class_types(uint8_t* in, uint8_t* in_max, char* out_buf, char* out_max, int* err, FILE* F_out);
    uint8_t* dump_class_type(uint8_t* in, uint8_t* in_max, char* out_buf, char* out_max, int* err, FILE* F_out);
    uint8_t* dump_qr_sigs(uint8_t* in, uint8_t* in_max, char* out_buf, char* out_max, int cdns_version, int* err, FILE* F_out);
    uint8_t* dump_qr_sig(uint8_t* in, uint8_t* in_max, char* out_buf, char* out_max, int cdns_version, int* err, FILE* F_out);

    uint8_t* dump_list(uint8_t* in, uint8_t* in_max, char* out_buf, char* out_max, char const* indent, char const* list_name, int* err, FILE* F_out);
};

#endif

