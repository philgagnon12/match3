#pragma once

#include "match3/match3.h"


#ifdef __cplusplus
extern "C" {
#endif

void
m3_state_save( const struct m3_options* options,
               const struct m3_cell*    board,
               char**                   state,
               int*                     state_size );

#ifdef __cplusplus
}
#endif