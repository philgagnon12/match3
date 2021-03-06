#include <stdint.h>
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <time.h>

#include "match3/cell.h"
#include "match3/match.h"
#include "match3/swap.h"

void
m3_match_result_init( struct m3_match_result* match_result,
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
m3_match_result_add_match( struct m3_match_result* match_result,
                           const struct m3_cell*   cell )
{
    assert(match_result);
    assert(cell);

    const struct m3_cell**     matched_re = NULL;

    if( match_result->matched_size < (match_result->matched_count + 1) * sizeof( struct m3_cell* ) )
    {
        matched_re = realloc( (struct m3_cell**)match_result->matched,
                              match_result->matched_size + sizeof( struct m3_cell* ) );
        assert( matched_re );
        match_result->matched = matched_re;
        match_result->matched_size += sizeof( struct m3_cell* );
    }

    match_result->matched[match_result->matched_count] = cell;
    match_result->matched_count++;
}

void
m3_match_result_destroy( struct m3_match_result* match_result )
{
    assert(match_result);
    static const struct m3_match_result  match_result_const = M3_MATCH_RESULT_CONST;

    if( match_result->matched != NULL )
    {
        free((struct m3_cell**)match_result->matched);
    }
    *match_result = match_result_const;
}

// call once only per board
void
m3_match( const struct m3_options* options,
          const struct m3_cell*    cell,
          struct m3_match_result*  match_result )
{
    assert( options );
    assert( cell );
    assert( match_result );


    const struct m3_cell* cell_current = cell;

    while( cell_current != NULL )
    {
        if( ( cell_current->category | ( m3_cell_flag_wall | m3_cell_flag_wall_undefined ) ) != ( m3_cell_flag_wall | m3_cell_flag_wall_undefined ) )
        {
            m3_match_cell( options, cell_current, match_result );

            if( match_result->matched_count >= options->matches_required_to_clear )
            {
                return;
            }
        }

        cell_current = cell_current->next;
    }
}


void
m3_match_cell( const struct m3_options* options,
               const struct m3_cell*    cell,
               struct m3_match_result*  match_result )
{
    assert( options );
    assert( cell );
    assert( match_result );

    m3_match_routine* routines[] = {
        cell->vertical_routine,
        cell->horizontal_routine
    };

    if( ( cell->category & m3_cell_flag_wall ) == m3_cell_flag_wall )
    {
        return;
    }

    for( int i = 0; i < sizeof( routines ) / sizeof( m3_match_routine* ); i++ )
    {
        if( routines[i] == NULL )
        {
            continue;
        }

        m3_match_result_init( match_result, cell);

        // Match algorythm ( down / right traversal )
        routines[i]( options, cell, match_result );

        if( match_result->matched_count >= options->matches_required_to_clear )
        {
            return;
        }
    }

}

void
m3_match_either_cell( const struct m3_options* options,
                      const struct m3_cell*    cell_a,
                      const struct m3_cell*    cell_b,
                      struct m3_match_result*  cell_a_match_result,
                      struct m3_match_result*  cell_b_match_result )
{
    assert( options );
    assert( cell_a );
    assert( cell_b );
    assert( cell_a_match_result );
    assert( cell_b_match_result );

    const void* cells_w_match_results[][2] = {
        { cell_a, cell_a_match_result },
        { cell_b, cell_b_match_result }
    };


    for(uint8_t i = 0; i < (uint8_t)((sizeof(cells_w_match_results) / sizeof(void*)))/2; i++)
    {
        m3_match_cell( options,
                       (const struct m3_cell*)cells_w_match_results[i][0],
                       (struct m3_match_result*)cells_w_match_results[i][1] );
    }
}

void
m3_match_vertical( const struct m3_options* options,
                   const struct m3_cell*    cell,
                   struct m3_match_result*  match_result )
{
    assert( options );
    assert( cell );
    assert( cell->top );
    assert( cell->bottom );
    assert( match_result );


    const struct m3_cell* cell_current = cell;
    const struct m3_cell* cell_top = cell->top;
    const struct m3_cell* cell_bottom = cell->bottom;

    const struct m3_cell* cell_top_most = cell;
    const struct m3_cell* cell_bottom_most = cell;

    const struct m3_cell** cell_edges[] = {
        &cell_top_most,
        &cell_bottom_most
    };

    struct m3_cell* cell_temp_mut = NULL;
    m3_match_routine* horizontal_routine = NULL;
    m3_match_routine* vertical_routine = NULL;


    while( cell_current->category == cell_top->category )
    {

        m3_match_result_add_match( match_result, cell_top);

        cell_current = cell_top;
        cell_top_most = cell_top;
        cell_top = cell_top->top;
    }

    cell_current = cell;

    while( cell_current->category == cell_bottom->category )
    {
        m3_match_result_add_match( match_result, cell_bottom);

        cell_current = cell_bottom;
        cell_bottom_most = cell_bottom;
        cell_bottom = cell_bottom->bottom;
    }


    if( match_result->matched_count >= options->matches_required_to_clear )
    {
        // Allows for L and T matches
        if( cell->horizontal_routine != NULL )
        {
            for( uint8_t i = 0; i < sizeof(cell_edges) / sizeof(const struct m3_cell**); i++)
            {
                // Allows to assign to const cell
                cell_temp_mut = (struct m3_cell*)*cell_edges[i];

                // Store and unset to not infinitely call each other
                horizontal_routine = cell_temp_mut->horizontal_routine;
                vertical_routine = cell_temp_mut->vertical_routine;

                cell_temp_mut->horizontal_routine = NULL;
                cell_temp_mut->vertical_routine = NULL;

                horizontal_routine(options, (const struct m3_cell*)cell_temp_mut, match_result);

                cell_temp_mut->horizontal_routine = horizontal_routine;
                cell_temp_mut->vertical_routine = vertical_routine;
            }
        }
    }
}

void
m3_match_horizontal( const struct m3_options* options,
                     const struct m3_cell*    cell,
                     struct m3_match_result*  match_result )
{
    assert( options );
    assert( cell );
    assert( cell->right );
    assert( cell->left );
    assert( match_result );

    const struct m3_cell* cell_current = cell;
    const struct m3_cell* cell_right = cell->right;
    const struct m3_cell* cell_left = cell->left;

    const struct m3_cell* cell_right_most = cell;
    const struct m3_cell* cell_left_most = cell;

    const struct m3_cell** cell_edges[] = {
        &cell_right_most,
        &cell_left_most
    };

    struct m3_cell* cell_temp_mut = (struct m3_cell*)cell;
    m3_match_routine* horizontal_routine = NULL;
    m3_match_routine* vertical_routine = NULL;

    while( cell_current->category == cell_right->category )
    {
        m3_match_result_add_match( match_result, cell_right);

        cell_current = cell_right;
        cell_right_most = cell_right;
        cell_right = cell_current->right;

    }

    cell_current = cell;

    while( cell_current->category == cell_left->category )
    {
        m3_match_result_add_match( match_result, cell_left);

        cell_current = cell_left;
        cell_left_most = cell_left;
        cell_left = cell_current->left;

    }


    if( match_result->matched_count >= options->matches_required_to_clear )
    {
        // Allows for L and T matches
        if( cell->vertical_routine != NULL )
        {
            for( uint8_t i = 0; i < sizeof(cell_edges) / sizeof(const struct m3_cell**); i++)
            {
                // Allows to assign to const cell
                cell_temp_mut = (struct m3_cell*)*cell_edges[i];

                // Store and unset to not infinitely call each other
                horizontal_routine = cell_temp_mut->horizontal_routine;
                vertical_routine = cell_temp_mut->vertical_routine;

                cell_temp_mut->horizontal_routine = NULL;
                cell_temp_mut->vertical_routine = NULL;

                vertical_routine(options, (const struct m3_cell*)cell_temp_mut, match_result);

                cell_temp_mut->horizontal_routine = horizontal_routine;
                cell_temp_mut->vertical_routine = vertical_routine;
            }
        }
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
m3_match_help( const struct m3_options*      options,
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

    struct m3_match_result    match_result_subject = M3_MATCH_RESULT_CONST;
    struct m3_match_result    match_result_target  = M3_MATCH_RESULT_CONST;

    struct m3_match_result* const match_results[] = {
        &match_result_subject,
        &match_result_target
    };

    static m3_swap_routine* swap_routines[] = {
        &m3_swap_top,
        &m3_swap_right,
        &m3_swap_bottom,
        &m3_swap_left
    };

    // Reset
    *match_help_result = match_help_result_const;

    while( cell_current != NULL )
    {
        if( ( cell_current->category & m3_cell_flag_wall ) != m3_cell_flag_wall )
        {
            for( uint8_t i = 0; i < sizeof( swap_routines ) / sizeof( m3_swap_routine* ); i++ )
            {
                subject = (struct m3_cell*)cell_current;
                target  = NULL;

                swap_routines[i]( &subject, &target );

                if( subject != NULL && target != NULL )
                {
                    m3_match_either_cell( options,
                                          subject,
                                          target,
                                          &match_result_subject,
                                          &match_result_target );

                    swap_routines[i]( &subject, &target );

                    for( uint8_t m = 0; m < sizeof(match_results) / sizeof(struct m3_match_result*); m++)
                    {
                        if( match_results[m]->matched_count >= options->matches_required_to_clear )
                        {
                            match_help_result->swap_match = match_results[m]->matched[0];
                        }
                    }

                    if( match_help_result->swap_match != NULL )
                    {
                        match_help_result->swap_subject   = subject;
                        match_help_result->swap_target    = target;

                        for( uint8_t m = 0; m < sizeof(match_results) / sizeof(struct m3_match_result*); m++)
                        {
                            m3_match_result_destroy(match_results[m]);
                        }
                        return;
                    }

                }
            }
        }

        cell_current = cell_current->next;
    } // while

    for( uint8_t m = 0; m < sizeof(match_results) / sizeof(struct m3_match_result*); m++)
    {
        m3_match_result_destroy(match_results[m]);
    }
}

int
m3_match_help_has_swapped_and_matched( struct m3_match_help_result match_help_result )
{
    return ( match_help_result.swap_subject != NULL && 
             match_help_result.swap_target  != NULL &&
             match_help_result.swap_match   != NULL );
}

void
m3_match_clear( const struct m3_options* options,
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
        ((struct m3_cell*)match_result->matched[i])->category = m3_cell_flag_color | m3_cell_flag_color_open;
    }
}

void
m3_match_clear_sort( const struct m3_options*  options,
                     struct m3_match_result*   match_result )
{
    assert( options );
    assert( match_result );

    const struct m3_cell* cell_first_top_color = NULL;

    struct m3_cell* cell_to_fallthrough = NULL;

    // slide / rotate the cleared cells
    for( uint8_t i = 0; i < match_result->matched_count; i++ )
    {
        m3_cell_find_first_top_color( (struct m3_cell*)match_result->matched[i], &cell_first_top_color );

        cell_to_fallthrough = (struct m3_cell*)cell_first_top_color;

        while( cell_to_fallthrough != NULL )
        {
            m3_cell_fallthrough( options, &cell_to_fallthrough );

            m3_cell_find_first_top_color( cell_to_fallthrough, &cell_first_top_color );

            cell_to_fallthrough = (struct m3_cell*)cell_first_top_color;
        }
    }
}
