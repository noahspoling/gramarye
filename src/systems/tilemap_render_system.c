#include "systems/tilemap_render_system.h"
#include "raylib.h"
#include <math.h>

void TilemapRenderSystem_init(TilemapRenderSystem* system, Tilemap* tilemap, Atlas* atlas, int tileSize) {
    system->renderTarget = LoadRenderTexture(tilemap->width * tileSize, tilemap->height * tileSize);
    SetTextureFilter(system->renderTarget.texture, TEXTURE_FILTER_POINT);
    system->initialized = true;
    
    // Render initial tilemap
    BeginTextureMode(system->renderTarget);
    ClearBackground(BLACK);
    for (int y = 0; y < tilemap->height; y++) {
        for (int x = 0; x < tilemap->width; x++) {
            Tile* tile = Tilemap_get_tile(tilemap, x, y);
            if (tile) {
                Rectangle sourceRect = Atlas_getRect(atlas, tile->tile_id);
                Rectangle destRect = { x * tileSize, y * tileSize, tileSize, tileSize };
                DrawTextureRec(atlas->texture, sourceRect, (Vector2){ destRect.x, destRect.y }, WHITE);
            }
        }
    }
    EndTextureMode();
}

void TilemapRenderSystem_update_tile(TilemapRenderSystem* system, Tilemap* tilemap, Atlas* atlas, int x, int y, int tileSize) {
    if (!system->initialized) return;
    
    Tile* tile = Tilemap_get_tile(tilemap, x, y);
    if (tile) {
        Rectangle sourceRect = Atlas_getRect(atlas, tile->tile_id);
        Rectangle destRect = { x * tileSize, y * tileSize, tileSize, tileSize };
        BeginTextureMode(system->renderTarget);
        DrawTextureRec(atlas->texture, sourceRect, (Vector2){ destRect.x, destRect.y }, WHITE);
        EndTextureMode();
    }
}

void TilemapRenderSystem_render(TilemapRenderSystem* system, Tilemap* tilemap, Camera2DEx* cam, AspectFit fit, int tileSize) {
    if (!system->initialized) return;
    
    float viewW = cam->logicalSize.x / cam->zoom;
    float viewH = cam->logicalSize.y / cam->zoom;
    
    // IMPORTANT: RenderTexture2D is vertically flipped in Raylib when sampled.
    // Use a negative source height to flip it back while also applying camera cropping.
    // Raylib pattern for flipped render textures while cropping:
    // use src.y at the *bottom* of the desired region when height is negative.
    Rectangle srcZoom = { cam->pos.x, cam->pos.y + viewH, viewW, -viewH };
    DrawTexturePro(system->renderTarget.texture, srcZoom, fit.dest, (Vector2){0,0}, 0.0f, WHITE);
}

void TilemapRenderSystem_render_entities(TilemapRenderSystem* system, 
                                         SpriteRegistry* spriteReg, 
                                         PositionRegistry* posReg,
                                         Camera2DEx* cam, 
                                         AspectFit fit, 
                                         int tileSize) {
    if (!system->initialized) return;
    
    // Note: This is a simplified version that renders entities directly to screen
    // In a more advanced system, you might want to render entities to the render texture
    // For now, we'll render them on top of the tilemap
    
    // We need to iterate through all entities - for now, we'll need a way to get all entities
    // This is a limitation - we'd need an entity iterator. For MVP, we'll handle player separately
    // in main.c for now
}

bool TilemapRenderSystem_handle_click(TilemapRenderSystem* system, 
                                       Vector2 mousePos, 
                                       Camera2DEx* cam, 
                                       AspectFit fit,
                                       int tileSize,
                                       int* outTileX, 
                                       int* outTileY) {
    // Check if click is within the letterboxed destination rect
    if (mousePos.x < fit.dest.x || mousePos.x > fit.dest.x + fit.dest.width ||
        mousePos.y < fit.dest.y || mousePos.y > fit.dest.y + fit.dest.height) {
        return false;
    }

    // Use the shared camera helper so click mapping stays consistent with render + zoom.
    Vector2 world = Camera_ScreenToWorld(cam, fit, mousePos);

    // Use floor to avoid truncation edge cases (esp. near tile boundaries).
    int tileX = (int)floorf(world.x / (float)tileSize);
    int tileY = (int)floorf(world.y / (float)tileSize);
    
    if (outTileX) *outTileX = tileX;
    if (outTileY) *outTileY = tileY;
    
    return true;
}

void TilemapRenderSystem_cleanup(TilemapRenderSystem* system) {
    if (system->initialized) {
        UnloadRenderTexture(system->renderTarget);
        system->initialized = false;
    }
}
