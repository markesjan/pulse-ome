add_library(pulse_proto INTERFACE)
add_library(pulse::proto ALIAS pulse_proto)

target_include_directories(pulse_proto
    INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(pulse_proto
    INTERFACE
    pulse_compiler_flags
)
