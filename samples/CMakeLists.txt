
add_executable( general general.c )
target_link_libraries( general match3 )

add_executable( board_builder board_builder.c )
find_library( MATH m )
target_link_libraries( board_builder match3 ${MATH} )
