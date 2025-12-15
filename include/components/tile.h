#ifndef TILE_H
#define TILE_H

#include <stdint.h>
#include <stdbool.h>

typedef struct Tile {
    uint16_t tile_id;
    uint8_t flags;  // bit flags for tile properties
} Tile;

Tile* Tile_new(uint16_t tile_id, uint8_t flags);
void Tile_set_flag(Tile* tile, uint8_t flag);
void Tile_clear_flag(Tile* tile, uint8_t flag);
bool Tile_has_flag(const Tile* tile, uint8_t flag);


#endif // TILE_H