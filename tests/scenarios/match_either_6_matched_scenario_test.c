#include <assert.h>

#include "match3/match3.h"
#include "match3/options.h"
#include "match3/cell.h"
#include "match3/board.h"
#include "match3/swap.h"
#include "match3/match.h"
#include "match3/print.h"

int
main( void )
{
    const uint8_t colors[] = {
        m3_cell_color_red,
        m3_cell_color_green,
        m3_cell_color_blue,
        m3_cell_color_yellow,
        m3_cell_color_purple
    };

    struct m3_options options = M3_OPTIONS_CONST;
    options.seed = 1;
    options.columns = 2;
    options.rows = 3;
    options.colors = colors;
    options.colors_size = sizeof(colors);

    assert( 0 == m3_options_are_valid(&options) );

    struct m3_cell* board = NULL;

    m3_board_build( &options, &board );

    uint8_t colors_columns[] = { m3_cell_color_green, m3_cell_color_red, m3_cell_color_red,
                                 m3_cell_color_red, m3_cell_color_green, m3_cell_color_green };

    m3_board_fill_columns( &options,
                           board,
                           colors_columns,
                           sizeof(colors_columns) );

    struct m3_cell* cell_a = board->right->bottom;
    struct m3_cell* cell_b = cell_a->right;

    m3_swap_right( &cell_a, &cell_b );

    struct m3_match_result match_result_a = M3_MATCH_RESULT_CONST;
    struct m3_match_result match_result_b = M3_MATCH_RESULT_CONST;

    m3_match_either_cell( &options,
                          cell_a,
                          cell_b,
                          &match_result_a,
                          &match_result_b );

    assert(match_result_a.matched_count == 3);
    assert(match_result_b.matched_count == 3);

    assert(cell_a == match_result_a.matched[0]);
    assert(cell_b == match_result_b.matched[0]);

    assert(cell_a != cell_b);


    return 0;

}