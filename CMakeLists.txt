cmake_minimum_required(VERSION 3.27)
project(rayui C)
set(CMAKE_C_STANDARD 99)

# WebAssembly toggle (must be before fetching raylib)
option(BUILD_WEB "Build with Emscripten (WebAssembly)" OFF)

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

file(GLOB SRC_FILES "src/*.c")
file(GLOB CORE_FILES "src/core/*.c")
file(GLOB HASH_FILES "src/core/hash/*.c")
file(GLOB COMPONENT_FILES "src/components/*.c")
file(GLOB SYSTEM_FILES "src/systems/*.c")
file(GLOB SCREEN_FILES "src/systems/screens/*.c")
# UI files
file(GLOB UI_FILES "src/ui/*.c")
file(GLOB UI_SCREEN_FILES "src/ui/screens/*.c")
file(GLOB UI_ELEMENT_FILES "src/ui/elements/*.c")
file(GLOB UI_SHARED_FILES "src/ui/shared/*.c")
file(GLOB UI_LAYOUT_FILES "src/ui/layout/*.c")

# Filter platform specific files
if(WIN32)
    file(GLOB CORE_PLATFORM_FILES "src/core/platform/win/*.c")
else()
    file(GLOB CORE_PLATFORM_FILES "src/core/platform/unix/*.c")
endif()

# Don't compile platform-specific implementations from the root `src/core/` glob.
# We compile exactly one platform backend from `src/core/platform/*` instead.
list(FILTER CORE_FILES EXCLUDE REGEX ".*/thread-nt\\.c$")
list(FILTER CORE_FILES EXCLUDE REGEX ".*/thread\\.c$")

# CII ships some libc replacement/compat files that clash with or depend on
# legacy glibc internals (e.g. sys_errlist/sys_nerr). Prefer the platform libc.
list(FILTER CORE_FILES EXCLUDE REGEX ".*/strerror\\.c$")
list(FILTER CORE_FILES EXCLUDE REGEX ".*/strncmp\\.c$")
list(FILTER CORE_FILES EXCLUDE REGEX ".*/memcmp\\.c$")
list(FILTER CORE_FILES EXCLUDE REGEX ".*/memmove\\.c$")

# TODO Reorganize this for segmenting by target executable
message(STATUS "SRC_FILES: ${SRC_FILES} 
                CORE_FILES: ${CORE_FILES}
                CORE_PLATFORM_FILES: ${CORE_PLATFORM_FILES}
                HASH_FILES: ${HASH_FILES}
                COMPONENT_FILES: ${COMPONENT_FILES} 
                SYSTEM_FILES: ${SYSTEM_FILES} 
                SCREEN_FILES: ${SCREEN_FILES}
                UI_FILES: ${UI_FILES}
                UI_ELEMENT_FILES: ${UI_ELEMENT_FILES}
                UI_SHARED_FILES: ${UI_SHARED_FILES}
                UI_LAYOUT_FILES: ${UI_LAYOUT_FILES}"
)
add_executable(game ${SRC_FILES} 
                           ${CORE_FILES}
                           ${CORE_PLATFORM_FILES}
                           ${HASH_FILES}
                           ${COMPONENT_FILES} 
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
        # Uncomment to package assets into the .data file:
    --preload-file ${CMAKE_SOURCE_DIR}/resources@resources
        # Optional: use Emscripten default shell page, or supply raylib's custom shell:
        # --shell-file ${raylib_SOURCE_DIR}/src/shell.html
    )
endif()

# Core Include
# target_include_directories(core PRIVATE ${CMAKE_SOURCE_DIR}/include
#                                                            ./include/core)

# Game Include
target_compile_options(game PUBLIC)
target_link_libraries(game PUBLIC raylib sxml Threads::Threads)
target_include_directories(game PUBLIC 
                                ./include 
                                ./include/core
                                ./include/core/platform
                                ./include/core/hash
                                ./include/components 
                                ./include/systems 
                                ./include/systems/screens
                                ./include/ui
                                ./include/ui/elements
                                ./include/ui/shared
                                ./include/ui/layout)

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