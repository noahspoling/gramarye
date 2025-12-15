#ifndef TILEMAP_H
#define TILEMAP_H

#include <stdint.h>
#include "table.h"
#include "arena.h"
#include "int_coord_hash.h"
#include "tile.h"

// Tilemap structure
// Used for the maxxed out presentation layer of the tilemap
// in a client server model it would set the chunks to be sent to the client


typedef struct Tilemap {
    int width;
    int height;
    Table_T tiles;              // Table_T with TileCoord* as key, Tile* as value
    Arena_T arena;              // Arena for memory management
} Tilemap;

Tilemap *Tilemap_new(int width, int height, Arena_T arena);
void Tilemap_set_tile(Tilemap *map, int x, int y, uint16_t tile_id);
Tile *Tilemap_get_tile(Tilemap *map, int x, int y);


#endif // TILEMAP_H
