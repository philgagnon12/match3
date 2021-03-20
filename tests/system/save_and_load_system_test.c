#include <assert.h>
#include <stdlib.h>

#include "match3/match3.h"
#include "match3/options.h"
#include "match3/cell.h"
#include "match3/board.h"
#include "match3/state.h"

int
main(void)
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
    options.columns = 3;
    options.rows = 2;
    options.colors = colors;
    options.colors_size = sizeof(colors);

    assert( 0 == m3_options_are_valid(&options) );

    struct m3_cell* board_to_save = NULL;

    m3_board_build(&options, &board_to_save);
    m3_board_rand(&options, board_to_save);

    char* save_state = NULL;
    size_t save_state_size = 0;

    assert( 0 == m3_state_save( &options,
                                board_to_save,
                                &save_state,
                                &save_state_size ) );

    m3_board_destroy( board_to_save );
    m3_options_destroy(&options);

    struct m3_options* options_loaded = NULL;
    struct m3_cell* board_loaded = NULL;


    assert( 0 == m3_state_load( save_state,
                                save_state_size,
                                &options_loaded,
                                &board_loaded ) );


    free(save_state); // TODO remove in favor of m3_state_destroy()
    m3_board_destroy( board_loaded );
    m3_options_destroy(options_loaded);


    return 0;
}