cmake_minimum_required(VERSION 3.28)
project(PulseOME VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(ENABLE_PROFILING "Enable flags for profiling and call stack unwinding (-g -fno-omit-frame-pointer)" OFF)

add_library(pulse_compiler_flags INTERFACE)
target_compile_features(pulse_compiler_flags INTERFACE cxx_std_23)
target_compile_options(pulse_compiler_flags INTERFACE
        -Wall
        -Wextra
        -Wpedantic
        -Wshadow
        -Wconversion
        -Wsign-conversion
        -march=native
)

if (ENABLE_PROFILING)
    target_compile_options(pulse_compiler_flags INTERFACE
            -g
            -fno-omit-frame-pointer
    )
    message(STATUS "Profiling flags ENABLED (-g -fno-omit-frame-pointer)")
endif ()

include(CheckIPOSupported)
check_ipo_supported(RESULT ipo_supported OUTPUT ipo_error)
if (NOT ipo_supported)
    message(WARNING "IPO / LTO is not supported by your compiler: ${ipo_error}")
endif ()

option(ENABLE_PGO_GENERATE "Compile with PGO instrumentation" OFF)
option(ENABLE_PGO_USE "Compile with PGO profile data" OFF)

if (ENABLE_PGO_GENERATE)
    set(PGO_FLAGS "-fprofile-generate")
    message(STATUS "PGO: Profile Generation ENABLED")
elseif (ENABLE_PGO_USE)
    if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
        set(PGO_FLAGS
                "-fprofile-use=default.profdata"
                "-Wno-profile-instr-out-of-date"
                "-Wno-profile-instr-unprofiled"
        )
        message(STATUS "PGO: Profile Use ENABLED (Clang mode)")
    else ()
        set(PGO_FLAGS
                "-fprofile-use"
                "-fprofile-correction"
                "-Wno-missing-profile"
        )
        message(STATUS "PGO: Profile Use ENABLED (GCC mode)")
    endif ()
endif ()

function(apply_pgo TARGET_NAME)
    if (PGO_FLAGS)
        target_compile_options(${TARGET_NAME} PRIVATE ${PGO_FLAGS})
        target_link_options(${TARGET_NAME} PRIVATE ${PGO_FLAGS})
    endif ()
endfunction()

include(FetchContent)

FetchContent_Declare(
        googletest
        SYSTEM
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG v1.17.0
)
FetchContent_Declare(
        googlebenchmark
        SYSTEM
        GIT_REPOSITORY https://github.com/google/benchmark.git
        GIT_TAG v1.9.5
)
set(BENCHMARK_ENABLE_TESTING OFF CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(googletest googlebenchmark)

add_subdirectory(libs/bus)
add_subdirectory(libs/proto)
add_subdirectory(apps/ome)
add_subdirectory(apps/feed)
add_subdirectory(apps/gui)
enable_testing()
add_subdirectory(benchmarks)
add_subdirectory(tests)
