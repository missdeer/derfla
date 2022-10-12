cmake_minimum_required(VERSION 3.21)
project(everythingWrapper VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(QT NAMES Qt6 REQUIRED COMPONENTS Core)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets REQUIRED)
find_package(Qt6 COMPONENTS Core5Compat REQUIRED)

# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON) # if you have any .ui files
set(CMAKE_AUTORCC OFF) # if you have any .qrc files

if (MSVC)
    add_compile_options(/utf-8)
endif()

include_directories(${CMAKE_CURRENT_SOURCE_DIR} 
        ${CMAKE_CURRENT_SOURCE_DIR}/sdk/include 
        ${CMAKE_CURRENT_SOURCE_DIR}/sdk/ipc
        )

set(everythingWrapper_headers
        ${CMAKE_CURRENT_SOURCE_DIR}/everythingwrapper.h
        )

set(everythingWrapper_src
        ${CMAKE_CURRENT_SOURCE_DIR}/everythingwrapper.cpp
        )

IF(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
        qt_add_library(everythingWrapper STATIC
                ${everythingWrapper_headers}
                ${everythingWrapper_src}
                )
ELSE()
        add_library(everythingWrapper STATIC
                ${everythingWrapper_headers}
                ${everythingWrapper_src}
                )
ENDIF()

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
    set(LIBNAME "Everything64.lib")
ELSE()
    set(LIBNAME "Everything32.lib")
ENDIF()

target_link_directories(everythingWrapper PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/sdk/lib)

target_link_libraries(everythingWrapper
        Qt::Widgets
        ${LIBNAME}
        )

target_include_directories(everythingWrapper PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}"
        ${CMAKE_CURRENT_SOURCE_DIR}/sdk/include 
        ${CMAKE_CURRENT_SOURCE_DIR}/sdk/ipc
        )
