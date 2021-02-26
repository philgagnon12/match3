#pragma once

#include <stdlib.h>

struct m3_ref;

typedef void (release_routine)(void*);

struct m3_ref {
    int                 counter;
    release_routine*    release; // Custom on release function
};

#define M3_CONST_REF { \
    0,\
    NULL \
}

#define m3_awref( ptr, release_fn ) \
    ptr = malloc( sizeof( *ptr ) ); \
    if( ptr != NULL ) \
    { \
        ptr->ref.counter = 1;\
        ptr->ref.release = release_fn; \
    }


void*
_m3_acquire( void*          ptr,
             struct m3_ref* ref )
{
    ref->counter++;
    return ptr;
}

#define m3_acquire( ptr ) _m3_acquire( ptr, &ptr->ref )

void*
_m3_release( void*          ptr,
             struct m3_ref* ref )
{
    ref->counter--;
    if( ref->counter == 0 )
    {
        if( ref->release != NULL )
        {
            ref->release( ptr );
        }

        free( ptr );
        return NULL;
    }
    return ptr;
}

#define m3_release( ptr ) _m3_release( ptr, &ptr->ref )
