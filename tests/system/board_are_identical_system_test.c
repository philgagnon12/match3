#include <assert.h>

#include "match3/match3.h"
#include "match3/options.h"
#include "match3/cell.h"
#include "match3/board.h"
#include "match3/state.h"

int
main( void )
{
    // compare the same board but built using board_fill_columns and board_fill_rows

    const uint8_t colors[] = {
        m3_cell_color_red,
        m3_cell_color_green,
        m3_cell_color_blue,
        m3_cell_color_yellow,
        m3_cell_color_purple
    };

    struct m3_options options = M3_OPTIONS_CONST;
    options.seed = 1;
    options.columns = 3;
    options.rows = 2;
    options.colors = colors;
    options.colors_size = sizeof(colors);

    assert( 0 == m3_options_are_valid(&options) );

    struct m3_cell* board_a = NULL;
    struct m3_cell* board_b = NULL;

    m3_board_build( &options, &board_a );
    m3_board_build( &options, &board_b );


    assert( 0 == m3_board_are_identical(board_a, board_b) );
    assert( 0 == m3_board_are_identical(board_b, board_a) );

    uint8_t colors_columns[] = { m3_cell_color_red, m3_cell_color_green,
                                 m3_cell_color_red, m3_cell_color_green,
                                 m3_cell_color_red, m3_cell_color_green };

    m3_board_fill_columns( &options,
                           board_a,
                           colors_columns,
                           sizeof(colors_columns) );

    uint8_t colors_rows[] = { m3_cell_color_red, m3_cell_color_red, m3_cell_color_red,
                              m3_cell_color_green, m3_cell_color_green, m3_cell_color_green };

    m3_board_fill_rows( &options,
                        board_b,
                        colors_rows,
                        sizeof(colors_rows) );

    assert( 0 == m3_board_are_identical(board_a, board_b) );
    assert( 0 == m3_board_are_identical(board_b, board_a) );

    m3_board_destroy( board_a );
    m3_board_destroy( board_b );
    m3_options_destroy(&options);

    return 0;
}
