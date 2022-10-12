cmake_minimum_required(VERSION 3.21)
project(WinSparkle VERSION 1.0.0)

add_library(WinSparkle INTERFACE)

target_include_directories(WinSparkle INTERFACE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        )

if(CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64")
    set(ARCH "x64")
elseif(CMAKE_SYSTEM_PROCESSOR STREQUAL "amd64")
    set(ARCH "x64")
elseif(CMAKE_SYSTEM_PROCESSOR STREQUAL "AMD64")
    # cmake reports AMD64 on Windows, but we might be building for 32-bit.
    if(CMAKE_SIZEOF_VOID_P EQUAL 8)
        set(ARCH "x64")
    else()
        set(ARCH "x86")
    endif()
elseif(CMAKE_SYSTEM_PROCESSOR STREQUAL "x86")
    set(ARCH "x86")
elseif(CMAKE_SYSTEM_PROCESSOR STREQUAL "i386")
    set(ARCH "x86")
elseif(CMAKE_SYSTEM_PROCESSOR STREQUAL "i686")
    set(ARCH "x86")
elseif(CMAKE_SYSTEM_PROCESSOR STREQUAL "aarch64")
    set(ARCH "arm64")
elseif(CMAKE_SYSTEM_PROCESSOR STREQUAL "ARM64")
    set(ARCH "arm64")
elseif(CMAKE_SYSTEM_PROCESSOR STREQUAL "arm64")
    set(ARCH "arm64")
elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "^arm*")
    set(ARCH "arm64")
endif()

IF(ARCH STREQUAL "x64")
    set(LIBPATH "x64/Release")
ELSE()
    set(LIBPATH "Release")
ENDIF()

target_link_directories(WinSparkle INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/${LIBPATH}")

target_link_libraries(WinSparkle INTERFACE
        WinSparkle
        )
