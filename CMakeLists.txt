cmake_minimum_required( VERSION 2.8 )
project( fix_cpp )
set( VERSION_NUMBER "0.0.1" )

if( "${CMAKE_BUILD_TYPE}" STREQUAL "Release" )
    set( VERSION "${VERSION_NUMBER}-release" )
else()
    set( VERSION "${VERSION_NUMBER}-debug" )
endif()

set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wl,--no-as-needed" )

if( "$ENV{ENABLE_EMS}" STREQUAL "1" )
    set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DEMS" )
endif()

find_package( Boost REQUIRED COMPONENTS system )

if( "$ENV{ENABLE_EMS}" STREQUAL "1" )
    include_directories( include $ENV{PUBSUB_INCLUDEDIR} ${Boost_INCLUDE_DIRS} $ENV{LUA_INCLUDEDIR} $ENV{EMS_INCLUDEDIR} )
    link_directories( $ENV{EMS_LIBRARYDIR}/64 $ENV{EMS_LIBRARYDIR} )
else()
    include_directories( include $ENV{PUBSUB_INCLUDEDIR} ${Boost_INCLUDE_DIRS} $ENV{LUA_INCLUDEDIR} )
endif()

add_subdirectory( src )
