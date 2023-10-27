cmake_minimum_required(VERSION 3.21)
project(calculator VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
if(APPLE)
  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY
      "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Derfla.app/Contents/Extensions/calculator"
  )
else(APPLE)
  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY
      "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/extensions/calculator")
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

if(MSVC)
  add_compile_options(/utf-8)
endif()
if(WIN32)
  add_compile_definitions(UNICODE STRSAFE_NO_DEPRECATE)
endif(WIN32)

add_compile_definitions(UTIL_LIBRARY STATIC_LINKED _USE_MATH_DEFINES
                        SPEEDCRUNCH_VERSION="master" QT_USE_QSTRINGBUILDER)
if(APPLE)
  add_compile_definitions(LUA_USE_MACOSX)
endif(APPLE)
include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/core
  ${CMAKE_CURRENT_SOURCE_DIR}/math)

set(calculator_headers
    ${CMAKE_CURRENT_SOURCE_DIR}/stdafx.h
    ${CMAKE_CURRENT_SOURCE_DIR}/core/book.h
    ${CMAKE_CURRENT_SOURCE_DIR}/core/constants.h
    ${CMAKE_CURRENT_SOURCE_DIR}/core/errors.h
    ${CMAKE_CURRENT_SOURCE_DIR}/core/evaluator.h
    ${CMAKE_CURRENT_SOURCE_DIR}/core/functions.h
    ${CMAKE_CURRENT_SOURCE_DIR}/core/manualserver.h
    ${CMAKE_CURRENT_SOURCE_DIR}/core/numberformatter.h
    ${CMAKE_CURRENT_SOURCE_DIR}/core/opcode.h
    ${CMAKE_CURRENT_SOURCE_DIR}/core/pageserver.h
    ${CMAKE_CURRENT_SOURCE_DIR}/core/session.h
    ${CMAKE_CURRENT_SOURCE_DIR}/core/sessionhistory.h
    ${CMAKE_CURRENT_SOURCE_DIR}/core/settings.h
    ${CMAKE_CURRENT_SOURCE_DIR}/core/userfunction.h
    ${CMAKE_CURRENT_SOURCE_DIR}/core/variable.h
    ${CMAKE_CURRENT_SOURCE_DIR}/math/cmath.h
    ${CMAKE_CURRENT_SOURCE_DIR}/math/cnumberparser.h
    ${CMAKE_CURRENT_SOURCE_DIR}/math/floatcommon.h
    ${CMAKE_CURRENT_SOURCE_DIR}/math/floatconfig.h
    ${CMAKE_CURRENT_SOURCE_DIR}/math/floatconst.h
    ${CMAKE_CURRENT_SOURCE_DIR}/math/floatconvert.h
    ${CMAKE_CURRENT_SOURCE_DIR}/math/floaterf.h
    ${CMAKE_CURRENT_SOURCE_DIR}/math/floatexp.h
    ${CMAKE_CURRENT_SOURCE_DIR}/math/floatgamma.h
    ${CMAKE_CURRENT_SOURCE_DIR}/math/floathmath.h
    ${CMAKE_CURRENT_SOURCE_DIR}/math/floatincgamma.h
    ${CMAKE_CURRENT_SOURCE_DIR}/math/floatio.h
    ${CMAKE_CURRENT_SOURCE_DIR}/math/floatipower.h
    ${CMAKE_CURRENT_SOURCE_DIR}/math/floatlog.h
    ${CMAKE_CURRENT_SOURCE_DIR}/math/floatlogic.h
    ${CMAKE_CURRENT_SOURCE_DIR}/math/floatlong.h
    ${CMAKE_CURRENT_SOURCE_DIR}/math/floatnum.h
    ${CMAKE_CURRENT_SOURCE_DIR}/math/floatpower.h
    ${CMAKE_CURRENT_SOURCE_DIR}/math/floatseries.h
    ${CMAKE_CURRENT_SOURCE_DIR}/math/floattrig.h
    ${CMAKE_CURRENT_SOURCE_DIR}/math/hmath.h
    ${CMAKE_CURRENT_SOURCE_DIR}/math/number.h
    ${CMAKE_CURRENT_SOURCE_DIR}/math/quantity.h
    ${CMAKE_CURRENT_SOURCE_DIR}/math/rational.h
    ${CMAKE_CURRENT_SOURCE_DIR}/math/units.h)

