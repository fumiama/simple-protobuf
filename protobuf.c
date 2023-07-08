#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "simple_protobuf.h"

// read_num error when *offset < 0
static inline size_t read_num(FILE* fp, long* offset) {
    size_t c, n = 0;
    do {
        c = fgetc(fp);
        if (c < 0) {
            *offset = -1;
            return 0;
        }
        if (feof(fp)) return n;
        else n |= (c & 0x7f) << (7 * (*offset)++);
    } while ((c & 0x80));
    return n;
}

// peek_num error when *offset < 0
static inline size_t peek_num(FILE* fp, long* offset) {
    size_t c, n = 0;
    do {
        c = fgetc(fp);
        if (c < 0) {
            *offset = -1;
            return 0;
        }
        if (feof(fp)) return n;
        else n |= (c & 0x7f) << (7 * (*offset)++);
    } while ((c & 0x80));
    if (fseek(fp, -*offset, SEEK_CUR)) {
        *offset = -2;
        return 0;
    }
    return n;
}

// write_num returns offset
static inline long write_num(FILE* fp, size_t n) {
    if (n == 0) {
        if (fputc(0, fp) < 0) return -1;
        return 1;
    }
    long i = 0;
    while (n > 0) {
        int ch = n & 0x7f;
        if ((n >> 7) > 0) ch |= 0x80;
        if (fputc(ch, fp) < 0) return -2;
        n >>= 7;
        i++;
    }
    return i;
}

simple_pb_t* get_pb(FILE* fp) {
    long init_pos = ftell(fp);
    if (init_pos < 0) return NULL;
    long i = 0;
    uint32_t struct_len = (uint32_t)read_num(fp, &i);
    if (i < 0) return NULL;
    if (struct_len <= 1 || struct_len >= MAX_SIMPLE_PB_STRUCT_LEN) return NULL; // 1B<struct_len<1MB
    simple_pb_t* spb = malloc(struct_len + 2*sizeof(uint32_t));
    if (!spb) return NULL;
    spb->struct_len = struct_len;
    memset(spb->target, 0, struct_len);
    uint32_t offset, data_len;
    for (char* p = spb->target; p < spb->target+struct_len; p += offset) {
        i = 0;
        offset = (uint32_t)read_num(fp, &i);
        if (i < 0) return NULL;
        i = 0;
        data_len = (uint32_t)read_num(fp, &i);
        if (i < 0) return NULL;
        if (data_len == 0) continue;
        if (data_len <= offset) fread(p, data_len, 1, fp);
        else return NULL;
    }
    spb->real_len = ftell(fp) - init_pos;
    return spb;
}

// get_pb_len returns 0 on error
uint32_t get_pb_len(FILE* fp) {
    long i = 0;
    uint32_t struct_len = (uint32_t)peek_num(fp, &i);
    if (i < 0) return 0;
    if (struct_len <= 1 || struct_len >= MAX_SIMPLE_PB_STRUCT_LEN) return 0; // 1B<struct_len<1MB
    return struct_len + 2*sizeof(uint32_t);
}

simple_pb_t* read_pb_into(FILE* fp, simple_pb_t* spb) {
    if(!spb) return NULL;
    long init_pos = ftell(fp);
    long i = 0;
    uint32_t struct_len = (uint32_t)read_num(fp, &i);
    if (i < 0) return NULL;
    if (struct_len <= 1 || struct_len >= MAX_SIMPLE_PB_STRUCT_LEN) return NULL; // 1B<struct_len<1MB
    spb->struct_len = struct_len;
    memset(spb->target, 0, struct_len);
    uint32_t offset, data_len;
    for(char* p = spb->target; p < spb->target+struct_len; p += offset) {
        i = 0;
        offset = (uint32_t)read_num(fp, &i);
        if (i < 0) return NULL;
        i = 0;
        data_len = (uint32_t)read_num(fp, &i);
        if (i < 0) return NULL;
        if (data_len == 0) continue;
        if (data_len <= offset) fread(p, data_len, 1, fp);
    }
    spb->real_len = ftell(fp) - init_pos;
    return spb;
}

long set_pb(FILE* fp, const uint32_t* items_len, uint32_t struct_len, const void* target) {
    uint32_t offset = 0;
    long i = 0;
    char* p = (char*)target;
    if (write_num(fp, struct_len) <= 0) return -1;
    while (offset < struct_len) {
        uint32_t data_len = items_len[i++];
        if (write_num(fp, data_len) <= 0) return -2;
        char* this = p + offset;
        offset += data_len;
        if (data_len > 1) while (data_len > 0 && !this[data_len - 1]) data_len--;
        if (write_num(fp, data_len) <= 0) return -3;
        if (data_len > 0) if (fwrite(this, data_len, 1, fp) <= 0) return -4;
    }
    return i;
}

// items_len = align_struct(uint32_t struct_size, uint32_t items_cnt, void* item_addr1, void* item_addr2...)
uint32_t* align_struct(uint32_t struct_size, uint32_t items_cnt, ...) {
    va_list list;
    va_start(list, items_cnt);
    uint32_t* items_len = malloc(struct_size*sizeof(uint32_t));
    if (items_len) {
        void* this;
        void* next = va_arg(list, void*);
        void* end = next + struct_size;
        for (uint32_t i = 0; i < items_cnt - 1; i++) {
            this = next;
            next = va_arg(list, void*);
            items_len[i] = next - this;
        }
        items_len[items_cnt-1] = end - next;
    }
    va_end(list);
    return items_len;
}

// items_len = align_struct_into(uint32_t struct_size, uint32_t* items_len, uint32_t items_cnt, void* item_addr1, void* item_addr2...)
uint32_t* align_struct_into(uint32_t struct_size, uint32_t* items_len, uint32_t items_cnt, ...) {
    va_list list;
    va_start(list, items_cnt);
    if (items_len) {
        void* this;
        void* next = va_arg(list, void*);
        void* end = next + struct_size;
        for (uint32_t i = 0; i < items_cnt - 1; i++) {
            this = next;
            next = va_arg(list, void*);
            items_len[i] = next - this;
        }
        items_len[items_cnt-1] = end - next;
    }
    va_end(list);
    return items_len;
}
