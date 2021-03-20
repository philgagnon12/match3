#pragma once

#include "match3/match3.h"
#include "match3/options.h"
#include "match3/cell.h"

#ifdef __cplusplus
extern "C" {
#endif

void
m3_board_build( const struct m3_options*   options,
                struct m3_cell**           board );

void
m3_board_rand( struct m3_options*    options,
               struct m3_cell*       board );

void
m3_board_shuffle( struct m3_options* options,
                  struct m3_cell*    board );

void
m3_board_destroy( struct m3_cell* board );

unsigned int
m3_board_are_identical( const struct m3_cell* board_a,
                        const struct m3_cell* board_b );

// Assign color to cell flagged as open color, going from column to column, top to bottom
void
m3_board_fill_columns( const struct m3_options* options,
                       struct m3_cell*          board,
                       uint8_t*                 colors,
                       size_t                   colors_size );

// Assign color to cell flagged as open color, going from row to row, left to right
void
m3_board_fill_rows( const struct m3_options* options,
                    struct m3_cell*          board,
                    uint8_t*                 colors,
                    size_t                   colors_size );


#ifdef __cplusplus
}
#endif
