#pragma once

#include    "stdc_common.h"
#include    "linux_common.h"
#include    "miscutils.h"
#include    "list.h"
#include    "joint.h"
#include    "cursor.h"

struct sslk_inv_trace {
    // TODO implementation
};

// TODO implementation
struct sslk_text_backend {
    jointed;
    intrusive;
    char name[];
};

#define sslk_backend struct sslk_text_backend backend_;
#define sslk_backend_from_ptr(v_) (&((v_)->backend_))
#define sslk_backend_from_imm(v_) (&((v_).backend_))
#define sslk_backend_val(p_, t_) container_of((p_), t_, backend_)

struct sslk_text_buffer {
    int (*insert_line)(struct sslk_text_buffer *, uint64_t, int);
    int (*delete_line)(struct sslk_text_buffer *, uint64_t, int);
    int (*insert_cstr)(struct sslk_text_buffer *, const char *, uint64_t, uint64_t);
    int (*delete_sgmt)(struct sslk_text_buffer *, uint64_t, uint64_t, uint64_t);
};

#define sslk_buffer struct sslk_text_buffer buffer_ctl_;
#define sslk_buffer_from_ptr(v_) (&((v_)->buffer_ctl_))
#define sslk_buffer_from_imm(v_) (&((v_).buffer_ctl_))
#define sslk_buffered_val(p_, t_) container_of((p_), t_, buffer_ctl_)
