#pragma once
#include <stdint.h>

#include "match3/cell.h"

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

void
m3_print_hex( const uint8_t* bytes,
              int            bytes_count );
void
m3_print_bits( const uint8_t byte );

void
m3_print_options( const struct m3_options* options );

#ifdef __cplusplus
}
#endif
