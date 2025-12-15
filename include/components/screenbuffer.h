#ifndef SCREENBUFFER_H
#define SCREENBUFFER_H

#include "raylib.h"

typedef struct ScreenBuffer {
    RenderTexture2D renderFrame;
    RenderTexture2D renderFrameTemp;
} ScreenBuffer;

#endif // SCREENBUFFER_H