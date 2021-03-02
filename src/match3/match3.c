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

    enum cell_masks colors[] = {
        cell_mask_color_red,
        cell_mask_color_green,
        cell_mask_color_blue,
        cell_mask_color_yellow,
        cell_mask_color_purple
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


    const struct m3_cell* swap_subject = NULL;
    const struct m3_cell* swap_target = NULL;

    match_help( &options, board->right->bottom, &swap_subject, &swap_target );

    if( swap_subject == NULL && swap_target == NULL )
    {
        printf("No way to end the game\n");
    }
    else
    {
        printf("\n\nSwap\n\n");
        print_neighbours( *swap_subject );
        printf("\n");
        printf("With\n\n");
        print_neighbours( *swap_target );
        printf("\n");

        // swap match and clear
        swap( (struct m3_cell**)&swap_subject, (struct m3_cell**)&swap_target );

        struct m3_match_result match_result = M3_MATCH_RESULT_CONST;

        match_cell( &options, swap_subject, &match_result );

        if( match_result.matched_count < options.matches_required_to_clear )
        {
            match_cell( &options, swap_target, &match_result );
        }

        match_clear( &options, &match_result );

        print_board( *board );
        printf("\n");

        // slide / rotate the cleared cells
        for( uint8_t i = 0; i < match_result.matched_count; i++ )
        {
            struct m3_cell* cell_top_most = (struct m3_cell*)match_result.matched[i];
            cell_pop_unshift( &options, &cell_top_most);

        }
        match_result_destroy( &match_result );

        print_board( *board );
        printf("\n");

        // Below could be instead done at the same time as cell_pop_unshift  with
        // rand_cell( &options, cell_top_most);
        // but i want print board

        struct m3_cell* cell_current = board;
        while( cell_current != NULL )
        {
            if( ( cell_current->category | ( cell_mask_color | cell_mask_color_open ) ) == ( cell_mask_color | cell_mask_color_open ) )
            {
                // fill in new colors
                cell_rand( &options, cell_current);
            }

            cell_current = cell_current->next;
        }

        print_board( *board);
        printf("\n");
    }


    printf("seed %d\n", seed );

    board_destroy( board );
    printf("done\n");

    return 0;
}
