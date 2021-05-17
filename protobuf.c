#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "simple_protobuf.h"

//#define DEBUG

static uint64_t read_num(FILE* fp) {
    uint8_t c;
    uint64_t n = 0;
    uint8_t i = 0;
    do {
        c = fgetc(fp);
        n |= (c & 0x7f) << (7 * i++);
    } while(c & 0x80);
    return n;
}

static int write_num(FILE* fp, uint64_t n) {
    char* c = (char*)(&n);
    int i = 0;
    while(n > 0) {
        #ifdef WORDS_BIGENDIAN
            int ch = c[7] & 0x7f;
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
    uint64_t struct_len = read_num(fp);
    if(struct_len > 1) {
        SIMPLE_PB* spb = malloc(struct_len + sizeof(uint64_t));
        #ifdef DEBUG
            printf("Malloc %llu + %lu bytes.\n", struct_len, sizeof(uint64_t));
        #endif
        if(spb) {
            spb->len = struct_len;
            char* p = spb->target;
            char* end = p + struct_len;
            memset(p, 0, struct_len);
            while(p < end) {
                uint64_t offset = 1u << fgetc(fp);
                uint64_t data_len = read_num(fp);
                #ifdef DEBUG
                    printf("Offset: %llu, data_len: %llu.\n", offset, data_len);
                #endif
                fread(p, data_len, 1, fp);
                p += offset;
            }
            return spb;
        }
    }
    return NULL;
}

int set_pb(FILE* fp, uint8_t* items_type, uint64_t struct_len, void* target) {
    uint64_t offset = 0;
    uint32_t i = 0;
    char* p = (char*)target;
    write_num(fp, struct_len);
    #ifdef DEBUG
        printf("struct_len: %llu bytes.\n", struct_len);
    #endif
    while(offset < struct_len) {
        uint8_t type = items_type[i++];
        uint64_t data_len = 1u << type;
        fputc(type, fp);
        char* this = p + offset;
        offset += data_len;
        if(data_len > 1) while(!this[data_len - 1]) data_len--;
        write_num(fp, data_len);
        fwrite(this, data_len, 1, fp);
    }
    return i;
}