#include "systems/tile_edit_system.h"

#include "raylib.h"
#include "gramarye_chunk_renderer/chunk_render_system.h"  // For RenderVector2 type

// Tilemap functions
#include "tilemap/tilemap.h"  // Tilemap_set_tile

void TileEditSystem_place_tile_at_mouse(GameState* state, AspectFit fit, Vector2 mousePos) {
    TraceLog(LOG_DEBUG, "TileEditSystem_place_tile_at_mouse: Applying place tile at mouse: %f, %f", mousePos.x, mousePos.y);
    if (!state) return;

    int tileX = 0, tileY = 0;
    RenderVector2 renderMousePos = {mousePos.x, mousePos.y};

    if (!ChunkRenderSystem_handle_click(&state->chunkRenderer, renderMousePos, 
                                       (CameraHandle)&state->cam, (AspectFitHandle)&fit, &tileX, &tileY)) {
        TraceLog(LOG_DEBUG, "TileEditSystem_place_tile_at_mouse: Failed to get tile at mouse: %f, %f", mousePos.x, mousePos.y);
        return;
    }
    // Allow negative coordinates - the chunk system handles them correctly
    // Only check upper bounds if mapSize is used as a limit
    // (Remove this check entirely if you want infinite maps)
    // if (tileX >= state->mapSize || tileY >= state->mapSize) return;

    TraceLog(LOG_DEBUG, "TileEditSystem_place_tile_at_mouse: Placing tile at: %d, %d", tileX, tileY);
    state->hasLastClick = true;
    state->lastClickTileX = tileX;
    state->lastClickTileY = tileY;

    Tilemap_set_tile(state->tilemap, tileX, tileY, 4);
    ChunkRenderSystem_mark_chunk_dirty(&state->chunkRenderer, tileX, tileY);
}


