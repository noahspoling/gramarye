#ifndef UI_SYSTEM_H
#define UI_SYSTEM_H

#include "systems/game_state.h"

void UISystem_begin(void);
void UISystem_draw_hud(GameState* state);
void UISystem_end_and_render(void);

#endif // UI_SYSTEM_H


