cmake_minimum_required(VERSION 3.27)
project(rayui C)
set(CMAKE_C_STANDARD 99)

# Generate compile_commands.json for IntelliSense
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# WebAssembly toggle (must be before fetching raylib)
option(BUILD_WEB "Build with Emscripten (WebAssembly)" OFF)

# Local module development toggle
option(USE_LOCAL_MODULES "Use local module folders instead of FetchContent" OFF)

# When building for web (via -DBUILD_WEB=ON or emcmake), set platform and output suffix
if(EMSCRIPTEN OR BUILD_WEB)
    # Ensure raylib configures for Web before it's fetched/built
    set(PLATFORM Web CACHE STRING "" FORCE)
    # Make final output an .html launcher
    set(CMAKE_EXECUTABLE_SUFFIX ".html")
endif()

# Adding Raylib
include(FetchContent)
set(FETCHCONTENT_QUIET FALSE)
set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE) # don't build the supplied examples
set(BUILD_GAMES    OFF CACHE BOOL "" FORCE) # don't build the supplied example games
set(BUILD_TESTING  OFF CACHE BOOL "" FORCE) # don't build tests for dependencies (especially for web builds)

FetchContent_Declare(
    raylib
    GIT_REPOSITORY "https://github.com/raysan5/raylib.git"
    GIT_TAG "5.5"
    GIT_PROGRESS TRUE
    GIT_SHALLOW TRUE
)

FetchContent_MakeAvailable(raylib)

# Threads (pthreads on Linux/macOS)
find_package(Threads REQUIRED)

FetchContent_Declare(
    sxml
    GIT_REPOSITORY "https://github.com/capmar/sxml.git"
    GIT_TAG "master"
    GIT_PROGRESS TRUE
    GIT_SHALLOW TRUE
)

FetchContent_GetProperties(sxml)
if(NOT sxml_POPULATED)
    FetchContent_Populate(sxml)
    add_library(sxml STATIC ${sxml_SOURCE_DIR}/sxml.c)
    target_include_directories(sxml PUBLIC ${sxml_SOURCE_DIR})
endif()

# Fetch or use local gramarye-libcore library
if(USE_LOCAL_MODULES AND EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/../gramarye-libcore/CMakeLists.txt")
    message(STATUS "Using local gramarye-libcore")
    # Pass USE_LOCAL_MODULES to subdirectories so they don't fetch dependencies
    set(USE_LOCAL_MODULES ${USE_LOCAL_MODULES} CACHE BOOL "Use local module folders" FORCE)
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../gramarye-libcore gramarye-libcore)
else()
    FetchContent_Declare(
        gramarye_libcore
        GIT_REPOSITORY "https://github.com/noahspoling/gramarye-libcore.git"
        GIT_TAG "main"
        GIT_PROGRESS TRUE
        GIT_SHALLOW TRUE
    )
    FetchContent_MakeAvailable(gramarye_libcore)
endif()

# Add include directories only (no link dependency) to avoid CMake export issues
FetchContent_GetProperties(raylib)
if(TARGET gramarye-libcore AND raylib_POPULATED)
    # Raylib headers are in the src directory
    target_include_directories(gramarye-libcore PRIVATE ${raylib_SOURCE_DIR}/src)
endif()

# Fetch or use local gramarye-ecs library
if(USE_LOCAL_MODULES AND EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/../gramarye-ecs/CMakeLists.txt")
    message(STATUS "Using local gramarye-ecs")
    # Pass USE_LOCAL_MODULES to subdirectories
    set(USE_LOCAL_MODULES ${USE_LOCAL_MODULES} CACHE BOOL "Use local module folders" FORCE)
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../gramarye-ecs gramarye-ecs)
else()
    FetchContent_Declare(
        gramarye_ecs
        GIT_REPOSITORY "https://github.com/noahspoling/gramarye-ecs.git"
        GIT_TAG "main"
        GIT_PROGRESS TRUE
        GIT_SHALLOW TRUE
    )
    # Disable tests for web builds (they use GCC extensions incompatible with Emscripten)
    if(EMSCRIPTEN OR BUILD_WEB)
        set(BUILD_TESTING OFF CACHE BOOL "" FORCE)
    endif()
    FetchContent_MakeAvailable(gramarye_ecs)
endif()

