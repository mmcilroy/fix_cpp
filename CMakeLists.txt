cmake_minimum_required( VERSION 2.8 )
project( stock_exchange_cpp )
set( VERSION_NUMBER "0.0.1" )

if( "${CMAKE_BUILD_TYPE}" STREQUAL "Release" )
    set( VERSION "${VERSION_NUMBER}-release" )
else()
    set( VERSION "${VERSION_NUMBER}-debug" )
endif()

set( CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -std=c++11 -Wl,--no-as-needed -g" )

include_directories( include /home/mmcilroy/pubsub_cpp/include /home/mmcilroy/tibcoems-6.11/include /usr/include/lua5.2 )
link_directories( /home/mmcilroy/tibcoems-6.11/lib/64 /home/mmcilroy/tibcoems-6.11/lib )
add_subdirectory( src )
