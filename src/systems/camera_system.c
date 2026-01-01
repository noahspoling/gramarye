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
    // On web, when the canvas is scaled via CSS, we need to ensure the coordinate
    // space used for aspect fit calculation matches the coordinate space of
    // GetMousePosition().
    //
    // The issue: On web, GetMousePosition() accounts for CSS scaling and returns
    // coordinates in the displayed space. However, GetScreenWidth()/GetScreenHeight()
    // return the actual canvas dimensions, which might not match the displayed space
    // if CSS is scaling the canvas.
    //
    // Solution: Use GetRenderWidth()/GetRenderHeight() on web as well, which should
    // return the actual render target dimensions. GetMousePosition() on web should
    // return coordinates that match these dimensions after accounting for CSS scaling.
    // However, raylib's web implementation should handle this automatically.
    //
    // Actually, the real fix is simpler: On web, GetMousePosition() should return
    // coordinates in screen space (accounting for CSS), and GetScreenWidth()/Height()
    // should return the screen dimensions. But to be safe, let's use render dimensions
    // consistently and ensure GetMousePosition() matches.
    #ifdef PLATFORM_WEB
    // On web, use render dimensions to match the coordinate space
    // GetMousePosition() should return coordinates in this space (after CSS scaling is accounted for)
    return Camera_ComputeAspectFit(state->cam.logicalSize, GetRenderWidth(), GetRenderHeight());
    #else
    // On desktop, use render dimensions
    return Camera_ComputeAspectFit(state->cam.logicalSize, GetRenderWidth(), GetRenderHeight());
    #endif
}

void CameraSystem_clamp(GameState* state, AspectFit fit) {
    Camera_ClampToBounds(&state->cam, (Vector2){ state->mapSize * state->tileSize, state->mapSize * state->tileSize }, fit);
}


