#pragma once

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>


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

typedef void(match_routine)( const struct m3_options    options,
                             const struct m3_cell*      self,
                             const struct m3_cell**     matched );




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