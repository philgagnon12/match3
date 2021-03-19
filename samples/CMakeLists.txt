
add_executable( general general.c )
target_link_libraries( general match3 )

add_executable( board_builder board_builder.c )
target_link_libraries( board_builder match3 )
