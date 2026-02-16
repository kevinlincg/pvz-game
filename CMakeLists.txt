cmake_minimum_required(VERSION 3.16)

if(APPLE AND NOT CMAKE_OSX_ARCHITECTURES)
    set(CMAKE_OSX_ARCHITECTURES ${CMAKE_SYSTEM_PROCESSOR} CACHE STRING "macOS arch")
endif()

project(plant-legends VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# --- SF3 Engine Integration ---
option(PVZ_SF3_SOURCE "Include sf3-engine source for full debugging" ON)
set(SF3_ENGINE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../soulfire3" CACHE PATH "Path to sf3-engine root")

# Resolve to absolute
get_filename_component(SF3_ENGINE_DIR "${SF3_ENGINE_DIR}" ABSOLUTE)

if(NOT EXISTS "${SF3_ENGINE_DIR}/src/sf3.hpp")
    message(FATAL_ERROR "sf3-engine not found at: ${SF3_ENGINE_DIR}\n"
        "Set -DSF3_ENGINE_DIR=<path> to your sf3-engine directory.")
endif()

if(PVZ_SF3_SOURCE)
    message(STATUS "[Plant Legends] sf3-engine SOURCE mode")
    set(SF3_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
    set(SF3_BUILD_TESTS OFF CACHE BOOL "" FORCE)
    add_subdirectory(${SF3_ENGINE_DIR} ${CMAKE_BINARY_DIR}/sf3-engine)
else()
    message(FATAL_ERROR "Prebuilt mode not supported yet, use -DPVZ_SF3_SOURCE=ON")
endif()

# --- Lua Integration ---
# SF3 already includes Lua, so we can use it directly
include_directories(${SF3_ENGINE_DIR}/third_party/lua)

# --- Plant Legends Game Target ---
set(PLANT_LEGENDS_SOURCES
    src/main.cpp
    src/lua/lua_manager.cpp
    src/lua/lua_manager.hpp
)

add_executable(plant-legends ${PLANT_LEGENDS_SOURCES})

target_include_directories(plant-legends PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${SF3_ENGINE_DIR}/src
    ${SF3_ENGINE_DIR}/third_party
)

target_link_libraries(plant-legends PRIVATE sf3)

# Copy scripts to build directory
add_custom_command(TARGET plant-legends POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_SOURCE_DIR}/scripts
        $<TARGET_FILE_DIR:plant-legends>/scripts
    COMMENT "Copying Lua scripts to build directory"
)

# Copy assets to build directory
add_custom_command(TARGET plant-legends POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_SOURCE_DIR}/assets
        $<TARGET_FILE_DIR:plant-legends>/assets
    COMMENT "Copying assets to build directory"
)

# Platform-specific settings
if(EMSCRIPTEN)
    set_target_properties(plant-legends PROPERTIES SUFFIX ".html")
    target_link_options(plant-legends PRIVATE
        --preload-file ${CMAKE_CURRENT_SOURCE_DIR}/scripts@scripts
        --preload-file ${CMAKE_CURRENT_SOURCE_DIR}/assets@assets
        -sUSE_SDL=0
        -sMIN_WEBGL_VERSION=1
        -sMAX_WEBGL_VERSION=2
        -sFULL_ES2=1
        -sGL_ENABLE_GET_PROC_ADDRESS=1
        -sALLOW_MEMORY_GROWTH=1
        -sINITIAL_MEMORY=134217728
        -sSTACK_SIZE=2097152
        -sEXPORTED_FUNCTIONS=_main
        -sEXPORTED_RUNTIME_METHODS=ccall,cwrap
        --shell-file ${CMAKE_CURRENT_SOURCE_DIR}/web/shell.html
    )
elseif(APPLE)
    find_library(COREAUDIO_LIBRARY CoreAudio)
    find_library(AUDIOUNIT_LIBRARY AudioUnit)
    find_library(AUDIOTOOLBOX_LIBRARY AudioToolbox)
    target_link_libraries(plant-legends PRIVATE
        ${COREAUDIO_LIBRARY} ${AUDIOUNIT_LIBRARY} ${AUDIOTOOLBOX_LIBRARY})
elseif(WIN32)
    target_link_libraries(plant-legends PRIVATE winmm ole32 setupapi imm32 version)
elseif(UNIX)
    # Linux: common deps for SDL3
    target_link_libraries(plant-legends PRIVATE dl m)
endif()

message(STATUS "[Plant Legends] Build configuration complete")
message(STATUS "  - SF3 Engine: ${SF3_ENGINE_DIR}")
message(STATUS "  - Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "  - Sources: ${PLANT_LEGENDS_SOURCES}")

# --- Tests ---
option(BUILD_TESTS "Build test suite" ON)

if(BUILD_TESTS AND NOT EMSCRIPTEN)
    set(TEST_SOURCES
        tests/test_main.cpp
        src/lua/lua_manager.cpp
    )
    
    add_executable(plant-legends-tests ${TEST_SOURCES})
    
    target_include_directories(plant-legends-tests PRIVATE 
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${SF3_ENGINE_DIR}/src
        ${SF3_ENGINE_DIR}/third_party
    )
    
    target_link_libraries(plant-legends-tests PRIVATE lua54)
    
    # Copy scripts to test directory
    add_custom_command(TARGET plant-legends-tests POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_CURRENT_SOURCE_DIR}/scripts
            $<TARGET_FILE_DIR:plant-legends-tests>/scripts
        COMMENT "Copying Lua scripts to test directory"
    )
    
    # Enable testing
    enable_testing()
    add_test(NAME PlantLegendsTests COMMAND plant-legends-tests)
    
    message(STATUS "  - Tests: Enabled")
else()
    message(STATUS "  - Tests: Disabled")
endif()
