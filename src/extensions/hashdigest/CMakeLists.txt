cmake_minimum_required(VERSION 3.21)
project(hashdigest VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
IF(APPLE)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Derfla.app/Contents/Extensions/hashdigest")
ELSE(APPLE)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/extensions/hashdigest")
ENDIF(APPLE)
file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/extension.derflaext" DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core LinguistTools)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets Network LinguistTools REQUIRED)
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

set(hashdigest_headers
        ${CMAKE_CURRENT_SOURCE_DIR}/stdafx.h
        )

set(hashdigest_src
        ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
        )

qt_add_resources(qrcs ${CMAKE_CURRENT_SOURCE_DIR}/hashdigest.qrc)

set(TS_FILES 
        ${CMAKE_CURRENT_SOURCE_DIR}/translations/hashdigest_zh_CN.ts
        ${CMAKE_CURRENT_SOURCE_DIR}/translations/hashdigest_en_US.ts
        )
set_source_files_properties(${TS_FILES} PROPERTIES OUTPUT_LOCATION "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/translations")

IF(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
        qt_create_translation(QM_FILES 
            ${CMAKE_CURRENT_SOURCE_DIR} 
            ${TS_FILES}
            OPTIONS -no-obsolete
        )
        qt_add_executable(${PROJECT_NAME}  
                ${hashdigest_headers}
                ${hashdigest_src}
                ${qrcs}
                ${QM_FILES}
                ${CMAKE_CURRENT_SOURCE_DIR}/hashdigest.rc
                )
ELSE()
        qt5_create_translation(QM_FILES 
            ${CMAKE_CURRENT_SOURCE_DIR} 
            ${TS_FILES}
        )
        add_executable(${PROJECT_NAME}  
                ${hashdigest_headers}
                ${hashdigest_src}
                ${qrcs}
                ${QM_FILES}
                ${CMAKE_CURRENT_SOURCE_DIR}/hashdigest.rc
                )
ENDIF()

target_link_libraries(${PROJECT_NAME}  PRIVATE
        qtsingleapplication
        qtlockedfile
        DerflaUtil
        )

target_precompile_headers(${PROJECT_NAME} PRIVATE stdafx.h)