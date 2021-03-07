#pragma once

#include "match3/match3.h"
#include "match3/cell.h"

#ifdef __cplusplus
extern "C" {
#endif

void
m3_board_build( const struct m3_options*   options,
                struct m3_cell**           board );

void
m3_board_rand( const struct m3_options*    options,
               struct m3_cell*             board );

void
m3_board_shuffle( const struct m3_options* options,
                  struct m3_cell*          board );

void
m3_board_destroy( struct m3_cell* board );

#ifdef __cplusplus
}
#endif
