#pragma once

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "match3/match3.h"
#include "match3/match.h"


struct m3_cell {
    uint8_t             category;

    uint8_t             column;
    uint8_t             row;

    struct m3_cell*     top;
    struct m3_cell*     right;
    struct m3_cell*     bottom;
    struct m3_cell*     left;
    struct m3_cell*     next;

    m3_match_routine*      horizontal_routine;
    m3_match_routine*      vertical_routine;
};

#define M3_CELL_CONST { \
    m3_cell_flag_wall | m3_cell_flag_wall_undefined,   \
    0,      \
    0,      \
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
m3_cell_rand( struct m3_options* options,
              struct m3_cell*    cell );

void
m3_cell_find_first_top_color( const struct m3_cell*    board,
                              const struct m3_cell**   cell_first_top_color );

// a color cell can fall through color-open cell
void
m3_cell_fallthrough( const struct m3_options* options,
                     struct m3_cell**         cell );

void
m3_cell_star_unique( const struct m3_options* options,
                     struct m3_cell*          cell );

uint8_t
m3_cell_are_neighbours( struct m3_cell* subject,
                        struct m3_cell* target );

#ifdef __cplusplus
}
#endif
