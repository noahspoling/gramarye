#pragma once

#include "raylib.h"

typedef struct {
    Vector2 pos;            // world position in pixels
    float zoom;             // zoom factor (1.0 = 1:1)
    float minZoom;
    float maxZoom;
    float speed;            // pan speed in pixels per second
    Vector2 logicalSize;    // render texture logical size in pixels
} Camera2DEx;

typedef struct {
    Rectangle dest; // destination rectangle on screen (letterboxed/pillarboxed)
    float scale;    // scale applied from logical to screen
} AspectFit;

void Camera_Init(Camera2DEx *cam, Vector2 logicalSize);
void Camera_UpdateInputs(Camera2DEx *cam, float dt);

// Compute aspect-fit rectangle based on window size and logical size
AspectFit Camera_ComputeAspectFit(Vector2 logicalSize, int winW, int winH);

// Transform helpers
Vector2 Camera_WorldToScreen(const Camera2DEx *cam, AspectFit fit, Vector2 world);
Vector2 Camera_ScreenToWorld(const Camera2DEx *cam, AspectFit fit, Vector2 screen);

// Optional: clamp camera so view stays within bounds of a map of given size
void Camera_ClampToBounds(Camera2DEx *cam, Vector2 mapPixelSize, AspectFit fit);
