#include <stdio.h>
#include "raylib.h"
#define CLAY_IMPLEMENTATION
#include "clay.h"
#include "../raylib/clay_renderer_raylib.c"
#include "arena.h"
#include "assert.h"
#include "array.h"
#include "int_coord_hash.h"
#include "tilemap.h"
#include "atlas.h"
#include "atlas_table.h"
// #include "ui/elements/button.h"

#include "systems/kcomb_image.h"
#include "camera.h"

#define TILE_SIZE 16
#define MAP_SIZE 128

const Clay_Color COLOR_LIGHT = (Clay_Color){224, 215, 210, 255};
const Clay_Color COLOR_RED = (Clay_Color){168, 66, 28, 255};
const Clay_Color COLOR_ORANGE = (Clay_Color){255, 138, 50, 255};
const Clay_Color COLOR_WHITE = (Clay_Color){ .r = 255, .g = 255, .b = 255};
const Clay_Color COLOR_DARK = (Clay_Color){100, 100, 100, 255};

const float ScreenWidth = 1600.0f;
const float ScreenHeight = 900.0f;

typedef struct {
    int x, y;
} Player;

bool isDebug = false;

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

    // Texture2D tileset = LoadTexture("resources/spritesheet-export.png");

    /*
    =======================Texture Loading======================
    */

    AtlasTable atlasTable = AtlasTable_new();
    AtlasTable_add(&atlasTable, "ground", Atlas_new(400));
    Atlas* atlas = AtlasTable_get(&atlasTable, "ground");
    Atlas_setTexture(atlas, "resources/spritesheet-export.png");
    for(int i = 0; i < 9; i++) {
        for(int j = 0; j < 1; j++) {
            Rectangle rect = {i * 16, j * 16, 16, 16};
            Atlas_addRect(atlas, i + j * 18, rect);
        }
    }

    /*
    ======================Tilemap Initialization======================
    */

    TraceLog(LOG_INFO, "Initializing arena and tilemap");
    Arena_T arena = Arena_new();
    Tilemap* tilemap = Tilemap_new(MAP_SIZE, MAP_SIZE, arena);

    for (int y = 0; y < MAP_SIZE; y++) {
        for (int x = 0; x < MAP_SIZE; x++) {
            int rand = GetRandomValue(0, 4);
            Tilemap_set_tile(tilemap, x, y, rand);
        }
    }
    RenderTexture2D tilemapRenderTarget = LoadRenderTexture(MAP_SIZE * TILE_SIZE, MAP_SIZE * TILE_SIZE);
    SetTextureFilter(tilemapRenderTarget.texture, TEXTURE_FILTER_POINT);
    Texture2D tilemapTexture = tilemapRenderTarget.texture;

    for (int y = 0; y < MAP_SIZE; y++) {
        for (int x = 0; x < MAP_SIZE; x++) {
            Tile* tile = Tilemap_get_tile(tilemap, x, y);
            if (tile) {
                Rectangle sourceRect = Atlas_getRect(atlas, tile->tile_id);
                Rectangle destRect = { x * TILE_SIZE, y * TILE_SIZE, TILE_SIZE, TILE_SIZE };
                BeginTextureMode(tilemapRenderTarget);
                DrawTextureRec(atlas->texture, sourceRect, (Vector2){ destRect.x, destRect.y }, WHITE);
                EndTextureMode();
            }
        }
    }
    KeyboardKey key;

    SetTargetFPS(60);

    // Camera setup
    Camera2DEx cam;
    // Camera_Init(&cam, (Vector2){ MAP_SIZE * TILE_SIZE, MAP_SIZE * TILE_SIZE });
    Camera_Init(&cam, (Vector2){ ScreenWidth, ScreenHeight });
    int frame = 0;
    //Main game loop
    while (!WindowShouldClose())    // Detect window close button
    {
        frame++;
        BeginDrawing();
        ClearBackground(YELLOW);
        Clay_BeginLayout();

        // Update camera inputs
        Camera_UpdateInputs(&cam, GetFrameTime());

        if(IsKeyPressed(KEY_F11)) {
            if (isDebug) {
                TraceLog(LOG_INFO, "Disabling debug mode");
                isDebug = false;
            } else {
                TraceLog(LOG_INFO, "Enabling debug mode");
                isDebug = true;
            }
        }

        if (frame % 60 == 0 && isDebug) {
            TraceLog(LOG_INFO, "Camera Position: (%.2f, %.2f), Zoom: %.2f", cam.pos.x, cam.pos.y, cam.zoom);
            int mouseX = GetMouseX();
            int mouseY = GetMouseY();
            TraceLog(LOG_INFO, "Mouse Position: (%d, %d)", mouseX, mouseY);

                // Compute aspect fit and camera viewport sizes
                AspectFit fitDbg = Camera_ComputeAspectFit(cam.logicalSize, GetRenderWidth(), GetRenderHeight());
                float viewW = cam.logicalSize.x / cam.zoom;
                float viewH = cam.logicalSize.y / cam.zoom;

                // Convert mouse (screen) -> world (tilemap texture) coordinates
                float relX = (float)mouseX - fitDbg.dest.x;
                float relY = (float)mouseY - fitDbg.dest.y;
                float worldX = cam.pos.x + (relX * (viewW / fitDbg.dest.width));
                // IMPORTANT: Our rendering uses a negative src height to flip vertically,
                // so the screen Y maps to world Y without inversion here to keep top=0, bottom=MAP_SIZE-1
                float worldY = cam.pos.y + (relY * (viewH / fitDbg.dest.height));

            // Tile coordinates (indices)
            int tileX = (int)(worldX / TILE_SIZE);
            int tileY = (int)(worldY / TILE_SIZE);
            if (tileX < 0) tileX = 0; else if (tileX >= MAP_SIZE) tileX = MAP_SIZE - 1;
            if (tileY < 0) tileY = 0; else if (tileY >= MAP_SIZE) tileY = MAP_SIZE - 1;

            Tile* hovered = Tilemap_get_tile(tilemap, tileX, tileY);
            int tileId = hovered ? hovered->tile_id : -1;
            TraceLog(LOG_INFO, "World Pos: (%.2f, %.2f) -> Tile: (%d, %d), ID: %d", worldX, worldY, tileX, tileY, tileId);

                        // Compute screen-space rectangle for the hovered tile and draw an outline
            // World-space tile rect
            float tileWorldX = (float)tileX * TILE_SIZE;
            float tileWorldY = (float)tileY * TILE_SIZE;

            // Aspect-fit and viewport under zoom (reuse fitDbg/viewW/viewH above)
            float viewW_dbg = viewW;
            float viewH_dbg = viewH;

            // World -> screen mapping (matching DrawTexturePro(tilemapTexture, srcZoom, fit.dest, ...))
            float sx = fitDbg.dest.x + (tileWorldX - cam.pos.x) * (fitDbg.dest.width / viewW_dbg);
            float sy = fitDbg.dest.y + (tileWorldY - cam.pos.y) * (fitDbg.dest.height / viewH_dbg);
            float sw = (float)TILE_SIZE * (fitDbg.dest.width / viewW_dbg);
            float sh = (float)TILE_SIZE * (fitDbg.dest.height / viewH_dbg);

            DrawRectangleLinesEx((Rectangle){ sx, sy, sw, sh }, 2.0f, (Color){255, 255, 0, 255});
        }

        // Recompute aspect fit on resize
        AspectFit fit = Camera_ComputeAspectFit(cam.logicalSize, GetRenderWidth(), GetRenderHeight());
        Camera_ClampToBounds(&cam, (Vector2){ MAP_SIZE * TILE_SIZE, MAP_SIZE * TILE_SIZE }, fit);
        

        // Draw the render texture to the screen (Y-flipped source)
        Rectangle src = { 0, 0, (float)tilemapTexture.width, (float)-tilemapTexture.height };
        // Apply zoom by scaling destination size around camera viewport
        // Compute logical viewport under zoom
        float viewW = cam.logicalSize.x / cam.zoom;
        float viewH = cam.logicalSize.y / cam.zoom;
        // Source rectangle in texture corresponding to camera position and viewport
        Rectangle srcZoom = {
            cam.pos.x, cam.pos.y,
            viewW, -viewH
        };
        // Destination keeps aspect-fit letterboxing
        DrawTexturePro(tilemapTexture, srcZoom, fit.dest, (Vector2){0,0}, 0.0f, WHITE);

        


        Clay_RenderCommandArray renderCommands = Clay_EndLayout();
        Clay_Raylib_Render(renderCommands, NULL);
        EndDrawing();
    }


    // De-init

    Atlas_free(atlas);

    return 0;
}