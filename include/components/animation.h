#ifndef ANIMATION_H
#define ANIMATION_H

#include "raylib.h"
#include <stdbool.h>

typedef struct Animation {
    int frameCount;
    int currentFrame;
    float frameDuration; // Duration of each frame in seconds
    bool loop;           // Should the animation loop
    int spriteIndex;    // Index of the sprite in the atlas
    Rectangle* frames;   // Array of frames (rectangles)
} Animation;

#endif // ANIMATION_H