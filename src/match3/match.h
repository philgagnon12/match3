#pragma once

#include "match3/match3.h"


struct m3_match_result
{
    const struct m3_cell**  matched;
    uint8_t                 matched_count;  // we keep count and size to prevent additionnal realloc
    size_t                  matched_size;   // we keep count and size to prevent additionnal realloc
};

#define M3_MATCH_RESULT_CONST { \
  NULL, \
  0,    \
  0     \
}

typedef void(match_routine)( const struct m3_options* options,
                             const struct m3_cell*    self,
                             struct m3_match_result*  matched_result );


// Will allocate memory for matched
// and reset the array and count
void
match_result_init( struct m3_match_result* match_result,
                   const struct m3_cell*   cell );

// Will increase size of array if needed
// add a match result
void
match_result_add_match( struct m3_match_result* match_result,
                        const struct m3_cell*   cell );

void
match_result_destroy( struct m3_match_result* match_result );

// call once only per board
void
match( const struct m3_options* options,
       const struct m3_cell*    cell,
       struct m3_match_result*  match_result );

void
match_cell( const struct m3_options* options,
            const struct m3_cell*    cell,
            struct m3_match_result*  match_result );

void
match_vertical( const struct m3_options* options,
                const struct m3_cell*    cell,
                struct m3_match_result*  match_result );

void
match_horizontal( const struct m3_options* options,
                  const struct m3_cell*    cell,
                  struct m3_match_result*  match_result );

// TODO could be a match_help_best for the
// 1st arg is actually a board
// swap_subject and swap_target will be NULL if there is no way to make a match
// match help cannot detect horizontal or vertical matches since the match starts at the cell
// TODO match_help should also set the cell that will match after the swap is done kind of ? const struct m3_cell** swap_match
void
match_help( const struct m3_options* options,
            struct m3_cell*          cell,
            const struct m3_cell**   swap_subject,
            const struct m3_cell**   swap_target );

void
match_clear( const struct m3_options* options,
             struct m3_match_result*  match_result );
