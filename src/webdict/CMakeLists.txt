cmake_minimum_required(VERSION 3.21)
project(webdict VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
IF(APPLE)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Derfla.app/Content/Extensions/webdict")
ELSE(APPLE)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/extensions/webdict")
ENDIF(APPLE)
file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/extension.derflaext" DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

find_package(QT NAMES Qt6 REQUIRED COMPONENTS Core LinguistTools)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets Network LinguistTools REQUIRED)
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

set(webdict_headers
        ${CMAKE_CURRENT_SOURCE_DIR}/stdafx.h
        ${CMAKE_CURRENT_SOURCE_DIR}/youdao.h
        )

set(webdict_src
        ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/youdao.cpp
        )

qt_add_resources(qrcs ${CMAKE_CURRENT_SOURCE_DIR}/webdict.qrc)

IF(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
        qt_create_translation(QM_FILES 
            ${CMAKE_SOURCE_DIR} 
            ${CMAKE_CURRENT_SOURCE_DIR}/translations/webdict_zh_CN.ts
            ${CMAKE_CURRENT_SOURCE_DIR}/translations/webdict_en_US.ts
            OPTIONS -no-obsolete
        )
        qt_add_executable(${PROJECT_NAME}  
                ${webdict_headers}
                ${webdict_src}
                ${qrcs}
                ${CMAKE_CURRENT_SOURCE_DIR}/webdict.rc
                )
ELSE()
        qt_create_translation(QM_FILES 
            ${CMAKE_SOURCE_DIR} 
            ${CMAKE_CURRENT_SOURCE_DIR}/translations/webdict_zh_CN.ts
            ${CMAKE_CURRENT_SOURCE_DIR}/translations/webdict_en_US.ts
        )
        add_executable(${PROJECT_NAME}  
                ${webdict_headers}
                ${webdict_src}
                ${qrcs}
                ${CMAKE_CURRENT_SOURCE_DIR}/webdict.rc
                )
ENDIF()

target_link_libraries(${PROJECT_NAME}  PRIVATE
        qtsingleapplication
        qtlockedfile
        DerflaUtil
        )

        
file(GLOB QMs ${CMAKE_CURRENT_SOURCE_DIR}/translations/*.qm)
foreach( qm ${QMs})
        add_custom_command(
                TARGET ${PROJECT_NAME}  POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/translations/
                COMMAND ${CMAKE_COMMAND} -E copy ${qm} ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/translations/)
endforeach( qm )