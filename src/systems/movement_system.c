#include "systems/movement_system.h"

// ECS functions
#include "core/position.h"  // Position_get, Position_set

// Tilemap functions
#include "tilemap/tilemap.h"  // Tilemap_get_tile

static bool is_tile_walkable(uint16_t tile_id) {
    // Current demo: treat all tiles as walkable.
    (void)tile_id;
    return true;
}

void MovementSystem_apply_move(GameState* state, int dx, int dy) {
    if (!state) return;
    Position* p = Position_get(state->ecs, state->player, state->positionTypeId);
    if (!p) return;

    int newX = p->x + dx;
    int newY = p->y + dy;
    if (newX < 0 || newX >= state->mapSize || newY < 0 || newY >= state->mapSize) return;

    Tile* targetTile = Tilemap_get_tile(state->tilemap, newX, newY);
    if (targetTile && is_tile_walkable(targetTile->tile_id)) {
        Position_set(state->ecs, state->player, state->positionTypeId, newX, newY);
    }
}


