add_library(pulse_bus INTERFACE)
add_library(pulse::bus ALIAS pulse_bus)

target_include_directories(pulse_bus
        INTERFACE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(pulse_bus
        INTERFACE
        pulse_compiler_flags
)
