#include "systems/ui_system.h"

#include "raylib.h"
#include "clay.h"
#include "clay_renderer_raylib.h"

void UISystem_begin(void) {
    Clay_BeginLayout();
}

void UISystem_draw_hud(GameState* state) {
    if (!state) return;
    Position* p = Position_get(state->ecs, state->player, state->positionTypeId);
    if (!p) return;
    DrawText(TextFormat("Player: (%d, %d)  Zoom: %.2f", p->x, p->y, state->cam.zoom), 10, 10, 20, BLACK);
}

void UISystem_end_and_render(void) {
    Clay_RenderCommandArray renderCommands = Clay_EndLayout();
    Clay_Raylib_Render(renderCommands, NULL);
}


