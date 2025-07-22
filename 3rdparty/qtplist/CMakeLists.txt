cmake_minimum_required(VERSION 3.21)
project(qtplist VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Xml REQUIRED)
IF (Qt6_FOUND)
        find_package(Qt6 COMPONENTS Core5Compat REQUIRED)
ENDIF(Qt6_FOUND)

# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON) # if you have any .ui files
set(CMAKE_AUTORCC OFF) # if you have any .qrc files

set(CMAKE_INCLUDE_CURRENT_DIR ON)

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
