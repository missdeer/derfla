cmake_minimum_required(VERSION 3.21)
project(Derfla VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core LinguistTools)
find_package(
  Qt${QT_VERSION_MAJOR}
  COMPONENTS Widgets Xml Network LinguistTools
  REQUIRED)
if(Qt6_FOUND)
  find_package(
    Qt6
    COMPONENTS Core5Compat
    REQUIRED)
endif(Qt6_FOUND)

# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON) # if you have any .ui files
set(CMAKE_AUTORCC OFF) # if you have any .qrc files

if(MSVC)
  add_compile_options(/utf-8)
endif()
if(WIN32)
  add_compile_definitions(UNICODE)
endif(WIN32)

add_compile_definitions(UTIL_LIBRARY STATIC_LINKED)
if(APPLE)
  add_compile_definitions(LUA_USE_MACOSX)
endif(APPLE)

include_directories(${Qt${QT_VERSION_MAJOR}Gui_PRIVATE_INCLUDE_DIRS})
include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}/alfredui
  ${CMAKE_CURRENT_SOURCE_DIR}/commonui
  ${CMAKE_CURRENT_SOURCE_DIR}/derflaui
  ${CMAKE_CURRENT_SOURCE_DIR}/extension
  ${CMAKE_CURRENT_SOURCE_DIR}/macui
  ${CMAKE_CURRENT_SOURCE_DIR}/updater)

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
    ${CMAKE_CURRENT_SOURCE_DIR}/commonui/commonwidget.h)

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
    ${CMAKE_CURRENT_SOURCE_DIR}/commonui/commonwidget.cpp)

set(Derfla_ui
    ${CMAKE_CURRENT_SOURCE_DIR}/alfredui/doublelistitem.ui
    ${CMAKE_CURRENT_SOURCE_DIR}/alfredui/listitem.ui
    ${CMAKE_CURRENT_SOURCE_DIR}/alfredui/alfredwidget.ui
    ${CMAKE_CURRENT_SOURCE_DIR}/derflaui/candidatelist.ui
    ${CMAKE_CURRENT_SOURCE_DIR}/commonui/preferencedialog.ui)

if(APPLE)
  list(APPEND Derfla_headers
       ${CMAKE_CURRENT_SOURCE_DIR}/macui/CocoaInitializer.h
       ${CMAKE_CURRENT_SOURCE_DIR}/macui/darkmode.h)
  list(APPEND Derfla_src ${CMAKE_CURRENT_SOURCE_DIR}/macui/CocoaInitializer.mm
       ${CMAKE_CURRENT_SOURCE_DIR}/macui/darkmode.mm)
endif(APPLE)

qt_add_resources(qrcs ${CMAKE_CURRENT_SOURCE_DIR}/Derfla.qrc)

set(TS_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/translations/derfla_zh_CN.ts
    ${CMAKE_CURRENT_SOURCE_DIR}/translations/derfla_en_US.ts
    ${CMAKE_CURRENT_SOURCE_DIR}/translations/derfla_zh_TW.ts
    ${CMAKE_CURRENT_SOURCE_DIR}/translations/derfla_zh_HK.ts
    ${CMAKE_CURRENT_SOURCE_DIR}/translations/derfla_da.ts # 丹麦语
    ${CMAKE_CURRENT_SOURCE_DIR}/translations/derfla_fi.ts # 芬兰语
    ${CMAKE_CURRENT_SOURCE_DIR}/translations/derfla_el.ts # 希腊语
    ${CMAKE_CURRENT_SOURCE_DIR}/translations/derfla_he.ts # 希伯来语
    ${CMAKE_CURRENT_SOURCE_DIR}/translations/derfla_hu.ts # 匈牙利语
    ${CMAKE_CURRENT_SOURCE_DIR}/translations/derfla_is.ts # 冰岛语
    ${CMAKE_CURRENT_SOURCE_DIR}/translations/derfla_nb.ts # 挪威语
    ${CMAKE_CURRENT_SOURCE_DIR}/translations/derfla_es.ts # 西班牙语
    ${CMAKE_CURRENT_SOURCE_DIR}/translations/derfla_pt.ts # 葡萄牙语
    ${CMAKE_CURRENT_SOURCE_DIR}/translations/derfla_ro.ts # 罗马尼亚语
    ${CMAKE_CURRENT_SOURCE_DIR}/translations/derfla_sk.ts # 斯洛伐克语
    ${CMAKE_CURRENT_SOURCE_DIR}/translations/derfla_uk.ts # 乌克兰语
    ${CMAKE_CURRENT_SOURCE_DIR}/translations/derfla_ja.ts # 日语
    ${CMAKE_CURRENT_SOURCE_DIR}/translations/derfla_ko.ts # 韩语
    ${CMAKE_CURRENT_SOURCE_DIR}/translations/derfla_fr.ts # 法语
    ${CMAKE_CURRENT_SOURCE_DIR}/translations/derfla_de.ts # 德语
    ${CMAKE_CURRENT_SOURCE_DIR}/translations/derfla_it.ts # 意大利
    ${CMAKE_CURRENT_SOURCE_DIR}/translations/derfla_ru.ts # 俄语
    ${CMAKE_CURRENT_SOURCE_DIR}/translations/derfla_nl.ts # 荷兰
    ${CMAKE_CURRENT_SOURCE_DIR}/translations/derfla_sv.ts # 瑞典
    ${CMAKE_CURRENT_SOURCE_DIR}/translations/derfla_th.ts # 泰国
    ${CMAKE_CURRENT_SOURCE_DIR}/translations/derfla_tr.ts # 土耳其
    ${CMAKE_CURRENT_SOURCE_DIR}/translations/derfla_pl.ts # 波兰
    ${CMAKE_CURRENT_SOURCE_DIR}/translations/derfla_cs.ts # 捷克
)

