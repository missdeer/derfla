cmake_minimum_required(VERSION 3.21)
project(qtlockedfile VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core Widgets Network)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets Network REQUIRED)
IF (Qt6_FOUND)
        find_package(Qt6 COMPONENTS Core5Compat REQUIRED)
ENDIF(Qt6_FOUND)

# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON) # if you have any .ui files
set(CMAKE_AUTORCC OFF) # if you have any .qrc files

set(CMAKE_INCLUDE_CURRENT_DIR ON)

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
        qt_add_library(${PROJECT_NAME}  STATIC
                ${qtlockedfile_headers}
                ${qtlockedfile_src}
                )
ELSE()
        add_library(${PROJECT_NAME}  STATIC
                ${qtlockedfile_headers}
                ${qtlockedfile_src}
                )
ENDIF()

target_link_libraries(${PROJECT_NAME} 
        Qt${QT_VERSION_MAJOR}::Widgets
        Qt${QT_VERSION_MAJOR}::Network)

IF (Qt6_FOUND)
target_link_libraries(${PROJECT_NAME} 
        Qt${QT_VERSION_MAJOR}::Core5Compat )
ENDIF(Qt6_FOUND)

target_include_directories(qtlockedfile INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}")
