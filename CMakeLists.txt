cmake_minimum_required(VERSION 3.23)
project(database)

set(CMAKE_CXX_STANDARD 17)

link_directories(lib)

add_executable(database main.cpp)
add_subdirectory(lib)