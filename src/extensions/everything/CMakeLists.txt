cmake_minimum_required(VERSION 3.21)
project(everything VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
if(APPLE)
  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY
      "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Derfla.app/Contents/Extensions/everything"
  )
else(APPLE)
  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY
      "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/extensions/everything")
endif(APPLE)
file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/extension.derflaext"
     DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core LinguistTools)
find_package(
  Qt${QT_VERSION_MAJOR}
  COMPONENTS Widgets Network Xml Concurrent Sql LinguistTools
  REQUIRED)
if(Qt6_FOUND)
  find_package(
    Qt6
    COMPONENTS Core5Compat
    REQUIRED)
endif(Qt6_FOUND)
if(WIN32)
  add_compile_definitions(UNICODE)
endif(WIN32)

# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON) # if you have any .ui files
set(CMAKE_AUTORCC OFF) # if you have any .qrc files

if(MSVC)
  add_compile_options(/utf-8)
endif()

add_compile_definitions(UTIL_LIBRARY STATIC_LINKED)
if(APPLE)
  add_compile_definitions(LUA_USE_MACOSX)
endif(APPLE)
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

set(everything_headers ${CMAKE_CURRENT_SOURCE_DIR}/stdafx.h)

set(everything_src ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp)

if(APPLE)
  list(APPEND everything_headers ${CMAKE_CURRENT_SOURCE_DIR}/mdfindwrapper.h)
  list(APPEND everything_src ${CMAKE_CURRENT_SOURCE_DIR}/mdfindwrapper.mm)
endif(APPLE)

qt_add_resources(qrcs ${CMAKE_CURRENT_SOURCE_DIR}/everything.qrc)

set(TS_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/translations/everything_zh_CN.ts
    ${CMAKE_CURRENT_SOURCE_DIR}/translations/everything_en_US.ts
    ${CMAKE_CURRENT_SOURCE_DIR}/translations/everything_zh_TW.ts
    ${CMAKE_CURRENT_SOURCE_DIR}/translations/everything_zh_HK.ts
    ${CMAKE_CURRENT_SOURCE_DIR}/translations/everything_da.ts # 丹麦语
    ${CMAKE_CURRENT_SOURCE_DIR}/translations/everything_fi.ts # 芬兰语
    ${CMAKE_CURRENT_SOURCE_DIR}/translations/everything_el.ts # 希腊语
    ${CMAKE_CURRENT_SOURCE_DIR}/translations/everything_he.ts # 希伯来语
    ${CMAKE_CURRENT_SOURCE_DIR}/translations/everything_hu.ts # 匈牙利语
    ${CMAKE_CURRENT_SOURCE_DIR}/translations/everything_is.ts # 冰岛语
    ${CMAKE_CURRENT_SOURCE_DIR}/translations/everything_nb.ts # 挪威语
    ${CMAKE_CURRENT_SOURCE_DIR}/translations/everything_es.ts # 西班牙语
    ${CMAKE_CURRENT_SOURCE_DIR}/translations/everything_pt.ts # 葡萄牙语
    ${CMAKE_CURRENT_SOURCE_DIR}/translations/everything_ro.ts # 罗马尼亚语
    ${CMAKE_CURRENT_SOURCE_DIR}/translations/everything_sk.ts # 斯洛伐克语
    ${CMAKE_CURRENT_SOURCE_DIR}/translations/everything_uk.ts # 乌克兰语
    ${CMAKE_CURRENT_SOURCE_DIR}/translations/everything_ja.ts # 日语
    ${CMAKE_CURRENT_SOURCE_DIR}/translations/everything_ko.ts # 韩语
    ${CMAKE_CURRENT_SOURCE_DIR}/translations/everything_fr.ts # 法语
    ${CMAKE_CURRENT_SOURCE_DIR}/translations/everything_de.ts # 德语
    ${CMAKE_CURRENT_SOURCE_DIR}/translations/everything_it.ts # 意大利
    ${CMAKE_CURRENT_SOURCE_DIR}/translations/everything_ru.ts # 俄语
    ${CMAKE_CURRENT_SOURCE_DIR}/translations/everything_nl.ts # 荷兰
    ${CMAKE_CURRENT_SOURCE_DIR}/translations/everything_sv.ts # 瑞典
    ${CMAKE_CURRENT_SOURCE_DIR}/translations/everything_th.ts # 泰国
    ${CMAKE_CURRENT_SOURCE_DIR}/translations/everything_tr.ts # 土耳其
    ${CMAKE_CURRENT_SOURCE_DIR}/translations/everything_pl.ts # 波兰
    ${CMAKE_CURRENT_SOURCE_DIR}/translations/everything_cs.ts # 捷克
)
set_source_files_properties(
  ${TS_FILES} PROPERTIES OUTPUT_LOCATION
                         "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/translations")

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
  qt_create_translation(QM_FILES ${CMAKE_CURRENT_SOURCE_DIR} ${TS_FILES}
                        OPTIONS -no-obsolete)
  qt_add_executable(${PROJECT_NAME} ${everything_headers} ${everything_src}
                    ${qrcs} ${CMAKE_CURRENT_SOURCE_DIR}/everything.rc)
else()
  qt5_create_translation(QM_FILES ${CMAKE_CURRENT_SOURCE_DIR} ${TS_FILES})
  add_executable(
    ${PROJECT_NAME} ${everything_headers} ${everything_src} ${qrcs} ${QM_FILES}
                    ${CMAKE_CURRENT_SOURCE_DIR}/everything.rc)
endif()

target_link_libraries(
  ${PROJECT_NAME}
  PRIVATE DerflaUtil Qt${QT_VERSION_MAJOR}::Xml
          Qt${QT_VERSION_MAJOR}::Concurrent Qt${QT_VERSION_MAJOR}::Sql)

if(WIN32)
  target_link_libraries(${PROJECT_NAME} PRIVATE everythingWrapper)
endif(WIN32)

if(APPLE)
  target_link_libraries(${PROJECT_NAME} PRIVATE objc "-framework CoreServices")
else(APPLE)
  target_precompile_headers(${PROJECT_NAME} PRIVATE stdafx.h)
endif(APPLE)
set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME "et")