# Exclude test targets from build (they use GCC extensions incompatible with Emscripten)
# This is especially important for web builds where nested functions don't work
if(TARGET ecs_tests AND (EMSCRIPTEN OR BUILD_WEB))
    set_target_properties(ecs_tests PROPERTIES EXCLUDE_FROM_ALL TRUE)
endif()

# Fetch or use local gramarye-query-engine library
if(USE_LOCAL_MODULES AND EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/../gramarye-query-engine/CMakeLists.txt")
    message(STATUS "Using local gramarye-query-engine")
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../gramarye-query-engine gramarye-query-engine)
else()
    FetchContent_Declare(
        gramarye_query_engine
        GIT_REPOSITORY "https://github.com/noahspoling/gramarye-query-engine.git"
        GIT_TAG "main"
        GIT_PROGRESS TRUE
        GIT_SHALLOW TRUE
    )
    # Ensure tests are disabled for web builds
    if(EMSCRIPTEN OR BUILD_WEB)
        set(BUILD_TESTING OFF CACHE BOOL "" FORCE)
    endif()
    FetchContent_MakeAvailable(gramarye_query_engine)
endif()

# Exclude test targets from build (they may have compatibility issues with Emscripten)
if(EMSCRIPTEN OR BUILD_WEB)
    if(TARGET query_tests)
        set_target_properties(query_tests PROPERTIES EXCLUDE_FROM_ALL TRUE)
    endif()
    if(TARGET test_shell)
        set_target_properties(test_shell PROPERTIES EXCLUDE_FROM_ALL TRUE)
    endif()
endif()

# Fetch or use local gramarye-components library
if(USE_LOCAL_MODULES AND EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/../gramarye-components/CMakeLists.txt")
    message(STATUS "Using local gramarye-components")
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../gramarye-components gramarye-components)
else()
    FetchContent_Declare(
        gramarye_components
        GIT_REPOSITORY "https://github.com/noahspoling/gramarye-components.git"
        GIT_TAG "main"
        GIT_PROGRESS TRUE
        GIT_SHALLOW TRUE
    )
    FetchContent_MakeAvailable(gramarye_components)
endif()

# Fetch or use local gramarye-component-functions library
if(USE_LOCAL_MODULES AND EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/../gramarye-component-functions/CMakeLists.txt")
    message(STATUS "Using local gramarye-component-functions")
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../gramarye-component-functions gramarye-component-functions)
else()
    FetchContent_Declare(
        gramarye_component_functions
        GIT_REPOSITORY "https://github.com/noahspoling/gramarye-component-functions.git"
        GIT_TAG "main"
        GIT_PROGRESS TRUE
        GIT_SHALLOW TRUE
    )
    FetchContent_MakeAvailable(gramarye_component_functions)
endif()

if(USE_LOCAL_MODULES AND EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/../gramarye-renderer-interface/CMakeLists.txt")
    message(STATUS "Using local gramarye-renderer-interface")
    # Pass USE_LOCAL_MODULES to subdirectories
    set(USE_LOCAL_MODULES ${USE_LOCAL_MODULES} CACHE BOOL "Use local module folders" FORCE)
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../gramarye-renderer-interface gramarye-renderer-interface)
else()
    FetchContent_Declare(
        gramarye_renderer_interface
        GIT_REPOSITORY "https://github.com/noahspoling/gramarye-renderer-interface.git"
        GIT_TAG "master"
        GIT_PROGRESS TRUE
        GIT_SHALLOW TRUE
    )
    FetchContent_MakeAvailable(gramarye_renderer_interface)
endif()

# Fetch or use local gramarye-renderer-raylib library
if(USE_LOCAL_MODULES AND EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/../gramarye-raylib-implementation/CMakeLists.txt")
    message(STATUS "Using local gramarye-raylib-implementation")
    # Pass USE_LOCAL_MODULES to subdirectories
    set(USE_LOCAL_MODULES ${USE_LOCAL_MODULES} CACHE BOOL "Use local module folders" FORCE)
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../gramarye-raylib-implementation gramarye-raylib-impl)
else()
    FetchContent_Declare(
        gramarye_renderer_raylib
        GIT_REPOSITORY "https://github.com/noahspoling/gramarye-raylib-implementation.git"
        GIT_TAG "main"
        GIT_PROGRESS TRUE
        GIT_SHALLOW TRUE
    )
    FetchContent_MakeAvailable(gramarye_renderer_raylib)
endif()

