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
    char bin_buffer[] = { 0x01, 0x00, 0x00, 0x00, 0x03, 0x02, 0x03, 0x05,
                          0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x41,
                          0x42, 0x43, 0x44, 0x45, 0x41, 0x42, 0x43, 0x44,
                          0x45, 0x41 };

    struct m3_state state_bin = M3_STATE_CONST;

    state_bin.buffer = bin_buffer;
    state_bin.buffer_size = sizeof(bin_buffer);

    struct m3_options* options_bin = NULL;
    struct m3_cell* board_bin = NULL;

    assert( 0 == m3_state_load(&state_bin,
                               &options_bin,
                               &board_bin) );


    #define hex_string "0100000003020305000000000000004142434445414243444541"

    struct m3_state state_hex_string = M3_STATE_CONST;

    state_hex_string.buffer = hex_string;
    state_hex_string.buffer_size = sizeof(hex_string);

    struct m3_options* options_hex_string = NULL;
    struct m3_cell* board_hex_string = NULL;


    assert( 0 == m3_state_load_from_hex_string(&state_hex_string,
                                               &options_hex_string,
                                               &board_hex_string ) );

    assert( 0 == m3_board_are_identical(board_bin, board_hex_string) );

    m3_state_destroy(&state_hex_string);
    m3_options_destroy(options_hex_string);
    m3_board_destroy(board_hex_string);

    m3_state_destroy(&state_bin);
    m3_options_destroy(options_bin);
    m3_board_destroy(board_bin);


    return 0;
}
