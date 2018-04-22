#pragma once

#include    "stdc_common.h"
#include    "miscutils.h"
#include    "list.h"

#define     DEFAULT_LOAD    0.7
#define     MAX_CAP         128

struct scntx_entry {
    void *key, *value;
    char key_imm[64];
};

struct scntx {
    uint64_t size, cap;
    int (*cmp)(void *, void *);
    uint64_t (*hash)(const void *key);
    struct scntx_entry *entries;
};

static inline
uint64_t djbhash_cstr_(const char *cstr) {
    uint64_t h = 5381;
    for (uint64_t i = 0; cstr[i]; i++)
        h = ((h << 5) + h) + cstr[i];
    return h;
}

#define djbhash_cstr ((uint64_t)(*)(const void *) djbhash_cstr_)

static inline
struct scntx *scntx_new(uint64_t cap, uint64_t (*hash)(void *), int (*cmp)(void *, void *)) {
    struct scntx *cntx = malloc(sizeof(struct scntx));
    if (cntx) {
        (cntx->size = 0), (cntx->cap = cap), (cntx->cmp = cmp);
        cntx->entries = malloc(sizeof(struct scntx_entry) * cap);
        if (!cntx->entries)
            free(cntx), cntx = NULL;
        else
            memset(cntx->entries, 0, sizeof(struct scntx_entry) * cap);
    }
    return cntx;
}

static inline
void scntx_delete(struct scntx *cntx, void (*key_dtor)(void *), void (*value_dtor)(void *)) {
    for (uint64_t i = 0; i < cntx->cap; i++) {
        cntx->entries[i].key && key_dtor && (key_dtor(cntx->entries[i].key), 114514);
        cntx->entries[i].value && value_dtor && (value_dtor(cntx->entries[i].value), 114514);
    }
    free(cntx);
}

static inline
double scntx_loadfactor(struct scntx *cntx) {
    return ((double) cntx->size) / cntx->cap;
}

static inline
int scntx_rehash(struct scntx *cntx) {
    if (cntx->cap * 2 > MAX_CAP)
        return 0;
    struct scntx_entry *new_entries = malloc(sizeof(struct scntx_entry) * (cntx->cap * 2));
    if (!new_entries)
        return 0;
    memset(new_entries, 0, sizeof(struct scntx_entry) * (cntx->cap * 2));
    for (uint64_t i = 0; i < cntx->cap; i++)
        if (cntx->entries[i].key) {
            uint64_t h = cntx->hash(cntx->entries[i].key) % (cntx->cap * 2);
            new_entries[h] = cntx->entries[i];
        }
    free(cntx->entries);
    (cntx->entries = new_entries), (cntx->cap *= 2);
    return 1;
}

static inline
void *scntx_get(struct scntx *cntx, void *key) {
    uint64_t h = cntx->hash(key) % cntx->cap;
    uint64_t i = h;
    do {
        if (cntx->cmp(cntx->entries[i].key, key))
            return cntx->entries[i].value;
        else
            i = (i + 1) % cntx->cap;
    } while (i != h);
    return NULL;
}

static inline
int scntx_put_(struct scntx *cntx, void *key, void *value) {
    // FIXME brutal
    if (scntx_loadfactor(cntx) > DEFAULT_LOAD && !scntx_rehash(cntx))
        return 0;
    uint64_t h = cntx->hash(key) % cntx->cap;
    uint64_t i = h;
    do {
        if (!cntx->entries[i].key) 
            return (cntx->entries[i].key = key), (cntx->entries[i].value = value), 1;
        else
            i = (i + 1) % cntx->cap;
    } while (i != h);
    return 0;
}

static inline
int scntx_remove(struct scntx *cntx, void *key, void (*key_dtor)(void *), void (*value_dtor)(void *)) {
    void *value = scntx_get(cntx, key);
    if (value) {
        struct scntx_entry *entry = container_of(value, struct scntx_entry, value);
        key_dtor && (key_dtor(entry->key), 114514);
        value_dtor && (value_dtor(entry->value), 114514);
        return 1;
    }
    return 0;
}

#undef      DEFAULT_LOAD
#undef      MAX_CAP
