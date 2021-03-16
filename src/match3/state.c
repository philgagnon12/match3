#include <stdint.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>

#include "match3/match3.h"
#include "match3/cell.h"


#include "match3/print.h" // TODO remove

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

    uint8_t* state_buffer = NULL;
    int state_buffer_size = 0;
    uint8_t* state_buffer_re = NULL;
    int state_buffer_size_req = 0;

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
    printf("most_sign_bit_pos %u\n", most_sign_bit_pos );

    while( cell_current != NULL )
    {
        if( ( cell_current->category & m3_cell_flag_color ) != m3_cell_flag_color )
        {
            cell_current = cell_current->next;
            continue;
        }

        state_buffer_size_req = (int)ceil((double)((++cell_count)*most_sign_bit_pos)/8);

        if( state_buffer_size < state_buffer_size_req )
        {
            state_buffer_re = realloc( state_buffer, state_buffer_size_req );
            assert(state_buffer_re);
            state_buffer_size = state_buffer_size_req;
            state_buffer = state_buffer_re;
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


        state_buffer[state_buffer_size-1] = ( state_buffer[state_buffer_size-1] << most_sign_bit_pos ) | *color_index;

        // m3_print_bits(state_buffer[state_buffer_size-1]);
        // printf("\n");

        cell_current = cell_current->next;
    } // while

    state_buffer_size_req = sizeof( options->seed ) +
                            sizeof( options->columns ) +
                            state_buffer_size;

    state_buffer_re = realloc( state_buffer, state_buffer_size_req );
    assert(state_buffer_re);
    state_buffer_size = state_buffer_size_req;
    state_buffer = state_buffer_re;

    state_buffer = state_buffer >> (sizeof( options->columns )*8);
    *state_buffer = options->columns;


    for(int i = 0; i < state_buffer_size; i++)
    {
        m3_print_bits(state_buffer[i]);
        printf(" ");
    }
        printf("\n");



}
