cmake_minimum_required(VERSION 3.21)
project(everything VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(QT NAMES Qt6 REQUIRED COMPONENTS Core LinguistTools)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets Network Xml Concurrent Sql LinguistTools REQUIRED)
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

set(everything_headers
        ${CMAKE_CURRENT_SOURCE_DIR}/stdafx.h
        )

set(everything_src
        ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
        )

IF(APPLE)
    list(APPEND everything_headers ${CMAKE_CURRENT_SOURCE_DIR}/mdfindwrapper.h)
    list(APPEND everything_src ${CMAKE_CURRENT_SOURCE_DIR}/mdfindwrapper.mm)
ENDIF(APPLE)

qt_add_resources(qrcs ${CMAKE_CURRENT_SOURCE_DIR}/everything.qrc)

IF(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
        qt_create_translation(QM_FILES 
            ${CMAKE_SOURCE_DIR} 
            ${CMAKE_CURRENT_SOURCE_DIR}/translations/everything_zh_CN.ts
            ${CMAKE_CURRENT_SOURCE_DIR}/translations/everything_en_US.ts
            OPTIONS -no-obsolete
        )
        qt_add_executable(everything 
                ${everything_headers}
                ${everything_src}
                ${qrcs}
                ${CMAKE_CURRENT_SOURCE_DIR}/everything.rc
                )
ELSE()
        qt_create_translation(QM_FILES 
            ${CMAKE_SOURCE_DIR} 
            ${CMAKE_CURRENT_SOURCE_DIR}/translations/everything_zh_CN.ts
            ${CMAKE_CURRENT_SOURCE_DIR}/translations/everything_en_US.ts
        )
        add_executable(everything 
                ${everything_headers}
                ${everything_src}
                ${qrcs}
                ${CMAKE_CURRENT_SOURCE_DIR}/everything.rc
                )
ENDIF()

target_link_libraries(everything PRIVATE
        qtsingleapplication
        qtlockedfile
        DerflaUtil
        everythingWrapper
        Qt${QT_VERSION_MAJOR}::Xml
        Qt${QT_VERSION_MAJOR}::Concurrent
        Qt${QT_VERSION_MAJOR}::Sql
        )

IF(APPLE)
    target_link_libraries(everything PRIVATE objc "-framework CoreServices")
ENDIF(APPLE)