cmake_minimum_required(VERSION 3.28)
project(dust)

set(CMAKE_CXX_STANDARD 20)

include_directories(include libs/glm-1.0.1-light "$ENV{VULKAN_SDK}/Include" libs/glfw-3.4.bin.WIN64/include)
link_directories("$ENV{VULKAN_SDK}/Lib" libs/glfw-3.4.bin.WIN64/lib-mingw-w64)
link_libraries(vulkan-1.lib libglfw3.a)

add_library(dust SHARED
        include/dust.h
        src/dust.cpp
        include/ds_vulkan.h
        src/ds_vulkan.cpp)

add_custom_command(TARGET dust POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
            ${CMAKE_SOURCE_DIR}/cmake-build-debug/libdust.dll
            ${CMAKE_SOURCE_DIR}/../dust_demo/cmake-build-debug/libdust.dll)
