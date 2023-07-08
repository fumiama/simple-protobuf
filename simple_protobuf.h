#ifndef _SIMPLE_PROTOBUF_H_
#define _SIMPLE_PROTOBUF_H_

#include <stdio.h>
#include <stdint.h>

#define MAX_SIMPLE_PB_STRUCT_LEN (1ull<<20)

struct simple_pb_t {
    uint32_t struct_len, real_len;
    char target[];
};
typedef struct simple_pb_t simple_pb_t;

simple_pb_t* get_pb(FILE* fp);

// get_pb_len returns 0 on error
uint32_t get_pb_len(FILE* fp);

simple_pb_t* read_pb_into(FILE* fp, simple_pb_t* spb);

long set_pb(FILE* fp, const uint32_t* items_len, uint32_t struct_len, const void* target);

// items_len = align_struct(uint32_t struct_size, uint32_t items_cnt, void* item_addr1, void* item_addr2...)
uint32_t* align_struct(uint32_t struct_size, uint32_t items_cnt, ...);

// items_len = align_struct_into(uint32_t struct_size, uint32_t* items_len, uint32_t items_cnt, void* item_addr1, void* item_addr2...)
uint32_t* align_struct_into(uint32_t struct_size, uint32_t* items_len, uint32_t items_cnt, ...);

#endif
