#include "camera.h"
#include "raylib.h"
#include <math.h>

static float clampf(float v, float a, float b) { return v < a ? a : (v > b ? b : v); }

void Camera_Init(Camera2DEx *cam, Vector2 logicalSize) {
    cam->logicalSize = logicalSize;
    cam->zoom = 1.0f;
    cam->minZoom = 0.5f;
    cam->maxZoom = 4.0f;
    cam->speed = 400.0f; // pixels/sec
    cam->pos = (Vector2){ logicalSize.x * 0.5f - 200.0f, logicalSize.y * 0.5f - 200.0f };
}

void Camera_UpdateInputs(Camera2DEx *cam, float dt) {
    Vector2 dir = {0};
    if (IsKeyDown(KEY_W) || IsKeyDown(KEY_UP)) dir.y -= 1.0f;
    if (IsKeyDown(KEY_S) || IsKeyDown(KEY_DOWN)) dir.y += 1.0f;
    if (IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT)) dir.x -= 1.0f;
    if (IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT)) dir.x += 1.0f;
    if (dir.x != 0 || dir.y != 0) {
        float len = sqrtf(dir.x*dir.x + dir.y*dir.y);
        dir.x /= len; dir.y /= len;
    }
    // Pan independent of zoom (world space speed)
    cam->pos.x += dir.x * cam->speed * dt;
    cam->pos.y += dir.y * cam->speed * dt;

    // Zoom via mouse wheel
    float wheel = GetMouseWheelMove();
    if (wheel != 0.0f) {
        float target = cam->zoom + wheel * 0.1f;
        cam->zoom = clampf(target, cam->minZoom, cam->maxZoom);
    }
}

AspectFit Camera_ComputeAspectFit(Vector2 logicalSize, int winW, int winH) {
    AspectFit fit;
    float sx = (float)winW / logicalSize.x;
    float sy = (float)winH / logicalSize.y;
    fit.scale = sx < sy ? sx : sy;
    float dx = logicalSize.x * fit.scale;
    float dy = logicalSize.y * fit.scale;
    float ox = ((float)winW - dx) * 0.5f;
    float oy = ((float)winH - dy) * 0.5f;
    fit.dest = (Rectangle){ ox, oy, dx, dy };
    return fit;
}

Vector2 Camera_WorldToScreen(const Camera2DEx *cam, AspectFit fit, Vector2 world) {
    Vector2 logical = { (world.x - cam->pos.x) * cam->zoom, (world.y - cam->pos.y) * cam->zoom };
    return (Vector2){ fit.dest.x + logical.x * fit.scale, fit.dest.y + logical.y * fit.scale };
}

Vector2 Camera_ScreenToWorld(const Camera2DEx *cam, AspectFit fit, Vector2 screen) {
    Vector2 logical = { (screen.x - fit.dest.x) / fit.scale, (screen.y - fit.dest.y) / fit.scale };
    return (Vector2){ logical.x / cam->zoom + cam->pos.x, logical.y / cam->zoom + cam->pos.y };
}

void Camera_ClampToBounds(Camera2DEx *cam, Vector2 mapPixelSize, AspectFit fit) {
    // Visible size in world pixels under current zoom
    float vw = cam->logicalSize.x / cam->zoom;
    float vh = cam->logicalSize.y / cam->zoom;
    float maxX = mapPixelSize.x - vw;
    float maxY = mapPixelSize.y - vh;
    cam->pos.x = clampf(cam->pos.x, 0.0f, maxX > 0 ? maxX : 0.0f);
    cam->pos.y = clampf(cam->pos.y, 0.0f, maxY > 0 ? maxY : 0.0f);
}
