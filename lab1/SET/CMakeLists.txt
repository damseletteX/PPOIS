cmake_minimum_required(VERSION 3.24)
project(CantorSet LANGUAGES CXX)
 
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
 
add_library(cantor_set STATIC
    src/set.cpp
)
target_include_directories(cantor_set PUBLIC include)
 
option(ENABLE_COVERAGE "Build with --coverage instrumentation for gcov" OFF)
if(ENABLE_COVERAGE)
    target_compile_options(cantor_set PUBLIC --coverage -O0)
    target_link_options(cantor_set PUBLIC --coverage)
endif()
 
add_executable(set_cli src/main.cpp)
target_link_libraries(set_cli PRIVATE cantor_set)
 
enable_testing()
 
include(FetchContent)
FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG        v1.15.2
    FIND_PACKAGE_ARGS NAMES GTest
)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)
 
add_executable(run_gtests unit-test/test.cpp)
target_link_libraries(run_gtests PRIVATE cantor_set GTest::gtest_main)
include(GoogleTest)
gtest_discover_tests(run_gtests)
 
