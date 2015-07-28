fix_cpp
=======

A simple application for prototyping FIX applications using LUA

To build you first need to set the following env variables depending on your system...
```
export PUBSUB_INCLUDEDIR=$HOME/pubsub_cpp/include
export LUA_INCLUDEDIR=/usr/include/lua5.2
```

If you are planning on using EMS you also need to set...
```
export EMS_INCLUDEDIR=$HOME/tibcoems-6.11/include
export EMS_LIBRARYDIR=$HOME/tibcoems-6.11/lib
export ENABLE_EMS=1
```

Example commands to build on a linux machine...
```
mkdir build
cd build
cmake ..
make
```

To run...
```
cd ../scripts
../build/src/lua_app tcp_acceptor.lua
```
