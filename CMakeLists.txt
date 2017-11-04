cmake_minimum_required(VERSION 2.8)

project(libpartikel LANGUAGES C)

#Needed for old Qt Creator versions, so header files are shown in project tree
#FILE(GLOB_RECURSE LibFiles "*.h")
#add_custom_target(headers SOURCES ${LibFiles})

set (CMAKE_C_STANDARD 99)

SET (CMAKE_C_FLAGS_INIT                "-Wall -std=c99")
SET (CMAKE_C_FLAGS_DEBUG_INIT          "-g")
SET (CMAKE_C_FLAGS_RELEASE_INIT        "-O4 -DNDEBUG")

add_executable(${PROJECT_NAME} "main.c")
target_link_libraries(${PROJECT_NAME} raylib glfw m X11)
