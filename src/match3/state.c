#include <stdint.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <assert.h>

#include "match3/match3.h"
#include "match3/cell.h"
#include "match3/board.h"

// TODO state_save & load destroy

void
m3_state_save( const struct m3_options* options,
               const struct m3_cell*    board,
               char**                   state,
               int*                     state_size )
{
    assert( options );
    assert( board );
    assert( state );
    assert( state_size );

    const int colors_count = options->colors_size / sizeof( *options->colors );

    // Position of the most significant bit
    uint8_t most_sign_bit_pos = 1;

    const struct m3_cell* cell_current = board;

    int cell_count = 0;

    uint8_t* cells = NULL;
    int cells_size = 0;
    uint8_t* cells_re = NULL;
    int cells_size_req = 0;

    uint8_t* color_index = NULL;

    for( uint8_t i = 1; i != 0; i = i << 1 )
    {
        if( ( i & colors_count ) == i )
        {
            most_sign_bit_pos++;
        }
    }

    if( most_sign_bit_pos < 4 )
    {
        most_sign_bit_pos = 4;
    }
    else
    {
        most_sign_bit_pos = 8;
    }

    while( cell_current != NULL )
    {
        if( ( cell_current->category & m3_cell_flag_color ) != m3_cell_flag_color )
        {
            cell_current = cell_current->next;
            continue;
        }

        cells_size_req = (int)ceil((double)((++cell_count)*most_sign_bit_pos)/8);

        if( cells_size < cells_size_req )
        {
            cells_re = realloc( cells, cells_size_req );
            assert(cells_re);
            cells_size = cells_size_req;
            cells = cells_re;
        }

        color_index = NULL;
        uint8_t i = 0;

        for( ; i < colors_count; i++ )
        {
            if( cell_current->category == options->colors[i] )
            {
                color_index = &i;
                break;
            }
        }

        assert( color_index );


        cells[cells_size-1] = ( cells[cells_size-1] << most_sign_bit_pos ) | *color_index;

        cell_current = cell_current->next;
    } // while

    *state_size = sizeof( options->seed ) +
                  sizeof( options->columns ) +
                  cells_size;

    *state = malloc( *state_size );
    assert( *state );

    memcpy( *state,
            &options->seed,
            sizeof( options->seed ) );

    memcpy( *state + sizeof( options->seed ),
            &options->columns,
            sizeof( options->columns ) );

    memcpy( *state + sizeof( options->seed ) + sizeof( options->columns ),
            cells,
            cells_size );

    free( cells );
}

void
m3_state_save_2( const struct m3_options* options,
               const struct m3_cell*    board,
               char**                   state,
               int*                     state_size )
{
    assert( options );
    assert( board );
    assert( state );
    assert( state_size );

    char* state_offset = NULL;
    char* state_re = NULL;

    const int colors_count = options->colors_size / sizeof( *options->colors );

    const struct m3_cell* cell_current = board;

    *state_size = sizeof( options->seed ) +
                  sizeof( options->columns ) +
                  sizeof( options->rows ) +
                  sizeof( options->colors_size) +
                  options->colors_size;

    *state = state_offset = malloc( *state_size );

    memcpy( state_offset,
            &options->seed,
            sizeof( options->seed ) );

    state_offset += sizeof( options->seed );

    memcpy( state_offset,
            &options->columns,
            sizeof( options->columns ) );

    state_offset += sizeof( options->columns );

    memcpy( state_offset,
            &options->rows,
            sizeof( options->rows ) );

    state_offset += sizeof( options->rows );

    memcpy( state_offset,
            &options->colors_size,
            sizeof( options->colors_size ) );

    state_offset += sizeof( options->colors_size );

    for(int i = 0; i < colors_count; i++)
    {
        memcpy( state_offset,
                &options->colors[i],
                sizeof( options->colors[i] ) );

        state_offset += sizeof( options->colors[i] );
    }

    while( cell_current != NULL )
    {
        if( ( cell_current->category & m3_cell_flag_color ) != m3_cell_flag_color )
        {
            cell_current = cell_current->next;
            continue;
        }

        state_re = realloc(*state, *state_size + sizeof(cell_current->category));
        assert(state_re);
        *state = state_offset = state_re;

        state_offset += *state_size;

        memcpy( state_offset,
                &cell_current->category,
                sizeof(cell_current->category) );

        *state_size += sizeof(cell_current->category);

        cell_current = cell_current->next;
    }

}

