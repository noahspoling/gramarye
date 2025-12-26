#include "systems/chunk_render_system.h"
#include "components/position.h"
#include "raylib.h"
#include <math.h>
#include <string.h>

// Helper: Get chunk coordinates from tile coordinates
static void get_chunk_coord(ChunkRenderSystem* system, int tileX, int tileY, int* outChunkX, int* outChunkY) {
    // Handle negative coordinates correctly (floor division)
    if (tileX >= 0) {
        *outChunkX = tileX / system->chunkSize;
    } else {
        *outChunkX = (tileX - system->chunkSize + 1) / system->chunkSize;
    }
    if (tileY >= 0) {
        *outChunkY = tileY / system->chunkSize;
    } else {
        *outChunkY = (tileY - system->chunkSize + 1) / system->chunkSize;
    }
}

// Helper: Get local tile coordinates within chunk
static void get_local_coord(ChunkRenderSystem* system, int tileX, int tileY, int* outLocalX, int* outLocalY) {
    int chunkX, chunkY;
    get_chunk_coord(system, tileX, tileY, &chunkX, &chunkY);
    *outLocalX = tileX - (chunkX * system->chunkSize);
    *outLocalY = tileY - (chunkY * system->chunkSize);
}

// Helper: Get or create chunk render data
static ChunkRenderData* get_or_create_chunk(ChunkRenderSystem* system, int chunkX, int chunkY) {
    IntCoord coord = {chunkX, chunkY};
    ChunkRenderData* chunk = (ChunkRenderData*)Table_get(system->chunks, &coord);
    
    if (!chunk) {
        chunk = (ChunkRenderData*)Arena_alloc(system->arena, sizeof(ChunkRenderData), __FILE__, __LINE__);
        chunk->chunkX = chunkX;
        chunk->chunkY = chunkY;
        chunk->isDirty = true;
        chunk->isLoaded = false;
        chunk->lastUpdateFrame = 0;
        
        // Create render texture for chunk
        int chunkPixelSize = system->chunkSize * system->tileSize;
        chunk->renderTexture = LoadRenderTexture(chunkPixelSize, chunkPixelSize);
        SetTextureFilter(chunk->renderTexture.texture, TEXTURE_FILTER_POINT);
        
        IntCoord* key = (IntCoord*)Arena_alloc(system->arena, sizeof(IntCoord), __FILE__, __LINE__);
        key->x = chunkX;
        key->y = chunkY;
        Table_put(system->chunks, key, chunk);
    }
    
    return chunk;
}

// Helper: Render a chunk's tiles to its render texture
static void render_chunk_tiles(ChunkRenderSystem* system, ChunkRenderData* chunk) {
    if (!chunk || !system->tilemap || !system->atlas) return;
    
    BeginTextureMode(chunk->renderTexture);
    ClearBackground(BLACK);
    
    int startTileX = chunk->chunkX * system->chunkSize;
    int startTileY = chunk->chunkY * system->chunkSize;
    
    for (int localY = 0; localY < system->chunkSize; localY++) {
        for (int localX = 0; localX < system->chunkSize; localX++) {
            int tileX = startTileX + localX;
            int tileY = startTileY + localY;
            
            Tile* tile = Tilemap_get_tile(system->tilemap, tileX, tileY);
            if (tile) {
                Rectangle sourceRect = Atlas_getRect(system->atlas, tile->tile_id);
                Rectangle destRect = {
                    (float)(localX * system->tileSize),
                    (float)(localY * system->tileSize),
                    (float)system->tileSize,
                    (float)system->tileSize
                };
                DrawTextureRec(system->atlas->texture, sourceRect, (Vector2){destRect.x, destRect.y}, WHITE);
            }
        }
    }
    
    EndTextureMode();
    chunk->isDirty = false;
    chunk->isLoaded = true;
    chunk->lastUpdateFrame = system->currentFrame;
}

