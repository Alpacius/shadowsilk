#pragma once

#include    "stdc_common.h"

struct sslk_cursor {
    uint64_t locx, locy;
    char ref_imm[];
};

static inline
struct sslk_cursor *sslk_cursor_new(const char *refname) {
    size_t l = strlen(refname) + 1;
    struct sslk_cursor *cur = malloc(sizeof(struct sslk_cursor) + l);
    if (cur)
        (cur->locx = cur->locy = 0),
        memcpy(cur->ref_imm, refname, l);
    return cur;
}

static inline
struct sslk_cursor *sslk_cursor_delete(struct sslk_cursor *cur) {
    free(cur);
}
