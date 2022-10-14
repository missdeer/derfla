cmake_minimum_required(VERSION 3.21)
project(qglobalshortcut VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets Network REQUIRED)
IF (Qt6_FOUND)
        find_package(Qt6 COMPONENTS Core5Compat REQUIRED)
ENDIF(Qt6_FOUND)

# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON) # if you have any .ui files
set(CMAKE_AUTORCC OFF) # if you have any .qrc files

if (MSVC)
    add_compile_options(/utf-8)
endif()
IF(WIN32)
    add_compile_definitions(UNICODE)
ENDIF(WIN32)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

set(qglobalshortcut_headers
        ${CMAKE_CURRENT_SOURCE_DIR}/qglobalshortcut.h
        )

set(qglobalshortcut_src
        ${CMAKE_CURRENT_SOURCE_DIR}/qglobalshortcut.cc
        )

IF(WIN32)
        list(APPEND qglobalshortcut_src ${CMAKE_CURRENT_SOURCE_DIR}/qglobalshortcut_win.cc)
ENDIF(WIN32)
IF(APPLE)
        list(APPEND qglobalshortcut_src ${CMAKE_CURRENT_SOURCE_DIR}/qglobalshortcut_macx.cpp)
ENDIF(APPLE)

IF(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
        qt_add_library(qglobalshortcut STATIC
                ${qglobalshortcut_headers}
                ${qglobalshortcut_src}
                )
ELSE()
        add_library(qglobalshortcut STATIC
                ${qglobalshortcut_headers}
                ${qglobalshortcut_src}
                )
ENDIF()

target_link_libraries(qglobalshortcut
        Qt::Widgets
        User32
        )

target_include_directories(qglobalshortcut INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}")
