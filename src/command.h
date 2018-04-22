#pragma once

#include    "stdc_common.h"
#include    "miscutils.h"


struct sslk_astnode {
    uint32_t type;
    struct sslk_astnode children[];
};

struct sslk_command {
    struct {
        uint64_t cursorx, cursory;
        // TODO scntx
    };
    struct sslk_astnode *cached;
    char literal[];
};
