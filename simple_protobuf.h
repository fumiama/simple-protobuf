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

uint8_t first_set(uint64_t n) {
    uint8_t i = 0;
    while(!(n & 1)) {
        n >>= 1;
        i++;
    }
    return i;
}

void align_struct(uint8_t* items_type, uint64_t items_cnt, uint64_t struct_size) {
    uint64_t sum = 0;
    uint64_t min_cnt = 0;
    uint8_t min = 255;
    for(uint64_t i = 0; i < items_cnt; i++) {
        sum += 1u << items_type[i];
        if(min > items_type[i]) min = items_type[i];
    }
    while(sum < struct_size) {
        uint8_t new_min = 255;
        sum = 0;
        for(uint64_t i = 0; i < items_cnt; i++) {
            if(items_type[i] == min) items_type[i]++;
            if(new_min > items_type[i]) new_min = items_type[i];
            sum += 1u << items_type[i];
        }
    }
}

#endif