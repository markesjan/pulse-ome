cmake_minimum_required(VERSION 3.28)
project(MatchingEngine VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_EXTENSIONS False)

add_executable(matching_engine
        src/main.cpp
        src/DataLoader.cpp
        src/MatchingEngine.cpp
        src/OrderBook.cpp
        src/ResultWriter.cpp
)

target_include_directories(matching_engine PRIVATE ${CMAKE_SOURCE_DIR}/include)

target_compile_options(matching_engine PRIVATE
        -Wall
        -Wextra
        -Wpedantic
        -Wshadow
        -Wconversion
        -Wsign-conversion
)
