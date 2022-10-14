cmake_minimum_required(VERSION 3.21)
project(Derfla VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core LinguistTools)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets Xml Network LinguistTools REQUIRED)
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

include_directories(${Qt${QT_VERSION_MAJOR}Gui_PRIVATE_INCLUDE_DIRS})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/alfredui
        ${CMAKE_CURRENT_SOURCE_DIR}/commonui
        ${CMAKE_CURRENT_SOURCE_DIR}/derflaui
        ${CMAKE_CURRENT_SOURCE_DIR}/extension
        ${CMAKE_CURRENT_SOURCE_DIR}/macui
        ${CMAKE_CURRENT_SOURCE_DIR}/updater
        )

set(Derfla_headers
        ${CMAKE_CURRENT_SOURCE_DIR}/stdafx.h
        ${CMAKE_CURRENT_SOURCE_DIR}/alfredui/alfredtheme.h 
        ${CMAKE_CURRENT_SOURCE_DIR}/alfredui/doublelistitem.h 
        ${CMAKE_CURRENT_SOURCE_DIR}/alfredui/geticon.h 
        ${CMAKE_CURRENT_SOURCE_DIR}/alfredui/listitem.h 
        ${CMAKE_CURRENT_SOURCE_DIR}/alfredui/alfredlistwidget.h 
        ${CMAKE_CURRENT_SOURCE_DIR}/alfredui/plaintext.h 
        ${CMAKE_CURRENT_SOURCE_DIR}/alfredui/alfredwidget.h 
        ${CMAKE_CURRENT_SOURCE_DIR}/derflaui/derflawidget.h 
        ${CMAKE_CURRENT_SOURCE_DIR}/derflaui/charlineedit.h 
        ${CMAKE_CURRENT_SOURCE_DIR}/derflaui/candidatelist.h 
        ${CMAKE_CURRENT_SOURCE_DIR}/derflaui/candidatelistdelegate.h 
        ${CMAKE_CURRENT_SOURCE_DIR}/derflaui/candidatelistwidget.h
        ${CMAKE_CURRENT_SOURCE_DIR}/derflaui/skinmanager.h 
        ${CMAKE_CURRENT_SOURCE_DIR}/extension/extension.h 
        ${CMAKE_CURRENT_SOURCE_DIR}/extension/extensionmanager.h 
        ${CMAKE_CURRENT_SOURCE_DIR}/extension/extensionmodel.h 
        ${CMAKE_CURRENT_SOURCE_DIR}/commonui/preferencedialog.h 
        ${CMAKE_CURRENT_SOURCE_DIR}/commonui/booleaneditor.h 
        ${CMAKE_CURRENT_SOURCE_DIR}/derflaapp.h 
        ${CMAKE_CURRENT_SOURCE_DIR}/actionexecutor.h  
        ${CMAKE_CURRENT_SOURCE_DIR}/alfredui/thememanager.h 
        ${CMAKE_CURRENT_SOURCE_DIR}/commonui/commonwidget.h
        )

set(Derfla_src
        ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/alfredui/alfredtheme.cpp 
        ${CMAKE_CURRENT_SOURCE_DIR}/alfredui/doublelistitem.cpp 
        ${CMAKE_CURRENT_SOURCE_DIR}/alfredui/listitem.cpp 
        ${CMAKE_CURRENT_SOURCE_DIR}/alfredui/alfredlistwidget.cpp 
        ${CMAKE_CURRENT_SOURCE_DIR}/alfredui/plaintext.cpp 
        ${CMAKE_CURRENT_SOURCE_DIR}/alfredui/alfredwidget.cpp 
        ${CMAKE_CURRENT_SOURCE_DIR}/derflaui/derflawidget.cpp 
        ${CMAKE_CURRENT_SOURCE_DIR}/derflaui/charlineedit.cpp 
        ${CMAKE_CURRENT_SOURCE_DIR}/derflaui/candidatelist.cpp 
        ${CMAKE_CURRENT_SOURCE_DIR}/derflaui/candidatelistdelegate.cpp 
        ${CMAKE_CURRENT_SOURCE_DIR}/derflaui/candidatelistwidget.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/derflaui/skinmanager.cpp 
        ${CMAKE_CURRENT_SOURCE_DIR}/extension/extension.cpp 
        ${CMAKE_CURRENT_SOURCE_DIR}/extension/extensionmanager.cpp 
        ${CMAKE_CURRENT_SOURCE_DIR}/extension/extensionmodel.cpp 
        ${CMAKE_CURRENT_SOURCE_DIR}/commonui/preferencedialog.cpp 
        ${CMAKE_CURRENT_SOURCE_DIR}/commonui/booleaneditor.cpp 
        ${CMAKE_CURRENT_SOURCE_DIR}/derflaapp.cpp 
        ${CMAKE_CURRENT_SOURCE_DIR}/actionexecutor.cpp  
        ${CMAKE_CURRENT_SOURCE_DIR}/alfredui/thememanager.cpp 
        ${CMAKE_CURRENT_SOURCE_DIR}/commonui/commonwidget.cpp
        )

