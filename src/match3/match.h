#pragma once

#include "match3/cell.h"


struct m3_match_result
{
    struct m3_cell*     matched;
    struct m3_cell*     matched_with;
    match_routine*      routine;
};


// call once only per board
void
match( const struct m3_options  options,
       const struct m3_cell*    cell,
       const struct m3_cell**   matched );


void
match_cell( const struct m3_options options,
            const struct m3_cell*   cell,
            const struct m3_cell**  matched );

void
match_vertical( const struct m3_options  options,
                const struct m3_cell*    cell,
                const struct m3_cell**   matched );

void
match_horizontal( const struct m3_options  options,
                  const struct m3_cell*    cell,
                  const struct m3_cell**   matched );

// TODO could be a match_help_best for the
// 1st arg is actually a board
// swap_subject and swap_target will be NULL if there is no way to make a match
// match help cannot detect horizontal or vertical matches since the match starts at the cell
void
match_help( const struct m3_options options,
            struct m3_cell*         cell,
            const struct m3_cell**  swap_subject,
            const struct m3_cell**  swap_target );