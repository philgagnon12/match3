#pragma once

#include <stdint.h>

enum m3_cell_flags {
    m3_cell_flag_wall              = (1 << 7),
    m3_cell_flag_wall_undefined    = (0 << 0),
    m3_cell_flag_wall_top          = (1 << 0),
    m3_cell_flag_wall_right        = (1 << 1),
    m3_cell_flag_wall_bottom       = (1 << 2),
    m3_cell_flag_wall_left         = (1 << 3),

    m3_cell_flag_color             = (1 << 6),
    m3_cell_flag_color_open        = (0 << 0)
};

enum m3_cell_masks {
    m3_cell_mask_wall   = ( 0xFF >> 2 ) | m3_cell_flag_wall,
    m3_cell_mask_color  = ( 0xFF >> 2 ) | m3_cell_flag_color
};

// This is only a preset of colors
enum m3_cell_colors {
    m3_cell_color_red       = ( m3_cell_flag_color | 1 ) & m3_cell_mask_color,
    m3_cell_color_green     = ( m3_cell_flag_color | 2 ) & m3_cell_mask_color,
    m3_cell_color_blue      = ( m3_cell_flag_color | 3 ) & m3_cell_mask_color,
    m3_cell_color_yellow    = ( m3_cell_flag_color | 4 ) & m3_cell_mask_color,
    m3_cell_color_purple    = ( m3_cell_flag_color | 5 ) & m3_cell_mask_color
};


struct m3_options {
    unsigned int        seed;
    uint8_t             columns;
    uint8_t             rows;
    uint8_t             matches_required_to_clear;
    uint8_t*            colors;
    size_t              colors_size;
};

#define M3_OPTIONS_CONST { \
    0,      \
    10,     \
    6,      \
    3,      \
    NULL,   \
    0       \
}

struct m3_cell;