// TODO options will need a destroy when 
// state_load_destroy
// TODO validationwith state_size for every field
void
m3_state_load_2( char*              state,
               int                state_size,
               struct m3_options** options,
               struct m3_cell**   board )
{
    assert(state);
    assert(options);
    assert(board);

    struct m3_cell* cell_current = NULL;

    int cell_index = 0;

    *options = malloc( sizeof(**options) );
    assert(*options);

    memcpy( &(*options)->seed,
            state,
            sizeof((*options)->seed) );

    state += sizeof((*options)->seed);

    memcpy( &(*options)->columns,
            state,
            sizeof((*options)->columns) );

    state += sizeof((*options)->columns);

    memcpy( &(*options)->rows,
            state,
            sizeof((*options)->rows) );

    state += sizeof((*options)->rows);

    memcpy( &(*options)->colors_size,
            state,
            sizeof((*options)->colors_size) );

    state += sizeof((*options)->colors_size);

    (*options)->colors = malloc((*options)->colors_size);
    assert((*options)->colors);

    for(int i = 0; i < (int)((*options)->colors_size / sizeof(*((*options)->colors))); i++)
    {
        memcpy( &((*options)->colors[i]),
                state,
                sizeof(*((*options)->colors)));
        
        state += sizeof(*((*options)->colors));
    }



    m3_board_build(*options, board);
    assert(board);

    cell_current = *board;

    while( cell_current != NULL )
    {
        if( ( cell_current->category & m3_cell_flag_color ) != m3_cell_flag_color )
        {
            cell_current = cell_current->next;
            continue;
        }

        cell_current->category = (uint8_t)state[cell_index];
        cell_index++;

        cell_current = cell_current->next;
    }

}

void
m3_state_load( char*              state,
               int                state_size,
               struct m3_options* options,
               struct m3_cell**   board )
{
    assert(state);
    assert(options);
    assert(board);

    const int colors_count = options->colors_size / sizeof( *options->colors );

    uint8_t most_sign_bit_pos = 1;

    struct m3_cell* cell_current = NULL;

    // seed
    // ====
    if( state_size < sizeof(options->seed))
        return;

    memcpy( &options->seed,
            state,
            sizeof( options->seed ) );

    state = state + sizeof( options->seed );
    state_size -= sizeof( options->seed );

    // columns
    // =======

    if( state_size < sizeof(options->columns) )
        return;

    memcpy( &options->columns,
            state,
            sizeof( options->columns ));

    state = state + sizeof( options->columns );
    state_size -= sizeof( options->columns );


    for( uint8_t i = 1; i != 0; i = i << 1 )
    {
        if( ( i & colors_count ) == i )
        {
            most_sign_bit_pos++;
        }
    }

    if( most_sign_bit_pos < 4 )
    {
        most_sign_bit_pos = 4;
    }
    else
    {
        most_sign_bit_pos = 8;
    }

    int cells_count = state_size / ( (double)most_sign_bit_pos / 8 );
    int colors_per_byte = (8 / ( (double)most_sign_bit_pos / 8 ) ) / 8;

    options->rows = cells_count / options->columns;

    m3_board_build( options, board );

    assert(*board);
    cell_current = *board;

    while( (cell_current->category & m3_cell_flag_color) != m3_cell_flag_color )
    {
        cell_current = cell_current->next;
    }

    uint8_t color_index_a = 0;
    uint8_t color_index_b = 0;

    for( int i = 0; i < state_size; i++ )
    {
        uint8_t* color_indexes[] = {
            NULL,
            NULL
        };

        if( most_sign_bit_pos == 4 )
        {
            color_index_a = (state[i] >> 4);
            color_index_b = ((uint8_t)(state[i] << 4) >> 4);

            color_indexes[0] = &color_index_a;
            color_indexes[1] = &color_index_b;
        }
        else
        {
            color_index_a = state[i];
            color_indexes[0] = &color_index_a;
        }

        for( int ci = 0; ci < sizeof(color_indexes) / sizeof(uint8_t*); ci++ )
        {
            if( color_indexes[ci] != NULL )
            {
                if(*color_indexes[ci] >= colors_count)
                    return; // ERROR

                cell_current->category = options->colors[*color_indexes[ci]];
                cell_current = cell_current->next;

                while( cell_current != NULL && (cell_current->category & m3_cell_flag_color) != m3_cell_flag_color )
                {
                    cell_current = cell_current->next;
                }
            }
        }
    }
}
