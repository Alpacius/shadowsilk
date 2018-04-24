#pragma once

#include    "stdc_common.h"
#include    "linux_common.h"
#include    "miscutils.h"

struct sslk_promise {
    void (*dtor_)(void *);
    void (*abandon)(struct sslk_promise *);
    void *(*get)(struct sslk_promise *);
};

#define promised struct sslk_promise promise_
#define promise_from_imm(v_) (&((v_).promise_))
#define promise_from_ptr(v_) (&((v_)->promise_))
#define promised_val(p_, t_) container_of((p_), t_, promise_)

#define p_gen_default_abandon(type_, hint_) \
    static inline \
    void promise_abandon_##hint_ (struct sslk_promise *p) { \
        if (p->dtor_) \
            p->dtor_(promised_val(p, type_)); \
    }
#define p_default_abandon(hint_) promise_abandon_##hint_
