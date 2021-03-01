
add_executable( match3
    match3.c
    match.c
    swap.c
)

target_include_directories( match3 PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/.." )