// Helper: Get observer tile position
static void get_observer_tile_pos(ChunkRenderSystem* system, 
                                   Observer* observer,
                                   ECS* ecs,
                                   ComponentTypeId positionTypeId,
                                   int* outTileX, int* outTileY) {
    if (observer->type == OBSERVER_ENTITY) {
        Position* pos = Position_get(ecs, observer->data.entityId, positionTypeId);
        if (pos) {
            *outTileX = pos->x;
            *outTileY = pos->y;
        } else {
            *outTileX = 0;
            *outTileY = 0;
        }
    } else {
        *outTileX = observer->data.manual.tileX;
        *outTileY = observer->data.manual.tileY;
    }
}

void ChunkRenderSystem_init(ChunkRenderSystem* system,
                            Arena_T arena,
                            Tilemap* tilemap,
                            Atlas* atlas,
                            int tileSize,
                            int chunkSize,
                            int renderRadius,
                            int simulationRadius) {
    memset(system, 0, sizeof(ChunkRenderSystem));
    system->arena = arena;
    system->tilemap = tilemap;
    system->atlas = atlas;
    system->tileSize = tileSize;
    system->chunkSize = chunkSize;
    system->renderRadius = renderRadius;
    system->simulationRadius = simulationRadius;
    system->chunks = Table_new(256, IntCoord_cmp, IntCoord_hash);
    system->observerCapacity = 16;
    system->observers = (Observer*)Arena_alloc(arena, sizeof(Observer) * system->observerCapacity, __FILE__, __LINE__);
    system->observerCount = 0;
    system->currentFrame = 0;
}

void ChunkRenderSystem_add_entity_observer(ChunkRenderSystem* system,
                                           ECS* ecs,
                                           EntityId entity,
                                           ComponentTypeId positionTypeId) {
    // Check if observer already exists
    for (size_t i = 0; i < system->observerCount; i++) {
        if (system->observers[i].type == OBSERVER_ENTITY && 
            system->observers[i].data.entityId.high == entity.high &&
            system->observers[i].data.entityId.low == entity.low) {
            return; // Already exists
        }
    }
    
    // Resize if needed
    if (system->observerCount >= system->observerCapacity) {
        size_t newCapacity = system->observerCapacity * 2;
        Observer* newObservers = (Observer*)Arena_alloc(system->arena, sizeof(Observer) * newCapacity, __FILE__, __LINE__);
        memcpy(newObservers, system->observers, sizeof(Observer) * system->observerCount);
        system->observers = newObservers;
        system->observerCapacity = newCapacity;
    }
    
    system->observers[system->observerCount].type = OBSERVER_ENTITY;
    system->observers[system->observerCount].data.entityId = entity;
    system->observerCount++;
}

void ChunkRenderSystem_add_manual_observer(ChunkRenderSystem* system, int tileX, int tileY) {
    // Check if observer already exists
    for (size_t i = 0; i < system->observerCount; i++) {
        if (system->observers[i].type == OBSERVER_MANUAL &&
            system->observers[i].data.manual.tileX == tileX &&
            system->observers[i].data.manual.tileY == tileY) {
            return; // Already exists
        }
    }
    
    // Resize if needed
    if (system->observerCount >= system->observerCapacity) {
        size_t newCapacity = system->observerCapacity * 2;
        Observer* newObservers = (Observer*)Arena_alloc(system->arena, sizeof(Observer) * newCapacity, __FILE__, __LINE__);
        memcpy(newObservers, system->observers, sizeof(Observer) * system->observerCount);
        system->observers = newObservers;
        system->observerCapacity = newCapacity;
    }
    
    system->observers[system->observerCount].type = OBSERVER_MANUAL;
    system->observers[system->observerCount].data.manual.tileX = tileX;
    system->observers[system->observerCount].data.manual.tileY = tileY;
    system->observerCount++;
}

