#ifndef HEALTH_H
#define HEALTH_H

#include "entity.h"
#include "arena.h"
#include "table.h"
#include "bar_value.h"

// Health component registry
typedef struct HealthRegistry {
    Arena_T arena;
    Table_T healths;  // EntityId -> BarValue*
} HealthRegistry;

// Create a new health registry
HealthRegistry* HealthRegistry_new(Arena_T arena);

// Add health component to entity
void Health_add(HealthRegistry* registry, EntityId entity, float maxHealth);

// Get health component for entity
BarValue* Health_get(HealthRegistry* registry, EntityId entity);

// Remove health component from entity
void Health_remove(HealthRegistry* registry, EntityId entity);

#endif // HEALTH_H

