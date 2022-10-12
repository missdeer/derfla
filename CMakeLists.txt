cmake_minimum_required(VERSION 3.15)
project(DerflaProject VERSION 1.0.0)

set(CMAKE_EXPORT_COMPILECOMMANDS ON)

IF(CMAKE_BUILD_TYPE MATCHES Debug)
    set(BUILD_TYPE Debug)
    add_compile_definitions(LOGS_ENABLED)
ELSE(CMAKE_BUILD_TYPE MATCHES Debug)
    set(BUILD_TYPE Release)
ENDIF(CMAKE_BUILD_TYPE MATCHES Debug)

#set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/bin") # .a .lib
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/bin") # .exe
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/bin") # .dll .dylib .so

add_subdirectory(3rdparty)
add_subdirectory(src)