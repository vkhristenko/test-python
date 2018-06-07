#include <stdio.h>
#include <inttypes.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>

//
// file context
//
struct FileContext {
    FILE *pfile;
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
    return (uint32_t)get_i32(ptr);
}

uint64_t get_u64(char *ptr) {
    uint64_t value;
    swap_64(ptr, (char*)&value);
    return value;
}

int64_t get_i64(char *ptr) {
    return (int64_t)get_u64(ptr);
}

double get_f64(char *ptr) {
    return (double)get_u64(ptr);
}

float get_f32(char *ptr) {
    return (float)get_u32(ptr);
}

uint32_t get_version(char *ptr) {
    uint16_t v = get_u16(ptr); ptr+=2;
    if ((v & 0x4000) == 0) 
        return (uint32_t)v;

    uint32_t nbytes = ((v & 0x3fff) << 16) + get_u16(ptr); ptr+=2;
    uint16_t res = get_u16(ptr); ptr+=2;
    return res;
}

/*
void get_string(char *src, char *dest) {
    uint8_t size = (uint8_t)*ptr; ptr++;
    if (size == 0)
        dest = "";
    else if (size == -1) {
        uint32_t size = get_u32(ptr); ptr+=4;
        dest = 
        memcpy(dest, src, size);
    } else 
}*/

// 
// bdebug stuff
//
struct Buffer {
    size_t nbytes;
    char *buffer;
};
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
// some macros
//

#define print_char(value) \
    printf(#value" = %c\n", value)

#define print_u32(value) \
    printf(#value" = %u\n", value)

#define print_u64(value) \
    printf(#value" = %"PRIu64"\n", value)

#define print_i32(value) \
    printf(#value" = %d\n", value)

#define print_i64(value) \
    printf(#value" = %"PRId64"\n", value)

#define print_f32(value) \
    printf(#value" = %f\n", value)

#define print_f64(value) \
    printf(#value" = %lf\n", value)


//
// product structs
//
struct PObject {};
struct PDatime {
    struct PObject object;
    uint32_t raw;
};

struct PDatime build_datime(char *buffer) {
    struct PDatime datime;
    datime.raw = get_u32(buffer); buffer+=4;
    return datime;
}

//
// file header 
//
struct PFileHeader {
    struct PObject object;
    uint32_t version;
    uint32_t begin;
    uint64_t end;
    uint64_t seek_free;
    uint32_t nbytes_free;
    uint32_t nfree;
    uint32_t nbytes_name;
    char units;
    uint32_t compress;
    uint64_t seek_info;
    uint32_t nbytes_info;
};

void print_file_header(struct PFileHeader *pheader) {
    printf("Product <PFileHeader>:\n");
    printf("-----------------------\n");
    print_u32(pheader->version);
    print_u32(pheader->begin);
    print_u64(pheader->end);
    print_u64(pheader->seek_free);
    print_u32(pheader->nbytes_free);
    print_u32(pheader->nfree);
    print_u32(pheader->nbytes_name);
    print_char(pheader->units);
    print_u32(pheader->compress);
    print_u64(pheader->seek_info);
    print_u32(pheader->nbytes_info);
}

struct PFileHeader build_file_header(struct Buffer buffer) {
    if (strncmp(buffer.buffer, "root", 4)) {
        printf("not a root file\n");
        assert(0);
    }
    buffer.buffer+=4;
    dump_buffer(buffer, 20);

    struct PFileHeader header;

    header.version = get_u32(buffer.buffer);
    printf("version = %d\n", header.version);
    buffer.buffer+=4;
    dump_buffer(buffer, 20);

    header.begin = get_u32(buffer.buffer);
    printf("begin = %d\n", header.begin);
    buffer.buffer+=4;

    if (header.version > 1000000u) {
        header.end = get_u64(buffer.buffer); buffer.buffer+=8;
        header.seek_free = get_u64(buffer.buffer); buffer.buffer+=8;
    }
    header.nbytes_free = get_u32(buffer.buffer); buffer.buffer+=4;
    header.nfree = get_u32(buffer.buffer); buffer.buffer+=4;
    header.nbytes_name = get_u32(buffer.buffer); buffer.buffer+=4;
    header.units = *(buffer.buffer); buffer.buffer++;
    header.compress = get_u32(buffer.buffer); buffer.buffer+=4;
    header.seek_info = get_u64(buffer.buffer); buffer.buffer+=8;
    header.nbytes_info = get_u32(buffer.buffer); buffer.buffer+=4;

    return header;
}

//
// key product
//
struct PKey {
    struct PObject object;
    uint32_t total_bytes;
    int32_t version;
    uint32_t obj_bytes;
    struct PDatime date_time;
    uint16_t cycle;
    uint64_t seek_key;
    uint64_t seek_pdir;
    char *class_name;
    char *obj_name;
    char *obj_title;
};

//
// directory product
//
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

//
// some logic
//
void get_top_dir(struct FileContext ctx) {
    char buffer[300];
    size_t nbytes = fread((void*)buffer, 1, 300, ctx.pfile);
    struct Buffer buf = {nbytes, buffer};

    printf("read nbytes = %lu\n", nbytes);
    dump_buffer(buf, 20);

    struct PFileHeader header = build_file_header(buf);
    print_file_header(&header);
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