set(calculator_src
    ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/core/book.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/core/constants.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/core/evaluator.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/core/functions.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/core/manualserver.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/core/numberformatter.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/core/opcode.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/core/pageserver.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/core/session.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/core/sessionhistory.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/core/settings.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/core/userfunction.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/core/variable.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/math/cmath.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/math/cnumberparser.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/math/floatcommon.c
    ${CMAKE_CURRENT_SOURCE_DIR}/math/floatconst.c
    ${CMAKE_CURRENT_SOURCE_DIR}/math/floatconvert.c
    ${CMAKE_CURRENT_SOURCE_DIR}/math/floaterf.c
    ${CMAKE_CURRENT_SOURCE_DIR}/math/floatexp.c
    ${CMAKE_CURRENT_SOURCE_DIR}/math/floatgamma.c
    ${CMAKE_CURRENT_SOURCE_DIR}/math/floathmath.c
    ${CMAKE_CURRENT_SOURCE_DIR}/math/floatincgamma.c
    ${CMAKE_CURRENT_SOURCE_DIR}/math/floatio.c
    ${CMAKE_CURRENT_SOURCE_DIR}/math/floatipower.c
    ${CMAKE_CURRENT_SOURCE_DIR}/math/floatlog.c
    ${CMAKE_CURRENT_SOURCE_DIR}/math/floatlogic.c
    ${CMAKE_CURRENT_SOURCE_DIR}/math/floatlong.c
    ${CMAKE_CURRENT_SOURCE_DIR}/math/floatnum.c
    ${CMAKE_CURRENT_SOURCE_DIR}/math/floatpower.c
    ${CMAKE_CURRENT_SOURCE_DIR}/math/floatseries.c
    ${CMAKE_CURRENT_SOURCE_DIR}/math/floattrig.c
    ${CMAKE_CURRENT_SOURCE_DIR}/math/hmath.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/math/number.c
    ${CMAKE_CURRENT_SOURCE_DIR}/math/quantity.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/math/rational.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/math/units.cpp)

qt_add_resources(qrcs ${CMAKE_CURRENT_SOURCE_DIR}/calculator.qrc)

set(TS_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/translations/calculator_zh_CN.ts
    ${CMAKE_CURRENT_SOURCE_DIR}/translations/calculator_en_US.ts
    ${CMAKE_CURRENT_SOURCE_DIR}/translations/calculator_zh_TW.ts
    ${CMAKE_CURRENT_SOURCE_DIR}/translations/calculator_zh_HK.ts
    ${CMAKE_CURRENT_SOURCE_DIR}/translations/calculator_da.ts # 丹麦语
    ${CMAKE_CURRENT_SOURCE_DIR}/translations/calculator_fi.ts # 芬兰语
    ${CMAKE_CURRENT_SOURCE_DIR}/translations/calculator_el.ts # 希腊语
    ${CMAKE_CURRENT_SOURCE_DIR}/translations/calculator_he.ts # 希伯来语
    ${CMAKE_CURRENT_SOURCE_DIR}/translations/calculator_hu.ts # 匈牙利语
    ${CMAKE_CURRENT_SOURCE_DIR}/translations/calculator_is.ts # 冰岛语
    ${CMAKE_CURRENT_SOURCE_DIR}/translations/calculator_nb.ts # 挪威语
    ${CMAKE_CURRENT_SOURCE_DIR}/translations/calculator_es.ts # 西班牙语
    ${CMAKE_CURRENT_SOURCE_DIR}/translations/calculator_pt.ts # 葡萄牙语
    ${CMAKE_CURRENT_SOURCE_DIR}/translations/calculator_ro.ts # 罗马尼亚语
    ${CMAKE_CURRENT_SOURCE_DIR}/translations/calculator_sk.ts # 斯洛伐克语
    ${CMAKE_CURRENT_SOURCE_DIR}/translations/calculator_uk.ts # 乌克兰语
    ${CMAKE_CURRENT_SOURCE_DIR}/translations/calculator_ja.ts # 日语
    ${CMAKE_CURRENT_SOURCE_DIR}/translations/calculator_ko.ts # 韩语
    ${CMAKE_CURRENT_SOURCE_DIR}/translations/calculator_fr.ts # 法语
    ${CMAKE_CURRENT_SOURCE_DIR}/translations/calculator_de.ts # 德语
    ${CMAKE_CURRENT_SOURCE_DIR}/translations/calculator_it.ts # 意大利
    ${CMAKE_CURRENT_SOURCE_DIR}/translations/calculator_ru.ts # 俄语
    ${CMAKE_CURRENT_SOURCE_DIR}/translations/calculator_nl.ts # 荷兰
    ${CMAKE_CURRENT_SOURCE_DIR}/translations/calculator_sv.ts # 瑞典
    ${CMAKE_CURRENT_SOURCE_DIR}/translations/calculator_th.ts # 泰国
    ${CMAKE_CURRENT_SOURCE_DIR}/translations/calculator_tr.ts # 土耳其
    ${CMAKE_CURRENT_SOURCE_DIR}/translations/calculator_pl.ts # 波兰
    ${CMAKE_CURRENT_SOURCE_DIR}/translations/calculator_cs.ts # 捷克
)
set_source_files_properties(
  ${TS_FILES} PROPERTIES OUTPUT_LOCATION
                         "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/translations")

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
  qt_create_translation(QM_FILES ${CMAKE_CURRENT_SOURCE_DIR} ${TS_FILES}
                        OPTIONS -no-obsolete)
  qt_add_executable(${PROJECT_NAME} ${calculator_headers} ${calculator_src}
                    ${qrcs} ${CMAKE_CURRENT_SOURCE_DIR}/calculator.rc)
else()
  qt5_create_translation(QM_FILES ${CMAKE_CURRENT_SOURCE_DIR} ${TS_FILES})
  add_executable(
    ${PROJECT_NAME} ${calculator_headers} ${calculator_src} ${qrcs}
                    ${CMAKE_CURRENT_SOURCE_DIR}/calculator.rc)
endif()

target_link_libraries(${PROJECT_NAME} PRIVATE qtsingleapplication qtlockedfile
                                              DerflaUtil)

target_precompile_headers(${PROJECT_NAME} PRIVATE stdafx.h)
