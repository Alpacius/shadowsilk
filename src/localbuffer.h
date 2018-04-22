#pragma once

#include    "stdc_common.h"
#include    "list.h"

#define     SSLK_BUFLINE_MAGIC      0xdeadbeef

struct gapbuffer {
    char *bufstart, *bufend;
    char *gapstart, *gapend;
    char *loc;
    uint64_t size;
};

struct bufline {
    intrusive;
    uint32_t pos, cap;
    uint32_t magic;
    struct gapbuffer buf;
};

struct bufline *bufline_init(struct bufline *ln, uint32_t pos, uint32_t cap);
struct bufline *bufline_ruin(struct bufline *ln);
struct bufline *bufline_new(uint32_t pos, uint32_t cap);
void bufline_delete(struct bufline *ln);
struct bufline *bufline_insert_str(struct bufline *ln, uint64_t loc, const char *str);
struct bufline *bufline_delete_str(struct bufline *ln, uint64_t loc, uint64_t size);
