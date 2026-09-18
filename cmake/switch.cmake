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

find_program(SWITCH_ELF2NRO NAMES elf2nro HINTS "${DEVKITPRO}/tools/bin")
find_program(SWITCH_NACPTOOL NAMES nacptool HINTS "${DEVKITPRO}/tools/bin")
if(NOT SWITCH_ELF2NRO OR NOT SWITCH_NACPTOOL)
    message(FATAL_ERROR
        "elf2nro and nacptool are required. Install devkitPro's switch-tools package.")
endif()

# Keep these values in the cache so release builds can supply their own
# Homebrew Menu metadata without changing the build scripts.
set(SWITCH_TITLE "OptiCraft Heritage Edition" CACHE STRING "NRO application title")
set(SWITCH_AUTHOR "OptiCraft Heritage contributors" CACHE STRING "NRO author")
set(SWITCH_VERSION "1.1-switch-dev" CACHE STRING "NRO version")
set(SWITCH_ICON "" CACHE FILEPATH "Optional 256x256 JPEG icon embedded in the NRO")

set(SWITCH_OUTPUT_DIR "${CMAKE_SOURCE_DIR}/bin/switch")
set(SWITCH_NACP "${CMAKE_CURRENT_BINARY_DIR}/OptiCraft.nacp")
set(SWITCH_NRO "${SWITCH_OUTPUT_DIR}/OptiCraft.nro")

add_custom_command(
    OUTPUT "${SWITCH_NACP}"
    COMMAND "${SWITCH_NACPTOOL}" --create
            "${SWITCH_TITLE}" "${SWITCH_AUTHOR}" "${SWITCH_VERSION}" "${SWITCH_NACP}"
    COMMENT "Creating Nintendo Switch application metadata"
    VERBATIM
)

set(_switch_elf2nro_arguments
    "$<TARGET_FILE:OptiCraft>" "${SWITCH_NRO}" "--nacp=${SWITCH_NACP}")
if(SWITCH_ICON)
    if(NOT EXISTS "${SWITCH_ICON}")
        message(FATAL_ERROR "SWITCH_ICON does not exist: ${SWITCH_ICON}")
    endif()
    list(APPEND _switch_elf2nro_arguments "--icon=${SWITCH_ICON}")
endif()

add_custom_command(
    OUTPUT "${SWITCH_NRO}"
    COMMAND ${CMAKE_COMMAND} -E make_directory "${SWITCH_OUTPUT_DIR}"
    COMMAND "${SWITCH_ELF2NRO}" ${_switch_elf2nro_arguments}
    DEPENDS OptiCraft "${SWITCH_NACP}"
    COMMENT "Packaging ${SWITCH_NRO}"
    COMMAND_EXPAND_LISTS
    VERBATIM
)
add_custom_target(switch-package ALL DEPENDS "${SWITCH_NRO}")

# nxlink transfers an already-packaged NRO; it does not convert an ELF into
# one. Keep network deployment optional so ordinary builds work without it.
find_program(SWITCH_NXLINK NAMES nxlink HINTS "${DEVKITPRO}/tools/bin")
if(SWITCH_NXLINK)
    add_custom_target(switch-nxlink
        COMMAND "${SWITCH_NXLINK}" "${SWITCH_NRO}"
        DEPENDS switch-package
        COMMENT "Sending OptiCraft.nro with nxlink"
        USES_TERMINAL
        VERBATIM
    )
endif()

add_custom_target(switch-data
    COMMAND ${CMAKE_COMMAND} -E make_directory "${SWITCH_OUTPUT_DIR}/data"
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CMAKE_SOURCE_DIR}/data/assets" "${SWITCH_OUTPUT_DIR}/data/assets"
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CMAKE_SOURCE_DIR}/data/resources" "${SWITCH_OUTPUT_DIR}/data/resources"
    COMMENT "Staging game data for the future Switch port"
    VERBATIM
)
