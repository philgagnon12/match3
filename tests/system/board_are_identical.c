#include <assert.h>

#include "match3/match3.h"
#include "match3/board.h"
#include "match3/state.h"

int
main( void )
{

    // TODO compare the same board but built using board_build with --rows , then with --columns
  0x01, 0x00, 0x00, 0x00, 0x03, 0x02, 0x03, 0x05,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x41,
  0x42, 0x43, 0x44, 0x45, 0x41, 0x42, 0x41, 0x42,
  0x41, 0x42
    return 1;
}