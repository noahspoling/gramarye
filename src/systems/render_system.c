#include "systems/render_system.h"

#include "raylib.h"

static void render_player(GameState* state, AspectFit fit) {
    Position* p = Position_get(state->ecs, state->player, state->positionTypeId);
    Sprite* s = Sprite_get(state->ecs, state->player, state->spriteTypeId);
    if (!p || !s) return;

    float worldToScreenScale = fit.scale * state->cam.zoom;
    Vector2 screenPos = Camera_WorldToScreen(&state->cam, fit, (Vector2){
        (float)(p->x * state->tileSize),
        (float)(p->y * state->tileSize)
    });
    Rectangle src = Atlas_getRect(s->atlas, s->tile_id);
    Rectangle dst = { screenPos.x, screenPos.y, state->tileSize * worldToScreenScale, state->tileSize * worldToScreenScale };
    DrawTexturePro(s->atlas->texture, src, dst, (Vector2){0,0}, 0.0f, WHITE);
}

static void render_debug_last_click(GameState* state, AspectFit fit) {
    if (!state->debug || !state->hasLastClick) return;
    float worldToScreenScale = fit.scale * state->cam.zoom;
    Vector2 tl = Camera_WorldToScreen(&state->cam, fit, (Vector2){
        (float)(state->lastClickTileX * state->tileSize),
        (float)(state->lastClickTileY * state->tileSize)
    });
    Rectangle r = { tl.x, tl.y, state->tileSize * worldToScreenScale, state->tileSize * worldToScreenScale };
    DrawRectangleLinesEx(r, 2.0f, RED);
}

void RenderSystem_render(GameState* state, AspectFit fit) {
    if (!state) return;
    ChunkRenderSystem_render(&state->chunkRenderer, state->ecs, state->positionTypeId, &state->cam, fit);
    render_debug_last_click(state, fit);
    render_player(state, fit);
}