void ChunkRenderSystem_remove_entity_observer(ChunkRenderSystem* system, EntityId entity) {
    for (size_t i = 0; i < system->observerCount; i++) {
        if (system->observers[i].type == OBSERVER_ENTITY &&
            system->observers[i].data.entityId.high == entity.high &&
            system->observers[i].data.entityId.low == entity.low) {
            // Remove by swapping with last
            system->observers[i] = system->observers[system->observerCount - 1];
            system->observerCount--;
            return;
        }
    }
}

void ChunkRenderSystem_remove_manual_observer(ChunkRenderSystem* system, int tileX, int tileY) {
    for (size_t i = 0; i < system->observerCount; i++) {
        if (system->observers[i].type == OBSERVER_MANUAL &&
            system->observers[i].data.manual.tileX == tileX &&
            system->observers[i].data.manual.tileY == tileY) {
            // Remove by swapping with last
            system->observers[i] = system->observers[system->observerCount - 1];
            system->observerCount--;
            return;
        }
    }
}

void ChunkRenderSystem_update(ChunkRenderSystem* system,
                               ECS* ecs,
                               ComponentTypeId positionTypeId) {
    system->currentFrame++;
    
    // Collect chunks that should be loaded (render radius)
    // and chunks that should be simulated (simulation radius)
    // For now, we'll just ensure chunks in render radius are loaded
    
    // Mark chunks that should be visible based on observers
    for (size_t i = 0; i < system->observerCount; i++) {
        int observerTileX, observerTileY;
        get_observer_tile_pos(system, &system->observers[i], ecs, positionTypeId, &observerTileX, &observerTileY);
        
        int observerChunkX, observerChunkY;
        get_chunk_coord(system, observerTileX, observerTileY, &observerChunkX, &observerChunkY);
        
        // Load chunks in render radius
        for (int dy = -system->renderRadius; dy <= system->renderRadius; dy++) {
            for (int dx = -system->renderRadius; dx <= system->renderRadius; dx++) {
                int chunkX = observerChunkX + dx;
                int chunkY = observerChunkY + dy;
                
                // Check if within render radius (circular check)
                float dist = sqrtf((float)(dx * dx + dy * dy));
                if (dist <= (float)system->renderRadius) {
                    ChunkRenderData* chunk = get_or_create_chunk(system, chunkX, chunkY);
                    if (chunk->isDirty || !chunk->isLoaded) {
                        render_chunk_tiles(system, chunk);
                    }
                }
            }
        }
    }
    
    // TODO: Unload chunks outside simulation radius of all observers
    // This would require iterating all chunks and checking distance to nearest observer
}

