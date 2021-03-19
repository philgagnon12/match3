#pragma once

#include "match3/match3.h"

struct m3_options;

struct m3_options {
    int                 seed;
    uint8_t             columns;
    uint8_t             rows;
    uint8_t             matches_required_to_clear;
    uint8_t*            colors;
    size_t              colors_size;
    OPTIONAL void (*destroy)( struct m3_options* );
};

#define M3_OPTIONS_CONST { \
    0,      \
    10,     \
    6,      \
    3,      \
    NULL,   \
    0,      \
    NULL    \
}

#ifdef __cplusplus
extern "C" {
#endif

void
m3_options_destroy( struct m3_options* options );

int
m3_options_are_valid( const struct m3_options* options );

#ifdef __cplusplus
}
#endif