# Fetch or use local gramarye-chunk-renderer library
if(USE_LOCAL_MODULES AND EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/../gramarye-chunk-renderer/CMakeLists.txt")
    message(STATUS "Using local gramarye-chunk-renderer")
    # Pass USE_LOCAL_MODULES to subdirectories
    set(USE_LOCAL_MODULES ${USE_LOCAL_MODULES} CACHE BOOL "Use local module folders" FORCE)
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../gramarye-chunk-renderer gramarye-chunk-renderer)
else()
    FetchContent_Declare(
        gramarye_chunk_renderer
        GIT_REPOSITORY "https://github.com/noahspoling/gramarye-chunk-renderer.git"
        GIT_TAG "master"
        GIT_PROGRESS TRUE
        GIT_SHALLOW TRUE
    )
    FetchContent_MakeAvailable(gramarye_chunk_renderer)
endif()

# Fetch or use local gramarye-event-bus library
if(USE_LOCAL_MODULES AND EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/../gramarye-event-bus/CMakeLists.txt")
    message(STATUS "Using local gramarye-event-bus")
    # Pass USE_LOCAL_MODULES to subdirectories
    set(USE_LOCAL_MODULES ${USE_LOCAL_MODULES} CACHE BOOL "Use local module folders" FORCE)
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../gramarye-event-bus gramarye-event-bus)
else()
    FetchContent_Declare(
        gramarye_event_bus
        GIT_REPOSITORY "https://github.com/noahspoling/gramarye-event-bus.git"
        GIT_TAG "main"
        GIT_PROGRESS TRUE
        GIT_SHALLOW TRUE
    )
    FetchContent_MakeAvailable(gramarye_event_bus)
endif()

# Fetch or use local gramarye-chunk-controller library
if(USE_LOCAL_MODULES AND EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/../gramarye-chunk-controller/CMakeLists.txt")
    message(STATUS "Using local gramarye-chunk-controller")
    # Pass USE_LOCAL_MODULES to subdirectories
    set(USE_LOCAL_MODULES ${USE_LOCAL_MODULES} CACHE BOOL "Use local module folders" FORCE)
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../gramarye-chunk-controller gramarye-chunk-controller)
else()
    FetchContent_Declare(
        gramarye_chunk_controller
        GIT_REPOSITORY "https://github.com/noahspoling/gramarye-chunk-controller.git"
        GIT_TAG "main"
        GIT_PROGRESS TRUE
        GIT_SHALLOW TRUE
    )
    FetchContent_MakeAvailable(gramarye_chunk_controller)
endif()

# Configure renderer-raylib after it's created
if(TARGET gramarye-renderer-raylib)
    target_link_libraries(gramarye-renderer-raylib PUBLIC raylib)
    # Make raylib headers available to renderer-raylib
    FetchContent_GetProperties(raylib)
    if(raylib_POPULATED)
        target_include_directories(gramarye-renderer-raylib PRIVATE ${raylib_SOURCE_DIR}/src)
    endif()
    # Make renderer headers available to renderer-raylib (from gramarye/include/renderer)
    # Also make clay headers available (clay.h is in gramarye/include)
    target_include_directories(gramarye-renderer-raylib PUBLIC 
        ${CMAKE_CURRENT_SOURCE_DIR}/include)
endif()

file(GLOB SRC_FILES "src/*.c")
file(GLOB RENDERER_FILES "src/renderer/*.c")
file(GLOB INPUT_FILES "src/input/*.c")
# Note: CORE_FILES, HASH_FILES, and CORE_PLATFORM_FILES are now provided by xmos-libcore
# Component files are now in gramarye-components and gramarye-component-functions
# Chunk render system is now in gramarye-chunk-renderer library
file(GLOB SYSTEM_FILES "src/systems/*.c")
# Remove chunk_render_system.c from SYSTEM_FILES (it's now in gramarye-chunk-renderer)
list(REMOVE_ITEM SYSTEM_FILES "${CMAKE_CURRENT_SOURCE_DIR}/src/systems/chunk_render_system.c")
file(GLOB SCREEN_FILES "src/systems/screens/*.c")
# UI files
file(GLOB UI_FILES "src/ui/*.c")
file(GLOB UI_SCREEN_FILES "src/ui/screens/*.c")
file(GLOB UI_ELEMENT_FILES "src/ui/elements/*.c")
file(GLOB UI_SHARED_FILES "src/ui/shared/*.c")
file(GLOB UI_LAYOUT_FILES "src/ui/layout/*.c")

