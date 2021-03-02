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

void
cell_find_first_top_color( const struct m3_cell*    board,
                           const struct m3_cell**   cell_first_top_color )
{

    assert(board);
    assert(cell_first_top_color);

    const struct m3_cell* first_top_color = board->top;

    // Reset
    *cell_first_top_color = NULL;

    while( ( first_top_color->category & cell_mask_wall ) != cell_mask_wall &&
           first_top_color->category != ( cell_mask_color | cell_mask_color_open ) )
    {
        *cell_first_top_color = first_top_color;
        first_top_color = first_top_color->top;
    }
}

// a color cell can fall through color-open cell
void
cell_fallthrough( const struct m3_options* options,
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
        assert( subject->bottom );
        if( (subject->bottom->category & (cell_mask_color | cell_mask_color_open )) == (cell_mask_color | cell_mask_color_open ))
        {
            swap_bottom( &subject, &target );
            subject = target;            
        }
        else
        {
            subject = NULL;
        }
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

