#include    "localbuffer.h"

#define     GAPSIZE     64
#define     LNSIZE      256

static inline
struct gapbuffer gapbuffer_new(uint64_t size) {
    struct gapbuffer gb = { .bufstart = NULL, .bufend = NULL, .gapstart = NULL, .gapend = NULL, .loc = NULL, .size = 0 };
    if (gb.bufstart = malloc(sizeof(wchar_t) * size)) 
        (gb.loc = gb.gapstart = gb.bufstart), (gb.gapend = gb.bufend = gb.bufstart + size);
    return gb;
}

static inline
void gapbuffer_delete(struct gapbuffer gb) {
    free(gb.bufstart), (gb.size = 0), (gb.bufstart = gb.bufend = gb.gapstart = gb.gapend = gb.loc);
}

// XXX 'tis no serious service - failure & loud crash is acceptable - at least for now
static inline
struct gapbuffer gapbuffer_expand(struct gapbuffer gb, uint64_t size) {
    if (size > gb.gapend - gb.gapstart) {
        wchar_t *oldbuf = gb.gapstart;
        gb.bufstart = realloc(gb.gapstart, (gb.bufend - gb.bufstart + size + GAPSIZE) * sizeof(wchar_t));
        if (gb.gapstart) 
            (gb.loc += gb.bufstart - oldbuf), (gb.bufend += gb.bufstart - oldbuf), (gb.gapstart += gb.bufstart - oldbuf), (gb.gapend += gb.bufstart - oldbuf);
    }
    return gb;
}

static inline
struct gapbuffer gapbuffer_correct_gap(struct gapbuffer gb) {
    if (gb.loc == gb.gapstart)
        return gb;
    if (gb.loc == gb.gapend)
        return (gb.loc = gb.gapstart), gb;
    (gb.loc < gb.gapstart) ?
        (memmove(gb.loc + (gb.gapend - gb.gapstart), gb.loc, (gb.gapstart - gb.loc) * sizeof(wchar_t)), (gb.gapend -= gb.gapstart - gb.loc), (gb.gapstart = gb.loc))
    :
        (memmove(gb.gapstart, gb.gapend, (gb.loc - gb.gapend) * sizeof(wchar_t)), (gb.gapstart += gb.loc - gb.gapend), (gb.gapend = gb.loc), (gb.loc = gb.gapstart));
    return gb;
}

static inline 
struct gapbuffer gapbuffer_expand_gap(struct gapbuffer gb, uint64_t size) {
    if (size > gb.gapend - gb.gapstart) {
        size += GAPSIZE;
        gb = gapbuffer_expand(gb, size);
        memmove(gb.gapend + size, gb.gapend, (gb.bufend - gb.gapend) * sizeof(wchar_t));
        (gb.gapend += size), (gb.bufend += size);
    }
    return gb;
}

static inline
struct gapbuffer gapbuffer_setloc(struct gapbuffer gb, uint64_t off) {
    return ({ if ((gb.loc = gb.bufstart + off) > gb.gapstart) { (gb.loc += gb.gapend - gb.gapstart); } }), gb;
}

static inline
struct gapbuffer gapbuffer_delete_segment(struct gapbuffer gb, uint64_t size) {
    (gb.loc != gb.gapstart) && ((gb = gapbuffer_correct_gap(gb)), 114514);
    return (gb.size -= size), (gb.gapend += size), gb;
}

static inline
struct gapbuffer gapbuffer_insert_cstr(struct gapbuffer gb, const wchar_t *cstr) {
    (gb.loc != gb.gapstart) && ((gb = gapbuffer_correct_gap(gb)), 114514);
    size_t len = wcslen(cstr);
    (len > gb.gapend - gb.gapstart) && ((gb = gapbuffer_expand_gap(gb, len)), 114514);
    while (*cstr) {
        *(gb.gapstart++) = *(cstr++);
    }
    return (gb.loc = gb.gapstart), (gb.size += len), gb;
}

struct bufline *bufline_init(struct bufline *ln, uint32_t pos, uint32_t cap) {
    (ln->pos = pos), (ln->cap = cap), (ln->magic = SSLK_BUFLINE_MAGIC);
    ln->buf = gapbuffer_new(cap);
    return list_init(ihandle_of(ln, ->)), ln;
}

struct bufline *bufline_ruin(struct bufline *ln) {
    if (!list_node_isolated(ihandle_of(ln, ->)))
        list_del(ihandle_of(ln, ->));
    gapbuffer_delete(ln->buf);
    return ln;
}

struct bufline *bufline_new(uint32_t pos, uint32_t cap) {
    struct bufline *ln = malloc(sizeof(struct bufline));
    return ln ? bufline_init(ln, pos, cap) : NULL;
}

void bufline_delete(struct bufline *ln) {
    free(bufline_ruin(ln));
}

struct bufline *bufline_insert_str(struct bufline *ln, uint64_t loc, const wchar_t *str) {
    return (ln->buf = gapbuffer_setloc(ln->buf, loc)), (ln->buf = gapbuffer_insert_cstr(ln->buf, str)), ln;
}

struct bufline *bufline_delete_str(struct bufline *ln, uint64_t loc, uint64_t size) {
    return (ln->buf = gapbuffer_setloc(ln->buf, loc)), (ln->buf = gapbuffer_delete_segment(ln->buf, size)), ln;
}

struct localbuf *localbuf_init(struct localbuf *bf) {
    list_init(&(bf->lines));
    list_add_tail(ihandle_of(bufline_new(0, LNSIZE), ->), &(bf->lines));
    return (bf->size = 1), (bf->quick_cursor.locy = 0), (bf->quick_cursor.lnref = bf->lines.next), bf;
}

struct localbuf *localbuf_ruin(struct localbuf *bf) {
    list_foreach(&(bf->lines))
        bufline_delete(intrusive_ref(struct bufline));
    list_init(&(bf->lines));
    return (bf->size = 0), (bf->quick_cursor.locy = 0), (bf->quick_cursor.lnref = NULL), bf;
}

struct localbuf *localbuf_new(void) {
    struct localbuf *bf = malloc(sizeof(struct localbuf));
    return bf ? localbuf_init(bf) : NULL;
}

void localbuf_delete(struct localbuf *bf) {
    free(localbuf_ruin(bf));
}
