#ifndef _SIMPLE_PROTOBUF_H_
#define _SIMPLE_PROTOBUF_H_

#include <stdio.h>
#include <stdint.h>

struct SIMPLE_PB {
    uint32_t struct_len, real_len;
    char target[];
};
typedef struct SIMPLE_PB SIMPLE_PB;

SIMPLE_PB* get_pb(FILE* fp);

uint32_t get_pb_len(FILE* fp);

SIMPLE_PB* read_pb_into(FILE* fp, SIMPLE_PB* spb);

int set_pb(FILE* fp, uint32_t* items_len, uint32_t struct_len, void* target);

// items_len = align_struct(uint32_t struct_size, uint32_t items_cnt, void* item_addr1, void* item_addr2...)
uint32_t* align_struct(uint32_t struct_size, uint32_t items_cnt, ...);

#endif