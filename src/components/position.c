#include "position.h"
#include "mem.h"

PositionRegistry* PositionRegistry_new(Arena_T arena) {
    PositionRegistry* registry = (PositionRegistry*)Arena_alloc(arena, sizeof(PositionRegistry), __FILE__, __LINE__);
    registry->arena = arena;
    registry->positions = Table_new(1024, EntityId_cmp, EntityId_hash);
    return registry;
}

void Position_add(PositionRegistry* registry, EntityId entity, int x, int y) {
    Position* pos = (Position*)Arena_alloc(registry->arena, sizeof(Position), __FILE__, __LINE__);
    pos->x = x;
    pos->y = y;
    
    EntityId* idKey = (EntityId*)Arena_alloc(registry->arena, sizeof(EntityId), __FILE__, __LINE__);
    *idKey = entity;
    
    Table_put(registry->positions, idKey, pos);
}

Position* Position_get(PositionRegistry* registry, EntityId entity) {
    return (Position*)Table_get(registry->positions, &entity);
}

void Position_set(PositionRegistry* registry, EntityId entity, int x, int y) {
    Position* pos = Position_get(registry, entity);
    if (pos) {
        pos->x = x;
        pos->y = y;
    }
}

void Position_remove(PositionRegistry* registry, EntityId entity) {
    Table_remove(registry->positions, &entity);
}

