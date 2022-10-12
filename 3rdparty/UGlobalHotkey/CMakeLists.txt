cmake_minimum_required(VERSION 3.21)
project(UGlobalHotkey VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(QT NAMES Qt6 REQUIRED COMPONENTS Core)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets Network REQUIRED)
find_package(Qt6 COMPONENTS Core5Compat REQUIRED)

# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON) # if you have any .ui files
set(CMAKE_AUTORCC OFF) # if you have any .qrc files

if (MSVC)
    add_compile_options(/utf-8)
endif()

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

set(UGlobalHotkey_headers
        ${CMAKE_CURRENT_SOURCE_DIR}/hotkeymap.h
        ${CMAKE_CURRENT_SOURCE_DIR}/uglobal.h
        ${CMAKE_CURRENT_SOURCE_DIR}/uexception.h
        ${CMAKE_CURRENT_SOURCE_DIR}/uglobalhotkeys.h
        ${CMAKE_CURRENT_SOURCE_DIR}/ukeysequence.h
        )

set(UGlobalHotkey_src
        ${CMAKE_CURRENT_SOURCE_DIR}/uexception.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/uglobalhotkeys.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/ukeysequence.cpp
        )


IF(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
        qt_add_library(UGlobalHotkey STATIC
                ${UGlobalHotkey_headers}
                ${UGlobalHotkey_src}
                )
ELSE()
        add_library(UGlobalHotkey STATIC
                ${UGlobalHotkey_headers}
                ${UGlobalHotkey_src}
                )
ENDIF()

target_link_libraries(UGlobalHotkey
        Qt::Widgets
        User32
        )

target_include_directories(UGlobalHotkey INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}")
