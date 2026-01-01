#include "systems/movement_system.h"

#include "core/position.h"

#include "tilemap/tilemap.h"

static bool is_tile_walkable(uint16_t tile_id) {
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


