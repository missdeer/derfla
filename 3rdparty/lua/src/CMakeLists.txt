cmake_minimum_required(VERSION 3.15)
project(lua VERSION 1.0.0)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(lua_src
        ./lapi.c
        ./lcode.c
        ./lctype.c
        ./ldebug.c
        ./ldo.c
        ./ldump.c
        ./lfunc.c
        ./lgc.c
        ./linit.c
        ./llex.c
        ./lmem.c
        ./lobject.c
        ./lopcodes.c
        ./lparser.c
        ./lstate.c
        ./lstring.c
        ./ltable.c
        ./ltm.c
        ./lundump.c
        ./lvm.c
        ./lzio.c
        ./lauxlib.c
        ./lbaselib.c
        ./lbitlib.c
        ./lcorolib.c
        ./ldblib.c
        ./liolib.c
        ./lmathlib.c
        ./loadlib.c
        ./loslib.c
        ./lstrlib.c
        ./ltablib.c
        ./lutf8lib.c
        )

add_library(lua STATIC ${lua_src})

target_include_directories (lua PUBLIC ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR})