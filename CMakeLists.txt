
project( match3 )

add_subdirectory( src/match3 )
add_subdirectory( samples )

include( CTest )

if( BUILD_TESTING )
    add_subdirectory( tests/system )
    add_subdirectory( tests/scenarios )
endif()
