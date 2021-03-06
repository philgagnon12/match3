#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "match3/board.h"
#include "match3/options.h"
#include "match3/match.h"

void
m3_board_build( const struct m3_options*   options,
                struct m3_cell**           board )
{
    assert( options );
    assert( board );

    uint8_t columns = options->columns + 2; // right & left wall
    uint8_t rows = options->rows + 2; // top & bottom wall

    assert( columns > 2 );
    assert( rows > 2 );

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

    // Reset
    *board = NULL;

    srand( options->seed );

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


            // The output board is assigned to the first cell
            if( *board == NULL )
            {
                *board = cell_current;
            }

            cell_category = 0;

            // Left wall
            if( c == 0 )
            {
                cell_category |= m3_cell_flag_wall | m3_cell_flag_wall_left;
            }

            if( c == columns - 1 )
            {
                cell_category |= m3_cell_flag_wall | m3_cell_flag_wall_right;
            }

            if( r == 0 )
            {
                cell_category |= m3_cell_flag_wall | m3_cell_flag_wall_top;
            }

            if( r == rows - 1 )
            {
                cell_category |= m3_cell_flag_wall | m3_cell_flag_wall_bottom;
            }

            if( c > 0 &&
                c < ( columns - 1 ) &&
                r > 0 &&
                r < ( rows - 1 ) )
            {
                cell_category |= m3_cell_flag_color | m3_cell_flag_color_open;
                cell_current->horizontal_routine = &m3_match_horizontal;
                cell_current->vertical_routine = &m3_match_vertical;
            }

            cell_current->category = cell_category;

            cell_current->column = c;
            cell_current->row    = r;

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
m3_board_rand( struct m3_options*   options,
               struct m3_cell*      cell )
{

    assert( cell );

    struct m3_cell* cell_current      = cell;

    while( cell_current != NULL )
    {
        if( cell_current->category == ( m3_cell_flag_color | m3_cell_flag_color_open ) )
        {
            m3_cell_rand( options, cell_current );
        }

        cell_current = cell_current->next;
    }
}

void
m3_board_shuffle( struct m3_options* options,
                  struct m3_cell*    board )
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
            m3_cell_rand( options, matched_result_first_cell );
            m3_match_cell( options, matched_result_first_cell, &matched_result );
            matched_result_first_cell = (struct m3_cell*)matched_result.matched[0];
        }

        m3_match( options, matched_result_first_cell, &matched_result );
        matched_result_first_cell = (struct m3_cell*)matched_result.matched[0];

    }

    m3_match_result_destroy(&matched_result);

    printf("shuffled\n");
}

void
m3_board_destroy( struct m3_cell* board )
{
    assert( board );

    struct m3_cell* cell_free      = board->next;
    struct m3_cell* cell_next      = NULL;

    while( cell_free != NULL )
    {
        cell_next = cell_free->next;
        free( cell_free );
        cell_free = cell_next;
    }

    // destroy the 1 undefined wall that is re-used alot
    assert( board->top);
    assert( board->top->category == ( m3_cell_flag_wall | m3_cell_flag_wall_undefined ) );
    free( board->top );

    free( board );
}

// Return 0 when identical
unsigned int
m3_board_are_identical( const struct m3_cell* board_a,
                        const struct m3_cell* board_b )
{
    assert(board_a);
    assert(board_b);

    const struct m3_cell* cell_current_a = board_a;
    const struct m3_cell* cell_current_b = board_b;

    unsigned int differences = 0;

    while( cell_current_a != NULL || cell_current_b != NULL )
    {
        if( cell_current_a != NULL && cell_current_b != NULL )
        {
            if( cell_current_a->category != cell_current_b->category )
                differences++;
        }

        if( cell_current_a != NULL && cell_current_b == NULL )
                differences++;

        if( cell_current_b != NULL && cell_current_a == NULL )
                differences++;

        if( cell_current_a != NULL )
            cell_current_a = cell_current_a->next;

        if( cell_current_b != NULL )
            cell_current_b = cell_current_b->next;
    }

    return differences;
}

