# Native Nintendo Switch Homebrew bring-up target.
cmake_minimum_required(VERSION 3.21)

option(SWITCH_BRINGUP "Build the native Switch hardware smoke test" ON)
if(NOT SWITCH_BRINGUP)
    message(FATAL_ERROR "The Switch full-game target is not implemented yet; build with -DSWITCH_BRINGUP=ON.")
endif()

add_executable(OptiCraft "${CMAKE_SOURCE_DIR}/src/switch/tools/SwitchBringup.cpp")
set_target_properties(OptiCraft PROPERTIES
    SUFFIX ".elf"
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED YES
    CXX_EXTENSIONS NO
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin/switch"
)
target_compile_definitions(OptiCraft PRIVATE SWITCH_PLATFORM=1)
target_compile_options(OptiCraft PRIVATE
    -march=armv8-a+crc
    -mtp=soft
    -fPIE
    -ffunction-sections
    -fdata-sections
)
target_include_directories(OptiCraft PRIVATE "${LIBNX}/include")
target_link_directories(OptiCraft PRIVATE "${LIBNX}/lib")
target_link_libraries(OptiCraft PRIVATE nx m)
target_link_options(OptiCraft PRIVATE
    "-specs=${LIBNX}/switch.specs"
    -march=armv8-a+crc
    -mtp=soft
    -fPIE
    "-Wl,-Map,${CMAKE_BINARY_DIR}/OptiCraft.map"
    -Wl,--gc-sections
)

find_program(SWITCH_NXLINK NAMES nxlink HINTS "${DEVKITPRO}/tools/bin")
if(NOT SWITCH_NXLINK)
    message(FATAL_ERROR "nxlink was not found. Install devkitPro's switch-tools package.")
endif()

set(SWITCH_OUTPUT_DIR "${CMAKE_SOURCE_DIR}/bin/switch")
add_custom_command(TARGET OptiCraft POST_BUILD
    COMMAND "${SWITCH_NXLINK}" -o "${SWITCH_OUTPUT_DIR}/OptiCraft.nro" "$<TARGET_FILE:OptiCraft>"
    COMMENT "nxlink: ${SWITCH_OUTPUT_DIR}/OptiCraft.nro"
    VERBATIM
)

add_custom_target(switch-data
    COMMAND ${CMAKE_COMMAND} -E make_directory "${SWITCH_OUTPUT_DIR}/data"
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CMAKE_SOURCE_DIR}/data/assets" "${SWITCH_OUTPUT_DIR}/data/assets"
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CMAKE_SOURCE_DIR}/data/resources" "${SWITCH_OUTPUT_DIR}/data/resources"
    COMMENT "Staging game data for the future Switch port"
    VERBATIM
)
