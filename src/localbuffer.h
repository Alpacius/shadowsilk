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
