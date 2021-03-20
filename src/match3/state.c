#include <stdint.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <assert.h>

#include "match3/match3.h"
#include "match3/state.h"
#include "match3/cell.h"
#include "match3/board.h"

void
m3_state_destroy( struct m3_state* state )
{
    assert(state);

    if(state->destroy != NULL)
    {
        state->destroy(state);
    }
}

static
void
state_destroy( struct m3_state* state )
{
    assert(state);

    if(state->buffer != NULL)
    {
        free(state->buffer);
    }
}

// Return zero on success
int
m3_state_save( const struct m3_options* options,
               const struct m3_cell*    board,
               struct m3_state*         state )
{
    assert( options );
    assert( board );
    assert( state );

    int ret = 0;

    char* state_offset = NULL;
    char* state_re = NULL;

    const int colors_count = options->colors_size / sizeof( *options->colors );

    const struct m3_cell* cell_current = board;

    if( m3_options_are_valid( options ) != 0 )
        ret = 1;

    if( ret == 0 )
    {
        state->destroy = &state_destroy;

        state->buffer_size = sizeof( options->seed ) +
                             sizeof( options->columns ) +
                             sizeof( options->rows ) +
                             sizeof( options->matches_required_to_clear ) +
                             sizeof( options->colors_size ) +
                             options->colors_size;

        state->buffer = state_offset = malloc( state->buffer_size );

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
                &options->matches_required_to_clear,
                sizeof( options->matches_required_to_clear ) );

        state_offset += sizeof( options->matches_required_to_clear );

        memcpy( state_offset,
                &options->colors_size,
                sizeof( options->colors_size ) );

        state_offset += sizeof( options->colors_size );
    }

    for(int i = 0; ret == 0 && i < colors_count; i++)
    {
        memcpy( state_offset,
                &options->colors[i],
                sizeof( options->colors[i] ) );

        state_offset += sizeof( options->colors[i] );
    }

    while( ret == 0 && cell_current != NULL )
    {
        if( ( cell_current->category & m3_cell_flag_color ) != m3_cell_flag_color )
        {
            cell_current = cell_current->next;
            continue;
        }

        int i = 0;
        int* color_index = NULL;

        // Check that its a defined color
        for(; color_index == NULL && i < colors_count; i++)
        {
            if( cell_current->category == options->colors[i] )
                color_index = &i;
        }

        if( color_index == NULL )
            ret = 1;

        if( ret == 0 )
        {
            state_re = realloc(state->buffer, state->buffer_size + sizeof(cell_current->category));
            assert(state_re);
            state->buffer = state_offset = state_re;

            state_offset += state->buffer_size;

            memcpy( state_offset,
                    &cell_current->category,
                    sizeof(cell_current->category) );

            state->buffer_size += sizeof(cell_current->category);

            cell_current = cell_current->next;
        }
    }

    if( ret != 0 )
    {
        if( state != NULL )
        {
            m3_state_destroy(state);
        }
    }

    return ret;
}

static
void
options_destroy( struct m3_options* options )
{
    assert(options);

    if(options->colors != NULL)
    {
        free((uint8_t*)options->colors);
    }

    free(options);
}

// Return zero on success
int
m3_state_load( const struct m3_state*   state,
               struct m3_options**      options,
               struct m3_cell**         board )
{
    assert(state);
    assert(options);
    assert(board);

    int ret = 0;

    char* state_buffer = (char*)state->buffer;
    size_t state_buffer_size = state->buffer_size;

    struct m3_cell* cell_current = NULL;

    int cell_index = 0;

    *options = malloc( sizeof(**options) );
    assert(*options);
    (*options)->destroy = &options_destroy;

    if( state_buffer_size < sizeof((*options)->seed) )
        ret = 1;


    if( ret == 0 )
    {
        memcpy( &(*options)->seed,
                state_buffer,
                sizeof((*options)->seed) );

        state_buffer += sizeof((*options)->seed);
        state_buffer_size -= sizeof((*options)->seed);

        if( state_buffer_size < sizeof((*options)->columns) )
            ret = 1;
    }

    if( ret == 0 )
    {
        memcpy( &(*options)->columns,
                state_buffer,
                sizeof((*options)->columns) );

        state_buffer += sizeof((*options)->columns);
        state_buffer_size -= sizeof((*options)->columns);

        if( state_buffer_size < sizeof((*options)->rows) )
            ret = 1;
    }

    if( ret == 0 )
    {
        memcpy( &(*options)->rows,
                state_buffer,
                sizeof((*options)->rows) );

        state_buffer += sizeof((*options)->rows);
        state_buffer_size -= sizeof((*options)->rows);

        if( state_buffer_size < sizeof((*options)->matches_required_to_clear) )
            ret = 1;
    }

    if( ret == 0 )
    {
        memcpy( &(*options)->matches_required_to_clear,
                state_buffer,
                sizeof((*options)->matches_required_to_clear) );

        state_buffer += sizeof((*options)->matches_required_to_clear);
        state_buffer_size -= sizeof((*options)->matches_required_to_clear);

        if( state_buffer_size < sizeof((*options)->colors_size) )
            ret = 1;
    }

    if( ret == 0 )
    {
        memcpy( &(*options)->colors_size,
                state_buffer,
                sizeof((*options)->colors_size) );

        state_buffer += sizeof((*options)->colors_size);
        state_buffer_size -= sizeof((*options)->colors_size);
    }

    if( ret == 0 )
    {
        (*options)->colors = malloc((*options)->colors_size);

        if( (*options)->colors == NULL )
            ret = 1;
    }

    for(int i = 0; ret == 0 && i < (int)((*options)->colors_size / sizeof(*((*options)->colors))); i++)
    {
        if( state_buffer_size < sizeof(*((*options)->colors)) )
            ret = 1;

        if( ret == 0 )
        {
            memcpy( (uint8_t*)(&((*options)->colors[i])),
                    state_buffer,
                    sizeof(*((*options)->colors)));

            state_buffer += sizeof(*((*options)->colors));
            state_buffer_size -= sizeof(*((*options)->colors));
        }
    }

    if( ret == 0 )
    {
        if( m3_options_are_valid( *options ) != 0 )
            ret = 1;
    }

    if( ret == 0 )
    {
        // Check that our given columns and rows gives us the amount of cells from the state_buffer_size that remains
        if( state_buffer_size < (size_t)((*options)->columns * (*options)->rows) )
            ret = 1;
    }

    if( ret == 0 )
    {
        m3_board_build(*options, board);
        assert(board);

        cell_current = *board;
    }

    while( ret == 0 && cell_current != NULL )
    {
        if( ( cell_current->category & m3_cell_flag_color ) != m3_cell_flag_color )
        {
            cell_current = cell_current->next;
            continue;
        }

        cell_current->category = (uint8_t)state_buffer[cell_index];
        cell_index++;

        // Check that the color is one of the defined ones
        int i = 0;
        int* color_index = NULL;

        for(; color_index == NULL && i < (int)((*options)->colors_size / sizeof(*((*options)->colors))); i++)
        {
            if( cell_current->category == (*options)->colors[i] )
                color_index = &i;
        }

        if( color_index == NULL )
            ret = 1;

        if( ret == 0 )
        {
            cell_current = cell_current->next;
        }
    }

    if( ret != 0 )
    {
        if( *board != NULL )
        {
            m3_board_destroy( *board );
            *board = NULL;
        }
        if( *options != NULL )
        {
            m3_options_destroy(*options);
            *options = NULL;
        }
    }

    return ret;
}

