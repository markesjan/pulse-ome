cmake_minimum_required(VERSION 3.28)
project(MatchingEngine VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_EXTENSIONS False)

include(FetchContent)

FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG        v1.17.0
)
FetchContent_MakeAvailable(googletest)

FetchContent_Declare(
        googlebenchmark
        GIT_REPOSITORY https://github.com/google/benchmark.git
        GIT_TAG        v1.9.5
)
set(BENCHMARK_ENABLE_TESTING OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googlebenchmark)

add_library(engine_lib
        src/ConfigParser.cpp
        src/DataLoader.cpp
        src/OrderBook.cpp
        src/ResultWriter.cpp
)

target_include_directories(engine_lib PUBLIC ${CMAKE_SOURCE_DIR}/include)

target_compile_options(engine_lib PRIVATE
        -Wall
        -Wextra
        -Wpedantic
        -Wshadow
        -Wconversion
        -Wsign-conversion
)

add_executable(matching_engine src/main.cpp)
target_link_libraries(matching_engine PRIVATE engine_lib)

#add_executable(unit_tests test/unit/xxx.cpp)
#target_link_libraries(unit_tests PRIVATE gtest_main engine_lib)

add_executable(integration_tests test/integration/OrderMatchingTest.cpp)
target_link_libraries(integration_tests PRIVATE gtest_main engine_lib)

include(GoogleTest)
#gtest_discover_tests(unit_tests)
gtest_discover_tests(integration_tests)

add_executable(engine_benchmarks benchmark/OrderBookBenchmark.cpp)
target_link_libraries(engine_benchmarks PRIVATE benchmark::benchmark engine_lib)
