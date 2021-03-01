#include <stdint.h>
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <time.h>

#include "match3/cell.h"
#include "match3/match.h"
#include "match3/swap.h"

void
match_result_init( struct m3_match_result*    match_result,
                   const struct m3_cell*      cell )
{
    assert(match_result);
    assert(cell);

    if( match_result->matched == NULL )
    {
        match_result->matched = malloc( sizeof( struct m3_cell* ) );
        assert( match_result->matched );
        match_result->matched_size = sizeof( struct m3_cell* );
    }

    // Reset
    for( uint8_t i = 0; i < match_result->matched_size / sizeof(struct m3_cell* ); i++ )
    {
        match_result->matched[i] = NULL;
    }

    match_result->matched[0] = cell;
    match_result->matched_count = 1;
}


void
match_result_add_match( struct m3_match_result*    match_result,
                        const struct m3_cell*      cell )
{
    assert(match_result);
    assert(cell);

    const struct m3_cell**     matched_re = NULL;

    if( match_result->matched_size < (match_result->matched_count + 1) * sizeof( struct m3_cell* ) )
    {
        matched_re = realloc( match_result->matched,
                              match_result->matched_size + sizeof( struct m3_cell* ) );
        assert( matched_re );
        match_result->matched = matched_re;
        match_result->matched_size += sizeof( struct m3_cell* );
    }

    match_result->matched[match_result->matched_count] = cell;
    match_result->matched_count++;
}

void
match_result_destroy( struct m3_match_result*    match_result )
{
    assert(match_result);
    static const struct m3_match_result  match_result_const = M3_MATCH_RESULT_CONST;

    if( match_result->matched != NULL )
    {
        free(match_result->matched);
    }
    *match_result = match_result_const;
}

// call once only per board
void
match( const struct m3_options    options,
       const struct m3_cell*      cell,
       struct m3_match_result*    match_result )
{
    assert( cell );
    assert( match_result );

    struct m3_cell* neighbours[] = {
        cell->bottom,
        cell->right
    };

    if( ( cell->category & cell_mask_wall ) == cell_mask_wall )
    {
        return;
    }

    match_result_init( match_result, cell);

    match_cell( options, cell, match_result );

    if( match_result->matched_count >= options.matches_required_to_clear )
    {
        return;
    }

    for( int i = 0; i < sizeof( neighbours ) / sizeof( struct m3_cell* ); i++ )
    {

        // Make sure neighbours are not NULL
        assert( neighbours[i] );

        // Recursively match
        match( options, neighbours[i], match_result );

        if( match_result->matched_count >= options.matches_required_to_clear )
        {
            return;
        }
    }
}


void
match_cell( const struct m3_options options,
            const struct m3_cell*   cell,
            struct m3_match_result*    match_result )
{

    assert( cell );
    assert( match_result );

    match_routine* routines[] = {
        cell->bottom_routine,
        cell->right_routine
    };

    if( ( cell->category & cell_mask_wall ) == cell_mask_wall )
    {
        return;
    }

    match_result_init( match_result, cell);

    for( int i = 0; i < sizeof( routines ) / sizeof( match_routine* ); i++ )
    {
        if( routines[i] == NULL )
        {
            continue;
        }

        // Match algorythm ( down / right traversal )
        routines[i]( options, cell, match_result );

        if( match_result->matched_count >= options.matches_required_to_clear )
        {
            return;
        }
    }

}

void
match_vertical( const struct m3_options  options,
                const struct m3_cell*    cell,
                struct m3_match_result*    match_result )
{
    assert( cell );
    assert( cell->top );    
    assert( cell->bottom );
    assert( match_result );


    uint8_t match_count = 1;
    const struct m3_cell* cell_current = cell;
    const struct m3_cell* cell_top = cell->top;
    const struct m3_cell* cell_bottom = cell->bottom;



    match_result_init( match_result, cell);

    while( cell_current->category == cell_top->category )
    {

        match_result_add_match( match_result, cell_top);

        match_count++;
        cell_current = cell_top;
        cell_top = cell_top->top;
    }

    cell_current = cell;

    while( cell_current->category == cell_bottom->category )
    {
        match_result_add_match( match_result, cell_bottom);

        match_count++;
        cell_current = cell_bottom;
        cell_bottom = cell_bottom->bottom;
    }


    if( match_count >= options.matches_required_to_clear )
    {
        
        printf("\nits bottom a match %02X %d\n", cell->category, match_count );

        for( uint8_t i = 0; i < match_result->matched_count; i++ )
        {
            print_neighbours( *match_result->matched[i] );
        }

    }
}

void
match_horizontal( const struct m3_options  options,
                  const struct m3_cell*    cell,
                  struct m3_match_result*    match_result )
{
    assert( cell );
    assert( cell->right );
    assert( cell->left );
    assert( match_result );

    uint8_t match_count = 1;
    const struct m3_cell* cell_current = cell;
    const struct m3_cell* cell_right = cell->right;
    const struct m3_cell* cell_left = cell->left;

    const struct m3_cell**     matched_re = NULL;

    match_result_init( match_result, cell);

    while( cell_current->category == cell_right->category )
    {
        match_result_add_match( match_result, cell_right);

        match_count++;
        cell_current = cell_right;
        cell_right = cell_current->right;

    }

    cell_current = cell;

    while( cell_current->category == cell_left->category )
    {
        match_result_add_match( match_result, cell_left);

        match_count++;
        cell_current = cell_left;
        cell_left = cell_current->left;

    }


    if( match_count >= options.matches_required_to_clear )
    {
        printf("\nits right a match %02X %d\n", cell->category, match_count );

        for( uint8_t i = 0; i < match_result->matched_count; i++ )
        {
            print_neighbours( *match_result->matched[i] );
        }
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

    struct m3_match_result    match_result = M3_MATCH_RESULT_CONST;

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


            swap_routines[i]( &subject, &target );

            if( subject != NULL && target != NULL )
            {
                match( options, cell, &match_result );

                // Always undo the swap
                swap_routines[i]( &subject, &target );

                if( match_result.matched_count >= options.matches_required_to_clear )
                {
                    *swap_subject   = subject;
                    *swap_target    = target;

                    match_result_destroy(&match_result);
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

    match_result_destroy(&match_result);

}
