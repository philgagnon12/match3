#include <stdint.h>
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <time.h>

enum cell_masks {
    cell_mask_wall              = (1 << 7),
    cell_mask_wall_undefined    = (0 << 0),
    cell_mask_wall_top          = (1 << 0),
    cell_mask_wall_right        = (1 << 1),
    cell_mask_wall_bottom       = (1 << 2),
    cell_mask_wall_left         = (1 << 3),

    cell_mask_color             = (1 << 6),
    cell_mask_color_undefined   = (0 << 0),
    cell_mask_color_red         = (1 << 0),
    cell_mask_color_green       = (1 << 1),
    cell_mask_color_blue        = (1 << 2),
    cell_mask_color_yellow      = (1 << 3),
    cell_mask_color_purple      = (1 << 4),
};



// TODO colors array should be part of options so i can add remove colors to increase, decrease diffculty
struct m3_options {
    int     seed;
    uint8_t columns;
    uint8_t rows;
    uint8_t matches_required_to_clear;
    enum cell_masks* colors;
    size_t colors_size;
};

struct m3_cell;

typedef void(match_routine)( const struct m3_options    options,
                             const struct m3_cell*      self,
                             const struct m3_cell**     matched );

typedef void(swap_routine)( struct m3_cell**   subject,
                            struct m3_cell**   target );


struct m3_cell {
    uint8_t             category;

    struct m3_cell*     top;
    struct m3_cell*     right;
    struct m3_cell*     bottom;
    struct m3_cell*     left;

    match_routine*      right_routine;
    match_routine*      bottom_routine;
};

#define M3_CELL_CONST { \
    cell_mask_wall | cell_mask_wall_undefined,   \
    NULL,   \
    NULL,   \
    NULL,   \
    NULL,   \
    NULL,   \
    NULL,   \
}


void
print_cell( const struct m3_cell cell )
{
    switch( cell.category )
    {
        case ( cell_mask_wall | cell_mask_wall_top | cell_mask_wall_right ):
        case ( cell_mask_wall | cell_mask_wall_top | cell_mask_wall_left ):
        case ( cell_mask_wall | cell_mask_wall_bottom | cell_mask_wall_right ):
        case ( cell_mask_wall | cell_mask_wall_bottom | cell_mask_wall_left ):
            printf("x");
            break;
        case ( cell_mask_wall | cell_mask_wall_top ):
        case ( cell_mask_wall | cell_mask_wall_bottom ):
            printf("-");
            break;
        case ( cell_mask_wall | cell_mask_wall_right ):
        case ( cell_mask_wall | cell_mask_wall_left ):
            printf("|");
            break;

        case ( cell_mask_color | cell_mask_color_red ):
            printf("r");
            break;

        case ( cell_mask_color | cell_mask_color_green ):
            printf("g");
            break;

        case ( cell_mask_color | cell_mask_color_blue ):
            printf("b");
            break;

        case ( cell_mask_color | cell_mask_color_yellow ):
            printf("y");
            break;

        case ( cell_mask_color | cell_mask_color_purple ):
            printf("p");
            break;

        case ( cell_mask_color ):
            printf("c");
            break;

            // TODO remove default
        default:
            printf("!");
    }
}


void
print_neighbours( const struct m3_cell cell )
{
    printf(" ");
    print_cell( *cell.top );
    printf("\n");
    print_cell( *cell.left );
    print_cell( cell );
    print_cell( *cell.right );
    printf("\n");
    printf(" ");
    print_cell( *cell.bottom );
    printf("\n");
}

void
match_cell( const struct m3_options options,
            const struct m3_cell*   cell,
            const struct m3_cell**  matched )
{

    assert( cell );
    assert( matched );

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
        routines[i]( options, cell, matched );

        if( *matched != NULL )
        {
            return;
        }
    }

}


