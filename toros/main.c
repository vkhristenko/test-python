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

uint16_t get_u16(char ** ptr) {
    uint16_t value;
    swap_16(*ptr, (char*)&value);
    *ptr+=2;
    return value;
}

int16_t get_i16(char **ptr) {
    return  (int16_t)get_u16(ptr);
}

int32_t get_i32(char **ptr) {
    int value;
    swap_32(*ptr, (char*)&value);
    *ptr+=4;
    return value;
}

uint32_t get_u32(char ** ptr) {
    return (uint32_t)get_i32(ptr);
}

uint64_t get_u64(char **ptr) {
    uint64_t value;
    swap_64(*ptr, (char*)&value);
    *ptr+=8;
    return value;
}

int64_t get_i64(char **ptr) {
    return (int64_t)get_u64(ptr);
}

double get_f64(char **ptr) {
    return (double)get_u64(ptr);
}

float get_f32(char **ptr) {
    return (float)get_u32(ptr);
}

uint32_t get_version(char **ptr) {
    uint16_t v = get_u16(ptr);
    if ((v & 0x4000) == 0) 
        return (uint32_t)v;

    uint32_t nbytes = ((v & 0x3fff) << 16) + get_u16(ptr);
    uint16_t res = get_u16(ptr);
    return res;
}

void get_string(char **src, char **dest) {
    int8_t size = **src; (*src)++;

    // If the string is empty
    if (size == 0) {
        (*dest) = malloc(1);
        (*dest)[0] = '\0';
        return;
    }
    
    // if more input is needed
    if (size == -1) {
        int size = get_i32(src); 
        (*dest) = malloc(size);
        memcpy(*dest, *src, size);
        src+=size;
        return;
    }

    // allocate size bytes and perform mem copy
    (*dest) = malloc(size);
    memcpy(*dest, *src, size);
    *src+=size;
}

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

