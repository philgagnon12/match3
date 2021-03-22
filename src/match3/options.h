#pragma once

#include <stdlib.h>

#include "match3/match3.h"

struct m3_options;

struct m3_options {
    int                 seed;
    uint8_t             columns;
    uint8_t             rows;
    uint8_t             matches_required_to_clear;
    const uint8_t*      colors;
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

// Returns zero when valid
int
m3_options_are_valid( const struct m3_options* options );


struct m3_options_find_colors_result {
    const uint8_t** colors;
    uint32_t        colors_count;
    size_t          colors_size;
};

#define M3_OPTIONS_FIND_COLORS_RESULT_CONST { \
    NULL,   \
    0,      \
    0       \
}

void
m3_options_find_colors_result_init( struct m3_options_find_colors_result* find_colors_result,
                                    const uint8_t*                        color );

void
m3_options_find_colors_result_add( struct m3_options_find_colors_result*    find_colors_result,
                                   const uint8_t*                           color );

void
m3_options_find_colors_result_destroy( struct m3_options_find_colors_result* find_colors_result );

// Used when validating existence of colors
void
m3_options_find_colors( const struct m3_options*                options,
                        uint8_t*                                colors_to_find,
                        size_t                                  colors_to_find_size,
                        struct m3_options_find_colors_result*   find_colors_result );

#ifdef __cplusplus
}
#endif