# TODO Reorganize this for segmenting by target executable
message(STATUS "SRC_FILES: ${SRC_FILES} 
                SYSTEM_FILES: ${SYSTEM_FILES} 
                SCREEN_FILES: ${SCREEN_FILES}
                UI_FILES: ${UI_FILES}
                UI_ELEMENT_FILES: ${UI_ELEMENT_FILES}
                UI_SHARED_FILES: ${UI_SHARED_FILES}
                UI_LAYOUT_FILES: ${UI_LAYOUT_FILES}"
)
add_executable(game        ${SRC_FILES} 
                           ${RENDERER_FILES}
                           ${INPUT_FILES}
                           ${SYSTEM_FILES} 
                           ${SCREEN_FILES}
                           ${UI_FILES}
                           ${UI_ELEMENT_FILES}
                           ${UI_SHARED_FILES}
                           ${UI_LAYOUT_FILES}
)

# Web-specific compile/link options for your app
if(EMSCRIPTEN OR BUILD_WEB)
    target_compile_definitions(game PUBLIC PLATFORM_WEB)
    
    target_link_options(game PRIVATE
        -sASYNCIFY
        -sALLOW_MEMORY_GROWTH=1
        -sFORCE_FILESYSTEM=1
        # Package assets into the .data file:
        --preload-file ${CMAKE_SOURCE_DIR}/resources@resources
        # Use custom shell.html which constrains the game to a centered, contained box
        --shell-file ${CMAKE_SOURCE_DIR}/shell.html
    )
endif()

# Core Include
# target_include_directories(core PRIVATE ${CMAKE_SOURCE_DIR}/include
#                                                            ./include/core)

# Game Include
target_compile_options(game PUBLIC)

# Link raylib to component-functions (needed for Atlas, Animation, etc.)
if(TARGET gramarye-component-functions)
    target_link_libraries(gramarye-component-functions PUBLIC raylib)
    # Make raylib headers available to component-functions
    FetchContent_GetProperties(raylib)
    if(raylib_POPULATED)
        target_include_directories(gramarye-component-functions PRIVATE ${raylib_SOURCE_DIR}/src)
    endif()
endif()

# Configure chunk-renderer after it's created
if(TARGET gramarye-chunk-renderer)
    target_link_libraries(gramarye-chunk-renderer PUBLIC raylib)
    # Make raylib headers available to chunk-renderer
    FetchContent_GetProperties(raylib)
    if(raylib_POPULATED)
        target_include_directories(gramarye-chunk-renderer PRIVATE ${raylib_SOURCE_DIR}/src)
    endif()
    # Make chunk-controller headers available to chunk-renderer (optional dependency)
    if(TARGET gramarye-chunk-controller)
        target_include_directories(gramarye-chunk-renderer PRIVATE
            $<TARGET_PROPERTY:gramarye-chunk-controller,INTERFACE_INCLUDE_DIRECTORIES>
        )
    endif()
endif()

target_link_libraries(game PUBLIC 
    raylib 
    sxml 
    Threads::Threads
    gramarye-libcore  # Link against gramarye-libcore library
    gramarye-ecs
    gramarye-query-engine
    gramarye-renderer-interface  # Renderer interface library
    gramarye-component-functions  # This transitively includes gramarye-components
    gramarye-renderer-raylib  # Raylib renderer implementation
    gramarye-event-bus  # Event bus system
    gramarye-chunk-controller  # Chunk update/management system
    gramarye-chunk-renderer  # Chunk render system
    # Note: gramarye-components is linked via gramarye-component-functions
    # to ensure correct include path order (component-functions headers come first)
)
target_include_directories(game PUBLIC 
                                ./include 
                                # Core includes come from gramarye-libcore library
                                # Component includes come from gramarye-components and gramarye-component-functions
                                ./include/systems 
                                ./include/systems/screens
                                ./include/ui
                                ./include/ui/elements
                                ./include/ui/shared
                                ./include/ui/layout
                                ./include/renderer)

if(MSVC)
  set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG}")
else()
  set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG}")
  set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE}")
endif()

add_custom_command(
        TARGET game POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_SOURCE_DIR}/resources
        ${CMAKE_CURRENT_BINARY_DIR}/resources)