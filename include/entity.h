#ifndef ENTITY_H
#define ENTITY_H

#include <stdint.h>
#include <stdbool.h>
#include "arena.h"
#include "table.h"
#include "uuid_hash.h"

// Entity ID is a UUID
typedef uuid_key_t EntityId;

// Component type flags (bitmask)
typedef enum {
    COMPONENT_NONE = 0,
    COMPONENT_POSITION = 1 << 0,
    COMPONENT_HEALTH = 1 << 1,
    COMPONENT_SPRITE = 1 << 2,
} ComponentType;

// Entity registry structure
typedef struct EntityRegistry {
    Arena_T arena;
    Table_T entityTable;  // UUID -> ComponentType (bitset)
    uint64_t nextIdHigh;
    uint64_t nextIdLow;
} EntityRegistry;

// Create a new entity registry
EntityRegistry* EntityRegistry_new(Arena_T arena);

// Generate a new entity ID
EntityId Entity_create(EntityRegistry* registry);

// Destroy an entity (removes from registry)
void Entity_destroy(EntityRegistry* registry, EntityId id);

// Check if entity has a component type
bool Entity_has_component(EntityRegistry* registry, EntityId id, ComponentType type);

// Add component type to entity
void Entity_add_component_type(EntityRegistry* registry, EntityId id, ComponentType type);

// Remove component type from entity
void Entity_remove_component_type(EntityRegistry* registry, EntityId id, ComponentType type);

// Comparison function for UUID keys (for Table_T)
int EntityId_cmp(const void *x, const void *y);

// Hash function for UUID keys (for Table_T)
unsigned EntityId_hash(const void *key);

#endif // ENTITY_H

