add_library(pulse_ome_lib
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

add_library(pulse::ome ALIAS pulse_ome_lib)
add_library(pulse::ome_lib ALIAS pulse_ome_lib)

target_include_directories(pulse_ome_lib
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(pulse_ome_lib
        PUBLIC
        pulse::bus
        PRIVATE
        pulse_compiler_flags
)

if (ipo_supported)
    set_target_properties(pulse_ome_lib PROPERTIES INTERPROCEDURAL_OPTIMIZATION TRUE)
endif ()
apply_pgo(pulse_ome_lib)

add_executable(pulse_ome src/main.cpp)
target_link_libraries(pulse_ome
        PRIVATE
        pulse::ome
        pulse_compiler_flags
)

if (ipo_supported)
    set_target_properties(pulse_ome PROPERTIES INTERPROCEDURAL_OPTIMIZATION TRUE)
endif ()
apply_pgo(pulse_ome)
