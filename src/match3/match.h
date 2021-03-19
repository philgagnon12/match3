#pragma once

#include "match3/match3.h"
#include "match3/options.h"


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

typedef void(m3_match_routine)( const struct m3_options* options,
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

#ifdef __cplusplus
extern "C" {
#endif

// Will allocate memory for matched
// and reset the array and count
void
m3_match_result_init( struct m3_match_result* match_result,
                      const struct m3_cell*   cell );

// Will increase size of array if needed
// add a match result
void
m3_match_result_add_match( struct m3_match_result* match_result,
                           const struct m3_cell*   cell );

void
m3_match_result_destroy( struct m3_match_result* match_result );

// call once only per board
void
m3_match( const struct m3_options* options,
          const struct m3_cell*    cell,
          struct m3_match_result*  match_result );

void
m3_match_cell( const struct m3_options* options,
               const struct m3_cell*    cell,
               struct m3_match_result*  match_result );

// will try to do a match on cell_a and a match on cell_b
// to use after a swap()
void
m3_match_either_cell( const struct m3_options* options,
                      const struct m3_cell*    cell_a,
                      const struct m3_cell*    cell_b,
                      struct m3_match_result*  cell_a_match_result,
                      struct m3_match_result*  cell_b_match_result );

void
m3_match_vertical( const struct m3_options* options,
                   const struct m3_cell*    cell,
                   struct m3_match_result*  match_result );

void
m3_match_horizontal( const struct m3_options* options,
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
m3_match_help( const struct m3_options*      options,
               struct m3_cell*               board,
               struct m3_match_help_result*  match_help_result );
int
m3_match_help_has_swapped_and_matched( struct m3_match_help_result match_help_result );

void
m3_match_clear( const struct m3_options* options,
                struct m3_match_result*  match_result );

void
m3_match_clear_sort( const struct m3_options*  options,
                     struct m3_match_result*   match_result );

#ifdef __cplusplus
}
#endif
