#include "chunk.h"


Chunk* Chunk_new(int x, int y) {
    Chunk* chunk = (Chunk*)Arena_alloc(arena, sizeof(Chunk), __FILE__, __LINE__);
    chunk->x = x;
    chunk->y = y;
    chunk->data = (ChunkData*)Arena_alloc(arena, sizeof(ChunkData), __FILE__, __LINE__);
    chunk->data->entityArena = Arena_new();
    chunk->data->itemArena = Arena_new();
    chunk->data->tileArena = Arena_new();
    return chunk;
}

void Chunk_load(Chunk *chunk) {
    chunk->data->entityArena = Arena_new();
    chunk->data->itemArena = Arena_new();
    chunk->data->tileArena = Arena_new();
}
void Chunk_unload(Chunk *chunk);
/// @brief Sets a tile at the specified coordinates within the chunk
/// @param chunk 
/// @param x 
/// @param y 
/// @param tile_id
void Chunk_set_tile(Chunk *chunk, int x, int y, Tile *tile);
/// @brief Gets a tile at the specified coordinates within the chunk
/// @param chunk 
/// @param x 
/// @param y 
/// @return Tile*
Tile* Chunk_get_tile(Chunk *chunk, int x, int y);
/// @brief Moves an entity from one chunk to another
/// @param fromChunk 
/// @param toChunk 
/// @param entityId
void Chunk_move_entity_to_Chunk(Chunk *fromChunk, Chunk *toChunk, int entityId);
/// @brief Offloads an entity from the chunk to the world (removes from chunk)
/// @param fromChunk 
/// @param entityId
void Chunk_offload_entity_to_world(Chunk *fromChunk, int entityId);