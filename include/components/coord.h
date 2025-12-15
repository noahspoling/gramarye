#ifndef TILE_COORD_H
#define TILE_COORD_H

/// @brief Structure representing tile coordinates in a tilemap.
/// Used for a composite key in hash tables and for tile positioning.
typedef struct {
    int x;
    int y;
} TileCoord;

static int tilecoord_cmp(const void *a, const void *b) {
    const TileCoord *ta = a, *tb = b;
    return ta->x != tb->x || ta->y != tb-> y;
}

static unsigned tilecoord_hash(const void *key) {
    const TileCoord *tc = key;
    // Simple hash combining x and y
    return (unsigned)(tc->x * 73856093) ^ (unsigned)(tc->y * 19349663);
}

#endif // TILE_COORD_H