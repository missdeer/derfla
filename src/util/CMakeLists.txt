cmake_minimum_required(VERSION 3.21)
project(DerflaUtil VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
IF(APPLE)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Derfla.app/Content/Libs")
ENDIF(APPLE)

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

add_compile_definitions(UTIL_LIBRARY STATIC_LINKED)
IF(APPLE)
    add_compile_definitions(LUA_USE_MACOSX)
ENDIF(APPLE)
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

set(DerflaUtil_headers
        ${CMAKE_CURRENT_SOURCE_DIR}/util_global.h
        ${CMAKE_CURRENT_SOURCE_DIR}/stdafx.h
        ${CMAKE_CURRENT_SOURCE_DIR}/directory.h
        ${CMAKE_CURRENT_SOURCE_DIR}/util.h
        ${CMAKE_CURRENT_SOURCE_DIR}/derflaaction.h
        ${CMAKE_CURRENT_SOURCE_DIR}/scopedguard.h
        ${CMAKE_CURRENT_SOURCE_DIR}/luavm.h
        )

set(DerflaUtil_src
        ${CMAKE_CURRENT_SOURCE_DIR}/directory.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/util.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/derflaaction.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/scopedguard.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/luavm.cpp
        )

qt_add_resources(qrcs ${CMAKE_CURRENT_SOURCE_DIR}/util.qrc)

IF(WIN32)
        list(APPEND DerflaUtil_src 
            ${CMAKE_CURRENT_SOURCE_DIR}/win_util.cpp 
            ${CMAKE_CURRENT_SOURCE_DIR}/WinIconProvider.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/util.rc
            )
        list(APPEND DerflaUtil_headers 
            ${CMAKE_CURRENT_SOURCE_DIR}/win_util.h 
            ${CMAKE_CURRENT_SOURCE_DIR}/WinIconProvider.h
            )
ELSE(WIN32)
        IF(APPLE)
            list(APPEND DerflaUtil_src 
                ${CMAKE_CURRENT_SOURCE_DIR}/mac_util.mm
                )
        ELSE(APPLE)
            list(APPEND DerflaUtil_src 
                ${CMAKE_CURRENT_SOURCE_DIR}/unix_util.cpp
                )
            list(APPEND DerflaUtil_headers 
                ${CMAKE_CURRENT_SOURCE_DIR}/unix_util.h
                )
        ENDIF(APPLE)
ENDIF(WIN32)

IF(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
        qt_add_library(DerflaUtil SHARED
                ${DerflaUtil_headers}
                ${DerflaUtil_src}
                ${qrcs}
                )
ELSE()
        add_library(DerflaUtil SHARED
                ${DerflaUtil_headers}
                ${DerflaUtil_src}
                ${qrcs}
                )
ENDIF()

target_link_libraries(DerflaUtil 
        lua
        Qt::Widgets
        Qt::Core5Compat )

IF(WIN32)
    target_link_libraries(DerflaUtil Version.lib Comctl32.lib user32.lib Ole32.lib Gdi32.lib Shell32.lib uuid.lib advapi32.lib winmm.lib)
ELSE(WIN32)
    IF(APPLE)
        target_link_libraries(DerflaUtil "-framework AppKit")
    ELSE(APPLE)
    ENDIF(APPLE)
ENDIF(WIN32)

target_include_directories(DerflaUtil INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}")
