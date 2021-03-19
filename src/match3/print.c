#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include <match3/print.h>

void
m3_print_cell( const struct m3_cell cell )
{
    switch( cell.category )
    {
        case ( m3_cell_flag_wall | m3_cell_flag_wall_top | m3_cell_flag_wall_right ):
        case ( m3_cell_flag_wall | m3_cell_flag_wall_top | m3_cell_flag_wall_left ):
        case ( m3_cell_flag_wall | m3_cell_flag_wall_bottom | m3_cell_flag_wall_right ):
        case ( m3_cell_flag_wall | m3_cell_flag_wall_bottom | m3_cell_flag_wall_left ):
            printf("x");
            break;
        case ( m3_cell_flag_wall | m3_cell_flag_wall_top ):
        case ( m3_cell_flag_wall | m3_cell_flag_wall_bottom ):
            printf("-");
            break;
        case ( m3_cell_flag_wall | m3_cell_flag_wall_right ):
        case ( m3_cell_flag_wall | m3_cell_flag_wall_left ):
            printf("|");
            break;

        case m3_cell_color_red:
            printf("r");
            break;

        case m3_cell_color_green:
            printf("g");
            break;

        case m3_cell_color_blue:
            printf("b");
            break;

        case m3_cell_color_yellow:
            printf("y");
            break;

        case m3_cell_color_purple:
            printf("p");
            break;

        case ( m3_cell_flag_color ):
            printf("~");
            break;

            // TODO remove default
        default:
            printf("!");
    }
}

void
m3_print_neighbours( const struct m3_cell cell )
{
    printf(" ");
    m3_print_cell( *cell.top );
    printf("\n");
    m3_print_cell( *cell.left );
    m3_print_cell( cell );
    m3_print_cell( *cell.right );
    printf("\n");
    printf(" ");
    m3_print_cell( *cell.bottom );
    printf("\n");
}

void
m3_print_board( const struct m3_cell cell )
{
    const struct m3_cell* cell_left_most    = &cell;
    const struct m3_cell* cell_current      = &cell;


    while( ( cell_current->category | ( m3_cell_flag_wall | m3_cell_flag_wall_undefined ) ) != ( m3_cell_flag_wall | m3_cell_flag_wall_undefined ) )
    {

        m3_print_cell( *cell_current );

        if( ( cell_current->category & ( m3_cell_flag_wall | m3_cell_flag_wall_right ) ) == ( m3_cell_flag_wall | m3_cell_flag_wall_right ) )
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
m3_print_board_info( const struct m3_cell cell )
{
    const struct m3_cell* cell_left_most    = &cell;
    const struct m3_cell* cell_current      = &cell;


    while( ( cell_current->category | ( m3_cell_flag_wall | m3_cell_flag_wall_undefined ) ) != ( m3_cell_flag_wall | m3_cell_flag_wall_undefined ) )
    {

        m3_print_neighbours( *cell_current );
        printf("\n");

        if( ( cell_current->category & ( m3_cell_flag_wall | m3_cell_flag_wall_right ) ) == ( m3_cell_flag_wall | m3_cell_flag_wall_right ) )
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

void
m3_print_hex( const uint8_t* bytes,
              int            bytes_count )
{
    for(int i = 0; i < bytes_count; i++ )
    {
        printf("%02hhX", bytes[i] );
    }
}

void
m3_print_bits( const uint8_t byte )
{
    for( uint8_t i = 0x80; i != 0; i = i >> 1 )
    {
        if( ( byte & i ) == i )
        {
            printf( "1" );
        }
        else
        {
            printf( "0" );
        }

        if( i == 0x10 )
        {
            printf( " " );
        }
    }
}

void
m3_print_options( const struct m3_options* options )
{
    assert( options );
    // TODO print rows alternating between 1 char and the other

    void* labels_formats_values[][3] = {
        { "seed",                       "%d",   (void*)&options->seed },
        { "columns",                    "%u",   (void*)&options->columns },
        { "rows",                       "%u",   (void*)&options->rows },
        { "matches_required_to_clear",  "%u",   (void*)&options->matches_required_to_clear },
        { "colors",                     "%s",   ""},
        { "colors_size",                "%ld",  (void*)&options->colors_size },
        { "destroy",                    "%p",   (void*)options->destroy },
    };

    size_t longest_label_len = 0;
    size_t label_len = 0;

    char label_format[0x80] = {0};

    char row_style_char = 0x40;

    // Looking for longest label to determine padding for values
    for(int i = 0; i < sizeof(labels_formats_values) / sizeof(*labels_formats_values); i++ )
    {
        label_len = strlen((const char*)labels_formats_values[i][0]);

        if( label_len > longest_label_len )
            longest_label_len = label_len + 2;
    }

    sprintf(label_format, "%%-%ds", (int)longest_label_len );

    printf("options:\n");
    for(int i = 0; i < sizeof(labels_formats_values) / sizeof(*labels_formats_values); i++ )
    {
        row_style_char++;
        printf("%c| ", row_style_char );
        printf(label_format, (const char*)labels_formats_values[i][0]);

        if( 0 == strcmp( (const char*)labels_formats_values[i][1], "%d" ) )
        {
            printf((const char*)labels_formats_values[i][1],*((int*)labels_formats_values[i][2]));
        }
        else if( 0 == strcmp( (const char*)labels_formats_values[i][1], "%u" ) )
        {
            printf((const char*)labels_formats_values[i][1],*((uint8_t*)labels_formats_values[i][2]));
        }
        else if( 0 == strcmp( (const char*)labels_formats_values[i][1], "%s" ) )
        {
            printf((const char*)labels_formats_values[i][1],(const char*)labels_formats_values[i][2]);
        }
        else if( 0 == strcmp( (const char*)labels_formats_values[i][1], "%ld" ) )
        {
            printf((const char*)labels_formats_values[i][1],*((size_t*)labels_formats_values[i][2]));
        }
        else
        {
            printf((const char*)labels_formats_values[i][1],labels_formats_values[i][2]);
        }

        printf(" |%c\n", row_style_char );

        if( 0 == strcmp( (const char*)labels_formats_values[i][0], "colors" ) )
        {
            if( options->colors_size > 0 &&
                options->colors != NULL )
            {
                for(int ii = 0;  ii < options->colors_size / sizeof(*options->colors); ii++)
                {
                    printf("      ");
                    m3_print_bits( options->colors[ii] );
                    printf("  %#02hhx (%u)\n", options->colors[ii], options->colors[ii] );
                }
            }
        }
    } // for
}
