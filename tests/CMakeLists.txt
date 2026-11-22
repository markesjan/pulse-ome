file(GLOB INTEGRATION_TEST_SOURCES CONFIGURE_DEPENDS "*.cpp")
add_executable(integration_tests ${INTEGRATION_TEST_SOURCES})
target_link_libraries(integration_tests
        PRIVATE
        gmock_main
        pulse::ome
        pulse_compiler_flags
)

include(GoogleTest)
gtest_discover_tests(integration_tests)
