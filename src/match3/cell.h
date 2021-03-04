#pragma once

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "match3/match3.h"
#include "match3/match.h"


struct m3_cell {
    uint8_t             category;

    struct m3_cell*     top;
    struct m3_cell*     right;
    struct m3_cell*     bottom;
    struct m3_cell*     left;
    struct m3_cell*     next;

    match_routine*      right_routine;
    match_routine*      bottom_routine;
};

#define M3_CELL_CONST { \
    m3_cell_flag_wall | m3_cell_flag_wall_undefined,   \
    NULL,   \
    NULL,   \
    NULL,   \
    NULL,   \
    NULL,   \
    NULL,   \
    NULL,   \
}

#ifdef __cplusplus
extern "C" {
#endif

void
cell_rand( const struct m3_options* options,
           struct m3_cell*          cell );

void
cell_find_first_top_color( const struct m3_cell*    board,
                           const struct m3_cell**   cell_first_top_color );

// a color cell can fall through color-open cell
void
cell_fallthrough( const struct m3_options* options,
                  struct m3_cell**         cell );

void
cell_star_unique( const struct m3_options* options,
                  struct m3_cell*          cell );

#ifdef __cplusplus
}
#endif