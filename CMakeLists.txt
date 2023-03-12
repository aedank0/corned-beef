cmake_minimum_required(VERSION 3.19)

project(corned-beef)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED YES)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(corned-beef INTERFACE corned_beef.hpp)
target_compile_features(corned-beef INTERFACE cxx_std_20)

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
target_compile_features(testing PRIVATE cxx_std_20)

target_link_libraries(
    testing
    corned-beef
    GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(testing WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}")

find_package(Doxygen)

option(CB_BUILD_DOCS "Should documentation be build (most likely yes!)" ON)
option(CB_COMPILE_COLLISION_TESTS "Should collision tests be compiled?")

if(CB_BUILD_DOCS)
    if(DOXYGEN_FOUND)
        set(DOXYGEN_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/docs")

        doxygen_add_docs(
            doxygen
            "corned_beef.hpp"
            ALL
            COMMENT "Generate documentation"
        )
    else()
        message("Doxygen not found, cannot generate documents")
    endif()
endif()

if(CB_COMPILE_COLLISION_TESTS)
    target_compile_definitions(testing PRIVATE CB_COMPILE_COLLISION_TESTS)
endif()

if(DEFINED CB_XOR_OPERAND)
    target_compile_definitions(corned-beef INTERFACE CB_XOR_OPERAND=${CB_XOR_OPERAND})
endif()
