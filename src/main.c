#include <stdio.h>
#include "../raylib/raylib.h"
#define CLAY_IMPLEMENTATION
#include "clay.h"
#include "../raylib/clay_renderer_raylib.c"
#include "assert.h"



const Clay_Color COLOR_LIGHT = (Clay_Color){224, 215, 210, 255};
const Clay_Color COLOR_RED = (Clay_Color){168, 66, 28, 255};
const Clay_Color COLOR_ORANGE = (Clay_Color){255, 138, 50, 255};
const Clay_Color COLOR_WHITE = (Clay_Color){ .r = 255, .g = 255, .b = 255};
const Clay_Color COLOR_DARK = (Clay_Color){100, 100, 100, 255};

const float ScreenWidth = 1600.0f;
const float ScreenHeight = 900.0f;

void clay_assertion_callback(Clay_ErrorData errorText) {
    assert(errorText.userData); // Or handle errorText as needed
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

Clay_ElementDeclaration sidebarItemConfig = (Clay_ElementDeclaration) {
    .layout = {
        .sizing = { .width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_FIXED(50) }
    },
    .backgroundColor = COLOR_ORANGE
};

void SidebarItemComponent() {
    CLAY(sidebarItemConfig) {
        // children go here...
    }
}





int main(void) {

    // Core initialization


    //Raylib initialization
    Clay_Raylib_Initialize((int)ScreenWidth, (int)ScreenHeight, "Clay with Raylib Example", FLAG_BORDERLESS_WINDOWED_MODE | FLAG_VSYNC_HINT | FLAG_MSAA_4X_HINT | FLAG_WINDOW_RESIZABLE);
    SetExitKey(KEY_NULL);
    
    bool isMenuOpen = false;
    KeyboardKey key;
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

    SetTargetFPS(60);

    //Main game loop
        while (!WindowShouldClose())    // Detect window close button or ESC key
        {
        //Window events
        if (IsWindowResized()) {
            Clay_SetLayoutDimensions((Clay_Dimensions) { .width = (float)GetScreenWidth(), .height = (float)GetScreenHeight() });
        }
        // Clay_SetLayoutDimensions((Clay_Dimensions) { .width = (float)GetScreenWidth(), .height = ScreenHeight });

        // Context switch between menu and game
        if (isMenuOpen) {
            // Menu is open, handle menu-specific logic

            // Input
            key = GetKeyPressed();
            switch (key)
            {
            case KEY_M:
                isMenuOpen = false;
                break;
            
            default:
                break;
            }

            // Update

            // Render

            Clay_BeginLayout();
            CLAY({ .id = CLAY_ID("OuterContainer"), .layout = { .sizing = {CLAY_SIZING_GROW(0), CLAY_SIZING_GROW(0)}, .padding = CLAY_PADDING_ALL(16), .childGap = 16 }, .backgroundColor = {250,250,255,255} }) {
            CLAY({ .id = CLAY_ID("Header"), .layout = { .sizing = {CLAY_SIZING_GROW(0), CLAY_SIZING_FIXED(50)} }, .backgroundColor = COLOR_DARK }) {
                //    CLAY({ .id = CLAY_ID("HeaderText"), .layout = { .sizing = {CLAY_SIZING_GROW(0), CLAY_SIZING_GROW(0)} }, .text = "Hello, World!", .textConfig = { .fontSize = 32, .color = COLOR_WHITE, .horizontalAlign = CLAY_TEXT_ALIGN_CENTER, .verticalAlign = CLAY_TEXT_ALIGN_CENTER } }) {}
            }
            }
            Clay_RenderCommandArray renderCommands = Clay_EndLayout();

            BeginDrawing();
                ClearBackground(YELLOW);
                Clay_Raylib_Render(renderCommands, FONT_DEFAULT);
            EndDrawing();
        } else {
            // Game is active, handle game-specific logic

            // Input
            key = GetKeyPressed();
            switch (key)
            {
            case KEY_M:
                isMenuOpen = !isMenuOpen;
                break;
            default:
                break;
            }

            // Update

            // Render
            BeginDrawing();
                ClearBackground(RAYWHITE);
                DrawText("Press M to open the menu", 10, 10, 20, DARKGRAY);
            EndDrawing();
        }

        // //Input
        // key = GetKeyPressed();
        // if(key == KEY_M && isMenuOpen) {
        //     isMenuOpen = false;
        // }
        // else if(key == KEY_M && !isMenuOpen) {
        //     isMenuOpen = true;
        // }

        // //Update

        //Render

        

        // BeginDrawing();
        // ClearBackground(YELLOW);
        // if(isMenuOpen) {
        //     Clay_Raylib_Render(renderCommands, FONT_DEFAULT);
        // }
        // else {
        //     DrawText("Press M to open the menu", 10, 10, 20, DARKGRAY);
        //     if(IsKeyPressed(KEY_M)) {
        //         isMenuOpen = true;
        //     }
        // }

        // EndDrawing();
    }

    return 0;
}