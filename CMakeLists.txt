cmake_minimum_required(VERSION 3.0)

project(Tetris)

find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})

if (LINUX AND "${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -rdynamic")
endif (LINUX AND "${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")

add_executable(tetris src/main.c)
target_link_libraries(tetris ${SDL2_LIBRARIES})
