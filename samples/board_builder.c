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

            categories[categories_size-1] = (uint8_t)strtol( argv[i], NULL, 10 );
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
            struct m3_cell* cell_color_top_most = board->right->bottom;
            cell_current = cell_color_top_most;

            for(int i = 0; i < categories_size; i++ )
            {
                if( (cell_current->category & m3_cell_flag_wall) == m3_cell_flag_wall )
                {
                    break;
                }
                cell_current->category = categories[i] | m3_cell_flag_color;
                cell_current = cell_current->bottom;
                if( ( cell_current->category & m3_cell_flag_wall ) == m3_cell_flag_wall )
                {
                    cell_current = cell_color_top_most->right;
                    cell_color_top_most = cell_current;
                }
            }
        }

        if( rows != 0 )
        {
            struct m3_cell* cell_color_left_most = board->right->bottom;
            cell_current = cell_color_left_most;

            for(int i = 0; i < categories_size; i++ )
            {
                if( (cell_current->category & m3_cell_flag_wall) == m3_cell_flag_wall )
                {
                    break;
                }
                cell_current->category = categories[i] | m3_cell_flag_color;
                cell_current = cell_current->right;
                if( ( cell_current->category & m3_cell_flag_wall ) == m3_cell_flag_wall )
                {
                    cell_current = cell_color_left_most->bottom;
                    cell_color_left_most = cell_current;
                }
            }
        }

    }

    if( ret == 0 )
    {
        m3_print_board( *board );

        char* state = NULL;
        int state_size = 0;

        assert( 0 == m3_state_save( &options,
                                    board,
                                    &state,
                                    &state_size ) );

        // ./board_builder --rows 2 1 1 1 2 2 2 | tail -n 1 | xxd -r -p | xxd -i -c 8
        printf("m3_state_load():\n");
        m3_print_hex((const uint8_t*)state, state_size);
        printf("\n");
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

