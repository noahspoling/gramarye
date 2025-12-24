#include "int_coord_hash.h"

unsigned int IntCoord_hash(const void *key) {
    const IntCoord *coord = (const IntCoord *)key;
    unsigned hash = 2166136261u;
    hash = (hash ^ coord->x) * 16777619u;
    hash = (hash ^ coord->y) * 16777619u;
    return hash;
}

int IntCoord_cmp(const void *x, const void *y) {
    const IntCoord *a = (const IntCoord *)x;
    const IntCoord *b = (const IntCoord *)y;
    if (a->x != b->x) {
        return a->x - b->x;
    }
    return a->y - b->y;
}