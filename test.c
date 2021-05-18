#include <stdio.h>
#include <string.h>
#include "simple_protobuf.h"

struct TEST {
    uint8_t a;
    uint16_t b;
    uint32_t c;
    uint64_t d;
    char e[256];
};

struct TEST t;

int main() {
    t.a = 0xAB;
    t.b = 0xCDEF;
    t.c = 0xCCDDEE;
    t.d = 0xABCDEF12345678;
    strcpy(t.e, "Hello world! This is a message from simple protobuf.");
    uint32_t* items_len = align_struct(sizeof(struct TEST), 5, &t.a, &t.b, &t.c, &t.d, &t.e);
    for(int i = 0; i < 5; i++) {
        printf("Item %d has aligned size %u\n", i, items_len[i]);
    }
    FILE* fp = fopen("test.sp", "wb");
    if(fp) {
        set_pb(fp, items_len, sizeof(struct TEST), &t);
        memset(&t, 0, sizeof(struct TEST));
        fclose(fp);
        puts("Write file succeed.");
        fp = NULL;
        fp = fopen("test.sp", "rb");
        if(fp) {
            SIMPLE_PB* spb = get_pb(fp);
            memcpy(&t, spb->target, sizeof(struct TEST));
            printf("a:%u\nb:%u\nc:%u\nd:%llu\ne:%s\n", t.a, t.b, t.c, t.d, t.e);
        } else perror("[SPB]");
    } else perror("[SPB]");
}