cmake_minimum_required(VERSION 3.10)

project(libjson C)

set(CMAKE_C_STANDARD 99)

include_directories(include)

file(GLOB SOURCES "src/*.c")

add_library(json STATIC ${SOURCES})

enable_testing()

# Instead of combining all test files, create a separate executable for each test file
file(GLOB TEST_SOURCES "tests/*.c")
foreach(TEST_SOURCE ${TEST_SOURCES})
    get_filename_component(TEST_NAME ${TEST_SOURCE} NAME_WE)
    add_executable(${TEST_NAME} ${TEST_SOURCE})
    target_link_libraries(${TEST_NAME} json)
    add_test(NAME ${TEST_NAME} COMMAND ${TEST_NAME})
endforeach()

file(GLOB EXAMPLES "examples/*.c")
foreach(EXAMPLE ${EXAMPLES})
    get_filename_component(EXAMPLE_NAME ${EXAMPLE} NAME_WE)
    add_executable(${EXAMPLE_NAME} ${EXAMPLE})
    target_link_libraries(${EXAMPLE_NAME} json)
endforeach()