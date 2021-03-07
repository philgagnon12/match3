#pragma once

#include <match3/cell.h>

#ifdef __cplusplus
extern "C" {
#endif

void
m3_print_cell( const struct m3_cell cell );

void
m3_print_neighbours( const struct m3_cell cell );

void
m3_print_board( const struct m3_cell cell );

void
m3_print_board_info( const struct m3_cell cell );

#ifdef __cplusplus
}
#endif