void
print_board( const struct m3_cell cell )
{
    const struct m3_cell* cell_left_most    = &cell;
    const struct m3_cell* cell_current      = &cell;


    while( ( cell_current->category | ( cell_mask_wall | cell_mask_wall_undefined ) ) != ( cell_mask_wall | cell_mask_wall_undefined ) )
    {

        print_cell( *cell_current );

        if( ( cell_current->category & ( cell_mask_wall | cell_mask_wall_right ) ) == ( cell_mask_wall | cell_mask_wall_right ) )
        {
            printf("\n");
            cell_current = cell_left_most->bottom;
            cell_left_most = cell_current;
        }
        else
        {
            cell_current = cell_current->right;
        }
    } // while

}

int
unique_star_cell_compar( const void* a, const void* b)
{
    return rand();
}

void
unique_star_cell( const struct m3_options*  options,
                  struct m3_cell*           cell )
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
           &unique_star_cell_compar );

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


void
rand_cell( const struct m3_options*     options,
           struct m3_cell*              cell )
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
rand_board( const struct m3_options*    options,
            struct m3_cell*             cell )
{

    assert( cell );

    struct m3_cell* cell_left_most    = cell;
    struct m3_cell* cell_current      = cell;

    while( ( cell_current->category | ( cell_mask_wall | cell_mask_wall_undefined ) ) != ( cell_mask_wall | cell_mask_wall_undefined ) )
    {
        rand_cell( options, cell_current );

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
}

// subject and target will become NULL if cant swap
// TODO  swap_to_match would only swap when a match can happen
// TODO swap_freedom would let a user do a swap even when there would be no match
void
swap( struct m3_cell**   subject,
      struct m3_cell**   target )
{
    assert( subject );
    assert( target );

    struct m3_cell* cell_subject = *subject;
    struct m3_cell* cell_target = *target;

    assert( cell_subject );
    assert( cell_target );

    *subject = NULL;
    *target  = NULL;

    uint8_t category_subject = cell_subject->category;

    if( ( cell_subject->category & cell_mask_color ) == ( cell_target->category & cell_mask_color ) )
    {
            cell_subject->category = cell_target->category;
            cell_target->category  = category_subject;

            *subject = cell_subject;
            *target  = cell_target;
    }
}

void
swap_top( struct m3_cell**   subject,
          struct m3_cell**   target )
{
    assert( subject );
    assert( target );

    *target = (*subject)->top;

    swap( subject, target );
}

void
swap_right( struct m3_cell**   subject,
            struct m3_cell**   target )
{
    assert( subject );
    assert( target );

    *target = (*subject)->right;

    swap( subject, target );
}

void
swap_bottom( struct m3_cell**   subject,
             struct m3_cell**   target )
{
    assert( subject );
    assert( target );

    *target = (*subject)->bottom;

    swap( subject, target );
}

void
swap_left( struct m3_cell**   subject,
           struct m3_cell**   target )
{
    assert( subject );
    assert( target );

    *target = (*subject)->left;

