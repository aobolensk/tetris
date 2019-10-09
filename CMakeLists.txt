cmake_minimum_required(VERSION 3.0)

project(Tetris)

find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})

add_executable(tetris main.c)
target_link_libraries(tetris ${SDL2_LIBRARIES})
