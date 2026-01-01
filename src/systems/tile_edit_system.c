#include "systems/tile_edit_system.h"

#include "raylib.h"
#include "gramarye_chunk_renderer/chunk_render_system.h"  // For RenderVector2 type
#include "gramarye_chunk_controller/tile_update_queue.h"  // TileUpdateQueue

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

    TraceLog(LOG_DEBUG, "TileEditSystem_place_tile_at_mouse: Queueing tile update at: %d, %d", tileX, tileY);
    state->hasLastClick = true;
    state->lastClickTileX = tileX;
    state->lastClickTileY = tileY;

    TileUpdateCommand cmd = {
        .tileX = tileX,
        .tileY = tileY,
        .tile_id = 4
    };
    
    if (!TileUpdateQueue_push(&state->tileUpdateQueue, cmd)) {
        TraceLog(LOG_WARNING, "TileEditSystem_place_tile_at_mouse: Failed to queue tile update (queue full)");
    }
}


