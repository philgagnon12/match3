#pragma once

#include <stdint.h>

enum cell_masks {
    cell_mask_wall              = (1 << 7),
    cell_mask_wall_undefined    = (0 << 0),
    cell_mask_wall_top          = (1 << 0),
    cell_mask_wall_right        = (1 << 1),
    cell_mask_wall_bottom       = (1 << 2),
    cell_mask_wall_left         = (1 << 3),

    cell_mask_color             = (1 << 6),
    cell_mask_color_open        = (0 << 0),
    cell_mask_color_red         = (1 << 0),
    cell_mask_color_green       = (1 << 1),
    cell_mask_color_blue        = (1 << 2),
    cell_mask_color_yellow      = (1 << 3),
    cell_mask_color_purple      = (1 << 4),
};


struct m3_options {
    int                 seed;
    uint8_t             columns;
    uint8_t             rows;
    uint8_t             matches_required_to_clear;
    enum cell_masks*    colors;
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
