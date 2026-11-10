file(GLOB BENCHMARK_SOURCES CONFIGURE_DEPENDS "*.cpp")
add_executable(engine_benchmarks ${BENCHMARK_SOURCES})
target_link_libraries(engine_benchmarks
        PRIVATE
        benchmark::benchmark
        pulse::ome
        pulse_compiler_flags
)

if (ipo_supported)
    set_target_properties(engine_benchmarks PROPERTIES INTERPROCEDURAL_OPTIMIZATION TRUE)
endif ()
apply_pgo(engine_benchmarks)
