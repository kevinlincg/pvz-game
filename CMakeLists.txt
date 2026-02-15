cmake_minimum_required(VERSION 3.16)

if(APPLE AND NOT CMAKE_OSX_ARCHITECTURES)
    set(CMAKE_OSX_ARCHITECTURES ${CMAKE_SYSTEM_PROCESSOR} CACHE STRING "macOS arch")
endif()

project(pvz-game VERSION 1.0.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# --- SF3 Engine Integration ---
# Two modes:
#   PVZ_SF3_SOURCE=OFF (default): link pre-built library + headers (fast compile)
#   PVZ_SF3_SOURCE=ON:            add_subdirectory full source (deep debug)
#
# SF3_ENGINE_DIR: defaults to ../sf3-engine (sibling directory)
option(PVZ_SF3_SOURCE "Include sf3-engine source for full debugging" OFF)
set(SF3_ENGINE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../sf3-engine" CACHE PATH "Path to sf3-engine root")

# Resolve to absolute
get_filename_component(SF3_ENGINE_DIR "${SF3_ENGINE_DIR}" ABSOLUTE)

if(NOT EXISTS "${SF3_ENGINE_DIR}/src/sf3.hpp")
    message(FATAL_ERROR "sf3-engine not found at: ${SF3_ENGINE_DIR}\n"
        "Set -DSF3_ENGINE_DIR=<path> to your sf3-engine directory.")
endif()

if(PVZ_SF3_SOURCE)
    # ── Source mode: full sf3 source tree, ideal for debugging ──
    message(STATUS "[PVZ] sf3-engine SOURCE mode (full debug)")
    set(SF3_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
    set(SF3_BUILD_TESTS OFF CACHE BOOL "" FORCE)
    add_subdirectory(${SF3_ENGINE_DIR} ${CMAKE_BINARY_DIR}/sf3-engine)
else()
    # ── Prebuilt mode: .a / .lib + headers ──
    message(STATUS "[PVZ] sf3-engine PREBUILT mode")

    # Locate the prebuilt library
    if(WIN32)
        set(_sf3_lib_name "sf3.lib")
    else()
        set(_sf3_lib_name "libsf3.a")
    endif()

    # Search common build dirs
    set(SF3_BUILD_DIR "${SF3_ENGINE_DIR}/build" CACHE PATH "sf3-engine build directory")
    find_file(SF3_LIBRARY ${_sf3_lib_name}
        PATHS "${SF3_BUILD_DIR}" "${SF3_BUILD_DIR}/Release" "${SF3_BUILD_DIR}/Debug"
        NO_DEFAULT_PATH
    )
    if(NOT SF3_LIBRARY)
        message(FATAL_ERROR "Cannot find ${_sf3_lib_name} in ${SF3_BUILD_DIR}.\n"
            "Build sf3-engine first, or use -DPVZ_SF3_SOURCE=ON for source mode.")
    endif()
    message(STATUS "[PVZ] Using library: ${SF3_LIBRARY}")

    # Find SDL3
    find_package(SDL3 QUIET)
    if(NOT SDL3_FOUND)
        message(STATUS "SDL3 not found, fetching...")
        include(FetchContent)
        FetchContent_Declare(SDL3
            GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
            GIT_TAG main GIT_SHALLOW ON)
        set(SDL_SHARED OFF CACHE BOOL "" FORCE)
        set(SDL_STATIC ON CACHE BOOL "" FORCE)
        set(SDL_TEST OFF CACHE BOOL "" FORCE)
        FetchContent_MakeAvailable(SDL3)
        set(SDL3_LIBRARIES SDL3::SDL3-static)
    else()
        set(SDL3_LIBRARIES SDL3::SDL3)
    endif()

    # Create imported target
    add_library(sf3 STATIC IMPORTED)
    set_target_properties(sf3 PROPERTIES
        IMPORTED_LOCATION "${SF3_LIBRARY}"
        INTERFACE_INCLUDE_DIRECTORIES "${SF3_ENGINE_DIR}/src;${SF3_ENGINE_DIR}/third_party"
        INTERFACE_COMPILE_DEFINITIONS "SF3_STATIC"
    )
endif()

# --- PVZ Game Target ---
add_executable(pvz
    src/main.cpp
    src/pvz_systems.cpp
    src/pvz_renderer.cpp
    src/pvz_scenes.cpp
    src/pvz_hud.cpp
    src/pvz_font.cpp
)

target_include_directories(pvz PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)

if(PVZ_SF3_SOURCE)
    target_link_libraries(pvz PRIVATE sf3)
else()
    target_link_libraries(pvz PRIVATE sf3 ${SDL3_LIBRARIES})

    # Platform-specific transitive deps
    if(EMSCRIPTEN)
        # Emscripten: embed font asset, enable OpenGL ES, set memory
        set_target_properties(pvz PROPERTIES SUFFIX ".html")
        target_link_options(pvz PRIVATE
            --preload-file ${CMAKE_CURRENT_SOURCE_DIR}/assets/fonts/pvz_font_subset.ttf@assets/fonts/pvz_font_subset.ttf
            -sUSE_SDL=0
            -sMIN_WEBGL_VERSION=1
            -sMAX_WEBGL_VERSION=2
            -sFULL_ES2=1
            -sALLOW_MEMORY_GROWTH=1
            -sINITIAL_MEMORY=67108864
            -sSTACK_SIZE=1048576
            -sEXPORTED_FUNCTIONS=_main
            -sEXPORTED_RUNTIME_METHODS=ccall,cwrap
            --shell-file ${CMAKE_CURRENT_SOURCE_DIR}/web/shell.html
        )
    elseif(APPLE)
        find_library(COREAUDIO_LIBRARY CoreAudio)
        find_library(AUDIOUNIT_LIBRARY AudioUnit)
        find_library(AUDIOTOOLBOX_LIBRARY AudioToolbox)
        target_link_libraries(pvz PRIVATE
            ${COREAUDIO_LIBRARY} ${AUDIOUNIT_LIBRARY} ${AUDIOTOOLBOX_LIBRARY})
    elseif(WIN32)
        target_link_libraries(pvz PRIVATE winmm ole32 setupapi imm32 version)
    elseif(UNIX)
        # Linux: common deps for SDL3
        target_link_libraries(pvz PRIVATE dl m)
    endif()

    if(NOT EMSCRIPTEN)
        find_package(Threads REQUIRED)
        target_link_libraries(pvz PRIVATE Threads::Threads)
    endif()
endif()
