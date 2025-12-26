#include "systems/tile_edit_system.h"

#include "raylib.h"

void TileEditSystem_place_tile_at_mouse(GameState* state, AspectFit fit, Vector2 mousePos) {
    TraceLog(LOG_DEBUG, "TileEditSystem_place_tile_at_mouse: Applying place tile at mouse: %f, %f", mousePos.x, mousePos.y);
    if (!state) return;

    int tileX = 0, tileY = 0;

    if (!ChunkRenderSystem_handle_click(&state->chunkRenderer, mousePos, &state->cam, fit, &tileX, &tileY)) {
        TraceLog(LOG_DEBUG, "TileEditSystem_place_tile_at_mouse: Failed to get tile at mouse: %f, %f", mousePos.x, mousePos.y);
        return;
    }
    if (tileX < 0 || tileX >= state->mapSize || tileY < 0 || tileY >= state->mapSize) return;

    TraceLog(LOG_DEBUG, "TileEditSystem_place_tile_at_mouse: Placing tile at: %d, %d", tileX, tileY);
    state->hasLastClick = true;
    state->lastClickTileX = tileX;
    state->lastClickTileY = tileY;

    Tilemap_set_tile(state->tilemap, tileX, tileY, 4);
    ChunkRenderSystem_mark_chunk_dirty(&state->chunkRenderer, tileX, tileY);
}


