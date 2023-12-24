cmake_minimum_required(VERSION 3.21)
project(chatgpt VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
if(APPLE)
  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY
      "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Derfla.app/Contents/Extensions/chatgpt"
  )
else(APPLE)
  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY
      "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/extensions/chatgpt")
endif(APPLE)
file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/extension.derflaext"
     DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

if(NOT QT5 AND NOT QT6)
find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core LinguistTools)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets Xml Network LinguistTools REQUIRED)
elseif(QT5)
find_package(QT NAMES Qt5 REQUIRED COMPONENTS Core LinguistTools)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets Xml Network LinguistTools REQUIRED)
elseif(QT6)
find_package(QT NAMES Qt6 REQUIRED COMPONENTS Core LinguistTools)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets Xml Network LinguistTools REQUIRED)
endif()
IF (Qt6_FOUND)
        find_package(Qt6 COMPONENTS Core5Compat REQUIRED)
ENDIF(Qt6_FOUND)

# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON) # if you have any .ui files
set(CMAKE_AUTORCC OFF) # if you have any .qrc files

add_compile_definitions(UTIL_LIBRARY STATIC_LINKED)
if(APPLE)
  add_compile_definitions(LUA_USE_MACOSX)
endif(APPLE)
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

set(chatgpt_headers ${CMAKE_CURRENT_SOURCE_DIR}/stdafx.h)

set(chatgpt_src ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp)

qt_add_resources(qrcs ${CMAKE_CURRENT_SOURCE_DIR}/chatgpt.qrc)

set(TS_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/translations/chatgpt_zh_CN.ts
    ${CMAKE_CURRENT_SOURCE_DIR}/translations/chatgpt_en_US.ts
    ${CMAKE_CURRENT_SOURCE_DIR}/translations/chatgpt_zh_TW.ts
    ${CMAKE_CURRENT_SOURCE_DIR}/translations/chatgpt_zh_HK.ts
    ${CMAKE_CURRENT_SOURCE_DIR}/translations/chatgpt_da.ts # 丹麦语
    ${CMAKE_CURRENT_SOURCE_DIR}/translations/chatgpt_fi.ts # 芬兰语
    ${CMAKE_CURRENT_SOURCE_DIR}/translations/chatgpt_el.ts # 希腊语
    ${CMAKE_CURRENT_SOURCE_DIR}/translations/chatgpt_he.ts # 希伯来语
    ${CMAKE_CURRENT_SOURCE_DIR}/translations/chatgpt_hu.ts # 匈牙利语
    ${CMAKE_CURRENT_SOURCE_DIR}/translations/chatgpt_is.ts # 冰岛语
    ${CMAKE_CURRENT_SOURCE_DIR}/translations/chatgpt_nb.ts # 挪威语
    ${CMAKE_CURRENT_SOURCE_DIR}/translations/chatgpt_es.ts # 西班牙语
    ${CMAKE_CURRENT_SOURCE_DIR}/translations/chatgpt_pt.ts # 葡萄牙语
    ${CMAKE_CURRENT_SOURCE_DIR}/translations/chatgpt_ro.ts # 罗马尼亚语
    ${CMAKE_CURRENT_SOURCE_DIR}/translations/chatgpt_sk.ts # 斯洛伐克语
    ${CMAKE_CURRENT_SOURCE_DIR}/translations/chatgpt_uk.ts # 乌克兰语
    ${CMAKE_CURRENT_SOURCE_DIR}/translations/chatgpt_ja.ts # 日语
    ${CMAKE_CURRENT_SOURCE_DIR}/translations/chatgpt_ko.ts # 韩语
    ${CMAKE_CURRENT_SOURCE_DIR}/translations/chatgpt_fr.ts # 法语
    ${CMAKE_CURRENT_SOURCE_DIR}/translations/chatgpt_de.ts # 德语
    ${CMAKE_CURRENT_SOURCE_DIR}/translations/chatgpt_it.ts # 意大利
    ${CMAKE_CURRENT_SOURCE_DIR}/translations/chatgpt_ru.ts # 俄语
    ${CMAKE_CURRENT_SOURCE_DIR}/translations/chatgpt_nl.ts # 荷兰
    ${CMAKE_CURRENT_SOURCE_DIR}/translations/chatgpt_sv.ts # 瑞典
    ${CMAKE_CURRENT_SOURCE_DIR}/translations/chatgpt_th.ts # 泰国
    ${CMAKE_CURRENT_SOURCE_DIR}/translations/chatgpt_tr.ts # 土耳其
    ${CMAKE_CURRENT_SOURCE_DIR}/translations/chatgpt_pl.ts # 波兰
    ${CMAKE_CURRENT_SOURCE_DIR}/translations/chatgpt_cs.ts # 捷克
)
set_source_files_properties(
  ${TS_FILES} PROPERTIES OUTPUT_LOCATION
                         "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/translations")

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
  if (NOT MINGW)
    qt_create_translation(QM_FILES ${CMAKE_CURRENT_SOURCE_DIR} ${TS_FILES}
                          OPTIONS -no-obsolete)
    list(APPEND chatgpt_src ${QM_FILES})
  endif()
  qt_add_executable(
    ${PROJECT_NAME} ${chatgpt_headers} ${chatgpt_src} ${qrcs} ${QM_FILES}
    ${CMAKE_CURRENT_SOURCE_DIR}/chatgpt.rc)
else()
  if (NOT MINGW)
    qt5_create_translation(QM_FILES ${CMAKE_CURRENT_SOURCE_DIR} ${TS_FILES})
    list(APPEND chatgpt_src ${QM_FILES})
  endif()
  add_executable(
    ${PROJECT_NAME} ${chatgpt_headers} ${chatgpt_src} ${qrcs} ${QM_FILES}
                    ${CMAKE_CURRENT_SOURCE_DIR}/chatgpt.rc)
endif()

target_link_libraries(${PROJECT_NAME} PRIVATE
        qtsingleapplication
        qtlockedfile
        DerflaUtil
        )
        
target_precompile_headers(${PROJECT_NAME} PRIVATE stdafx.h)
