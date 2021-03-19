#pragma once

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "match3/match3.h"
#include "match3/options.h"
#include "match3/cell.h"

typedef void(m3_swap_routine)( struct m3_cell**   subject,
                               struct m3_cell**   target );


#ifdef __cplusplus
extern "C" {
#endif

// subject and target will become NULL if cant swap
// TODO  swap_to_match would only swap when a match can happen
// TODO swap_freedom would let a user do a swap even when there would be no match
void
m3_swap( struct m3_cell**   subject,
         struct m3_cell**   target );
void
m3_swap_top( struct m3_cell**   subject,
             struct m3_cell**   target );

void
m3_swap_right( struct m3_cell**   subject,
               struct m3_cell**   target );
void
m3_swap_bottom( struct m3_cell**   subject,
                struct m3_cell**   target );
void
m3_swap_left( struct m3_cell**   subject,
              struct m3_cell**   target );

#ifdef __cplusplus
}
#endif
