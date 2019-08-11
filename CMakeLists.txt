cmake_minimum_required(VERSION 3.9)

project(cis1_core_native)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()


include_directories(${CMAKE_SOURCE_DIR}/include)
add_executable(startjob src/startjob.cpp src/cis1_core.cpp)
target_link_libraries(startjob ${CONAN_LIBS})

install(TARGETS startjob DESTINATION bin)

if(BUILD_TESTING)
    add_subdirectory(test_package)
endif()
