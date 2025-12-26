#ifndef GAME_STATE_H
#define GAME_STATE_H

#include <stdbool.h>

#include "arena.h"
#include "raylib.h"

#include "atlas_table.h"
#include "atlas.h"
#include "tilemap.h"
#include "camera.h"
#include "gramarye_ecs/ecs.h"
#include "gramarye_ecs/entity.h"
#include "gramarye_ecs/component.h"

#include "components/health.h"
#include "components/position.h"
#include "components/sprite.h"

#include "systems/tilemap_render_system.h"

typedef struct GameState {
    Arena_T arena;
    int mapSize;
    int tileSize;

    AtlasTable atlasTable;
    Atlas* atlas;
    Tilemap* tilemap;

    ECS* ecs;
    ComponentTypeId positionTypeId;
    ComponentTypeId healthTypeId;
    ComponentTypeId spriteTypeId;
    EntityId player;

    Camera2DEx cam;
    TilemapRenderSystem tilemapRenderer;

    bool debug;

    // Debug: last click mapping visualization
    bool hasLastClick;
    int lastClickTileX;
    int lastClickTileY;
} GameState;

#endif // GAME_STATE_H


