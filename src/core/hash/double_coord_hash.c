#include "double_coord_hash.h"

static int double_coord_cmp(const void *x, const void *y) {
    const double_coord_t *a = (const double_coord_t *)x;
    const double_coord_t *b = (const double_coord_t *)y;
    return (a->x != b->x) || (a->y != b->y);
}

static unsigned double_coord_hash(const void *key) {
    const double_coord_t *coord = (const double_coord_t *)key;
    unsigned hash = 2166136261u;
    unsigned x_bits = *(unsigned*)&coord->x;
    unsigned y_bits = *(unsigned*)&coord->y;
    hash = (hash ^ x_bits) * 16777619u;
    hash = (hash ^ y_bits) * 16777619u;
    return hash;
}