set(Derfla_ui
        ${CMAKE_CURRENT_SOURCE_DIR}/alfredui/doublelistitem.ui 
        ${CMAKE_CURRENT_SOURCE_DIR}/alfredui/listitem.ui 
        ${CMAKE_CURRENT_SOURCE_DIR}/alfredui/alfredwidget.ui 
        ${CMAKE_CURRENT_SOURCE_DIR}/derflaui/candidatelist.ui 
        ${CMAKE_CURRENT_SOURCE_DIR}/commonui/preferencedialog.ui
        )
        
IF(APPLE)
    list(APPEND Derfla_headers 
        ${CMAKE_CURRENT_SOURCE_DIR}/macui/CocoaInitializer.h
        ${CMAKE_CURRENT_SOURCE_DIR}/macui/darkmode.h
        )
    list(APPEND Derfla_src 
        ${CMAKE_CURRENT_SOURCE_DIR}/macui/CocoaInitializer.mm
        ${CMAKE_CURRENT_SOURCE_DIR}/macui/darkmode.mm
        )
ENDIF(APPLE)

qt_add_resources(qrcs ${CMAKE_CURRENT_SOURCE_DIR}/Derfla.qrc)

IF(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
        qt_create_translation(QM_FILES 
            ${CMAKE_SOURCE_DIR} 
            ${CMAKE_CURRENT_SOURCE_DIR}/translations/Derfla_zh_CN.ts
            ${CMAKE_CURRENT_SOURCE_DIR}/translations/Derfla_en_US.ts
            OPTIONS -no-obsolete
        )
        qt_add_executable(${PROJECT_NAME}  
                ${Derfla_headers}
                ${Derfla_src}
                ${Derfla_ui}
                ${qrcs}
                ${QM_FILES}
                ${CMAKE_CURRENT_SOURCE_DIR}/Derfla.rc
                )
ELSE()
        qt5_create_translation(QM_FILES 
            ${CMAKE_SOURCE_DIR} 
            ${CMAKE_CURRENT_SOURCE_DIR}/translations/Derfla_zh_CN.ts
            ${CMAKE_CURRENT_SOURCE_DIR}/translations/Derfla_en_US.ts
        )
        add_executable(${PROJECT_NAME}  
                ${Derfla_headers}
                ${Derfla_src}
                ${Derfla_ui}
                ${qrcs}
                ${QM_FILES}
                ${CMAKE_CURRENT_SOURCE_DIR}/Derfla.rc
                )
ENDIF()

target_link_libraries(${PROJECT_NAME}  PRIVATE
        qtsingleapplication
        qtlockedfile
        DerflaUtil
        Qt${QT_VERSION_MAJOR}::Xml
        )

IF(WIN32)
    target_link_libraries(${PROJECT_NAME}  PRIVATE qglobalshortcut)
ELSE(WIN32)
    target_link_libraries(${PROJECT_NAME}  PRIVATE
        UGlobalHotkey 
        qtplist
        "-framework AppKit" 
        "-framework Carbon" 
        "-framework Foundation"
        "-framework ApplicationServices" 
        "-framework CoreServices"
        objc
        )
ENDIF(WIN32)

set_target_properties(${PROJECT_NAME}  PROPERTIES
        WIN32_EXECUTABLE ON
        MACOSX_BUNDLE ON
        )
    
        
file(GLOB QMs ${CMAKE_CURRENT_BINARY_DIR}/*.qm)
foreach( qm ${QMs})
        add_custom_command(
                TARGET ${PROJECT_NAME}  POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/translations/
                COMMAND ${CMAKE_COMMAND} -E copy ${qm} ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/translations/)
endforeach( qm )

file(GLOB SKINs ${CMAKE_CURRENT_SOURCE_DIR}/skins/*.zip)
file(GLOB THEMEs ${CMAKE_CURRENT_SOURCE_DIR}/themes/*.derflatheme)

IF(APPLE) 
        file(COPY ${SKINs} DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Derfla.app/Content/Resources/skins)
        file(COPY ${THEMEs} DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Derfla.app/Content/Resources/themes)
ELSE(APPLE)
        file(COPY ${SKINs} DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/skins)
        file(COPY ${THEMEs} DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/themes)
ENDIF()

target_precompile_headers(${PROJECT_NAME} PUBLIC stdafx.h)