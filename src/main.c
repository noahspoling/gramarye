#include <stdio.h>
#include <stdlib.h>
#include "raylib.h"  // Still needed for Vector2 type used by GameSystem
#define CLAY_IMPLEMENTATION
#include "clay.h"
#include "clay_renderer_raylib.h"  // Now from raylib-render-implementation
#include "arena.h"
#include "assert.h"

#include "camera.h"
#include "systems/game_system.h"
#include "renderer/renderer.h"
#include "renderer_raylib.h"

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
    
    // Create renderer (backend-agnostic)
    Renderer* renderer = RendererRaylib_create();
    if (!renderer) {
        fprintf(stderr, "Failed to create renderer\n");
        return 1;
    }
    
    // Initialize renderer window
    Renderer_init(renderer, (int)ScreenWidth, (int)ScreenHeight, "Gramarye Game"
                //, 0
                , FLAG_BORDERLESS_WINDOWED_MODE | FLAG_VSYNC_HINT | FLAG_MSAA_4X_HINT | FLAG_WINDOW_RESIZABLE
            ); // FLAG_BORDERLESS_WINDOWED_MODE | FLAG_VSYNC_HINT | FLAG_MSAA_4X_HINT | FLAG_WINDOW_RESIZABLE

    SetTraceLogLevel(LOG_DEBUG);
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

    // Get window size from renderer for game system
    RenderVector2 windowSize = Renderer_get_window_size(renderer);
    GameSystem* game = GameSystem_create(arena, MAP_SIZE, TILE_SIZE, (Vector2){ windowSize.x, windowSize.y });

    while (!Renderer_should_close(renderer)) {
        float dt = Renderer_get_delta_time(renderer);
        
        Renderer_begin_frame(renderer);
        
        // Clear background using rectangle (full screen clear)
        RenderCommand clearCmd = {0};
        clearCmd.type = RENDER_COMMAND_TYPE_RECTANGLE;
        clearCmd.bounds = (RenderRect){0, 0, windowSize.x, windowSize.y};
        clearCmd.color = (RenderColor){255, 0, 0, 255}; // RED background
        Renderer_execute_command(renderer, &clearCmd);
        
        GameSystem_frame(game, dt);
        TraceLog(LOG_DEBUG, "GameSystem_frame: dt = %f", dt);
        
        Renderer_end_frame(renderer);
    }

    GameSystem_destroy(game);
    Renderer_close(renderer);
    RendererRaylib_destroy(renderer);

    return 0;
}