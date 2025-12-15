#include "atlas.h"
#include "mem.h"
#include "except.h"
#include "table.h"
#include <stdlib.h>
#include <assert.h>

const Except_T Atlas_Uninitialized_Error = { "Atlas Error: Uninitialized" };
const Except_T Atlas_Capacity_Error = { "Atlas Error: Capacity exceeds maximum allowed" };

static int int_cmp(const void *x, const void *y) {
    return *(const int*)x != *(const int*)y;
}
static unsigned int_hash(const void *key) {
    return *(const int*)key;
}

Atlas* Atlas_new(int initialCapacity) {
    if (initialCapacity > ATLAS_MAX_CAPACITY) {
        Except_raise(&Atlas_Capacity_Error, __FILE__, __LINE__);
    }
    Atlas* atlas = Mem_alloc(sizeof(Atlas), __FILE__, __LINE__);
    atlas->rects = Table_new(initialCapacity, int_cmp, int_hash);
    atlas->rectCount = 0;
    atlas->capacity = initialCapacity;
    return atlas;
}

void Atlas_setTexture(Atlas* atlas, char *path) {
    assert(atlas != NULL || path != NULL); // Ensure atlas and path are not NULL
    if (*path == '\0') {
        Except_raise(&Atlas_Uninitialized_Error, __FILE__, __LINE__);
    }
    atlas->texture = LoadTexture(path);
}

void Atlas_addRect(Atlas* atlas, int index, Rectangle rect) {
    TraceLog(LOG_INFO, "Adding rect to atlas at index %d", index);
    if (atlas->rectCount >= atlas->capacity) {
        TraceLog(LOG_ERROR, "Atlas capacity exceeded at index %d", index);
        Except_raise(&Atlas_Capacity_Error, __FILE__, __LINE__);
    }
    int* key = Mem_alloc(sizeof(int), __FILE__, __LINE__);
    *key = index;
    Rectangle* value = Mem_alloc(sizeof(Rectangle), __FILE__, __LINE__);
    *value = rect;
    Table_put(atlas->rects, key, value);
    atlas->rectCount++;
}

Rectangle Atlas_getRect(const Atlas* atlas, int index) {
    int key = index;
    Rectangle* rect = Table_get(atlas->rects, &key);
    if (!rect) {
        Except_raise(&Atlas_Uninitialized_Error, __FILE__, __LINE__);
    }
    return *rect;
}

void Atlas_removeRect(Atlas* atlas, int index) {
    TraceLog(LOG_INFO, "Attempting to remove rect from atlas at index %d", index);
    int key = index;
    Rectangle* rect = Table_remove(atlas->rects, &key);
    if (rect) {
        TraceLog(LOG_INFO, "Removing rect from atlas at index %d", index);
        Mem_free(rect, __FILE__, __LINE__);
        atlas->rectCount--;
    }
}

void Atlas_clear(Atlas* atlas) {
    Table_free(&atlas->rects);
    atlas->rects = Table_new(ATLAS_INITIAL_CAPACITY, int_cmp, int_hash);
    atlas->rectCount = 0;
}

int Atlas_getRectCount(const Atlas* atlas) {
    return atlas->rectCount;
}

void Atlas_free(Atlas* atlas) {
    if (atlas) {
        Table_free(&atlas->rects);
        UnloadTexture(atlas->texture);
        Mem_free(atlas, __FILE__, __LINE__);
    }
}