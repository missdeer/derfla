cmake_minimum_required(VERSION 3.21)
project(everythingWrapper VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets REQUIRED)
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

include_directories(${CMAKE_CURRENT_SOURCE_DIR} 
        ${CMAKE_CURRENT_SOURCE_DIR}/sdk/include 
        ${CMAKE_CURRENT_SOURCE_DIR}/sdk/ipc
        )

set(everythingWrapper_headers
        ${CMAKE_CURRENT_SOURCE_DIR}/everythingwrapper.h
        )

set(everythingWrapper_src
        ${CMAKE_CURRENT_SOURCE_DIR}/everythingwrapper.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/sdk/src/Everything.c
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

target_link_libraries(everythingWrapper
        Qt::Widgets
        )

target_include_directories(everythingWrapper PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}"
        ${CMAKE_CURRENT_SOURCE_DIR}/sdk/include 
        ${CMAKE_CURRENT_SOURCE_DIR}/sdk/ipc
        )

target_compile_definitions(everythingWrapper PUBLIC
        EVERYTHINGUSERAPI=
        )
