cmake_minimum_required(VERSION 3.28)
project(dust)

set(CMAKE_CXX_STANDARD 20)

include_directories(include libs/glm-1.0.1-light libs/glfw-3.4.bin.WIN64/include libs/gl3w/include)
link_directories(libs/glfw-3.4.bin.WIN64/lib-mingw-w64)
link_libraries(libglfw3.a)

add_library(dust SHARED
        libs/gl3w/src/gl3w.c
        libs/gl3w/src/
        include/dust.h
        src/dust.cpp)

add_custom_command(TARGET dust POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
            ${CMAKE_SOURCE_DIR}/cmake-build-debug/libdust.dll
            ${CMAKE_SOURCE_DIR}/../dust_demo/cmake-build-debug/libdust.dll)
