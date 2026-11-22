add_library(pulse_bus INTERFACE)
add_library(pulse::bus ALIAS pulse_bus)

target_include_directories(pulse_bus
        INTERFACE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(pulse_bus
        INTERFACE
        pulse::proto
        pulse_compiler_flags
)

file(GLOB BUS_TEST_SOURCES CONFIGURE_DEPENDS "tests/*.cpp")
add_executable(bus_unit_tests ${BUS_TEST_SOURCES})
target_link_libraries(bus_unit_tests
        PRIVATE
        gmock_main
        pulse::bus
        pulse_compiler_flags
)

include(GoogleTest)
gtest_discover_tests(bus_unit_tests)
