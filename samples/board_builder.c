#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "match3/match3.h"
#include "match3/options.h"
#include "match3/board.h"
#include "match3/print.h"
#include "match3/state.h"

int
main( int argc, char* argv[] )
{
    int ret = 0;

    uint8_t columns = 0;
    uint8_t rows = 0;

    uint8_t* categories = NULL;
    size_t categories_size = 0;
    uint8_t* categories_re = NULL;

    struct m3_options options = M3_OPTIONS_CONST;

    struct m3_cell* board = NULL;

    for( int i = 1; i < argc; i++ )
    {
        if( 0 == strcmp("--columns", argv[i] ) && i+1 < argc )
        {
            columns = (uint8_t)strtol( argv[++i], NULL, 10 );
        }
        else if( 0 == strcmp("--rows", argv[i] ) && i+1 < argc )
        {
            rows = (uint8_t)strtol( argv[++i], NULL, 10 );
        }
        else
        {
            categories_size += sizeof( board->category );
            categories_re = realloc( categories, categories_size );
            assert( categories_re );
            categories = categories_re;

            categories[categories_size-1] = m3_cell_flag_color | (uint8_t)strtol( argv[i], NULL, 10 );
        }
    }

    if( columns == 0 && rows == 0 )
    {
        fprintf(stderr, "Error: --columns or --rows missing\n" );
        ret = 1;
    }

    if( columns != 0 && rows != 0 )
    {
        fprintf(stderr, "Error: use either --columns or --rows\n" );
        ret = 1;
    }

    if( ret == 0 )
    {
        uint8_t colors[] = {
            m3_cell_color_red,
            m3_cell_color_green,
            m3_cell_color_blue,
            m3_cell_color_yellow,
            m3_cell_color_purple
        };


        options.seed                        = 0x01;
        options.matches_required_to_clear   = 3;
        options.colors                      = colors;
        options.colors_size                 = sizeof( colors );

        if( columns != 0 )
        {
            options.columns = columns;
            options.rows = (uint8_t)(categories_size / columns);
        }

        if( rows != 0 )
        {
            options.rows = rows;
            options.columns = (uint8_t)(categories_size / rows);
        }

        m3_print_options( &options );
        printf("\n");

        if( 0 != m3_options_are_valid( &options ) )
        {
            fprintf(stderr, "Error: invalid options\n" );
            ret = 1;
        }

    }

    if( ret == 0 )
    {
        m3_board_build( &options, &board );

        struct m3_cell* cell_current = NULL;

        if( columns != 0 )
        {
            m3_board_fill_columns( &options,
                                   board,
                                   categories,
                                   categories_size );
        }

        if( rows != 0 )
        {
            m3_board_fill_rows( &options,
                                board,
                                categories,
                                categories_size );
        }

    }

    if( ret == 0 )
    {
        m3_print_board( *board );

        struct m3_state state = M3_STATE_CONST;

        assert( 0 == m3_state_save( &options,
                                    board,
                                    &state ) );

        // ./board_builder --rows 2 1 1 1 2 2 2 | tail -n 1 | xxd -r -p | xxd -i -c 8
        printf("m3_state_load():\n");
        m3_print_hex((const uint8_t*)state.buffer, state.buffer_size);
        printf("\n");

        m3_state_destroy(&state);
    }

    if( board != NULL )
    {
        m3_board_destroy( board );
    }

    if( categories != NULL )
    {
        free(categories);
    }

    m3_options_destroy( &options );

    return 0;
}

