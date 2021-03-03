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


///
// After you get a match_help_result from match_help() that evaluates to true from
// match_help_has_swapped_and_matched( match_help_result )
// Then you can
// 1. swap( swap_subject, swap_target )
// 2. match_cell( options, swap_match, &match_result )
// You will obtain a match_result that can be used on match_clear()
struct m3_match_help_result
{
    const struct m3_cell*   swap_subject;
    const struct m3_cell*   swap_target;
    const struct m3_cell*   swap_match;
};

#define M3_MATCH_HELP_RESULT_CONST { \
    NULL,   \
    NULL,   \
    NULL,   \
}

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


///
// After you get a match_help_result from match_help() that evaluates to true from
// match_help_has_swapped_and_matched( match_help_result )
// Then you can
// 1. swap( swap_subject, swap_target )
// 2. match_cell( options, swap_match, &match_result )
// You will obtain a match_result that can be used on match_clear()
void
match_help( const struct m3_options*      options,
            struct m3_cell*               board,
            struct m3_match_help_result*  match_help_result );
int
match_help_has_swapped_and_matched( struct m3_match_help_result match_help_result );

void
match_clear( const struct m3_options* options,
             struct m3_match_result*  match_result );

void
match_clear_sort( const struct m3_options*  options,
                  struct m3_match_result*   match_result );
