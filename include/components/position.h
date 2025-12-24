#ifndef POSITION_H
#define POSITION_H

#include "entity.h"
#include "arena.h"
#include "table.h"

typedef struct {
    int x;
    int y;
} Position;

// Position component registry
typedef struct PositionRegistry {
    Arena_T arena;
    Table_T positions;  // EntityId -> Position*
} PositionRegistry;

// Create a new position registry
PositionRegistry* PositionRegistry_new(Arena_T arena);

// Add position component to entity
void Position_add(PositionRegistry* registry, EntityId entity, int x, int y);

// Get position component for entity
Position* Position_get(PositionRegistry* registry, EntityId entity);

// Set position component for entity
void Position_set(PositionRegistry* registry, EntityId entity, int x, int y);

// Remove position component from entity
void Position_remove(PositionRegistry* registry, EntityId entity);

#endif // POSITION_H

