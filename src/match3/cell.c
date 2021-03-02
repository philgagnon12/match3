#include <stdint.h>
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>

#include "match3/cell.h"
#include "match3/swap.h"

void
cell_rand( const struct m3_options* options,
           struct m3_cell*          cell )
{
    assert( options );
    assert( cell );

    int rand_color = 0;
    double color_range = RAND_MAX / ( options->colors_size / sizeof( enum cell_masks ) );


    if( ( cell->category & cell_mask_color ) == cell_mask_color )
    {
        rand_color = rand();

        for( uint8_t c = 0; c < options->colors_size / sizeof( enum cell_masks ); c++ )
        {
            if( rand_color >= color_range * c && rand_color < ( color_range * (c+1) ) )
            {
                cell->category = ( cell_mask_color | options->colors[c] );
            }
        }
    }
}


// Primary use is to give it a cleared cell, so that it can end up at the top
void
cell_pop_unshift( const struct m3_options* options,
                  struct m3_cell**         cell )
{
    assert( options );
    assert( cell );
    assert( *cell );


    struct m3_cell* subject = NULL;
    struct m3_cell* target = NULL;

    subject = *cell;

    while( subject != NULL )
    {
        *cell = subject;
        swap_top( &subject, &target );
        subject = target;
    }
}



int
cell_star_unique_compar( const void* a, const void* b)
{
    int rand_int = rand();
    return rand_int;
}

void
cell_star_unique( const struct m3_options* options,
                  struct m3_cell*          cell )
{
    assert( options );
    assert( cell );


    struct m3_cell* cells[] = {
        cell->top,
        cell->right,
        cell->bottom,
        cell->left,
        cell
    };

    qsort( options->colors,
           options->colors_size / sizeof( enum cell_masks ),
           sizeof( enum cell_masks ),
           &cell_star_unique_compar );

    uint8_t const_colors_count = options->colors_size / sizeof( enum cell_masks );
    uint8_t colors_count = const_colors_count;

    for( uint8_t i = 0; i < sizeof( cells ) / sizeof( struct m3_cells* ); i++ )
    {
        if( ( cells[i]->category & cell_mask_color ) == cell_mask_color )
        {
            cells[i]->category = cell_mask_color | options->colors[--colors_count];


            if( colors_count == 0 )
            {
                colors_count = const_colors_count;
            }
        }
    }

}

