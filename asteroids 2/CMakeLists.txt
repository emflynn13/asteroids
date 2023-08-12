cmake_minimum_required(VERSION 3.24)
project(asteroids C)

set(CMAKE_C_STANDARD 11)
find_package(Curses REQUIRED)
include_directories(${CURSES_INCLUDE_DIR})

include_directories(.)

add_executable(asteroids
        asteroids.c
        asteroids.h
        move_ship.c)

target_link_libraries(${CMAKE_PROJECT_NAME} ${CURSES_LIBRARIES})