if(APPLE)
  set_source_files_properties(
    ${TS_FILES}
    PROPERTIES
      OUTPUT_LOCATION
      "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Derfla.app/Contents/Resources/translations"
  )
else(APPLE)
  set_source_files_properties(
    ${TS_FILES} PROPERTIES OUTPUT_LOCATION
                           "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/translations")
endif(APPLE)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
  qt_create_translation(QM_FILES ${CMAKE_CURRENT_SOURCE_DIR} ${TS_FILES}
                        OPTIONS -no-obsolete)
  qt_add_executable(
    ${PROJECT_NAME}
    ${Derfla_headers}
    ${Derfla_src}
    ${Derfla_ui}
    ${qrcs}
    ${QM_FILES}
    ${CMAKE_CURRENT_SOURCE_DIR}/Derfla.rc
    ${CMAKE_CURRENT_SOURCE_DIR}/Derfla.icns)
else()
  qt5_create_translation(QM_FILES ${CMAKE_CURRENT_SOURCE_DIR} ${TS_FILES})
  add_executable(
    ${PROJECT_NAME}
    ${Derfla_headers}
    ${Derfla_src}
    ${Derfla_ui}
    ${qrcs}
    ${QM_FILES}
    ${CMAKE_CURRENT_SOURCE_DIR}/Derfla.rc
    ${CMAKE_CURRENT_SOURCE_DIR}/Derfla.icns)
endif()

target_link_libraries(
  ${PROJECT_NAME} PRIVATE qtsingleapplication qtlockedfile DerflaUtil
                          Qt${QT_VERSION_MAJOR}::Xml)

if(WIN32)
  target_link_libraries(${PROJECT_NAME} PRIVATE qglobalshortcut)
else(WIN32)
  target_link_libraries(
    ${PROJECT_NAME}
    PRIVATE UGlobalHotkey
            qtplist
            "-framework AppKit"
            "-framework Carbon"
            "-framework Foundation"
            "-framework ApplicationServices"
            "-framework CoreServices"
            objc)
endif(WIN32)

set_target_properties(${PROJECT_NAME} PROPERTIES WIN32_EXECUTABLE ON
                                                 MACOSX_BUNDLE ON)

file(GLOB SKINs ${CMAKE_CURRENT_SOURCE_DIR}/skins/*.zip)
file(GLOB THEMEs ${CMAKE_CURRENT_SOURCE_DIR}/themes/*.derflatheme)

if(APPLE)
  set_source_files_properties("${CMAKE_CURRENT_SOURCE_DIR}/derfla.icns"
                              PROPERTIES MACOSX_PACKAGE_LOCATION Resources)
  set_target_properties(
    ${PROJECT_NAME}
    PROPERTIES RESOURCE "./derfla.icns"
               MACOSX_BUNDLE_BUNDLE_NAME ${PROJECT_NAME}
               MACOSX_BUNDLE_GUI_IDENTIFIER "com.ismisv.derfla"
               MACOSX_BUNDLE_ICON_FILE "derfla.icns"
               MACOSX_BUNDLE_INFO_STRING "Derfla"
               MACOSX_BUNDLE_INFO_PLIST
               "${CMAKE_CURRENT_SOURCE_DIR}/osxInfo.plist")
  file(
    COPY ${SKINs}
    DESTINATION
      ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Derfla.app/Contents/Resources/skins)
  file(
    COPY ${THEMEs}
    DESTINATION
      ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Derfla.app/Contents/Resources/themes)
else(APPLE)
  file(COPY ${SKINs} DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/skins)
  file(COPY ${THEMEs} DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/themes)
  target_precompile_headers(${PROJECT_NAME} PRIVATE stdafx.h)
endif(APPLE)
