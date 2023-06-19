cmake_minimum_required(VERSION 3.15)
project(DerflaSrc VERSION 1.0.0)

add_compile_definitions(STRSAFE_NO_DEPRECATE)

add_subdirectory(app)
add_subdirectory(libs)
add_subdirectory(extensions)
add_subdirectory(plugins)
