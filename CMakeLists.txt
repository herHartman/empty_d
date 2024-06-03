cmake_minimum_required(VERSION 3.27)

project(empty_d)

set(CMAKE_CXX_STANDARD 23)

find_package(Boost REQUIRED)
find_package(Boost REQUIRED COMPONENTS json)

file(GLOB_RECURSE SOURCES
  ${CMAKE_CURRENT_SOURCE_DIR}/core/src/*pp
)

add_executable(empty_d main.cpp
    ${SOURCES}
)


target_link_libraries(
  empty_d 
  PRIVATE Boost::boost Boost::json
  PRIVATE gtest gmock pthread
)

target_include_directories(empty_d PUBLIC 
    ${PROJECT_SOURCE_DIR}/core/src
)


