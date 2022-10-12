cmake_minimum_required(VERSION 3.15)
project(sqlite3 VERSION 1.0.0)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(sqlite3_src
        ./sqlite3.c
        )

add_library(sqlite3 STATIC ${sqlite3_src})

target_include_directories (sqlite3 PUBLIC ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR})