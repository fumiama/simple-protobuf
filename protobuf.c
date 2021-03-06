#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "simple_protobuf.h"

static uint32_t read_num(FILE* fp) {
    uint32_t c, n = 0;
    long i = 0;
    do {
        c = fgetc(fp);
        if(feof(fp)) return n;
        else n |= (c & 0x7f) << (7 * i++);
    } while((c & 0x80));
    return n;
}

static uint32_t peek_num(FILE* fp) {
    uint32_t c, n = 0;
    long i = 0;
    do {
        c = fgetc(fp);
        if(feof(fp)) return n;
        else n |= (c & 0x7f) << (7 * i++);
    } while((c & 0x80));
    fseek(fp, -i, SEEK_CUR);
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
    if(struct_len <= 1 || struct_len >= 1u<<20) return NULL; // 1B<struct_len<1MB
    SIMPLE_PB* spb = malloc(struct_len + 2 * sizeof(uint32_t));
    if(!spb) return NULL;
    spb->struct_len = struct_len;
    memset(spb->target, 0, struct_len);
    uint32_t offset, data_len;
    for(char* p = spb->target; p < spb->target+struct_len; p += offset) {
        offset = read_num(fp);
        data_len = read_num(fp);
        if(data_len > 0 && data_len <= offset) fread(p, data_len, 1, fp);
    }
    spb->real_len = ftell(fp) - init_pos;
    return spb;
}

uint32_t get_pb_len(FILE* fp) {
    uint32_t init_pos = ftell(fp);
    uint32_t struct_len = peek_num(fp);
    if(struct_len <= 1 || struct_len >= 1u<<20) return 0; // 1B<struct_len<1MB
    return struct_len + 2*sizeof(uint32_t);
}

SIMPLE_PB* read_pb_into(FILE* fp, SIMPLE_PB* spb) {
    if(!spb) return NULL;
    uint32_t init_pos = ftell(fp);
    uint32_t struct_len = read_num(fp);
    if(struct_len <= 1 || struct_len >= 1u<<20) return NULL; // 1B<struct_len<1MB
    spb->struct_len = struct_len;
    memset(spb->target, 0, struct_len);
    uint32_t offset, data_len;
    for(char* p = spb->target; p < spb->target+struct_len; p += offset) {
        offset = read_num(fp);
        data_len = read_num(fp);
        if(data_len > 0 && data_len <= offset) fread(p, data_len, 1, fp);
    }
    spb->real_len = ftell(fp) - init_pos;
    return spb;
}

int set_pb(FILE* fp, const uint32_t* items_len, uint32_t struct_len, const void* target) {
    uint32_t offset = 0;
    uint32_t i = 0;
    char* p = (char*)target;
    write_num(fp, struct_len);
    while(offset < struct_len) {
        uint32_t data_len = items_len[i++];
        write_num(fp, data_len);
        char* this = p + offset;
        offset += data_len;
        if(data_len > 1) while(data_len > 0 && !this[data_len - 1]) data_len--;
        write_num(fp, data_len);
        if(data_len > 0) fwrite(this, data_len, 1, fp);
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
