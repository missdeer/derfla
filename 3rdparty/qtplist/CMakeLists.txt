cmake_minimum_required(VERSION 3.21)
project(qtplist VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(QT NAMES Qt6 REQUIRED COMPONENTS Core)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Xml REQUIRED)
find_package(Qt6 COMPONENTS Core5Compat REQUIRED)

# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON) # if you have any .ui files
set(CMAKE_AUTORCC OFF) # if you have any .qrc files

if (MSVC)
    add_compile_options(/utf-8)
endif()

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

set(qtplist_headers
        ${CMAKE_CURRENT_SOURCE_DIR}/plistparser.h
        ${CMAKE_CURRENT_SOURCE_DIR}/plistserializer.h
        )

set(qtplist_src
        ${CMAKE_CURRENT_SOURCE_DIR}/plistparser.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/plistserializer.cpp
        )

IF(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
        qt_add_library(qtplist STATIC
                ${qtplist_headers}
                ${qtplist_src}
                )
ELSE()
        add_library(qtplist STATIC
                ${qtplist_headers}
                ${qtplist_src}
                )
ENDIF()

target_link_libraries(qtplist
        Qt::Xml
        )

target_include_directories(qtplist INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}")