#define print_u16(value) \
    printf(#value" = %hu\n", value)

#define print_i16(value) \
    printf(#value" = %hu\n", value)

#define print_string(value) \
    printf(#value" = %s\n", value);


//
// product structs
//
struct PObject {};
struct PDatime {
    struct PObject object;
    uint32_t raw;
};

struct PNamed {
    struct PObject object;
    char *name;
    char *title;
};

void print_named(struct PNamed *pnamed) {
    printf("Product <PNamed>:\n");
    printf("-------------------\n");
    print_string(pnamed->name);
    print_string(pnamed->title);
}

void print_datime(struct PDatime *pdatime) {
    printf("Product <PDatime>:\n");
    printf("-------------------\n");
    print_u32(pdatime->raw);
}

void from_buf_datime(char **buffer, struct PDatime *pdatime) {
    pdatime->raw = get_u32(buffer);
}

void ctor_datime(struct PDatime *pdatime) {}

void dtor_datime(struct PDatime *pdatime) {}

void from_buf_named(char **buffer, struct PNamed *pnamed) {
    get_string(buffer, &pnamed->name);
    get_string(buffer, &pnamed->title);
}

void ctor_named(struct PNamed *pnamed) {}

void dtor_named(struct PNamed *pnamed) {
    free(pnamed->name);
    free(pnamed->title);
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

void ctor_file_header(struct PFileHeader *pheader) {}

void dtor_file_header(struct PFileHeader *pheader) {}

void from_buf_file_header(char **buffer, struct PFileHeader *pheader) {
    printf("builing a file header\n");

    if (strncmp(*buffer, "root", 4)) {
        printf("not a root file\n");
        assert(0);
    }
    *buffer+=4;
    dump_raw(*buffer, 100, 20);

    pheader->version = get_u32(buffer);
    printf("version = %d\n", pheader->version);
    dump_raw(*buffer, 100, 20);

    pheader->begin = get_u32(buffer);
    printf("begin = %d\n", pheader->begin);

    if (pheader->version > 1000000u) {
        pheader->end = get_u64(buffer);
        pheader->seek_free = get_u64(buffer);
    }
    pheader->nbytes_free = get_u32(buffer);
    pheader->nfree = get_u32(buffer);
    pheader->nbytes_name = get_u32(buffer);
    pheader->units = **buffer; (*buffer)++; // manually change the buffer pointer
    pheader->compress = get_u32(buffer);
    pheader->seek_info = get_u64(buffer);
    pheader->nbytes_info = get_u32(buffer);
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
    uint16_t key_bytes;
    uint16_t cycle;
    uint64_t seek_key;
    uint64_t seek_pdir;
    char *class_name;
    char *obj_name;
    char *obj_title;
};

void print_key(struct PKey *pkey) {
    printf("Product <PKey>:\n");
    printf("----------------\n");
    print_u32(pkey->total_bytes);
    print_u32(pkey->version);
    print_u32(pkey->obj_bytes);
    print_datime(&(pkey->date_time));
    print_u16(pkey->key_bytes);
    print_u16(pkey->cycle);
    print_u64(pkey->seek_key);
    print_u64(pkey->seek_pdir);
    print_string(pkey->class_name);
    print_string(pkey->obj_name);
    print_string(pkey->obj_title);
}

void ctor_key(struct PKey *pkey) {}

void dtor_key(struct PKey *pkey) {
    free(pkey->class_name);
    free(pkey->obj_name);
    free(pkey->obj_title);
}

void from_buf_key(char **buffer, struct PKey *pkey) {
    pkey->total_bytes = get_u32(buffer);
    pkey->version = get_version(buffer);
    pkey->obj_bytes = get_u32(buffer);
    from_buf_datime(buffer, &(pkey->date_time));
    pkey->key_bytes = get_u16(buffer);
    pkey->cycle = get_u16(buffer);
    if (pkey->version > 1000) {
        pkey->seek_key = get_u64(buffer);
        pkey->seek_pdir = get_u64(buffer);
    } else {
        pkey->seek_key = get_u32(buffer);
        pkey->seek_pdir = get_u32(buffer);
    }
    get_string(buffer, &(pkey->class_name));
    get_string(buffer, &(pkey->obj_name));
    get_string(buffer, &(pkey->obj_title));
}

//
// directory product
//
struct PDirectory {
    struct PObject object;
    uint32_t version;
    struct PDatime date_time_c;
    struct PDatime date_time_m;
    uint32_t nbytes_keys;
    uint32_t nbytes_name;
    uint64_t seek_dir;
    uint64_t seek_parent;
    uint64_t seek_keys;
};

void ctor_dir(struct PDirectory *pdir) {}

void dtor_dir(struct PDirectory *pdir) {}

void print_dir(struct PDirectory *pdir) {
    printf("Product <PDirectory>:\n");
    printf("-----------------------\n");
    print_u32(pdir->version);
    print_datime(&(pdir->date_time_c));
    print_datime(&(pdir->date_time_m));
    print_u32(pdir->nbytes_keys);
    print_u32(pdir->nbytes_name);
    print_u64(pdir->seek_dir);
    print_u64(pdir->seek_parent);
    print_u64(pdir->seek_keys);
}

void from_buf_dir(char **buffer, struct PDirectory *pdir) {
    pdir->version = get_version(buffer);
    from_buf_datime(buffer, &(pdir->date_time_c)); // ptr is taken care of
    from_buf_datime(buffer, &(pdir->date_time_m)); // same
    pdir->nbytes_keys = get_u32(buffer);
    pdir->nbytes_name = get_u32(buffer);
    if (pdir->version > 1000) {
        pdir->seek_dir = get_u64(buffer);
        pdir->seek_parent = get_u64(buffer);
        pdir->seek_keys = get_u64(buffer);
    } else {
        pdir->seek_dir = get_u32(buffer);
        pdir->seek_parent = get_u32(buffer);
        pdir->seek_keys = get_u32(buffer);
    }
}

void list_keys(struct FileContext ctx, struct PDirectory *pdir, 
               struct PKey ** pkeys, int *pnkeys) {
    // read into the buffer
    char *buffer = malloc(pdir->nbytes_keys);
    fseek(ctx.pfile, pdir->seek_keys, SEEK_SET);
    size_t nbytes = fread((void*)buffer, 1, pdir->nbytes_keys, ctx.pfile);

    // tlist key
    struct PKey key;
    ctor_key(&key);
    from_buf_key(&buffer, &key);
    print_key(&key);
    printf("\n");

    
    // get how many keys we have and allocate enough space on heap
    *pnkeys = get_u32(&buffer);
    *pkeys = malloc(sizeof(struct PKey)*(*pnkeys));

    // iterate, construct, cast from from the in-memory buffer
    for (int i=0; i<(*pnkeys); i++) {
        // construct the i-th key in the allocate space
        ctor_key(&((*pkeys)[i]));
        // cast 
        from_buf_key(&buffer, &((*pkeys)[i]));
    }
}

//
// some logic
//
void get_top_dir(struct FileContext ctx, struct PDirectory *pdir) {
    char *buffer = malloc(300);
    size_t nbytes = fread((void*)buffer, 1, 300, ctx.pfile);
    char *start = buffer;

    printf("read nbytes = %lu\n", nbytes);
    dump_raw(buffer, nbytes, 20);
    dump_raw(buffer, nbytes, 20);
    dump_raw(buffer, nbytes, 20);

    // get the file header
    struct PFileHeader header;
    ctor_file_header(&header);
    from_buf_file_header(&buffer, &header);
    print_file_header(&header);

    printf("\n");

    // top dir key
    start+=100;
    struct PKey key;
    ctor_key(&key);
    from_buf_key(&start, &key);
    print_key(&key);
    printf("\n");

    // get named stuff
    struct PNamed named;
    ctor_named(&named);
    from_buf_named(&start, &named);
    print_named(&named);
    printf("\n");

    // dir
    from_buf_dir(&start, pdir);
    print_dir(pdir);
    printf("\n");
}

void get_blob(struct FileContext ctx, struct PKey *pkey, char **blob) {
    // read in the pre-allocated blob
    fseek(ctx.pfile, pkey->seek_key, SEEK_SET);
    size_t nbytes = fread((void*)*blob, 1, pkey->total_bytes, ctx.pfile);

    struct PKey key;
    ctor_key(&key);
    from_buf_key(blob, &key);
}

void dump_contents(struct FileContext ctx) {
    // get top dir
    struct PDirectory dir;
    ctor_dir(&dir);
    get_top_dir(ctx, &dir);

    // list keys in this dir
    struct PKey *pkeys; 
    int nkeys;
    list_keys(ctx, &dir, &pkeys, &nkeys);
    for (int i=0; i<nkeys; i++) {
        print_key(&(pkeys[i]));
        printf("\n");
    }
}

void test(char* filename) {
    printf("filename = %s\n", filename);

    // init the context
    struct FileContext ctx;
    ctx.pfile = fopen(filename, "r");

    dump_contents(ctx);
}

int main(int argc, char ** argv) {
    char *filename = argv[1];

    test(filename);
}
