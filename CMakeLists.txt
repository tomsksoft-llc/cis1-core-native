cmake_minimum_required(VERSION 3.9)

project(cis1_core_native LANGUAGES CXX)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/modules/")

find_package(Filesystem REQUIRED Final)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

include_directories(${CMAKE_SOURCE_DIR}/include)

add_library(cis1_core STATIC
        src/job_runner.cpp
        src/cis1_error_code.cpp
        src/cis1_context.cpp
        src/session.cpp
        src/build.cpp
        src/os_interface.cpp
        src/read_istream_kv_str.cpp
        src/ifstream_adapter.cpp
        src/ofstream_adapter.cpp
        src/fs_entry_adapter.cpp)

add_executable(startjob src/startjob.cpp)
add_executable(getparam src/getparam.cpp)
add_executable(setparam src/setparam.cpp)
add_executable(getvalue src/getvalue.cpp)
add_executable(setvalue src/setvalue.cpp)

target_link_libraries(startjob cis1_core ${CONAN_LIBS} std::filesystem)
target_link_libraries(getparam cis1_core ${CONAN_LIBS} std::filesystem)
target_link_libraries(setparam cis1_core ${CONAN_LIBS} std::filesystem)
target_link_libraries(getvalue cis1_core ${CONAN_LIBS} std::filesystem)
target_link_libraries(setvalue cis1_core ${CONAN_LIBS} std::filesystem)

set_property(TARGET cis1_core PROPERTY CXX_STANDARD 17)
set_property(TARGET startjob PROPERTY CXX_STANDARD 17)
set_property(TARGET getparam PROPERTY CXX_STANDARD 17)
set_property(TARGET setparam PROPERTY CXX_STANDARD 17)
set_property(TARGET getvalue PROPERTY CXX_STANDARD 17)
set_property(TARGET setvalue PROPERTY CXX_STANDARD 17)

install(TARGETS startjob DESTINATION bin)
install(TARGETS getparam DESTINATION bin)
install(TARGETS setparam DESTINATION bin)
install(TARGETS getvalue DESTINATION bin)
install(TARGETS setvalue DESTINATION bin)

if(BUILD_TESTING)
    add_subdirectory(test_package)
endif()
