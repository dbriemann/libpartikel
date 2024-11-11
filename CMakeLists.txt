cmake_minimum_required(VERSION 2.8)
set (CMAKE_C_COMPILER clang)

#Needed for old Qt Creator versions, so header files are shown in project tree
#FILE(GLOB_RECURSE LibFiles "*.h")
#add_custom_target(headers SOURCES ${LibFiles})

project(libpartikel LANGUAGES C)

set (CMAKE_C_STANDARD 99)

set (CMAKE_C_FLAGS_INIT           "-Wall -std=c11")
set (CMAKE_C_FLAGS_DEBUG          "-g")
set (CMAKE_C_FLAGS_RELEASE        "-O2 -DNDEBUG")

add_executable(demo "demo.c")
target_link_libraries(demo raylib glfw m X11)


