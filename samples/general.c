#include <stdint.h>
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#include <match3/match3.h>
#include <match3/cell.h>
#include <match3/match.h>
#include <match3/swap.h>
#include <match3/board.h>
#include <match3/print.h>
#include <match3/state.h>

void
usage(int argc, char* argv[])
{
    printf("%s\n", argv[0]);
    printf("%s ", argv[0]);
    printf("[seed] [columns rows]\n");

    printf("%s ", argv[0]);
    printf("[columns rows]\n");
}

// TODO find a way that seeds would work post shuffle, so that when you put in a seed it wouldnt have to shuffle the board
int
main( int argc, char* argv[] )
{

    int seed = 0;
    int columns = 10;
    int rows = 6;

    seed = (int)time(NULL);

    for( int i = 0; i < argc; i++ )
    {
        if( 0 == strcmp( "--help", argv[i]) )
        {
            usage(argc, argv);
            return 0;
        }
    }

    if( argc == 2 )
    {
        seed = atoi( argv[1] );
    }

    if( argc == 3 )
    {
        seed = (int)time(NULL);
        columns = atoi( argv[1] );
        rows = atoi( argv[2] );

    }

    if( argc > 3 )
    {
        seed = atoi( argv[1] );
        columns = atoi( argv[2] );
        rows = atoi( argv[3] );
    }

    const uint8_t colors[] = {
        m3_cell_color_red,
        m3_cell_color_green,
        m3_cell_color_blue,
        m3_cell_color_yellow,
        m3_cell_color_purple
    };

    struct m3_options options = M3_OPTIONS_CONST;

    options.seed                        = seed;
    options.columns                     = columns;
    options.rows                        = rows;
    options.matches_required_to_clear   = 3;
    options.colors                      = colors;
    options.colors_size                 = sizeof( colors );

    m3_print_options(&options);
    printf("\n");

    struct m3_cell* board = NULL;

    m3_board_build( &options, &board );

    m3_board_rand( &options, board);


    m3_board_shuffle( &options, board->right->bottom );

    m3_print_board( *board );
    printf("\n");

    struct m3_match_help_result match_help_result = M3_MATCH_HELP_RESULT_CONST;


    m3_match_help( &options,
                   board->right->bottom,
                   &match_help_result );

    if( !m3_match_help_has_swapped_and_matched( match_help_result ) )
    {
        printf("No way to end the game\n");
    }
    else
    {
        printf("\n\nSwap\n\n");
        m3_print_neighbours( *match_help_result.swap_subject );
        printf("\n");
        printf("With\n\n");
        m3_print_neighbours( *match_help_result.swap_target );
        printf("\n");

        // swap match and clear
        m3_swap( (struct m3_cell**)&match_help_result.swap_subject, (struct m3_cell**)&match_help_result.swap_target );

        struct m3_match_result match_result = M3_MATCH_RESULT_CONST;

        m3_match_cell( &options, match_help_result.swap_match, &match_result );

        m3_match_clear( &options, &match_result );


        m3_print_board( *board );
        printf("\n");

        m3_match_clear_sort( &options, &match_result );

        m3_match_result_destroy( &match_result );

        m3_print_board( *board );
        printf("\n");

        m3_board_rand( &options, board );

        m3_print_board( *board);
        printf("\n");
    }

    char* state = NULL;
    int state_size = 0;

    assert( 0 == m3_state_save( &options,
                                board,
                                &state,
                                &state_size ) );

    struct m3_cell* board_loaded = NULL;
    struct m3_options* options_loaded = NULL;

    assert( 0 == m3_state_load( state,
                                state_size,
                                &options_loaded,
                                &board_loaded ) );

    m3_print_board( *board_loaded);
    printf("\n");

    m3_print_options(options_loaded);

    m3_board_destroy( board );
    m3_board_destroy( board_loaded );
    m3_options_destroy( &options );
    m3_options_destroy( options_loaded );

    printf("seed %d\n", seed );
    printf("done\n");

    return 0;
}
