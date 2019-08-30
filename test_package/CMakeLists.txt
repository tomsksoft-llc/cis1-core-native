cmake_minimum_required(VERSION 3.9)

project(tests)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/modules/")

find_package(Filesystem REQUIRED Final)

if(BUILD_TESTING)
include_directories(${CMAKE_SOURCE_DIR}/test_package/include)
else()
include_directories(${CMAKE_SOURCE_DIR}/include)
endif()

add_executable(
    tests
    src/main.cpp
    src/read_istream_kv_str.cpp
    src/build.cpp
    src/init_context.cpp
    src/invoke_session.cpp
    src/prepare_build.cpp
    src/get_value.cpp
    src/get_param.cpp
    src/set_value.cpp
    src/set_param.cpp)

if(BUILD_TESTING)
target_link_libraries(tests ${CONAN_LIBS} cis1_core std::filesystem)
else()
target_link_libraries(tests ${CONAN_LIBS} std::filesystem)
endif()

enable_testing()
add_test(NAME tests
         WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/bin
         COMMAND ${PROJECT_NAME})
