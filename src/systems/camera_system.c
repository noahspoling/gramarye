#include "systems/camera_system.h"
#include "core/position.h"

void CameraSystem_apply_zoom(GameState* state, float wheel) {
    if (!state) return;
    if (wheel == 0.0f) return;

    Camera2DEx* cam = &state->cam;
    float target = cam->zoom + wheel * 0.1f;
    if (target < cam->minZoom) target = cam->minZoom;
    if (target > cam->maxZoom) target = cam->maxZoom;
    cam->zoom = target;
}

void CameraSystem_follow_player(GameState* state) {
    if (!state) return;
    Position* p = Position_get(state->ecs, state->player, state->positionTypeId);
    if (!p) return;

    float viewW = state->cam.logicalSize.x / state->cam.zoom;
    float viewH = state->cam.logicalSize.y / state->cam.zoom;
    float px = p->x * state->tileSize + state->tileSize * 0.5f;
    float py = p->y * state->tileSize + state->tileSize * 0.5f;
    state->cam.pos.x = px - viewW * 0.5f;
    state->cam.pos.y = py - viewH * 0.5f;
}

AspectFit CameraSystem_compute_fit(GameState* state) {
    // On web, when canvas is scaled via CSS, GetMousePosition() returns coordinates
    // in the displayed/scaled space, so we need to use screen dimensions
    #ifdef PLATFORM_WEB
    return Camera_ComputeAspectFit(state->cam.logicalSize, GetScreenWidth(), GetScreenHeight());
    #else
    return Camera_ComputeAspectFit(state->cam.logicalSize, GetRenderWidth(), GetRenderHeight());
    #endif
}

void CameraSystem_clamp(GameState* state, AspectFit fit) {
    Camera_ClampToBounds(&state->cam, (Vector2){ state->mapSize * state->tileSize, state->mapSize * state->tileSize }, fit);
}


