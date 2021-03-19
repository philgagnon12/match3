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
        // Check that color has color flag
        if( (options->colors[i] & m3_cell_flag_color) != m3_cell_flag_color )
            ret = 1;
    }

    return ret;
}
