cmake_minimum_required(VERSION 3.9)

project(cis1_core_native)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

add_executable(tests main.cpp)
target_link_libraries(tests ${CONAN_LIBS})

enable_testing()
add_test(NAME tests
         WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/bin
         COMMAND ${PROJECT_NAME})
