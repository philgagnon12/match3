#pragma once

#include "match3/cell.h"

void
board_build( struct m3_options options,
             struct m3_cell**   board );

void
board_rand( const struct m3_options*    options,
            struct m3_cell*             board );

void
board_shuffle( const struct m3_options*     options,
               struct m3_cell*              board );

void
board_destroy( struct m3_cell* board );