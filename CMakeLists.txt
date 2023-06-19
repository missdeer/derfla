cmake_minimum_required(VERSION 3.15)
project(DerflaProject VERSION 1.0.0)

set(CMAKE_EXPORT_COMPILECOMMANDS ON)

if(CMAKE_BUILD_TYPE MATCHES Debug)
  set(BUILD_TYPE Debug)
  add_compile_definitions(LOGS_ENABLED)
else(CMAKE_BUILD_TYPE MATCHES Debug)
  set(BUILD_TYPE Release)
endif(CMAKE_BUILD_TYPE MATCHES Debug)

if(CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64")
  set(ARCH "x86_64")
elseif(CMAKE_SYSTEM_PROCESSOR STREQUAL "amd64")
  set(ARCH "x86_64")
elseif(CMAKE_SYSTEM_PROCESSOR STREQUAL "AMD64")
  # cmake reports AMD64 on Windows, but we might be building for 32-bit.
  if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(ARCH "x86_64")
  else()
    set(ARCH "x86")
  endif()
elseif(CMAKE_SYSTEM_PROCESSOR STREQUAL "x86")
  set(ARCH "x86")
elseif(CMAKE_SYSTEM_PROCESSOR STREQUAL "i386")
  set(ARCH "x86")
elseif(CMAKE_SYSTEM_PROCESSOR STREQUAL "i686")
  set(ARCH "x86")
endif()

# set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin/${ARCH}")
# # .a .lib
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin/${ARCH}"
)# .exe
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin/${ARCH}"
)# .dll .dylib .so

set(CMAKE_OSX_ARCHITECTURES "arm64;x86_64")

add_subdirectory(3rdparty)
add_subdirectory(src)
