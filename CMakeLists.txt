cmake_minimum_required(VERSION 3.10)

project(libjson C)

set(CMAKE_C_STANDARD 99)

include_directories(include)

file(GLOB SOURCES "src/*.c")

add_library(json STATIC ${SOURCES})

enable_testing()

file(GLOB TEST_SOURCES "tests/*.c")
add_executable(json_tests ${TEST_SOURCES})
target_link_libraries(json_tests json)

add_test(NAME json_tests COMMAND json_tests)

file(GLOB EXAMPLES "examples/*.c")
foreach(EXAMPLE ${EXAMPLES})
    get_filename_component(EXAMPLE_NAME ${EXAMPLE} NAME_WE)
    add_executable(${EXAMPLE_NAME} ${EXAMPLE})
    target_link_libraries(${EXAMPLE_NAME} json)
endforeach()