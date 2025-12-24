#include "systems/tile_edit_system.h"

#include "raylib.h"

void TileEditSystem_place_tile_at_mouse(GameState* state, AspectFit fit, Vector2 mousePos) {
    if (!state) return;

    int tileX = 0, tileY = 0;
    TraceLog(LOG_INFO, "Applying place tile at mouse: %f, %f", mousePos.x, mousePos.y);

    if (!TilemapRenderSystem_handle_click(&state->tilemapRenderer, mousePos, &state->cam, fit, state->tileSize, &tileX, &tileY)) {
        return;
    }
    if (tileX < 0 || tileX >= state->mapSize || tileY < 0 || tileY >= state->mapSize) return;

    TraceLog(LOG_INFO, "Placing tile at: %d, %d", tileX, tileY);
    state->hasLastClick = true;
    state->lastClickTileX = tileX;
    state->lastClickTileY = tileY;

    Tilemap_set_tile(state->tilemap, tileX, tileY, 4);
    TilemapRenderSystem_update_tile(&state->tilemapRenderer, state->tilemap, state->atlas, tileX, tileY, state->tileSize);
}


