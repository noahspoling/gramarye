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
}

bool TilemapRenderSystem_handle_click(TilemapRenderSystem* system, 
                                       Vector2 mousePos, 
                                       Camera2DEx* cam, 
                                       AspectFit fit,
                                       int tileSize,
                                       int* outTileX, 
                                       int* outTileY) {
    (void)system; // Decoupled from render texture - we work purely in world/tile space
    
    TraceLog(LOG_DEBUG, "TilemapRenderSystem_handle_click: mousePos=(%f, %f), fit.dest=(%f, %f, %f, %f), fit.scale=%f", 
             mousePos.x, mousePos.y, fit.dest.x, fit.dest.y, fit.dest.width, fit.dest.height, fit.scale);
    
    // Check if click is within the letterboxed destination rect
    if (mousePos.x < fit.dest.x || mousePos.x > fit.dest.x + fit.dest.width ||
        mousePos.y < fit.dest.y || mousePos.y > fit.dest.y + fit.dest.height) {
        TraceLog(LOG_DEBUG, "TilemapRenderSystem_handle_click: Click outside bounds");
        return false;
    }
    
    Vector2 world = Camera_ScreenToWorld(cam, fit, mousePos);
    TraceLog(LOG_DEBUG, "TilemapRenderSystem_handle_click: world=(%f, %f), cam.pos=(%f, %f), cam.zoom=%f", 
             world.x, world.y, cam->pos.x, cam->pos.y, cam->zoom);
             
    int tileX = (int)floorf(world.x / (float)tileSize);
    int tileY = (int)floorf(world.y / (float)tileSize);
    TraceLog(LOG_DEBUG, "TilemapRenderSystem_handle_click: tileX=%d, tileY=%d (world.x/tileSize=%f, world.y/tileSize=%f)", 
             tileX, tileY, world.x / (float)tileSize, world.y / (float)tileSize);
    
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
