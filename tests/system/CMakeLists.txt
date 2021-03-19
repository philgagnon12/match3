
add_executable( test_board_are_identical board_are_identical.c )
target_link_libraries( test_board_are_identical match3 )
add_test( NAME board_are_identical_test COMMAND test_board_are_identical )
