#pragma once

#include "match3/match3.h"
#include "match3/options.h"

struct m3_state;
struct m3_state {
    char* buffer;
    size_t buffer_size;
    void (*destroy)(struct m3_state*);
};

#define M3_STATE_CONST { \
    NULL,   \
    0,      \
    NULL    \
}

#ifdef __cplusplus
extern "C" {
#endif

void
m3_state_destroy( struct m3_state* state );

// Return zero on success
int
m3_state_save( const struct m3_options* options,
               const struct m3_cell*    board,
               struct m3_state*         state );

// Return zero on success
int
m3_state_load( const struct m3_state*   state,
               struct m3_options**      options,
               struct m3_cell**         board );

#ifdef __cplusplus
}
#endif
