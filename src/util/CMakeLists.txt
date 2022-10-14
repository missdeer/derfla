cmake_minimum_required(VERSION 3.21)
project(DerflaUtil VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
IF(APPLE)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Derfla.app/Content/Libs")
ENDIF(APPLE)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core LinguistTools)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets LinguistTools REQUIRED)
IF (Qt5_FOUND)
        find_package(Qt5 COMPONENTS WinExtras REQUIRED)
ENDIF(Qt5_FOUND)
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
        qt_create_translation(QM_FILES 
                ${CMAKE_CURRENT_SOURCE_DIR} 
                ${CMAKE_CURRENT_SOURCE_DIR}/translations/util_zh_CN.ts
                ${CMAKE_CURRENT_SOURCE_DIR}/translations/util_en_US.ts
                OPTIONS -no-obsolete
        )
        qt_add_library(${PROJECT_NAME} SHARED
                ${DerflaUtil_headers}
                ${DerflaUtil_src}
                ${qrcs}
                ${QM_FILES}
                )
ELSE()
        qt5_create_translation(QM_FILES 
                ${CMAKE_CURRENT_SOURCE_DIR}
                ${CMAKE_CURRENT_SOURCE_DIR}/translations/util_zh_CN.ts
                ${CMAKE_CURRENT_SOURCE_DIR}/translations/util_en_US.ts
        ) 
       
        add_library(${PROJECT_NAME} SHARED
                ${DerflaUtil_headers}
                ${DerflaUtil_src}
                ${qrcs}
                ${QM_FILES}
                )
ENDIF()

target_link_libraries(${PROJECT_NAME} 
        lua
        Qt${QT_VERSION_MAJOR}::Widgets
        )

IF (Qt6_FOUND)
    target_link_libraries(${PROJECT_NAME} 
            Qt${QT_VERSION_MAJOR}::Core5Compat )
ENDIF(Qt6_FOUND)

IF(WIN32)
    IF (Qt5_FOUND)
        target_link_libraries(${PROJECT_NAME} 
                Qt${QT_VERSION_MAJOR}::WinExtras )
    ENDIF(Qt5_FOUND)
    set_target_properties(${PROJECT_NAME} PROPERTIES PREFIX "")
    target_link_libraries(${PROJECT_NAME} Version.lib Comctl32.lib user32.lib Ole32.lib Gdi32.lib Shell32.lib uuid.lib advapi32.lib winmm.lib)
ELSE(WIN32)
    IF(APPLE)
        target_link_libraries(${PROJECT_NAME} "-framework AppKit")
    ELSE(APPLE)
    ENDIF(APPLE)
ENDIF(WIN32)

target_include_directories(${PROJECT_NAME} INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}")


file(GLOB QMs ${CMAKE_CURRENT_BINARY_DIR}/*.qm)
foreach( qm ${QMs})
        add_custom_command(
                TARGET ${PROJECT_NAME}  POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/translations/
                COMMAND ${CMAKE_COMMAND} -E copy ${qm} ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/translations/)
endforeach( qm )

target_precompile_headers(${PROJECT_NAME} PUBLIC stdafx.h)