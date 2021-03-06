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

    printf("seed %d\n", seed );
    printf("columns %d\n", columns );
    printf("rows %d\n", rows );

    uint8_t colors[] = {
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


    struct m3_cell* board = NULL;

    board_build( &options, &board );

    board_rand( &options, board);


    board_shuffle( &options, board->right->bottom );

    print_board( *board );
    printf("\n");

    struct m3_match_help_result match_help_result = M3_MATCH_HELP_RESULT_CONST;


    match_help( &options,
                board->right->bottom,
                &match_help_result );

    if( !match_help_has_swapped_and_matched( match_help_result ) )
    {
        printf("No way to end the game\n");
    }
    else
    {
        printf("\n\nSwap\n\n");
        print_neighbours( *match_help_result.swap_subject );
        printf("\n");
        printf("With\n\n");
        print_neighbours( *match_help_result.swap_target );
        printf("\n");

        // swap match and clear
        m3_swap( (struct m3_cell**)&match_help_result.swap_subject, (struct m3_cell**)&match_help_result.swap_target );

        struct m3_match_result match_result = M3_MATCH_RESULT_CONST;

        match_cell( &options, match_help_result.swap_match, &match_result );

        match_clear( &options, &match_result );


        print_board( *board );
        printf("\n");

        match_clear_sort( &options, &match_result );

        match_result_destroy( &match_result );

        print_board( *board );
        printf("\n");

        board_rand( &options, board );

        print_board( *board);
        printf("\n");
    }


    printf("seed %d\n", seed );

    board_destroy( board );
    printf("done\n");

    return 0;
}
