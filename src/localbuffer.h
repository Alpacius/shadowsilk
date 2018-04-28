#pragma once

#include    <wchar.h>
#include    "stdc_common.h"
#include    "list.h"

#define     SSLK_BUFLINE_MAGIC      0xdeadbeef

struct gapbuffer {
    wchar_t *bufstart, *bufend;
    wchar_t *gapstart, *gapend;
    wchar_t *loc;
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
struct bufline *bufline_insert_str(struct bufline *ln, uint64_t loc, const wchar_t *str);
struct bufline *bufline_delete_str(struct bufline *ln, uint64_t loc, uint64_t size);

struct localbuf {
    intrusive_node(buflines) lines;
    struct {
        uint64_t locy;
        struct link_index *lnref;
    } quick_cursor;
    uint64_t size;
};
