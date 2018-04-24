#pragma once

#include    "stdc_common.h"
#include    "linux_common.h"
#include    "miscutils.h"
#include    "scntx.h"

#define     DEFAULT_COMMAND_CNTX_SIZE       8

#define     SSLK_AST_ABST                   0
#define     SSLK_AST_APPL                   1
#define     SSLK_AST_DATUM                  2

struct sslk_astnode {
    uint32_t type;
    struct sslk_astnode *children[];
};

struct sslk_command {
    struct {
        uint64_t cursorx, cursory;
        struct scntx *cntx;
    };
    struct sslk_astnode *cached;
    char literal[];
};
