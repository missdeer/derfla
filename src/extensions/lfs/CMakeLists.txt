cmake_minimum_required(VERSION 3.21)
project(lfs VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
if(APPLE)
  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY
      "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Derfla.app/Contents/Extensions/lfs")
else(APPLE)
  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY
      "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/extensions/lfs")
endif(APPLE)
file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/extension.derflaext"
     DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core LinguistTools)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets Xml Network LinguistTools REQUIRED)
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

set(lfs_headers
    ${CMAKE_CURRENT_SOURCE_DIR}/stdafx.h
    ${CMAKE_CURRENT_SOURCE_DIR}/localfsscanner.h
    ${CMAKE_CURRENT_SOURCE_DIR}/dbrw.h
    ${CMAKE_CURRENT_SOURCE_DIR}/localfsitem.h
    ${CMAKE_CURRENT_SOURCE_DIR}/localserver.h
    ${CMAKE_CURRENT_SOURCE_DIR}/localsocket.h)

set(lfs_src
    ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/localfsscanner.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/dbrw.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/localfsitem.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/localserver.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/localsocket.cpp)

set(TS_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/translations/lfs_zh_CN.ts
    ${CMAKE_CURRENT_SOURCE_DIR}/translations/lfs_en_US.ts
    ${CMAKE_CURRENT_SOURCE_DIR}/translations/lfs_zh_TW.ts
    ${CMAKE_CURRENT_SOURCE_DIR}/translations/lfs_zh_HK.ts
    ${CMAKE_CURRENT_SOURCE_DIR}/translations/lfs_da.ts # 丹麦语
    ${CMAKE_CURRENT_SOURCE_DIR}/translations/lfs_fi.ts # 芬兰语
    ${CMAKE_CURRENT_SOURCE_DIR}/translations/lfs_el.ts # 希腊语
    ${CMAKE_CURRENT_SOURCE_DIR}/translations/lfs_he.ts # 希伯来语
    ${CMAKE_CURRENT_SOURCE_DIR}/translations/lfs_hu.ts # 匈牙利语
    ${CMAKE_CURRENT_SOURCE_DIR}/translations/lfs_is.ts # 冰岛语
    ${CMAKE_CURRENT_SOURCE_DIR}/translations/lfs_nb.ts # 挪威语
    ${CMAKE_CURRENT_SOURCE_DIR}/translations/lfs_es.ts # 西班牙语
    ${CMAKE_CURRENT_SOURCE_DIR}/translations/lfs_pt.ts # 葡萄牙语
    ${CMAKE_CURRENT_SOURCE_DIR}/translations/lfs_ro.ts # 罗马尼亚语
    ${CMAKE_CURRENT_SOURCE_DIR}/translations/lfs_sk.ts # 斯洛伐克语
    ${CMAKE_CURRENT_SOURCE_DIR}/translations/lfs_uk.ts # 乌克兰语
    ${CMAKE_CURRENT_SOURCE_DIR}/translations/lfs_ja.ts # 日语
    ${CMAKE_CURRENT_SOURCE_DIR}/translations/lfs_ko.ts # 韩语
    ${CMAKE_CURRENT_SOURCE_DIR}/translations/lfs_fr.ts # 法语
    ${CMAKE_CURRENT_SOURCE_DIR}/translations/lfs_de.ts # 德语
    ${CMAKE_CURRENT_SOURCE_DIR}/translations/lfs_it.ts # 意大利
    ${CMAKE_CURRENT_SOURCE_DIR}/translations/lfs_ru.ts # 俄语
    ${CMAKE_CURRENT_SOURCE_DIR}/translations/lfs_nl.ts # 荷兰
    ${CMAKE_CURRENT_SOURCE_DIR}/translations/lfs_sv.ts # 瑞典
    ${CMAKE_CURRENT_SOURCE_DIR}/translations/lfs_th.ts # 泰国
    ${CMAKE_CURRENT_SOURCE_DIR}/translations/lfs_tr.ts # 土耳其
    ${CMAKE_CURRENT_SOURCE_DIR}/translations/lfs_pl.ts # 波兰
    ${CMAKE_CURRENT_SOURCE_DIR}/translations/lfs_cs.ts # 捷克
)
set_source_files_properties(
  ${TS_FILES} PROPERTIES OUTPUT_LOCATION
                         "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/translations")

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
  qt_create_translation(QM_FILES ${CMAKE_CURRENT_SOURCE_DIR} ${TS_FILES}
                        OPTIONS -no-obsolete)
  qt_add_executable(${PROJECT_NAME} ${lfs_headers} ${lfs_src} ${QM_FILES}
                    ${CMAKE_CURRENT_SOURCE_DIR}/lfs.rc)
else()
  qt5_create_translation(QM_FILES ${CMAKE_CURRENT_SOURCE_DIR} ${TS_FILES})
  add_executable(${PROJECT_NAME} ${lfs_headers} ${lfs_src} ${QM_FILES}
                                 ${CMAKE_CURRENT_SOURCE_DIR}/lfs.rc)
endif()

target_link_libraries(${PROJECT_NAME}  PRIVATE
        qtsingleapplication
        qtlockedfile
        qtplist
        DerflaUtil
        Qt${QT_VERSION_MAJOR}::Xml
        )

target_precompile_headers(${PROJECT_NAME} PRIVATE stdafx.h)
