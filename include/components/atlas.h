#ifndef ATLAS_H
#define ATLAS_H

#include "raylib.h"
#include "except.h"
#include "table.h"

#define ATLAS_MAX_CAPACITY 500
#define ATLAS_INITIAL_CAPACITY 400

typedef Table_T Atlas_Table;

extern const Except_T Atlas_Uninitialized_Error;
extern const Except_T Atlas_Capacity_Error;

typedef struct Atlas {
    Texture2D texture;
    Atlas_Table rects;
    int rectCount;
    int capacity;
} Atlas;

Atlas* Atlas_new(int initialCapacity);
void Atlas_addRect(Atlas* atlas, int index, Rectangle rect);
void Atlas_setTexture(Atlas* atlas, char *path);
Rectangle Atlas_getRect(const Atlas* atlas, int index);
void Atlas_removeRect(Atlas* atlas, int index);
void Atlas_clear(Atlas* atlas);
int Atlas_getRectCount(const Atlas* atlas);
void Atlas_free(Atlas* atlas);

#endif // ATLAS_H