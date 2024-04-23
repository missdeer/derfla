cmake_minimum_required(VERSION 3.21)
project(DerflaUtil VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
if(APPLE)
  set(CMAKE_LIBRARY_OUTPUT_DIRECTORY
      "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Derfla.app/Contents/Libs")
endif(APPLE)

if(NOT QT5 AND NOT QT6)
find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core LinguistTools)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets LinguistTools REQUIRED)
elseif(QT5)
find_package(QT NAMES Qt5 REQUIRED COMPONENTS Core LinguistTools)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets LinguistTools REQUIRED)
elseif(QT6)
find_package(QT NAMES Qt6 REQUIRED COMPONENTS Core LinguistTools)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets LinguistTools REQUIRED)
endif()
IF (Qt6_FOUND)
        find_package(Qt6 COMPONENTS Core5Compat REQUIRED)
ENDIF(Qt6_FOUND)

find_package(OpenMP REQUIRED)

# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON) # if you have any .ui files
set(CMAKE_AUTORCC OFF) # if you have any .qrc files

if(WIN32)
  if(Qt5_FOUND)
    find_package(Qt5 COMPONENTS WinExtras REQUIRED)
  endif(Qt5_FOUND)
endif(WIN32)

add_compile_definitions(UTIL_LIBRARY STATIC_LINKED)
if(APPLE)
  add_compile_definitions(LUA_USE_MACOSX)
endif(APPLE)

set(DerflaUtil_headers
    ${CMAKE_CURRENT_SOURCE_DIR}/util_global.h
    ${CMAKE_CURRENT_SOURCE_DIR}/stdafx.h
    ${CMAKE_CURRENT_SOURCE_DIR}/directory.h
    ${CMAKE_CURRENT_SOURCE_DIR}/util.h
    ${CMAKE_CURRENT_SOURCE_DIR}/derflaaction.h
    ${CMAKE_CURRENT_SOURCE_DIR}/scopedguard.h
    ${CMAKE_CURRENT_SOURCE_DIR}/luavm.h
    ${CMAKE_CURRENT_SOURCE_DIR}/sqlite/Sqlite3Constants.h
    ${CMAKE_CURRENT_SOURCE_DIR}/sqlite/Sqlite3DBManager.h
    ${CMAKE_CURRENT_SOURCE_DIR}/sqlite/Sqlite3Helper.h
    ${CMAKE_CURRENT_SOURCE_DIR}/sqlite/Sqlite3Statement.h)

set(DerflaUtil_src
    ${CMAKE_CURRENT_SOURCE_DIR}/directory.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/util.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/derflaaction.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/scopedguard.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/luavm.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/sqlite/Sqlite3DBManager.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/sqlite/Sqlite3Helper.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/sqlite/Sqlite3Statement.cpp)

qt_add_resources(qrcs ${CMAKE_CURRENT_SOURCE_DIR}/util.qrc)

if(WIN32)
  list(APPEND DerflaUtil_src ${CMAKE_CURRENT_SOURCE_DIR}/win_util.cpp
       ${CMAKE_CURRENT_SOURCE_DIR}/WinIconProvider.cpp
       ${CMAKE_CURRENT_SOURCE_DIR}/util.rc)
  list(APPEND DerflaUtil_headers ${CMAKE_CURRENT_SOURCE_DIR}/win_util.h
       ${CMAKE_CURRENT_SOURCE_DIR}/WinIconProvider.h)
else(WIN32)
  if(APPLE)
    list(APPEND DerflaUtil_src ${CMAKE_CURRENT_SOURCE_DIR}/mac_util.mm)
  else(APPLE)
    list(APPEND DerflaUtil_src ${CMAKE_CURRENT_SOURCE_DIR}/unix_util.cpp)
    list(APPEND DerflaUtil_headers ${CMAKE_CURRENT_SOURCE_DIR}/unix_util.h)
  endif(APPLE)
endif(WIN32)

