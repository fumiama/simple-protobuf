#ifndef _SIMPLE_PROTOBUF_H_
#define _SIMPLE_PROTOBUF_H_

#include <stdint.h>

struct SIMPLE_PB {
    uint64_t len;
    char target[];
};
typedef struct SIMPLE_PB SIMPLE_PB;

SIMPLE_PB* get_pb(FILE* fp);
int set_pb(FILE* fp, uint64_t* items_len, uint64_t struct_len, void* target);
uint8_t first_set(uint64_t n);
//uint64_t struct_size, uint32_t items_cnt, void* item_addr1, void* item_addr2...
uint64_t* align_struct(uint64_t struct_size, uint32_t items_cnt, ...);

#endif