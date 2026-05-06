cmake_minimum_required(VERSION 3.28)
project(MatchingEngine VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_EXTENSIONS False)

# Add these flags for debug and profiling: -g -fno-omit-frame-pointer
string(APPEND CMAKE_CXX_FLAGS_RELEASE " -O3")

include(FetchContent)

FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG v1.17.0
)
FetchContent_MakeAvailable(googletest)

FetchContent_Declare(
        googlebenchmark
        GIT_REPOSITORY https://github.com/google/benchmark.git
        GIT_TAG v1.9.5
)
set(BENCHMARK_ENABLE_TESTING OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googlebenchmark)

add_library(engine_lib
        src/Application.cpp
        src/ConfigParser.cpp
        src/LobsterOrderLoader.cpp
        src/LobsterStateLoader.cpp
        src/MatchingEngine.cpp
        src/OrderBook.cpp
        src/OrderGenerator.cpp
        src/OrderIdMapper.cpp
        src/PriceBitset.cpp
        src/ResultWriter.cpp
)

target_include_directories(engine_lib PUBLIC ${CMAKE_SOURCE_DIR}/include)

target_compile_options(engine_lib PUBLIC
        -Wall
        -Wextra
        -Wpedantic
        -Wshadow
        -Wconversion
        -Wsign-conversion
)

add_executable(matching_engine src/main.cpp)
target_link_libraries(matching_engine PRIVATE engine_lib)

file(GLOB BENCHMARK_SOURCES CONFIGURE_DEPENDS "benchmark/*.cpp")
add_executable(engine_benchmarks ${BENCHMARK_SOURCES})
target_link_libraries(engine_benchmarks PRIVATE benchmark::benchmark engine_lib)

file(GLOB UNIT_TEST_SOURCES CONFIGURE_DEPENDS "test/unit/*.cpp")
add_executable(unit_tests ${UNIT_TEST_SOURCES})
target_link_libraries(unit_tests PRIVATE gtest_main engine_lib)

file(GLOB INTEGRATION_TEST_SOURCES CONFIGURE_DEPENDS "test/integration/*.cpp")
add_executable(integration_tests ${INTEGRATION_TEST_SOURCES})
target_link_libraries(integration_tests PRIVATE gtest_main engine_lib)

include(GoogleTest)
gtest_discover_tests(unit_tests)
gtest_discover_tests(integration_tests)
