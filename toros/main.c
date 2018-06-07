#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>

//
// product structs
//
struct PObject {};
struct PDatime {
    struct PObject object;
    uint32_t raw;
};

struct PFileHeader {
    struct PObject object;
    uint32_t version;
    uint32_t begin;
    uint32_t end;
    uint64_t seek_free;
    uint32_t nbytes_free;
    uint32_t nfree;
    uint32_t nbytes_name;
    char units;
    uint32_t compress;
    uint64_t seek_info;
    uint32_t nbytes_info;
};

struct PKey {
    struct PObject object;
    uint32_t total_bytes;
    int32_t version;
    uint32_t obj_bytes;
    uint32_t PDatime date_time;
    uint16_t cycle;
    uint64_t seek_key;
    uint64_t seek_pdir;
    char *class_name;
    char *obj_name;
    char *obj_title;
};

struct PDirectory {
    struct PObject object;
    struct PDatime date_time_c;
    struct PDatime date_time_m;
    uint32_t nbytes_keys;
    uint32_t nbytes_name;
    uint64_t seek_dir;
    uint64_t seek_parent;
    uint64_t seek_keys;
};

struct FileContext {
    FILE *pfile;
};

//
// buffer functionality
//
struct Buffer {
    size_t nbytes;
    char *buffer;
};

void swap_16(char *src, char *dest) {
    dest[0] = src[1];
    dest[1] = src[0];
}

void swap_32(char *src, char *dest) {
    dest[0] = src[3];
    dest[1] = src[2];
    dest[2] = src[1];
    dest[3] = src[0];
}

void swap_64(char *src, char *dest) {
    dest[0] = src[7];
    dest[1] = src[6];
    dest[2] = src[5];
    dest[3] = src[4];
    dest[4] = src[3];
    dest[5] = src[2];
    dest[6] = src[1];
    dest[7] = src[0];
}

uint16_t get_u16(char *ptr) {
    uint16_t value;
    swap_16(ptr, (char*)&value);
    return value;
}

int16_t get_i16(char *ptr) {
    return (int16_t)get_u16(ptr);
}

int32_t get_i32(char *ptr) {
    int value;
    swap_32(ptr, (char*)&value);
    return value;
}

uint32_t get_u32(char *ptr) {
    return (uint32_t)get_int(ptr);
}

uint64_t get_u64(char *ptr) {
    uint64_t value;
    swap_64(ptr, (char*)&value);
    return value;
}

int64_t get_i64(char *ptr) {
    return (int64_t)get_u64(ptr);
}

double get_double(char *ptr) {
    return (double)get_long(ptr);
}

float get_float(char *ptr) {
    return (float)get_int(ptr);
}

//
// various printing of buffers
//
void dump_buffer(struct Buffer buf, int nbytes_per_row) {
    printf("dumping buffer of size = %lu\n", buf.nbytes);
    for (size_t i=0; i<nbytes_per_row*3; i++)
        printf("-");
    printf("\n");
    for (size_t i=0; i<buf.nbytes; i++) {
        if (i%nbytes_per_row == 0 && i>0)
            printf("\n");
        printf("%02X ", buf.buffer[i]);
    }
    printf("\n");
}

void dump_raw(char *buf, int size, int nbytes_per_row) {
    printf("dumping buffer of size = %d\n", size);
    for (size_t i=0; i<nbytes_per_row*3; i++)
        printf("-");
    printf("\n");
    for (size_t i=0; i<size; i++) {
        if (i%nbytes_per_row == 0 && i>0)
            printf("\n");
        printf("%02X ", buf[i]);
    }
    printf("\n");
}

//
// builder functions
//
void build_file_header(struct Buffer buffer) {
    if (strncmp(buffer.buffer, "root", 4)) {
        printf("not a root file\n");
        assert(0);
    }
    buffer.buffer+=4;
    dump_buffer(buffer, 20);

    uint32_t version = get_uint32(buffer.buffer);
    printf("version = %d\n", version);
    buffer.buffer+=4;
    dump_buffer(buffer, 20);

    uint32_t begin = get_uint32(buffer.buffer);
    printf("begin = %d\n", begin);
    buffer.buffer+=4;

    return;
}

//
// some logic
//
void get_top_dir(struct FileContext ctx) {
    char buffer[300];
    size_t nbytes = fread((void*)buffer, 1, 300, ctx.pfile);
    struct Buffer buf = {nbytes, buffer};

    printf("read nbytes = %lu\n", nbytes);
    dump_buffer(buf, 20);

    build_file_header(buf);
}

void debug(struct FileContext ctx) {
    get_top_dir(ctx);
}

void test(char* filename) {
    printf("filename = %s\n", filename);

    // init the context
    struct FileContext ctx;
    ctx.pfile = fopen(filename, "r");

    debug(ctx);
}

int main(int argc, char ** argv) {
    char *filename = argv[1];

    test(filename);
}
