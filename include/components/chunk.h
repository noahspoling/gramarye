// #ifndef CHUNK_H
// #define CHUNK_H

// #include "tile.h"
// #include "arena.h"
// #include "table.h"
// #include "int_coord_hash.h"

// typedef struct Chunk {
//     int chunkX, chunkY;
//     Table_T tileLookup;
//     Arena_T tileArena;
//     bool isLoaded;
// } Chunk;

// /// @brief  Creates a new chunk at the specified coordinates
// /// @param x 
// /// @param y 
// /// @return 
// Chunk* Chunk_new(int x, int y);
// /// @brief Loads the chunk data into memory (file, db, etc)
// /// @param chunk 
// void Chunk_load(Chunk *chunk);
// /// @brief Unloads the chunk data from memory and saves it (file, db, etc)
// /// @param chunk
// void Chunk_unload(Chunk *chunk);
// /// @brief Sets a tile at the specified coordinates within the chunk
// /// @param chunk 
// /// @param x 
// /// @param y 
// /// @param tile_id
// void Chunk_set_tile(Chunk *chunk, int x, int y, Tile *tile);
// /// @brief Gets a tile at the specified coordinates within the chunk
// /// @param chunk 
// /// @param x 
// /// @param y 
// /// @return Tile*
// Tile* Chunk_get_tile(Chunk *chunk, int x, int y);
// /// @brief Moves an entity from one chunk to another
// /// @param fromChunk 
// /// @param toChunk 
// /// @param entityId
// void Chunk_move_entity_to_Chunk(Chunk *fromChunk, Chunk *toChunk, int entityId);
// /// @brief Offloads an entity from the chunk to the world (removes from chunk)
// /// @param fromChunk 
// /// @param entityId
// void Chunk_offload_entity_to_world(Chunk *fromChunk, int entityId);


// #endif //CHUNK_H