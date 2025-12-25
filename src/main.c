#include <stdio.h>
#include "raylib.h"
#define CLAY_IMPLEMENTATION
#include "clay.h"
#include "../raylib/clay_renderer_raylib.c"
#include "arena.h"
#include "assert.h"

#include "camera.h"
#include "systems/game_system.h"

// Keep these here as bootstrap constants. GameSystem owns their usage.
#define TILE_SIZE 16
#define MAP_SIZE 128

const Clay_Color COLOR_LIGHT = (Clay_Color){224, 215, 210, 255};
const Clay_Color COLOR_RED = (Clay_Color){168, 66, 28, 255};
const Clay_Color COLOR_ORANGE = (Clay_Color){255, 138, 50, 255};
const Clay_Color COLOR_WHITE = (Clay_Color){ .r = 255, .g = 255, .b = 255};
const Clay_Color COLOR_DARK = (Clay_Color){100, 100, 100, 255};

const float ScreenWidth = 1600.0f;
const float ScreenHeight = 900.0f;

void clay_assertion_callback(Clay_ErrorData errorText) {
    // assert(errorText.userData); // Or handle errorText as needed
}

// Example measure text function
// static inline Clay_Dimensions MeasureText(Clay_StringSlice text, Clay_TextElementConfig *config, uintptr_t userData) {
//     // Clay_TextElementConfig contains members such as fontId, fontSize, letterSpacing etc
//     // Note: Clay_String->chars is not guaranteed to be null terminated
//     return (Clay_Dimensions) {
//             .width = text.length * config->fontSize, // <- this will only work for monospace fonts, see the renderers/ directory for more advanced text measurement
//             .height = config->fontSize
//     };
// }

int main(void) {
    /*
    =======================Library Initialization======================
    */
    
    // Initialize Raylib window
    Clay_Raylib_Initialize((int)ScreenWidth, (int)ScreenHeight, "Clay with Raylib Example", FLAG_BORDERLESS_WINDOWED_MODE | FLAG_VSYNC_HINT | FLAG_MSAA_4X_HINT | FLAG_WINDOW_RESIZABLE);
    // SetExitKey(KEY_NULL);

    //Clay initialization
    uint64_t clayMemorySize = Clay_MinMemorySize();
    Clay_Arena memoryArena = {
        .memory = malloc(clayMemorySize),
        .capacity = clayMemorySize,
    };
    Clay_Dimensions  dimentions = {
        .width = ScreenWidth,
        .height = ScreenHeight,
    };
    Clay_ErrorHandler errorHandler = {
        .errorHandlerFunction = clay_assertion_callback,
        .userData = NULL
    };
    Clay_Initialize(memoryArena, dimentions, errorHandler);

    Arena_T arena = Arena_new();

    SetTargetFPS(60);
    GameSystem* game = GameSystem_create(arena, MAP_SIZE, TILE_SIZE, (Vector2){ ScreenWidth, ScreenHeight });

    while (!WindowShouldClose()) {
        SetTraceLogLevel(LOG_DEBUG);
        float dt = GetFrameTime();
        BeginDrawing();
        GameSystem_frame(game, dt);
        EndDrawing();
    }

    GameSystem_destroy(game);

    return 0;
}