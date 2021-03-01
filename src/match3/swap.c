#pragma once

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "match3/swap.h"

// subject and target will become NULL if cant swap
// TODO  swap_to_match would only swap when a match can happen
// TODO swap_freedom would let a user do a swap even when there would be no match
void
swap( struct m3_cell**   subject,
      struct m3_cell**   target )
{
    assert( subject );
    assert( target );

    struct m3_cell* cell_subject = *subject;
    struct m3_cell* cell_target = *target;

    assert( cell_subject );
    assert( cell_target );

    *subject = NULL;
    *target  = NULL;

    uint8_t category_subject = cell_subject->category;

    if( ( cell_subject->category & cell_mask_color ) == ( cell_target->category & cell_mask_color ) )
    {
            cell_subject->category = cell_target->category;
            cell_target->category  = category_subject;

            *subject = cell_subject;
            *target  = cell_target;
    }
}

void
swap_top( struct m3_cell**   subject,
          struct m3_cell**   target )
{
    assert( subject );
    assert( target );

    *target = (*subject)->top;

    swap( subject, target );
}

void
swap_right( struct m3_cell**   subject,
            struct m3_cell**   target )
{
    assert( subject );
    assert( target );

    *target = (*subject)->right;

    swap( subject, target );
}

void
swap_bottom( struct m3_cell**   subject,
             struct m3_cell**   target )
{
    assert( subject );
    assert( target );

    *target = (*subject)->bottom;

    swap( subject, target );
}

void
swap_left( struct m3_cell**   subject,
           struct m3_cell**   target )
{
    assert( subject );
    assert( target );

    *target = (*subject)->left;

    swap( subject, target );
}