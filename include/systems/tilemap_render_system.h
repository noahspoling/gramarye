#ifndef TILEMAP_RENDER_SYSTEM_H
#define TILEMAP_RENDER_SYSTEM_H

#include "raylib.h"
#include "tilemap.h"
#include "atlas.h"
#include "camera.h"
#include "gramarye_ecs/ecs.h"
#include "gramarye_ecs/entity.h"
#include "gramarye_ecs/component.h"
#include "components/position.h"
#include "components/sprite.h"

typedef struct TilemapRenderSystem {
    RenderTexture2D renderTarget;
    bool initialized;
} TilemapRenderSystem;

// Initialize the render system with tilemap and atlas
void TilemapRenderSystem_init(TilemapRenderSystem* system, Tilemap* tilemap, Atlas* atlas, int tileSize);

// Update tile in render texture (call when tile changes)
void TilemapRenderSystem_update_tile(TilemapRenderSystem* system, Tilemap* tilemap, Atlas* atlas, int x, int y, int tileSize);

// Render the tilemap to screen
void TilemapRenderSystem_render(TilemapRenderSystem* system, Tilemap* tilemap, Camera2DEx* cam, AspectFit fit, int tileSize);

// Render entities (sprites) on top of tilemap
void TilemapRenderSystem_render_entities(TilemapRenderSystem* system, 
                                         ECS* ecs,
                                         ComponentTypeId spriteTypeId,
                                         ComponentTypeId positionTypeId,
                                         Camera2DEx* cam, 
                                         AspectFit fit, 
                                         int tileSize);

// Handle click to tile conversion (returns true if click was on tilemap)
bool TilemapRenderSystem_handle_click(TilemapRenderSystem* system, 
                                       Vector2 mousePos, 
                                       Camera2DEx* cam, 
                                       AspectFit fit,
                                       int tileSize,
                                       int* outTileX, 
                                       int* outTileY);

// Cleanup
void TilemapRenderSystem_cleanup(TilemapRenderSystem* system);

#endif // TILEMAP_RENDER_SYSTEM_H
