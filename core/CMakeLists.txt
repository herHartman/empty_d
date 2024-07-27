# cmake_minimum_required(VERSION 3.27)

# project(empty_d-core)

# set(CMAKE_CXX_STANDARD 23)


# include(cmake/StandardSettings.cmake)
# include(cmake/Utils.cmake)

# file(GLOB_RECURSE SOURCES
#   ${CMAKE_CURRENT_SOURCE_DIR}/src/*pp
# )

# file(GLOB_RECURSE C_SOURCES
#   ${CMAKE_CURRENT_SOURCE_DIR}/src/*c
# )

# file (GLOB_RECURSE UNIT_TEST_SOURCES
#   ${CMAKE_CURRENT_SOURCE_DIR}/*_test.cpp
# )

# file (GLOB_RECURSE HEADERS
#   ${CMAKE_CURRENT_SOURCE_DIR}/include/*.h
# )

# add_library(
#   empty_d-core
#   ${HEADERS}
#   ${SOURCES}
#   ${C_SOURCES}
# )


# add_library(empty_d-core::empty_d-core ALIAS empty_d-core)


# find_package(Boost REQUIRED)
# find_package(Boost REQUIRED COMPONENTS json)

# enable_testing()

# set(CORE_SOURCE_DIR ${PROJECT_SOURCE_DIR}/src)

# target_include_directories(empty_d-core PUBLIC 
#   ${CORE_SOURCE_DIR}
# )


# target_include_directories(
#   ${PROJECT_NAME} PUBLIC $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
#                          $<INSTALL_INTERFACE:include/${PROJECT_NAME}-${PROJECT_VERSION}>
# )


# add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/tests)


# target_link_libraries(
#     empty_d-core
#     PRIVATE Boost::boost Boost::json
# )