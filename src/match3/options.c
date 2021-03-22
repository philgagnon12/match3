#include <stdint.h>
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>

#include "match3/match3.h"
#include "match3/options.h"

void
m3_options_destroy( struct m3_options* options )
{
    assert(options);

    if( options->destroy != NULL )
        options->destroy(options);
}

// Returns zero when valid
int
m3_options_are_valid( const struct m3_options* options )
{
    assert(options);

    int ret = 0;
    int colors_count = 0;

    if( options->seed == 0 )
        ret = 1;

    if( options->columns == 0 )
        ret = 1;

    if( options->rows == 0 )
        ret = 1;

    if( options->matches_required_to_clear < 2 )
        ret = 1;

    if( options->columns * options->rows < options->matches_required_to_clear )
        ret = 1;

    // Need at least 5 colors
    if( options->colors_size < sizeof( *options->colors ) * 5 )
        ret = 1;

    if( ret == 0 )
    {
        colors_count = options->colors_size / sizeof( *options->colors );
    }

    for(int i = 0; ret == 0 && i < colors_count; i++)
    {
        // Check that color has color flag and that it cannot be m3_cell_flag_color_open, its a reserved color to m3
        if( (options->colors[i] & m3_cell_flag_color) != m3_cell_flag_color &&
            options->colors[i] != m3_cell_flag_color )
        {
            ret = 1;
        }
    }

    return ret;
}

void
m3_options_find_colors_result_init( struct m3_options_find_colors_result* find_colors_result,
                                    const uint8_t*                        color )
{
    assert(find_colors_result);

    if( find_colors_result->colors == NULL )
    {
        find_colors_result->colors_size = sizeof(*find_colors_result->colors);
        find_colors_result->colors = malloc( find_colors_result->colors_size );
        assert(find_colors_result->colors);
    }

    for( unsigned int i = 0; i < find_colors_result->colors_size / sizeof(*find_colors_result->colors); i++ )
    {
        find_colors_result->colors[i] = NULL;
    }

    find_colors_result->colors[0] = color;
    find_colors_result->colors_count = 1;
}

void
m3_options_find_colors_result_add( struct m3_options_find_colors_result*    find_colors_result,
                                   const uint8_t*                           color )
{
    assert(find_colors_result);

    uint8_t** colors_re = NULL;

    if(find_colors_result->colors_size < sizeof(*find_colors_result->colors) * (find_colors_result->colors_count+1) )
    {
        find_colors_result->colors_size += sizeof(*find_colors_result->colors);
        colors_re = realloc((uint8_t*)find_colors_result->colors, find_colors_result->colors_size );
        assert(colors_re);
        find_colors_result->colors = (const uint8_t**)colors_re;
    }

    find_colors_result->colors[find_colors_result->colors_count] = color;
    find_colors_result->colors_count++;
}

void
m3_options_find_colors_result_destroy( struct m3_options_find_colors_result* find_colors_result )
{
    assert(find_colors_result);

    static const struct m3_options_find_colors_result find_colors_result_const = M3_OPTIONS_FIND_COLORS_RESULT_CONST;

    if(find_colors_result->colors != NULL)
    {
        free((uint8_t*)find_colors_result->colors);
    }

    *find_colors_result = find_colors_result_const;
}


void
m3_options_find_colors( const struct m3_options*                options,
                        uint8_t*                                colors_to_find,
                        size_t                                  colors_to_find_size,
                        struct m3_options_find_colors_result*   find_colors_result )
{
    assert(options);
    assert(colors_to_find);
    assert(find_colors_result);

    const uint8_t* first = NULL;

    for(unsigned int i = 0; i < colors_to_find_size / sizeof(*colors_to_find); i++ )
    {
        for(uint8_t c = 0; c < options->colors_size / sizeof(*options->colors); c++)
        {
            if( colors_to_find[i] == options->colors[c] )
            {
                if( first == NULL )
                {
                    first = &options->colors[c];
                    m3_options_find_colors_result_init(find_colors_result, first);
                }
                else
                {
                    m3_options_find_colors_result_add(find_colors_result, &options->colors[c]);
                }
            }
        }
    }
}