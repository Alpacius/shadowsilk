#pragma once

#include    "stdc_common.h"
#include    "miscutils.h"

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
    void (*key_dtor)(void *);
    void (*value_dtor)(void *);
    struct scntx_entry *entries;
};

static inline
uint64_t djbhash_cstr(const void *arg) {
    const char *cstr = arg;
    uint64_t h = 5381;
    for (uint64_t i = 0; cstr[i]; i++)
        h = ((h << 5) + h) + cstr[i];
    return h;
}

static inline
int cstrcmp(void *lhs, void *rhs) {
    return strcmp((const char *) lhs, (const char *) rhs) == 0;
}

static inline
struct scntx *scntx_new(uint64_t cap, uint64_t (*hash)(const void *), int (*cmp)(void *, void *), void (*key_dtor)(void *), void (*value_dtor)(void *)) {
    struct scntx *cntx = malloc(sizeof(struct scntx));
    if (cntx) {
        (cntx->size = 0), (cntx->cap = cap), (cntx->cmp = cmp), (cntx->hash = hash), (cntx->key_dtor = key_dtor), (cntx->value_dtor = value_dtor);
        cntx->entries = malloc(sizeof(struct scntx_entry) * cap);
        if (!cntx->entries)
            free(cntx), cntx = NULL;
        else
            memset(cntx->entries, 0, sizeof(struct scntx_entry) * cap);
    }
    return cntx;
}

static inline
void scntx_delete(struct scntx *cntx) {
    for (uint64_t i = 0; i < cntx->cap; i++) {
        cntx->entries[i].key && cntx->key_dtor && (cntx->key_dtor(cntx->entries[i].key), 114514);
        cntx->entries[i].value && cntx->value_dtor && (cntx->value_dtor(cntx->entries[i].value), 114514);
    }
    free(cntx->entries);
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
    // FIXME brutal
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
int scntx_put(struct scntx *cntx, void *key, void *value) {
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
int scntx_put_simple(struct scntx *cntx, const char *key, void *value) {
    if (scntx_loadfactor(cntx) > DEFAULT_LOAD && !scntx_rehash(cntx))
        return 0;
    uint64_t h = cntx->hash(key) % cntx->cap;
    uint64_t i = h;
    do {
        if (!cntx->entries[i].key) 
            return 
                strncpy(cntx->entries[i].key_imm, key, 64), 
                (cntx->entries[i].key_imm[63] = 0), 
                (cntx->entries[i].key = cntx->entries[i].key_imm), 
                (cntx->entries[i].value = value), 
                1;
        else
            i = (i + 1) % cntx->cap;
    } while (i != h);
    return 0;
}

static inline
int scntx_remove(struct scntx *cntx, void *key) {
    void *value = scntx_get(cntx, key);
    if (value) {
        struct scntx_entry *entry = container_of(value, struct scntx_entry, value);
        cntx->key_dtor && (cntx->key_dtor(entry->key), entry->key = NULL);
        cntx->value_dtor && (cntx->value_dtor(entry->value), entry->value = NULL);
        return 1;
    }
    return 0;
}

#define scntx_simple(cap_, value_dtor_) scntx_new((cap_), djbhash_cstr, cstrcmp, NULL, (value_dtor_))

#undef      DEFAULT_LOAD
#undef      MAX_CAP
