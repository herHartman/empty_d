cmake_minimum_required(VERSION 3.27)

project(
  ${CMAKE_PROJECT_NAME}Tests
  LANGUAGES CXX
)

verbose_message("Adding tests under ${CMAKE_PROJECT_NAME}Tests...")

foreach(file ${UNIT_TEST_SOURCES})
  string(REGEX REPLACE "(.*/)([a-zA-Z0-9_ ]+)(\.cpp)" "\\2" test_name ${file}) 
  add_executable(${test_name}_Tests ${file} ${SOURCES})

    target_compile_features(${test_name}_Tests PUBLIC cxx_std_23)

    if(${CMAKE_PROJECT_NAME}_BUILD_EXECUTABLE)
        set(${CMAKE_PROJECT_NAME}_TEST_LIB ${CMAKE_PROJECT_NAME}_LIB)
    else()
        set(${CMAKE_PROJECT_NAME}_TEST_LIB ${CMAKE_PROJECT_NAME})
    endif()

    find_package(GTest REQUIRED)

    if(${CMAKE_PROJECT_NAME}_USE_GOOGLE_MOCK)
      set(GOOGLE_MOCK_LIBRARIES GTest::gmock GTest::gmock_main)
    endif()

    set(${CMAKE_PROJECT_NAME}_TEST_LIB empty_d-core)

    target_include_directories(${test_name}_Tests PUBLIC 
      ${CORE_SOURCE_DIR}
    )

    target_link_libraries(
    ${test_name}_Tests
    PUBLIC
        GTest::GTest
        GTest::Main
        ${GOOGLE_MOCK_LIBRARIES}
    )

    add_test(
      NAME
        ${test_name}
      COMMAND
        ${test_name}_Tests
    )
endforeach()

verbose_message("${CORE_SOURCE_DIR}")
verbose_message("Finished adding unit tests for ${CMAKE_PROJECT_NAME}.")
