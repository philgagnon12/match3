
add_library( match3
    match.c
    swap.c
    cell.c
    board.c
    print.c
    state.c
    options.c
)

target_include_directories( match3 PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/.." )

export( TARGETS match3 FILE match3.cmake )
