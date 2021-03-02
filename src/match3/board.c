#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "match3/board.h"
#include "match3/match.h"


void
board_build( struct m3_options options,
             struct m3_cell**   board )
{
    assert( board );

    uint8_t columns = options.columns;
    uint8_t rows = options.rows;

    assert( columns > 0 );
    assert( rows > 0 );

    uint8_t cell_category = 0;

    struct m3_cell* cell_wall_undefined = NULL;
    static const struct m3_cell const_cell_wall_undefined = M3_CELL_CONST;


    struct m3_cell* cell_current    = NULL;
    struct m3_cell* cell_top        = NULL;
    struct m3_cell* cell_top_most   = NULL;
    struct m3_cell* cell_right      = NULL;
    struct m3_cell* cell_bottom     = NULL;
    struct m3_cell* cell_left       = NULL;
    struct m3_cell* cell_previous   = NULL;


    srand( options.seed );  

    cell_wall_undefined = malloc( sizeof( struct m3_cell ) );
    assert( cell_wall_undefined );
    *cell_wall_undefined = const_cell_wall_undefined;

    cell_wall_undefined->top    = cell_wall_undefined;
    cell_wall_undefined->right  = cell_wall_undefined;
    cell_wall_undefined->bottom = cell_wall_undefined;
    cell_wall_undefined->left   = cell_wall_undefined;


    cell_top    = cell_wall_undefined;
    cell_right  = cell_wall_undefined;
    cell_bottom = cell_wall_undefined;
    cell_left   = cell_wall_undefined;


    for( uint8_t c = 0; c < columns; c++ )
    {
        for( uint8_t r = 0; r < rows; r++ )
        {

            cell_current = malloc( sizeof( struct m3_cell ) );
            assert( cell_current );
            *cell_current = const_cell_wall_undefined;

            cell_category = 0;

            // Left wall
            if( c == 0 )
            {
                cell_category |= cell_mask_wall | cell_mask_wall_left;
            }

            if( c == columns - 1 )
            {
                cell_category |= cell_mask_wall | cell_mask_wall_right;
            }

            if( r == 0 )
            {
                cell_category |= cell_mask_wall | cell_mask_wall_top;
            }

            if( r == rows - 1 )
            {
                cell_category |= cell_mask_wall | cell_mask_wall_bottom;
            }

            if( c > 0 &&
                c < ( columns - 1 ) &&
                r > 0 &&
                r < ( rows - 1 ) )
            {
                cell_category |= cell_mask_color | cell_mask_color_open;
                cell_current->right_routine = &match_horizontal;
                cell_current->bottom_routine = &match_vertical;

                // The output cell is assigned to the first color cell
                if( *board == NULL )
                {
                    *board = cell_current;
                }
            }

            cell_current->category = cell_category;

            cell_current->top = cell_top;
            cell_top->bottom = cell_current;


            cell_current->right = cell_right;
            cell_right->left = cell_current;

            cell_current->bottom = cell_bottom;
            cell_bottom->top = cell_current;

            cell_current->left = cell_left;
            cell_left->right = cell_current;

            if( cell_previous != NULL )
            {
                cell_previous->next = cell_current;
            }

            cell_previous = cell_current;

            // Setting up for next cell
            cell_top = cell_current;
            cell_left = cell_left->bottom;

            if( cell_top_most == NULL )
            {
                cell_top_most = cell_current;
            }
        } // for rows

        // Setting up for next cell
        cell_left       = cell_top_most;
        cell_top_most   = NULL;
        cell_top        = cell_wall_undefined;

    } // for columns
}

void
board_rand( const struct m3_options*    options,
            struct m3_cell*             cell )
{

    assert( cell );

    struct m3_cell* cell_current      = cell;

    while( cell_current != NULL )
    {
        if( ( cell_current->category | ( cell_mask_wall | cell_mask_wall_undefined ) ) != ( cell_mask_wall | cell_mask_wall_undefined ) )
        {
            cell_rand( options, cell_current );
        }

        cell_current = cell_current->next;
    }
}

void
board_shuffle( const struct m3_options*     options,
               struct m3_cell*              board )
{
    assert( options );
    assert( board );

    struct m3_match_result matched_result = M3_MATCH_RESULT_CONST;
    // Assigning to options.matches_required_to_clear just because easy to do the loop that way
    matched_result.matched_count = options->matches_required_to_clear;

    struct m3_cell* matched_result_first_cell = board;

    // Shuffling the board so that there is no automatic match
    while( matched_result.matched_count >= options->matches_required_to_clear )
    {
        while( matched_result.matched_count >= options->matches_required_to_clear )
        {
            matched_result.matched_count = 1;
            cell_rand( options, matched_result_first_cell );
            match_cell( *options, matched_result_first_cell, &matched_result );
            matched_result_first_cell = (struct m3_cell*)matched_result.matched[0];
        }

        match( *options, matched_result_first_cell, &matched_result );
        matched_result_first_cell = (struct m3_cell*)matched_result.matched[0];
        //print_board( *board->top->left );
        //printf("\n");

    }

    match_result_destroy(&matched_result);

    printf("shuffled\n");
}

void
board_destroy( struct m3_cell* board )
{
    assert( board );

    struct m3_cell* cell_left_most    = board;
    struct m3_cell* cell_current      = board;
    struct m3_cell* cell_free         = NULL;

    while( ( cell_current->category | ( cell_mask_wall | cell_mask_wall_undefined ) ) != ( cell_mask_wall | cell_mask_wall_undefined ) )
    {
        cell_free = cell_current;

        if( ( cell_current->category & ( cell_mask_wall | cell_mask_wall_right ) ) == ( cell_mask_wall | cell_mask_wall_right ) )
        {
            cell_current = cell_left_most->bottom;
            cell_left_most = cell_current;
        }
        else
        {
            cell_current = cell_current->right;
        }

        free( cell_free );
    } // while


    if( ( cell_current->category | ( cell_mask_wall | cell_mask_wall_undefined ) ) != ( cell_mask_wall | cell_mask_wall_undefined ) )
    {
        free( cell_current );
    }
}