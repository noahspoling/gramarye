#ifndef CAMERA_SYSTEM_H
#define CAMERA_SYSTEM_H

#include "systems/game_state.h"

void CameraSystem_apply_zoom(GameState* state, float wheel);
void CameraSystem_follow_player(GameState* state);
AspectFit CameraSystem_compute_fit(GameState* state);
void CameraSystem_clamp(GameState* state, AspectFit fit);

#endif // CAMERA_SYSTEM_H


