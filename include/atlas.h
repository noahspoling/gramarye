#ifndef ATLAS_H
#define ATLAS_H

#include "raylib.h"
#include "mem.h"
#include "array.h"

typedef struct {
    Texture2D texture;
    Array_T rectangles;
} Atlas;

Atlas* Atlas_Create_Empty(const char* imagePath);
Atlas* Atlas_Create_With_Rects(const char* imagePath, int tileWidth, int tileHeight, int rows, int cols);
void Atlas_Destroy(Atlas* atlas);
Texture2D* Atlas_GetTexture(const Atlas* atlas);
Rectangle Atlas_GetRect(const Atlas* atlas, int index);


#endif // ATLAS_H