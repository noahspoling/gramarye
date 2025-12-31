#ifndef CHUNK_RENDER_SYSTEM_H
#define CHUNK_RENDER_SYSTEM_H

#include "raylib.h"
#include "tilemap/tilemap.h"  // From gramarye-components
#include "textures/atlas.h"  // Full Atlas API from gramarye-component-functions
#include "camera.h"
#include "gramarye_ecs/ecs.h"
#include "gramarye_ecs/entity.h"
#include "gramarye_ecs/component.h"
#include "arena.h"
#include "table.h"
#include "int_coord_hash.h"

// Chunk render data structure
typedef struct ChunkRenderData {
    RenderTexture2D renderTexture;  // Render texture for this chunk
    int chunkX, chunkY;             // Chunk coordinates
    bool isDirty;                   // Needs re-rendering
    bool isLoaded;                  // Chunk data is loaded
    uint64_t lastUpdateFrame;       // Last frame this chunk was updated
} ChunkRenderData;

// Observer types
typedef enum {
    OBSERVER_ENTITY,    // Entity with Position component
    OBSERVER_MANUAL     // Manual position
} ObserverType;

typedef struct Observer {
    ObserverType type;
    union {
        EntityId entityId;      // For entity observers
        struct {
            int tileX, tileY;   // For manual observers
        } manual;
    } data;
} Observer;

// Chunk render system structure
typedef struct ChunkRenderSystem {
    Arena_T arena;
    Table_T chunks;              // IntCoord (chunkX, chunkY) -> ChunkRenderData*
    
    // Observer management
    Observer* observers;          // Array of observers
    size_t observerCount;
    size_t observerCapacity;
    
    // Configuration
    int chunkSize;               // Tiles per chunk (64x64)
    int tileSize;                // Pixels per tile
    int renderRadius;            // Chunks to render (5)
    int simulationRadius;         // Chunks to simulate (10)
    
    // References to game systems
    Tilemap* tilemap;
    Atlas* atlas;
    
    uint64_t currentFrame;       // Current frame counter
} ChunkRenderSystem;

// Initialize the chunk render system
void ChunkRenderSystem_init(ChunkRenderSystem* system, 
                            Arena_T arena,
                            Tilemap* tilemap, 
                            Atlas* atlas, 
                            int tileSize,
                            int chunkSize,
                            int renderRadius,
                            int simulationRadius);

// Add an entity observer (entity must have Position component)
void ChunkRenderSystem_add_entity_observer(ChunkRenderSystem* system, 
                                           ECS* ecs,
                                           EntityId entity,
                                           ComponentTypeId positionTypeId);

// Add a manual observer at specific tile coordinates
void ChunkRenderSystem_add_manual_observer(ChunkRenderSystem* system, int tileX, int tileY);

// Remove an entity observer
void ChunkRenderSystem_remove_entity_observer(ChunkRenderSystem* system, EntityId entity);

// Remove a manual observer at specific tile coordinates
void ChunkRenderSystem_remove_manual_observer(ChunkRenderSystem* system, int tileX, int tileY);

// Update chunk loading/unloading based on observers
void ChunkRenderSystem_update(ChunkRenderSystem* system, 
                              ECS* ecs,
                              ComponentTypeId positionTypeId);

// Render visible chunks
void ChunkRenderSystem_render(ChunkRenderSystem* system,
                              ECS* ecs,
                              ComponentTypeId positionTypeId,
                              Camera2DEx* cam, 
                              AspectFit fit);

// Convert tile coordinates to chunk coordinates
void ChunkRenderSystem_get_chunk_coord(ChunkRenderSystem* system, 
                                       int tileX, int tileY, 
                                       int* outChunkX, int* outChunkY);

// Convert chunk coordinates and local coordinates to tile coordinates
void ChunkRenderSystem_get_tile_coord(ChunkRenderSystem* system,
                                      int chunkX, int chunkY,
                                      int localX, int localY,
                                      int* outTileX, int* outTileY);

// Handle click to tile conversion (returns true if click was on tilemap)
bool ChunkRenderSystem_handle_click(ChunkRenderSystem* system,
                                    Vector2 mousePos,
                                    Camera2DEx* cam,
                                    AspectFit fit,
                                    int* outTileX,
                                    int* outTileY);

// Mark a chunk as dirty (needs re-rendering)
void ChunkRenderSystem_mark_chunk_dirty(ChunkRenderSystem* system, int tileX, int tileY);

// Cleanup resources
void ChunkRenderSystem_cleanup(ChunkRenderSystem* system);

#endif // CHUNK_RENDER_SYSTEM_H