// Assign color to cell flagged as open color, going from column to column, top to bottom
void
m3_board_fill_columns( const struct m3_options* options,
                       struct m3_cell*          board,
                       uint8_t*                 colors,
                       size_t                   colors_size )
{
    assert(options);
    assert(board);
    assert(colors);

    struct m3_options_find_colors_result find_colors_result = M3_OPTIONS_FIND_COLORS_RESULT_CONST;

    uint32_t colors_count = colors_size / sizeof(*colors);

    m3_options_find_colors( options,
                            colors,
                            colors_size,
                            &find_colors_result );

    assert(colors_count == find_colors_result.colors_count);

    struct m3_cell* cell_color_top_most = NULL;
    struct m3_cell* cell_current = board;

    while( cell_current != NULL && cell_color_top_most == NULL )
    {
        if( (cell_current->category & m3_cell_flag_color) == m3_cell_flag_color )
        {
            cell_color_top_most = cell_current;
        }

        cell_current = cell_current->next;
    }

    cell_current = cell_color_top_most;

    for( uint32_t c = 0; cell_current != NULL && c < find_colors_result.colors_count; c++)
    {
        while( cell_current != NULL && cell_current->category != (m3_cell_flag_color | m3_cell_flag_color_open))
        {
            cell_current = cell_current->bottom;

            if( cell_current != NULL && ( cell_current->category & m3_cell_flag_wall ) == m3_cell_flag_wall )
            {
                cell_current = cell_color_top_most->right;
                cell_color_top_most = cell_current;

				if (cell_current != NULL && (cell_current->category & m3_cell_flag_wall) == m3_cell_flag_wall)
				{
					cell_current = NULL;
				}
            }
        }
        if( cell_current != NULL )
        {
            if( cell_current->category == (m3_cell_flag_color | m3_cell_flag_color_open))
            {
                cell_current->category = *find_colors_result.colors[c];
            }
            cell_current = cell_current->bottom;
        }
    }

    m3_options_find_colors_result_destroy(&find_colors_result);
}

// Assign color to cell flagged as open color, going from row to row, left to right
void
m3_board_fill_rows( const struct m3_options* options,
                    struct m3_cell*          board,
                    uint8_t*                 colors,
                    size_t                   colors_size )
{
    assert(options);
    assert(board);
    assert(colors);

    struct m3_options_find_colors_result find_colors_result = M3_OPTIONS_FIND_COLORS_RESULT_CONST;

    uint32_t colors_count = colors_size / sizeof(*colors);

    m3_options_find_colors( options,
                            colors,
                            colors_size,
                            &find_colors_result );

    assert(colors_count == find_colors_result.colors_count);

    struct m3_cell* cell_color_left_most = NULL;
    struct m3_cell* cell_current = board;

    while( cell_current != NULL && cell_color_left_most == NULL )
    {
        if( (cell_current->category & m3_cell_flag_color) == m3_cell_flag_color )
        {
            cell_color_left_most = cell_current;
        }

        cell_current = cell_current->next;
    }

    cell_current = cell_color_left_most;

    for( uint32_t c = 0; cell_current != NULL && c < find_colors_result.colors_count; c++)
    {
        while( cell_current != NULL && cell_current->category != (m3_cell_flag_color | m3_cell_flag_color_open))
        {
            cell_current = cell_current->right;

            if( cell_current != NULL && ( cell_current->category & m3_cell_flag_wall ) == m3_cell_flag_wall )
            {
                cell_current = cell_color_left_most->bottom;
                cell_color_left_most = cell_current;

				if (cell_current != NULL && (cell_current->category & m3_cell_flag_wall) == m3_cell_flag_wall)
				{
					cell_current = NULL;
				}
            }
        }
        if( cell_current != NULL )
        {
            if( cell_current->category == (m3_cell_flag_color | m3_cell_flag_color_open))
            {
                cell_current->category = *find_colors_result.colors[c];
            }
            cell_current = cell_current->right;
        }
    }

    m3_options_find_colors_result_destroy(&find_colors_result);
}