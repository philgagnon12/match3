#include <stdint.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <assert.h>

#include "match3/match3.h"
#include "match3/cell.h"

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
