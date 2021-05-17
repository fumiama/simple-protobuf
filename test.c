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
uint64_t items_len[5] = {sizeof(t.a), sizeof(t.b), sizeof(t.c), sizeof(t.d), sizeof(t.e)};
uint8_t types_len[5];

int main() {
    t.a = 0xAB;
    t.b = 63345;
    t.c = 1234567890;
    t.d = 1234567898765435432ULL;
    strcpy(t.e, "Hello world! This is a message from simple protobuf.");
    for(int i = 0; i < 5; i++) {
        types_len[i] = first_set(items_len[i]);
        printf("Item %d has type %d with size %llu\n", i, types_len[i], items_len[i]);
    }
    align_struct(types_len, 5, sizeof(struct TEST));
    for(int i = 0; i < 5; i++) {
        printf("Item %d's type after align: %u\n", i, types_len[i]);
    }
    FILE* fp = fopen("test.sp", "wb");
    if(fp) {
        set_pb(fp, types_len, sizeof(struct TEST), &t);
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