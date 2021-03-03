#include <stdint.h>
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <time.h>

#include "match3/cell.h"
#include "match3/match.h"
#include "match3/swap.h"

void
match_result_init( struct m3_match_result* match_result,
                   const struct m3_cell*   cell )
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
match_result_add_match( struct m3_match_result* match_result,
                        const struct m3_cell*   cell )
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
match_result_destroy( struct m3_match_result* match_result )
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
match( const struct m3_options* options,
       const struct m3_cell*    cell,
       struct m3_match_result*  match_result )
{
    assert( options );
    assert( cell );
    assert( match_result );


    const struct m3_cell* cell_current = cell;

    while( cell_current != NULL )
    {
        if( ( cell_current->category | ( cell_mask_wall | cell_mask_wall_undefined ) ) != ( cell_mask_wall | cell_mask_wall_undefined ) )
        {
            match_cell( options, cell_current, match_result );

            if( match_result->matched_count >= options->matches_required_to_clear )
            {
                return;
            }
        }

        cell_current = cell_current->next;
    }
}


void
match_cell( const struct m3_options* options,
            const struct m3_cell*    cell,
            struct m3_match_result*  match_result )
{
    assert( options );
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

    for( int i = 0; i < sizeof( routines ) / sizeof( match_routine* ); i++ )
    {
        if( routines[i] == NULL )
        {
            continue;
        }

        // Match algorythm ( down / right traversal )
        routines[i]( options, cell, match_result );

        if( match_result->matched_count >= options->matches_required_to_clear )
        {
            return;
        }
    }

}

void
match_vertical( const struct m3_options* options,
                const struct m3_cell*    cell,
                struct m3_match_result*  match_result )
{
    assert( options );
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


    if( match_count >= options->matches_required_to_clear )
    {

        // printf("\nits bottom a match %02X %d\n", cell->category, match_count );

        // for( uint8_t i = 0; i < match_result->matched_count; i++ )
        // {
        //     print_neighbours( *match_result->matched[i] );
        // }

    }
}

void
match_horizontal( const struct m3_options* options,
                  const struct m3_cell*    cell,
                  struct m3_match_result*  match_result )
{
    assert( options );
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


    if( match_count >= options->matches_required_to_clear )
    {
        // printf("\nits right a match %02X %d\n", cell->category, match_count );

        // for( uint8_t i = 0; i < match_result->matched_count; i++ )
        // {
        //     print_neighbours( *match_result->matched[i] );
        // }
    }
}


///
// After you get a match_help_result from match_help() that evaluates to true from
// match_help_has_swapped_and_matched( match_help_result )
// Then you can
// 1. swap( swap_subject, swap_target )
// 2. match_cell( options, swap_match, &match_result )
// You will obtain a match_result that can be used on match_clear()
void
match_help( const struct m3_options*      options,
            struct m3_cell*               board,
            struct m3_match_help_result*  match_help_result )
{
    assert( options );
    assert( board );
    assert( match_help_result );

    static const struct m3_match_help_result match_help_result_const = M3_MATCH_HELP_RESULT_CONST;

    struct m3_cell* cell_current    = board;
    struct m3_cell* subject         = NULL;
    struct m3_cell* target          = NULL;

    struct m3_match_result    match_result = M3_MATCH_RESULT_CONST;

    static swap_routine* swap_routines[] = {
        &swap_top,
        &swap_right,
        &swap_bottom,
        &swap_left
    };

    struct m3_cell** subject_and_target[] = {
        &subject,
        &target
    };

    // Reset
    *match_help_result = match_help_result_const;

    while( cell_current != NULL )
    {
        if( ( cell_current->category & cell_mask_wall ) != cell_mask_wall )
        {
            for( uint8_t i = 0; i < sizeof( swap_routines ) / sizeof( swap_routine* ); i++ )
            {
                subject = (struct m3_cell*)cell_current;
                target  = NULL;

                swap_routines[i]( &subject, &target );

                if( subject != NULL && target != NULL )
                {
                    for( uint8_t j = 0; match_help_result->swap_match == NULL && j < sizeof( subject_and_target ) / sizeof( struct m3_cell**); j++ )
                    {
                        match_cell( options, *subject_and_target[j], &match_result );

                        if( match_result.matched_count >= options->matches_required_to_clear )
                        {
                            match_help_result->swap_match = match_result.matched[0];
                            break;
                        }
                    }

                    swap_routines[i]( &subject, &target );

                    if( match_help_result->swap_match != NULL )
                    {
                        match_help_result->swap_subject   = subject;
                        match_help_result->swap_target    = target;

                        match_result_destroy(&match_result);
                        return;
                    }

                }
            }
        }

        cell_current = cell_current->next;
    } // while

    match_result_destroy(&match_result);

}

int
match_help_has_swapped_and_matched( struct m3_match_help_result match_help_result )
{
    return ( match_help_result.swap_subject != NULL && 
             match_help_result.swap_target  != NULL &&
             match_help_result.swap_match   != NULL );
}

void
match_clear( const struct m3_options* options,
             struct m3_match_result*  match_result )
{

    assert( options );
    assert( match_result );

    if( match_result->matched_count < options->matches_required_to_clear )
    {
        return;
    }

    for( uint8_t i = 0; i < match_result->matched_count; i++ )
    {
        ((struct m3_cell*)match_result->matched[i])->category = cell_mask_color | cell_mask_color_open;
    }
}

void
match_clear_sort( const struct m3_options*  options,
                  struct m3_match_result*   match_result )
{
    assert( options );
    assert( match_result );

    const struct m3_cell* cell_first_top_color = NULL;

    struct m3_cell* cell_to_fallthrough = NULL;

    // slide / rotate the cleared cells
    for( uint8_t i = 0; i < match_result->matched_count; i++ )
    {
        cell_find_first_top_color( (struct m3_cell*)match_result->matched[i], &cell_first_top_color );

        cell_to_fallthrough = (struct m3_cell*)cell_first_top_color;

        while( cell_to_fallthrough != NULL )
        {
            cell_fallthrough( options, &cell_to_fallthrough );

            cell_find_first_top_color( cell_to_fallthrough, &cell_first_top_color );

            cell_to_fallthrough = (struct m3_cell*)cell_first_top_color;
        }
    }
}
