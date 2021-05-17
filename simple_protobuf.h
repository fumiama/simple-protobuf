#ifndef _SIMPLE_PROTOBUF_H_
#define _SIMPLE_PROTOBUF_H_

#include <stdint.h>

struct SIMPLE_PB {
    uint64_t len;
    char target[];
};
typedef struct SIMPLE_PB SIMPLE_PB;

SIMPLE_PB* get_pb(FILE* fp);
int set_pb(FILE* fp, uint8_t* items_type, uint64_t struct_len, void* target);
uint8_t first_set(uint64_t n);
void align_struct(uint8_t* items_type, uint64_t items_cnt, uint64_t struct_size);

#endif