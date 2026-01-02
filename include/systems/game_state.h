#ifndef GAME_STATE_H
#define GAME_STATE_H

#include <stdbool.h>

#include "arena.h"
#include "raylib.h"

#include "textures/atlas_table.h"
#include "tilemap/tilemap.h"
#include "camera.h"
#include "gramarye_ecs/ecs.h"
#include "gramarye_ecs/entity.h"
#include "gramarye_ecs/component.h"

// Component structs (from gramarye-components)
#include "core/bar_value.h"  // Health uses BarValue
#include "core/position.h"
#include "textures/sprite.h"

// ECS functions (from gramarye-component-functions, includes structs)
#include "core/health.h"  // Health_add, Health_get, etc.
#include "core/position.h"  // Position_add, Position_get, etc.
#include "textures/sprite.h"  // Sprite_add, Sprite_get, etc.

// Full Atlas API with raylib types (from gramarye-component-functions)
#include "textures/atlas.h"  // Full definition with Texture2D, Rectangle

#include "gramarye_chunk_renderer/chunk_render_system.h"
#include "gramarye_renderer/renderer.h"  // Renderer interface
#include "gramarye_ui/ui_provider.h"  // UI provider interface
#include "camera.h"  // Required for Camera2DEx and AspectFit used by chunk renderer
#include "gramarye_event_bus/event_bus.h"  // EventBus
#include "gramarye_chunk_controller/tile_update_queue.h"  // TileUpdateQueue
#include "gramarye_chunk_controller/chunk_manager_system.h"  // ChunkManagerSystem

typedef struct GameState {
    Arena_T arena;
    int mapSize;
    int tileSize;

    AtlasTable atlasTable;  // From textures/atlas_table.h
    Atlas* atlas;
    Tilemap* tilemap;

    ECS* ecs;
    ComponentTypeId positionTypeId;
    ComponentTypeId healthTypeId;
    ComponentTypeId spriteTypeId;
    EntityId player;

    Renderer* renderer;  // Renderer interface
    UIProvider* uiProvider;  // UI provider interface
    Camera2DEx cam;
    ChunkRenderSystem chunkRenderer;
    
    // Event and update systems
    EventBus* eventBus;
    TileUpdateQueue tileUpdateQueue;
    ChunkManagerSystem chunkManager;

    bool debug;

    // Debug: last click mapping visualization
    bool hasLastClick;
    int lastClickTileX;
    int lastClickTileY;

    // Turn tracking
    uint32_t turnCount;  // Current turn number (increments on movement)

    // UI popup state
    struct ClayUI_PopupState* popupState;
    
    // UI fonts (array of Font, allocated in arena)
    Font* uiFonts;
    int uiFontCount;
    
    // UI click blocking
    bool uiBlockingClick;
} GameState;

#endif // GAME_STATE_H