    swap( subject, target );
}

// call once only per board
void
match( const struct m3_options  options,
       const struct m3_cell*    cell,
       const struct m3_cell**   matched )
{
    assert( cell );
    assert( matched );

    struct m3_cell* neighbours[] = {
        cell->bottom,
        cell->right
    };

    if( ( cell->category & cell_mask_wall ) == cell_mask_wall )
    {
        return;
    }

    match_cell( options, cell, matched );

    if( *matched != NULL )
    {
        return;
    }

    for( int i = 0; i < sizeof( neighbours ) / sizeof( struct m3_cell* ); i++ )
    {

        // Make sure neighbours are not NULL
        assert( neighbours[i] );

        // Recursively match
        match( options, neighbours[i], matched );

        if( *matched != NULL )
        {
            return;
        }
    }
}


void
match_vertical( const struct m3_options  options,
                const struct m3_cell*    cell,
                const struct m3_cell**   matched )
{
    assert( cell );
    assert( cell->top );    
    assert( cell->bottom );
    assert( matched );


    uint8_t match_count = 1;
    const struct m3_cell* cell_current = cell;
    const struct m3_cell* cell_top = cell->top;
    const struct m3_cell* cell_bottom = cell->bottom;

    while( cell_current->category == cell_top->category )
    {
        match_count++;
        cell_current = cell_top;
        cell_top = cell_top->top;
    }

    cell_current = cell;

    while( cell_current->category == cell_bottom->category )
    {
        match_count++;
        cell_current = cell_bottom;
        cell_bottom = cell_bottom->bottom;
    }


    if( match_count >= options.matches_required_to_clear )
    {
        print_neighbours( *cell );
        printf("\nits bottom a match %02X %d\n", cell->category, match_count );

        // TODO undo
        *matched = cell;
    }
}

void
match_horizontal( const struct m3_options  options,
                  const struct m3_cell*    cell,
                  const struct m3_cell**   matched )
    {
    assert( cell );
    assert( cell->right );
    assert( cell->left );
    assert( matched );

    uint8_t match_count = 1;
    const struct m3_cell* cell_current = cell;
    const struct m3_cell* cell_right = cell->right;
    const struct m3_cell* cell_left = cell->left;

    while( cell_current->category == cell_right->category )
    {
        match_count++;
        cell_current = cell_right;
        cell_right = cell_current->right;

    }

    cell_current = cell;

    while( cell_current->category == cell_left->category )
    {
        match_count++;
        cell_current = cell_left;
        cell_left = cell_current->left;

    }


    if( match_count >= options.matches_required_to_clear )
    {
        print_neighbours( *cell );
        printf("\nits right a match %02X %d\n", cell->category, match_count );

        // TODO undo
        *matched = cell;
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

    const struct m3_cell* matched   = NULL;

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

            matched = NULL;

            swap_routines[i]( &subject, &target );

            if( subject != NULL && target != NULL )
            {
                match( options, cell, &matched );

                // Always undo the swap
                swap_routines[i]( &subject, &target );

                if( matched != NULL )
                {
                    *swap_subject   = subject;
                    *swap_target    = target;
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

}

void
board_build( struct m3_options options,
             struct m3_cell**   cell )
{
    assert( cell );

    uint8_t columns = options.columns;
    uint8_t rows = options.rows;

    assert( columns > 0 );
    assert( rows > 0 );

    uint8_t cell_category = 0;

    struct m3_cell* cell_wall_undefined = NULL;
    static const struct m3_cell const_cell_wall_undefined = M3_CELL_CONST;


    struct m3_cell* cell_current    = NULL;
    struct m3_cell* cell_top        = NULL;
    struct m3_cell* cell_top_most   = NULL;
    struct m3_cell* cell_right      = NULL;
    struct m3_cell* cell_bottom     = NULL;
    struct m3_cell* cell_left       = NULL;


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

            cell_category = 0;

            // Left wall
            if( c == 0 )
            {
                cell_category |= cell_mask_wall | cell_mask_wall_left;
            }

            if( c == columns - 1 )
            {
                cell_category |= cell_mask_wall | cell_mask_wall_right;
            }

            if( r == 0 )
            {
                cell_category |= cell_mask_wall | cell_mask_wall_top;
            }

            if( r == rows - 1 )
            {
                cell_category |= cell_mask_wall | cell_mask_wall_bottom;
            }

            if( c > 0 &&
                c < ( columns - 1 ) &&
                r > 0 &&
                r < ( rows - 1 ) )
            {
                cell_category |= cell_mask_color | cell_mask_color_undefined;
                cell_current->right_routine = &match_horizontal;
                cell_current->bottom_routine = &match_vertical;

                // The output cell is assigned to the first color cell
                if( *cell == NULL )
                {
                    *cell = cell_current;
                }
            }

            cell_current->category = cell_category;

            cell_current->top = cell_top;
            cell_top->bottom = cell_current;


            cell_current->right = cell_right;
            cell_right->left = cell_current;

            cell_current->bottom = cell_bottom;
            cell_bottom->top = cell_current;

            cell_current->left = cell_left;
            cell_left->right = cell_current;

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
board_destroy( struct m3_cell* cell )
{
    assert( cell );

    struct m3_cell* cell_left_most    = cell;
    struct m3_cell* cell_current      = cell;
    struct m3_cell* cell_free         = NULL;

    while( ( cell_current->category | ( cell_mask_wall | cell_mask_wall_undefined ) ) != ( cell_mask_wall | cell_mask_wall_undefined ) )
    {
        cell_free = cell_current;

        if( ( cell_current->category & ( cell_mask_wall | cell_mask_wall_right ) ) == ( cell_mask_wall | cell_mask_wall_right ) )
        {
            cell_current = cell_left_most->bottom;
            cell_left_most = cell_current;
        }
        else
        {
            cell_current = cell_current->right;
        }

        free( cell_free );
    } // while


    if( ( cell_current->category | ( cell_mask_wall | cell_mask_wall_undefined ) ) != ( cell_mask_wall | cell_mask_wall_undefined ) )
    {
        free( cell_current );
    }
}

void
print_board_info( const struct m3_cell cell )
{
    const struct m3_cell* cell_left_most    = &cell;
    const struct m3_cell* cell_current      = &cell;


    while( ( cell_current->category | ( cell_mask_wall | cell_mask_wall_undefined ) ) != ( cell_mask_wall | cell_mask_wall_undefined ) )
    {

        print_neighbours( *cell_current );
        printf("\n");

        if( ( cell_current->category & ( cell_mask_wall | cell_mask_wall_right ) ) == ( cell_mask_wall | cell_mask_wall_right ) )
        {
            cell_current = cell_left_most->bottom;
            cell_left_most = cell_current;
        }
        else
        {
            cell_current = cell_current->right;
        }
    } // while
}


// TODO find a way that seeds would work post shuffle, so that when you put in a seed it wouldnt have to shuffle the board
int
main( int argc, char* argv[] )
{

    int seed = 0;

    if( argc < 2 )
    {
        seed = (int)time(NULL);
    }
    else
    {
        seed = atoi( argv[1] );
    }

    printf("seed %d\n", seed );
    srand( seed );

    enum cell_masks colors[] = {
        cell_mask_color_red,
        cell_mask_color_green,
        cell_mask_color_blue,
        cell_mask_color_yellow,
        cell_mask_color_purple
    };

    const struct m3_options options = {
        seed,
        9, // columns
        8, // rows
        3,
        colors,
        sizeof( colors )
    };

    struct m3_cell* built_cell = NULL;

    board_build(options, &built_cell );

    rand_board( &options, built_cell->top->left );

    // Assigning first cell just because easy to do the loop that way
    struct m3_cell* matched = built_cell;

    // Shuffling the board so that there is no automatic match
    while( matched != NULL )
    {
        unique_star_cell( &options, matched );
        struct m3_cell* shuffled_cell = matched;
        matched = NULL;

        while( matched != NULL )
        {
            match_cell( options, shuffled_cell, (const struct m3_cell**)&matched );
        }

        match( options, built_cell, (const struct m3_cell**)&matched );
        print_board( *built_cell->top->left );
        printf("\n");
    }

    printf("shuffled\n");

    const struct m3_cell* swap_subject = NULL;
    const struct m3_cell* swap_target = NULL;

    match_help( options, built_cell, &swap_subject, &swap_target );

    if( swap_subject == NULL && swap_target == NULL )
    {
        printf("No way to end the game\n");
    }
    else
    {
        printf("Swap\n\n");
        print_neighbours( *swap_subject );
        printf("\n");
        printf("With\n\n");
        print_neighbours( *swap_target );
        printf("\n");
    }


    printf("seed %d\n", seed );

    board_destroy( built_cell->top->left );
    printf("done\n");

    return 0;
}
