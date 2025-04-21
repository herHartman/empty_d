cmake_minimum_required(VERSION 3.16)

project(empty_d)

set(CMAKE_CXX_STANDARD 14)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)


include(cmake/StandardSettings.cmake)
include(cmake/Utils.cmake)

file(GLOB_RECURSE SOURCES
  ${CMAKE_CURRENT_SOURCE_DIR}/src/*pp
)

file(GLOB_RECURSE C_SOURCES
  ${CMAKE_CURRENT_SOURCE_DIR}/src/*c
)


file (GLOB_RECURSE HEADERS
  ${CMAKE_CURRENT_SOURCE_DIR}/include/*.h
)


add_executable(
  empty_d
  ${SOURCES}
  ${C_SOURCES} 
  ${HEADERS}
  main.cpp
)

find_package(Boost REQUIRED)
find_package(Boost REQUIRED COMPONENTS json)


set(CORE_SOURCE_DIR ${PROJECT_SOURCE_DIR}/src)

target_include_directories(empty_d PUBLIC 
  ${CORE_SOURCE_DIR}
)

target_link_libraries(
    empty_d
    PRIVATE Boost::boost Boost::json
)