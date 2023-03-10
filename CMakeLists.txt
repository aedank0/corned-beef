cmake_minimum_required(VERSION 3.19)

project(corned-beef)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED YES)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(corned-beef INTERFACE corned-beef.hpp)

include(FetchContent)
FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG 50e07d1c92875e66138d5d5ee3bb46ef237115bb
)

if(WIN32)
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
endif()

FetchContent_MakeAvailable(googletest)

enable_testing()

add_executable(testing testing.cpp)

target_link_libraries(
    testing
    corned-beef
    GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(testing)

find_package(Doxygen REQUIRED)

set(DOXYGEN_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/docs")

doxygen_add_docs(
    doxygen
    "corned-beef.hpp"
    COMMENT "Generate documentation"
)
