cmake_minimum_required(VERSION 3.9)

project(cis1_core_native LANGUAGES CXX)

option(BUILD_DOC "Build documentation" ON)
option(BUILD_TESTING "Build tests" OFF)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/modules/")

find_package(Filesystem REQUIRED Final)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

include_directories(${CMAKE_SOURCE_DIR}/include)

add_library(cis1_core STATIC
        src/job_runner.cpp
        src/error_code.cpp
        src/context.cpp
        src/session.cpp
        src/os.cpp
        src/read_istream_kv_str.cpp
        src/ifstream_adapter.cpp
        src/ofstream_adapter.cpp
        src/fs_entry_adapter.cpp
        src/get_param.cpp
        src/set_param.cpp
        src/get_value.cpp
        src/set_value.cpp
        src/logger.cpp
        src/get_parent_id.cpp
        src/webui_session.cpp
        src/cron.cpp
        src/utils.cpp
        src/job.cpp)

add_executable(startjob src/startjob.cpp)
add_executable(getparam src/getparam.cpp)
add_executable(setparam src/setparam.cpp)
add_executable(getvalue src/getvalue.cpp)
add_executable(setvalue src/setvalue.cpp)
add_executable(cis_cron src/cis_cron.cpp)
add_executable(cis_cron_daemon src/cis_cron_daemon.cpp)
add_executable(maintenance src/maintenance.cpp)

target_link_libraries(startjob cis1_core ${CONAN_LIBS} std::filesystem)
target_link_libraries(getparam cis1_core ${CONAN_LIBS} std::filesystem)
target_link_libraries(setparam cis1_core ${CONAN_LIBS} std::filesystem)
target_link_libraries(getvalue cis1_core ${CONAN_LIBS} std::filesystem)
target_link_libraries(setvalue cis1_core ${CONAN_LIBS} std::filesystem)
target_link_libraries(cis_cron cis1_core ${CONAN_LIBS} std::filesystem)
target_link_libraries(cis_cron_daemon cis1_core ${CONAN_LIBS} std::filesystem)
target_link_libraries(maintenance cis1_core ${CONAN_LIBS} std::filesystem)

set_property(TARGET cis1_core PROPERTY CXX_STANDARD 17)
set_property(TARGET startjob PROPERTY CXX_STANDARD 17)
set_property(TARGET getparam PROPERTY CXX_STANDARD 17)
set_property(TARGET setparam PROPERTY CXX_STANDARD 17)
set_property(TARGET getvalue PROPERTY CXX_STANDARD 17)
set_property(TARGET setvalue PROPERTY CXX_STANDARD 17)
set_property(TARGET cis_cron PROPERTY CXX_STANDARD 17)
set_property(TARGET cis_cron_daemon PROPERTY CXX_STANDARD 17)
set_property(TARGET maintenance PROPERTY CXX_STANDARD 17)

install(TARGETS cis1_core DESTINATION lib)
install(TARGETS startjob DESTINATION bin)
install(TARGETS getparam DESTINATION bin)
install(TARGETS setparam DESTINATION bin)
install(TARGETS getvalue DESTINATION bin)
install(TARGETS setvalue DESTINATION bin)
install(TARGETS cis_cron DESTINATION bin)
install(TARGETS cis_cron_daemon DESTINATION bin)
install(TARGETS maintenance DESTINATION bin)

if(BUILD_DOC)
    find_package(Doxygen REQUIRED)
    # set input and output files
    set(DOXYGEN_IN ${CMAKE_SOURCE_DIR}/Doxyfile.in)
    set(DOXYGEN_OUT ${CMAKE_BINARY_DIR}/Doxyfile)

    # request to configure the file
    configure_file(${DOXYGEN_IN} ${DOXYGEN_OUT} @ONLY)
    message("Doxygen build started")

    # note the option ALL which allows to build the docs together with the application
    add_custom_target(
        doc_doxygen ALL
        COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_OUT}
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        COMMENT "Generating API documentation with Doxygen"
        VERBATIM)
endif(BUILD_DOC)

if(BUILD_TESTING)
    add_subdirectory(test_package)
endif(BUILD_TESTING)
