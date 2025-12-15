#include "tile.h"

Tile* Tile_new(uint16_t tile_id, uint8_t flags) {
    Tile* tile = (Tile*)malloc(sizeof(Tile));
    tile->tile_id = tile_id;
    tile->flags = flags;
    return tile;
}
void Tile_set_flag(Tile* tile, uint8_t flag);
void Tile_clear_flag(Tile* tile, uint8_t flag);
bool Tile_has_flag(const Tile* tile, uint8_t flag);