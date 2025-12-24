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
    s->entityRegistry = EntityRegistry_new(s->arena);
    s->positionRegistry = PositionRegistry_new(s->arena);
    s->healthRegistry = HealthRegistry_new(s->arena);
    s->spriteRegistry = SpriteRegistry_new(s->arena);

    s->player = Entity_create(s->entityRegistry);
    Entity_add_component_type(s->entityRegistry, s->player, COMPONENT_POSITION | COMPONENT_HEALTH | COMPONENT_SPRITE);

    int startX = s->mapSize / 2;
    int startY = s->mapSize / 2;
    Position_add(s->positionRegistry, s->player, startX, startY);
    Health_add(s->healthRegistry, s->player, 100.0f);
    Sprite_add(s->spriteRegistry, s->player, s->atlas, 4);
}

static void init_camera(GameState* s, Vector2 logicalSize) {
    Camera_Init(&s->cam, logicalSize);

    Position* p = Position_get(s->positionRegistry, s->player);
    float viewW = s->cam.logicalSize.x / s->cam.zoom;
    float viewH = s->cam.logicalSize.y / s->cam.zoom;
    float px = p->x * s->tileSize + s->tileSize * 0.5f;
    float py = p->y * s->tileSize + s->tileSize * 0.5f;
    s->cam.pos.x = px - viewW * 0.5f;
    s->cam.pos.y = py - viewH * 0.5f;
}

GameSystem* GameSystem_create(Arena_T arena, int mapSize, int tileSize, Vector2 logicalSize) {
    GameSystem* g = (GameSystem*)Arena_alloc(arena, sizeof(GameSystem), __FILE__, __LINE__);
    g->state.arena = arena;
    g->state.mapSize = mapSize;
    g->state.tileSize = tileSize;
    g->state.debug = false;
    g->state.hasLastClick = false;

    init_atlas(&g->state);
    init_tilemap(&g->state);
    init_entities(&g->state);

    TilemapRenderSystem_init(&g->state.tilemapRenderer, g->state.tilemap, g->state.atlas, g->state.tileSize);
    init_camera(&g->state, logicalSize);

    g->input = InputSystem_create(arena);

    return g;
}

void GameSystem_destroy(GameSystem* g) {
    if (!g) return;
    InputSystem_destroy(g->input);
    g->input = NULL;
    TilemapRenderSystem_cleanup(&g->state.tilemapRenderer);
    Atlas_free(g->state.atlas);
}

void GameSystem_frame(GameSystem* g, float dt) {
    (void)dt;
    if (!g) return;

    // 1) Poll input snapshot (main thread), input thread generates commands.
    InputSystem_poll_and_publish(g->input);

    // 2) Drain commands. Defer placement until after camera is updated/clamped.
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

    // 3) Camera update/clamp
    CameraSystem_follow_player(&g->state);
    AspectFit fit = CameraSystem_compute_fit(&g->state);
    CameraSystem_clamp(&g->state, fit);

    // 4) Apply placement with up-to-date camera + fit
    for (int i = 0; i < deferredCount; i++) {
        TileEditSystem_place_tile_at_mouse(&g->state, fit, deferredPlace[i].as.place.mousePos);
    }

    // 5) Render & UI
    ClearBackground(YELLOW);
    UISystem_begin();
    RenderSystem_render(&g->state, fit);
    UISystem_draw_hud(&g->state);
    UISystem_end_and_render();
}


