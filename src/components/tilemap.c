#include "tilemap.h"
#include "raylib.h"

Tilemap *Tilemap_new(int width, int height, Arena_T arena) {
    Tilemap *map = (Tilemap*)Arena_alloc(arena, sizeof(Tilemap), __FILE__, __LINE__);
    map->width = width;
    map->height = height;
    map->arena = arena;
    map->tiles = Table_new(1024, IntCoord_cmp, IntCoord_hash);
    return map;
}

void Tilemap_set_tile(Tilemap *map, int x, int y, uint16_t tile_id) {
    TraceLog(LOG_DEBUG, "Tilemap_set_tile: Setting tile at: %d, %d to: %d", x, y, tile_id);
    IntCoord *coord = (IntCoord*)Arena_alloc(map->arena, sizeof(IntCoord), __FILE__, __LINE__);
    coord->x = x;
    coord->y = y;
    Tile *tile = (Tile*)Arena_alloc(map->arena, sizeof(Tile), __FILE__, __LINE__);
    tile->tile_id = tile_id;
    Table_put(map->tiles, coord, tile);
}

Tile *Tilemap_get_tile(Tilemap *map, int x, int y) {
    IntCoord coord = {x, y};
    return Table_get(map->tiles, &coord);
}
