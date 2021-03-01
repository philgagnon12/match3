#pragma once

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "match3/cell.h"

typedef void(swap_routine)( struct m3_cell**   subject,
                            struct m3_cell**   target );


// subject and target will become NULL if cant swap
// TODO  swap_to_match would only swap when a match can happen
// TODO swap_freedom would let a user do a swap even when there would be no match
void
swap( struct m3_cell**   subject,
      struct m3_cell**   target );
void
swap_top( struct m3_cell**   subject,
          struct m3_cell**   target );

void
swap_right( struct m3_cell**   subject,
            struct m3_cell**   target );
void
swap_bottom( struct m3_cell**   subject,
             struct m3_cell**   target );
void
swap_left( struct m3_cell**   subject,
           struct m3_cell**   target );