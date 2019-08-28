cmake_minimum_required(VERSION 3.9)

project(tests)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

include_directories(${CMAKE_SOURCE_DIR}/test_package/include)

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

target_link_libraries(tests ${CONAN_LIBS} cis1_core std::filesystem)

enable_testing()
add_test(NAME tests
         WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/bin
         COMMAND ${PROJECT_NAME})