set(TS_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/translations/util_zh_CN.ts
    ${CMAKE_CURRENT_SOURCE_DIR}/translations/util_en_US.ts
    ${CMAKE_CURRENT_SOURCE_DIR}/translations/util_zh_TW.ts
    ${CMAKE_CURRENT_SOURCE_DIR}/translations/util_zh_HK.ts
    ${CMAKE_CURRENT_SOURCE_DIR}/translations/util_da.ts # 丹麦语
    ${CMAKE_CURRENT_SOURCE_DIR}/translations/util_fi.ts # 芬兰语
    ${CMAKE_CURRENT_SOURCE_DIR}/translations/util_el.ts # 希腊语
    ${CMAKE_CURRENT_SOURCE_DIR}/translations/util_he.ts # 希伯来语
    ${CMAKE_CURRENT_SOURCE_DIR}/translations/util_hu.ts # 匈牙利语
    ${CMAKE_CURRENT_SOURCE_DIR}/translations/util_is.ts # 冰岛语
    ${CMAKE_CURRENT_SOURCE_DIR}/translations/util_nb.ts # 挪威语
    ${CMAKE_CURRENT_SOURCE_DIR}/translations/util_es.ts # 西班牙语
    ${CMAKE_CURRENT_SOURCE_DIR}/translations/util_pt.ts # 葡萄牙语
    ${CMAKE_CURRENT_SOURCE_DIR}/translations/util_ro.ts # 罗马尼亚语
    ${CMAKE_CURRENT_SOURCE_DIR}/translations/util_sk.ts # 斯洛伐克语
    ${CMAKE_CURRENT_SOURCE_DIR}/translations/util_uk.ts # 乌克兰语
    ${CMAKE_CURRENT_SOURCE_DIR}/translations/util_ja.ts # 日语
    ${CMAKE_CURRENT_SOURCE_DIR}/translations/util_ko.ts # 韩语
    ${CMAKE_CURRENT_SOURCE_DIR}/translations/util_fr.ts # 法语
    ${CMAKE_CURRENT_SOURCE_DIR}/translations/util_de.ts # 德语
    ${CMAKE_CURRENT_SOURCE_DIR}/translations/util_it.ts # 意大利
    ${CMAKE_CURRENT_SOURCE_DIR}/translations/util_ru.ts # 俄语
    ${CMAKE_CURRENT_SOURCE_DIR}/translations/util_nl.ts # 荷兰
    ${CMAKE_CURRENT_SOURCE_DIR}/translations/util_sv.ts # 瑞典
    ${CMAKE_CURRENT_SOURCE_DIR}/translations/util_th.ts # 泰国
    ${CMAKE_CURRENT_SOURCE_DIR}/translations/util_tr.ts # 土耳其
    ${CMAKE_CURRENT_SOURCE_DIR}/translations/util_pl.ts # 波兰
    ${CMAKE_CURRENT_SOURCE_DIR}/translations/util_cs.ts # 捷克
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
  qt_add_library(${PROJECT_NAME} SHARED ${DerflaUtil_headers} ${DerflaUtil_src}
                 ${qrcs} ${QM_FILES})
else()
  qt5_create_translation(QM_FILES ${CMAKE_CURRENT_SOURCE_DIR} ${TS_FILES})

  add_library(${PROJECT_NAME} SHARED ${DerflaUtil_headers} ${DerflaUtil_src}
                                     ${qrcs} ${QM_FILES})
endif()

target_link_libraries(${PROJECT_NAME} 
        lua
        sqlite3
        Qt${QT_VERSION_MAJOR}::Widgets
        )

if (APPLE)
    target_link_libraries(${PROJECT_NAME} omp)
else()
    target_link_libraries(${PROJECT_NAME} OpenMP::OpenMP_CXX)
endif()


if(Qt6_FOUND)
  target_link_libraries(${PROJECT_NAME} Qt${QT_VERSION_MAJOR}::Core5Compat)
endif(Qt6_FOUND)

if(WIN32)
  if(Qt5_FOUND)
    target_link_libraries(${PROJECT_NAME} Qt${QT_VERSION_MAJOR}::WinExtras)
  endif(Qt5_FOUND)
  set_target_properties(${PROJECT_NAME} PROPERTIES PREFIX "")
  target_link_libraries(
    ${PROJECT_NAME}
    Version
    Comctl32
    user32
    Ole32
    Gdi32
    Shell32
    uuid
    advapi32
    winmm)
else(WIN32)
  if(APPLE)
    target_link_libraries(${PROJECT_NAME} "-framework AppKit")
  else(APPLE)
  endif(APPLE)
endif(WIN32)

target_include_directories(
  ${PROJECT_NAME} INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}"
                            "${CMAKE_CURRENT_SOURCE_DIR}/sqlite")

if(NOT APPLE)
  target_precompile_headers(${PROJECT_NAME} PRIVATE stdafx.h)
endif()
