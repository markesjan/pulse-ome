file(GLOB OME_BENCHMARK_SOURCES CONFIGURE_DEPENDS "*.cpp")
add_executable(ome_benchmarks ${OME_BENCHMARK_SOURCES})
target_link_libraries(ome_benchmarks
        PRIVATE
        benchmark::benchmark
        pulse::ome
        pulse_compiler_flags
)

if (ipo_supported)
    set_target_properties(ome_benchmarks PROPERTIES INTERPROCEDURAL_OPTIMIZATION TRUE)
endif ()
apply_pgo(ome_benchmarks)
