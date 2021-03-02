#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include <match3/print.h>

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
            printf("~");
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

