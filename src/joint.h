#pragma once

#include    "stdc_common.h"
#include    "miscutils.h"
#include    "promise.h"
#include    "command.h"

#define     SSLK_JOINT_RPC          0
#define     SSLK_JOINT_LPC          1
#define     SSLK_JOINT_PASSIVE      2
#define     SSLK_JOINT_PSEUDO       3

struct sslk_joint {
    uint32_t type;
    struct sslk_promise (*invoke)(struct sslk_command *);
};

#define jointed struct sslk_joint joint_
#define joint_from_ptr(v_) (&((v_)->joint_))
#define joint_from_imm(v_) (&((v_).joint_))
#define jointed_val(j_, t_) container_of((j_), t_, joint_)
