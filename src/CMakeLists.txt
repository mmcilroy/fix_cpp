add_executable( app app.cpp )
target_link_libraries( app boost_system pthread )

add_executable( lua_app lua_app.cpp )
target_link_libraries( lua_app boost_system lua5.2 pthread )