void ChunkRenderSystem_render(ChunkRenderSystem* system,
                              ECS* ecs,
                              ComponentTypeId positionTypeId,
                              Camera2DEx* cam,
                              AspectFit fit) {
    if (!system || !cam) return;
    
    // Collect all chunks that should be rendered (union of all observer render radii)
    // Use a table to track which chunks we've already rendered to avoid duplicates
    Table_T renderedChunks = Table_new(128, IntCoord_cmp, IntCoord_hash);
    
    for (size_t i = 0; i < system->observerCount; i++) {
        int observerTileX, observerTileY;
        get_observer_tile_pos(system, &system->observers[i], ecs, positionTypeId, &observerTileX, &observerTileY);
        
        int observerChunkX, observerChunkY;
        get_chunk_coord(system, observerTileX, observerTileY, &observerChunkX, &observerChunkY);
        
        // Render chunks in render radius
        for (int dy = -system->renderRadius; dy <= system->renderRadius; dy++) {
            for (int dx = -system->renderRadius; dx <= system->renderRadius; dx++) {
                int chunkX = observerChunkX + dx;
                int chunkY = observerChunkY + dy;
                
                float dist = sqrtf((float)(dx * dx + dy * dy));
                if (dist <= (float)system->renderRadius) {
                    IntCoord coord = {chunkX, chunkY};
                    
                    // Check if we've already rendered this chunk
                    if (Table_get(renderedChunks, &coord)) {
                        continue;
                    }
                    
                    // Mark as rendered
                    IntCoord* key = (IntCoord*)Arena_alloc(system->arena, sizeof(IntCoord), __FILE__, __LINE__);
                    key->x = chunkX;
                    key->y = chunkY;
                    Table_put(renderedChunks, key, (void*)1);
                    
                    ChunkRenderData* chunk = (ChunkRenderData*)Table_get(system->chunks, &coord);
                    if (chunk && chunk->isLoaded) {
                        // Calculate chunk world position in pixels
                        float chunkWorldX = (float)(chunkX * system->chunkSize * system->tileSize);
                        float chunkWorldY = (float)(chunkY * system->chunkSize * system->tileSize);
                        
                        // Get screen position
                        Vector2 screenPos = Camera_WorldToScreen(cam, fit, (Vector2){chunkWorldX, chunkWorldY});
                        
                        // Render chunk texture
                        float chunkPixelSize = (float)(system->chunkSize * system->tileSize);
                        Rectangle srcRect = {0, 0, chunkPixelSize, -chunkPixelSize}; // Negative height for flipped Y
                        Rectangle dstRect = {screenPos.x, screenPos.y, chunkPixelSize * cam->zoom * fit.scale, chunkPixelSize * cam->zoom * fit.scale};
                        DrawTexturePro(chunk->renderTexture.texture, srcRect, dstRect, (Vector2){0, 0}, 0.0f, WHITE);
                    }
                }
            }
        }
    }
}

void ChunkRenderSystem_get_chunk_coord(ChunkRenderSystem* system,
                                       int tileX, int tileY,
                                       int* outChunkX, int* outChunkY) {
    get_chunk_coord(system, tileX, tileY, outChunkX, outChunkY);
}

void ChunkRenderSystem_get_tile_coord(ChunkRenderSystem* system,
                                      int chunkX, int chunkY,
                                      int localX, int localY,
                                      int* outTileX, int* outTileY) {
    *outTileX = chunkX * system->chunkSize + localX;
    *outTileY = chunkY * system->chunkSize + localY;
}

bool ChunkRenderSystem_handle_click(ChunkRenderSystem* system,
                                    Vector2 mousePos,
                                    Camera2DEx* cam,
                                    AspectFit fit,
                                    int* outTileX,
                                    int* outTileY) {
    if (!system || !cam) return false;
    
    // Check if click is within the letterboxed destination rect
    if (mousePos.x < fit.dest.x || mousePos.x > fit.dest.x + fit.dest.width ||
        mousePos.y < fit.dest.y || mousePos.y > fit.dest.y + fit.dest.height) {
        return false;
    }
    
    // Convert screen position to world position
    Vector2 world = Camera_ScreenToWorld(cam, fit, mousePos);
    
    // Convert world position to tile coordinates
    int tileX = (int)floorf(world.x / (float)system->tileSize);
    int tileY = (int)floorf(world.y / (float)system->tileSize);
    
    if (outTileX) *outTileX = tileX;
    if (outTileY) *outTileY = tileY;
    
    return true;
}

void ChunkRenderSystem_mark_chunk_dirty(ChunkRenderSystem* system, int tileX, int tileY) {
    if (!system) return;
    
    int chunkX, chunkY;
    get_chunk_coord(system, tileX, tileY, &chunkX, &chunkY);
    
    IntCoord coord = {chunkX, chunkY};
    ChunkRenderData* chunk = (ChunkRenderData*)Table_get(system->chunks, &coord);
    if (chunk) {
        chunk->isDirty = true;
    }
}

void ChunkRenderSystem_cleanup(ChunkRenderSystem* system) {
    if (!system) return;
    
    // Unload all chunk render textures
    // Note: Table iteration would be needed to unload all chunks
    // For now, we'll just mark as not initialized
    // In a full implementation, we'd iterate the table and unload each texture
}

