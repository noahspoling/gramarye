#include <stdio.h>
#include <stdlib.h>
#include "raylib.h"
#include "arena.h"
#include "assert.h"

#include "camera.h"
#include "systems/game_system.h"
#include "gramarye_renderer/renderer.h"
#include "renderer_raylib.h"
#include "input_raylib.h"
#include "gramarye_ui/ui_provider.h"
#include "ui_provider_raylib.h"

#define TILE_SIZE 16
#define MAP_SIZE 128

const float ScreenWidth = 1600.0f;
const float ScreenHeight = 900.0f;

int main(void) {
    Renderer* renderer = RendererRaylib_create();
    if (!renderer) {
        fprintf(stderr, "Failed to create renderer\n");
        return 1;
    }
    
    Renderer_init(renderer, (int)ScreenWidth, (int)ScreenHeight, "Gramarye Game",
                  Renderer_get_default_window_flags() | WINDOW_FLAG_BORDERLESS);

    SetTraceLogLevel(LOG_DEBUG);
    
    UIProvider* uiProvider = UIProviderRaylib_create();
    if (!uiProvider) {
        fprintf(stderr, "Failed to create UI provider\n");
        Renderer_close(renderer);
        RendererRaylib_destroy(renderer);
        return 1;
    }
    
    size_t uiMemorySize = UIProvider_get_memory_size();
    void* uiMemory = malloc(uiMemorySize);
    if (!uiMemory) {
        fprintf(stderr, "Failed to allocate UI memory\n");
        UIProviderRaylib_destroy(uiProvider);
        Renderer_close(renderer);
        RendererRaylib_destroy(renderer);
        return 1;
    }
    
    int renderWidth = Renderer_get_render_width(renderer);
    int renderHeight = Renderer_get_render_height(renderer);
    UIDimensions dimensions = {
        .width = (float)renderWidth,
        .height = (float)renderHeight,
    };
    UIErrorHandler errorHandler = {
        .errorHandlerFunction = NULL,
        .userData = NULL
    };
    UIProvider_initialize(uiProvider, uiMemory, uiMemorySize, dimensions, &errorHandler);

    Arena_T arena = Arena_new();

    InputProvider* inputProvider = InputProviderRaylib_create();
    if (!inputProvider) {
        fprintf(stderr, "Failed to create input provider\n");
        Renderer_close(renderer);
        RendererRaylib_destroy(renderer);
        return 1;
    }

    RenderVector2 windowSize = Renderer_get_window_size(renderer);
    GameSystem* game = GameSystem_create(arena, MAP_SIZE, TILE_SIZE, (Vector2){ windowSize.x, windowSize.y }, renderer, inputProvider, uiProvider);

    while (!Renderer_should_close(renderer)) {
        float dt = Renderer_get_delta_time(renderer);
        
        Renderer_begin_frame(renderer);
        
        RenderCommand clearCmd = {0};
        clearCmd.type = RENDER_COMMAND_TYPE_RECTANGLE;
        clearCmd.bounds = (RenderRect){0, 0, windowSize.x, windowSize.y};
        clearCmd.color = (RenderColor){255, 0, 0, 255};
        Renderer_execute_command(renderer, &clearCmd);
        
        GameSystem_frame(game, dt);
        TraceLog(LOG_DEBUG, "GameSystem_frame: dt = %f", dt);
        
        Renderer_end_frame(renderer);
    }

    GameSystem_destroy(game);
    InputProviderRaylib_destroy(inputProvider);
    UIProvider_shutdown(uiProvider);
    UIProviderRaylib_destroy(uiProvider);
    Renderer_close(renderer);
    RendererRaylib_destroy(renderer);

    return 0;
}