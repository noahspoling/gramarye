#include "systems/game_system.h"

#include "raylib.h"

#include "clay_renderer_raylib.h"

#include "systems/game_state.h"
#include "systems/input_system.h"
#include "systems/movement_system.h"
#include "systems/camera_system.h"
#include "systems/tile_edit_system.h"
#include "systems/render_system.h"
#include "systems/ui_system.h"
#include "gramarye_event_bus/event_bus.h"
#include "gramarye_chunk_controller/tile_update_queue.h"
#include "gramarye_chunk_renderer/chunk_render_system.h"
#include "gramarye_chunk_controller/chunk_manager_system.h"
#include "camera.h"

#include "core/position.h"
#include "core/health.h"
#include "textures/sprite.h"

#include "textures/atlas.h"
#include "textures/atlas_table.h"
#include "tilemap/tilemap.h"

struct GameSystem {
    GameState state;
    InputSystem* input;
};

static void init_atlas(GameState* s) {
    s->atlasTable = AtlasTable_new();
    AtlasTable_add(&s->atlasTable, "ground", Atlas_new(400));
    s->atlas = AtlasTable_get(&s->atlasTable, "ground");
    Atlas_setTexture(s->atlas, "resources/spritesheet-export.png");

    for (int i = 0; i < 9; i++) {
        Rectangle rect = { (float)(i * 16), 0.0f, 16.0f, 16.0f };
        Atlas_addRect(s->atlas, i, rect);
    }
}

static void init_tilemap(GameState* s) {
    s->tilemap = Tilemap_new(s->mapSize, s->mapSize, s->arena);

    for (int y = 0; y < s->mapSize; y++) {
        for (int x = 0; x < s->mapSize; x++) {
            switch (x % 8) {
                case 0:
                case 1: Tilemap_set_tile(s->tilemap, x, y, 0); break;
                case 2:
                case 3: Tilemap_set_tile(s->tilemap, x, y, 1); break;
                case 4:
                case 5: Tilemap_set_tile(s->tilemap, x, y, 2); break;
                case 6:
                case 7: Tilemap_set_tile(s->tilemap, x, y, 3); break;
            }
        }
    }
}

static void init_entities(GameState* s) {
    s->ecs = ECS_new(s->arena);
    
    s->positionTypeId = ECS_register_component_type(s->ecs, "Position", sizeof(Position));
    s->healthTypeId = ECS_register_component_type(s->ecs, "Health", sizeof(BarValue));
    s->spriteTypeId = ECS_register_component_type(s->ecs, "Sprite", sizeof(Sprite));

    EntityRegistry* entityRegistry = ECS_get_entity_registry(s->ecs);
    s->player = Entity_create(entityRegistry);

    int startX = s->mapSize / 2;
    int startY = s->mapSize / 2;
    Position_add(s->ecs, s->player, s->positionTypeId, startX, startY);
    Health_add(s->ecs, s->player, s->healthTypeId, 100.0f);
    Sprite_add(s->ecs, s->player, s->spriteTypeId, s->atlas, 4);
}

static void init_camera(GameState* s, Vector2 logicalSize) {
    Camera_Init(&s->cam, logicalSize);

    Position* p = Position_get(s->ecs, s->player, s->positionTypeId);
    float viewW = s->cam.logicalSize.x / s->cam.zoom;
    float viewH = s->cam.logicalSize.y / s->cam.zoom;
    float px = p->x * s->tileSize + s->tileSize * 0.5f;
    float py = p->y * s->tileSize + s->tileSize * 0.5f;
    s->cam.pos.x = px - viewW * 0.5f;
    s->cam.pos.y = py - viewH * 0.5f;
}

GameSystem* GameSystem_create(Arena_T arena, int mapSize, int tileSize, Vector2 logicalSize, Renderer* renderer, InputProvider* inputProvider) {
    GameSystem* g = (GameSystem*)Arena_alloc(arena, sizeof(GameSystem), __FILE__, __LINE__);
    g->state.arena = arena;
    g->state.mapSize = mapSize;
    g->state.tileSize = tileSize;
    g->state.renderer = renderer;
    g->state.debug = false;
    g->state.hasLastClick = false;

    init_atlas(&g->state);
    init_tilemap(&g->state);
    init_entities(&g->state);

    g->state.eventBus = EventBus_new(arena);
    
    TileUpdateQueue_init(&g->state.tileUpdateQueue);
    
    ChunkManagerSystem_init(&g->state.chunkManager,
                            arena,
                            g->state.tilemap,
                            g->state.eventBus,
                            64);
    
    ChunkRenderSystem_init(&g->state.chunkRenderer,
                          g->state.arena,
                          g->state.tilemap,
                          g->state.atlas,
                          g->state.renderer,
                          g->state.tileSize,
                          64,
                          5,
                          10);
    
    ChunkRenderSystem_add_entity_observer(&g->state.chunkRenderer,
                                         g->state.ecs,
                                         g->state.player,
                                         g->state.positionTypeId);
    
    init_camera(&g->state, logicalSize);

    g->input = InputSystem_create(arena, inputProvider);

    return g;
}

void GameSystem_destroy(GameSystem* g) {
    if (!g) return;
    InputSystem_destroy(g->input);
    g->input = NULL;
    ChunkRenderSystem_cleanup(&g->state.chunkRenderer);
    Atlas_free(g->state.atlas);
}

void GameSystem_frame(GameSystem* g, float dt) {
    (void)dt;
    if (!g) return;

    InputSystem_poll_and_publish(g->input);

    InputCommand deferredPlace[64];
    int deferredCount = 0;

    InputCommand cmd;
    while (InputSystem_pop(g->input, &cmd)) {
        switch (cmd.type) {
            case Cmd_ToggleDebug:
                g->state.debug = !g->state.debug;
                break;
            case Cmd_Zoom:
                CameraSystem_apply_zoom(&g->state, cmd.as.zoom.wheel);
                break;
            case Cmd_Move:
                MovementSystem_apply_move(&g->state, cmd.as.move.dx, cmd.as.move.dy);
                break;
            case Cmd_PlaceTile:
                if (deferredCount < 64) deferredPlace[deferredCount++] = cmd;
                break;
            default:
                break;
        }
    }

    ChunkManagerSystem_process_updates(&g->state.chunkManager, &g->state.tileUpdateQueue);
    
    ChunkRenderSystem_update(&g->state.chunkRenderer, g->state.ecs, g->state.positionTypeId, &g->state.chunkManager);
    
    CameraSystem_follow_player(&g->state);
    AspectFit fit = CameraSystem_compute_fit(&g->state);
    CameraSystem_clamp(&g->state, fit);

    for (int i = 0; i < deferredCount; i++) {
        TileEditSystem_place_tile_at_mouse(&g->state, fit, deferredPlace[i].as.place.mousePos);
    }

    RenderSystem_render(&g->state, fit);
}


