#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include "simple_protobuf.h"

static uint32_t read_num(FILE* fp) {
    uint8_t c;
    uint32_t n = 0;
    uint8_t i = 0;
    do {
        c = fgetc(fp);
        n |= (c & 0x7f) << (7 * i++);
    } while(c & 0x80);
    return n;
}

static int write_num(FILE* fp, uint32_t n) {
    char* c = (char*)(&n);
    int i = 0;
    while(n > 0) {
        #ifdef WORDS_BIGENDIAN
            int ch = c[3] & 0x7f;
        #else
            int ch = *c & 0x7f;
        #endif
        if((n >> 7) > 0) ch |= 0x80;
        fputc(ch, fp);
        n >>= 7;
        i++;
    }
    return i;
}

SIMPLE_PB* get_pb(FILE* fp) {
    uint32_t init_pos = ftell(fp);
    uint32_t struct_len = read_num(fp);
    if(struct_len > 1) {
        SIMPLE_PB* spb = malloc(struct_len + sizeof(uint32_t));
        if(spb) {
            spb->struct_len = struct_len;
            spb->real_len = 0;
            char* p = spb->target;
            char* end = p + struct_len;
            memset(p, 0, struct_len);
            while(p < end) {
                uint32_t offset = read_num(fp);
                uint32_t data_len = read_num(fp);
                fread(p, data_len, 1, fp);
                p += offset;
            }
            spb->real_len = ftell(fp) - init_pos;
            return spb;
        }
    }
    return NULL;
}

int set_pb(FILE* fp, uint32_t* items_len, uint32_t struct_len, void* target) {
    uint32_t offset = 0;
    uint32_t i = 0;
    char* p = (char*)target;
    write_num(fp, struct_len);
    while(offset < struct_len) {
        uint32_t data_len = items_len[i++];
        write_num(fp, data_len);
        char* this = p + offset;
        offset += data_len;
        if(data_len > 1) while(!this[data_len - 1]) data_len--;
        write_num(fp, data_len);
        fwrite(this, data_len, 1, fp);
    }
    return i;
}

//uint32_t struct_size, uint32_t items_cnt, void* item_addr1, void* item_addr2...
uint32_t* align_struct(uint32_t struct_size, uint32_t items_cnt, ...) {
    va_list list;
    va_start(list, items_cnt);
    uint32_t* items_len = malloc(struct_size*sizeof(uint32_t));
    if(items_len) {
        void* this;
        void* next = va_arg(list, void*);
        void* end = next + struct_size;
        for(uint32_t i = 0; i < items_cnt - 1; i++) {
            this = next;
            next = va_arg(list, void*);
            items_len[i] = next - this;
        }
        items_len[items_cnt-1] = end - next;
    }
    va_end(list);
    return items_len;
}
