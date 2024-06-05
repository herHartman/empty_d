cmake_minimum_required(VERSION 3.27)

project(empty_d)

set(CMAKE_CXX_STANDARD 23)

add_subdirectory(core)

add_executable(${PROJECT_NAME} main.cpp)
