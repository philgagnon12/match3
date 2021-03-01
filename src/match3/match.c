#include <stdint.h>
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <time.h>

#include "match3/match.h"
#include "match3/swap.h"

// call once only per board
void
match( const struct m3_options  options,
       const struct m3_cell*    cell,
       const struct m3_cell**   matched )
{
    assert( cell );
    assert( matched );

    struct m3_cell* neighbours[] = {
        cell->bottom,
        cell->right
    };

    if( ( cell->category & cell_mask_wall ) == cell_mask_wall )
    {
        return;
    }

    match_cell( options, cell, matched );

    if( *matched != NULL )
    {
        return;
    }

    for( int i = 0; i < sizeof( neighbours ) / sizeof( struct m3_cell* ); i++ )
    {

        // Make sure neighbours are not NULL
        assert( neighbours[i] );

        // Recursively match
        match( options, neighbours[i], matched );

        if( *matched != NULL )
        {
            return;
        }
    }
}


void
match_cell( const struct m3_options options,
            const struct m3_cell*   cell,
            const struct m3_cell**  matched )
{

    assert( cell );
    assert( matched );

    match_routine* routines[] = {
        cell->bottom_routine,
        cell->right_routine
    };

    if( ( cell->category & cell_mask_wall ) == cell_mask_wall )
    {
        return;
    }

    for( int i = 0; i < sizeof( routines ) / sizeof( match_routine* ); i++ )
    {
        if( routines[i] == NULL )
        {
            continue;
        }

        // Match algorythm ( down / right traversal )
        routines[i]( options, cell, matched );

        if( *matched != NULL )
        {
            return;
        }
    }

}

void
match_vertical( const struct m3_options  options,
                const struct m3_cell*    cell,
                const struct m3_cell**   matched )
{
    assert( cell );
    assert( cell->top );    
    assert( cell->bottom );
    assert( matched );


    uint8_t match_count = 1;
    const struct m3_cell* cell_current = cell;
    const struct m3_cell* cell_top = cell->top;
    const struct m3_cell* cell_bottom = cell->bottom;

    while( cell_current->category == cell_top->category )
    {
        match_count++;
        cell_current = cell_top;
        cell_top = cell_top->top;
    }

    cell_current = cell;

    while( cell_current->category == cell_bottom->category )
    {
        match_count++;
        cell_current = cell_bottom;
        cell_bottom = cell_bottom->bottom;
    }


    if( match_count >= options.matches_required_to_clear )
    {
        print_neighbours( *cell );
        printf("\nits bottom a match %02X %d\n", cell->category, match_count );

        // TODO undo
        *matched = cell;
    }
}

void
match_horizontal( const struct m3_options  options,
                  const struct m3_cell*    cell,
                  const struct m3_cell**   matched )
    {
    assert( cell );
    assert( cell->right );
    assert( cell->left );
    assert( matched );

    uint8_t match_count = 1;
    const struct m3_cell* cell_current = cell;
    const struct m3_cell* cell_right = cell->right;
    const struct m3_cell* cell_left = cell->left;

    while( cell_current->category == cell_right->category )
    {
        match_count++;
        cell_current = cell_right;
        cell_right = cell_current->right;

    }

    cell_current = cell;

    while( cell_current->category == cell_left->category )
    {
        match_count++;
        cell_current = cell_left;
        cell_left = cell_current->left;

    }


    if( match_count >= options.matches_required_to_clear )
    {
        print_neighbours( *cell );
        printf("\nits right a match %02X %d\n", cell->category, match_count );

        // TODO undo
        *matched = cell;
    }
}


// TODO could be a match_help_best for the
// 1st arg is actually a board
// swap_subject and swap_target will be NULL if there is no way to make a match
// match help cannot detect horizontal or vertical matches since the match starts at the cell
void
match_help( const struct m3_options options,
            struct m3_cell*         cell,
            const struct m3_cell**  swap_subject,
            const struct m3_cell**  swap_target )
{
    assert( swap_subject );
    assert( swap_target );


    struct m3_cell* subject  = NULL;
    struct m3_cell* target   = NULL;

    const struct m3_cell* cell_left_most    = cell;
    const struct m3_cell* cell_current      = cell;

    const struct m3_cell* matched   = NULL;

    static swap_routine* swap_routines[] = {
        &swap_top,
        &swap_right,
        &swap_bottom,
        &swap_left
    };

    while( ( cell_current->category | ( cell_mask_wall | cell_mask_wall_undefined ) ) != ( cell_mask_wall | cell_mask_wall_undefined ) )
    {
        for( uint8_t i = 0; i < sizeof( swap_routines ) / sizeof( swap_routine* ); i++ )
        {
            subject = (struct m3_cell*)cell_current;
            target  = NULL;

            matched = NULL;

            swap_routines[i]( &subject, &target );

            if( subject != NULL && target != NULL )
            {
                match( options, cell, &matched );

                // Always undo the swap
                swap_routines[i]( &subject, &target );

                if( matched != NULL )
                {
                    *swap_subject   = subject;
                    *swap_target    = target;
                    return;
                }
            }
        }

        if( ( cell_current->category & ( cell_mask_wall | cell_mask_wall_right ) ) == ( cell_mask_wall | cell_mask_wall_right ) )
        {
            cell_current = cell_left_most->bottom;
            cell_left_most = cell_current;
        }
        else
        {
            cell_current = cell_current->right;
        }
    }

}