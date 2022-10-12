cmake_minimum_required(VERSION 3.21)
project(qtlockedfile VERSION 1.0.0)

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

set(qtlockedfile_headers
        ${CMAKE_CURRENT_SOURCE_DIR}/qtlockedfile.h
        )

set(qtlockedfile_src
        ${CMAKE_CURRENT_SOURCE_DIR}/qtlockedfile.cpp
        )

IF(WIN32)
        list(APPEND qtlockedfile_src ${CMAKE_CURRENT_SOURCE_DIR}/qtlockedfile_win.cpp)
ELSE(WIN32)
        list(APPEND qtlockedfile_src ${CMAKE_CURRENT_SOURCE_DIR}/qtlockedfile_unix.cpp)
ENDIF(WIN32)

IF(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
        qt_add_library(qtlockedfile STATIC
                ${qtlockedfile_headers}
                ${qtlockedfile_src}
                )
ELSE()
        add_library(qtlockedfile STATIC
                ${qtlockedfile_headers}
                ${qtlockedfile_src}
                )
ENDIF()

target_link_libraries(qtlockedfile
        Qt::Widgets
        Qt::Core5Compat
        Qt::Network)

target_include_directories(qtlockedfile INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}")
