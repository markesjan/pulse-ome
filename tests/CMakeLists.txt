file(GLOB UNIT_TEST_SOURCES CONFIGURE_DEPENDS "unit/*.cpp")
add_executable(unit_tests ${UNIT_TEST_SOURCES})
target_link_libraries(unit_tests
        PRIVATE
        gmock_main
        pulse::ome
        pulse_compiler_flags
)

file(GLOB INTEGRATION_TEST_SOURCES CONFIGURE_DEPENDS "integration/*.cpp")
add_executable(integration_tests ${INTEGRATION_TEST_SOURCES})
target_link_libraries(integration_tests
        PRIVATE
        gmock_main
        pulse::ome
        pulse_compiler_flags
)

include(GoogleTest)
gtest_discover_tests(unit_tests)
gtest_discover_tests(integration_tests)
