#ifndef UI_SYSTEM_H
#define UI_SYSTEM_H

#include "systems/game_state.h"

void UISystem_begin(GameState* state);
void UISystem_set_pointer_state(GameState* state, Vector2 mousePos, bool mouseDown);
void UISystem_draw_hud(GameState* state);
void UISystem_draw_popup(GameState* state);
void UISystem_update_popup_interactions(GameState* state, bool mousePressedThisFrame);
void UISystem_end_and_render(GameState* state);
bool UISystem_is_pointer_over_ui(GameState* state);
bool UISystem_check_ui_blocking(GameState* state, Vector2 mousePos, bool mouseDown);

#endif // UI_SYSTEM_H


