#include "entity.h"
#include "mem.h"
#include <string.h>

EntityRegistry* EntityRegistry_new(Arena_T arena) {
    EntityRegistry* registry = (EntityRegistry*)Arena_alloc(arena, sizeof(EntityRegistry), __FILE__, __LINE__);
    registry->arena = arena;
    registry->entityTable = Table_new(1024, EntityId_cmp, EntityId_hash);
    registry->nextIdHigh = 1;
    registry->nextIdLow = 0;
    return registry;
}

EntityId Entity_create(EntityRegistry* registry) {
    EntityId id;
    id.high = registry->nextIdHigh;
    id.low = registry->nextIdLow;
    
    // Increment ID (simple counter for now, could use proper UUID generation)
    registry->nextIdLow++;
    if (registry->nextIdLow == 0) {
        registry->nextIdHigh++;
    }
    
    // Initialize entity with no components
    ComponentType* componentBits = (ComponentType*)Arena_alloc(registry->arena, sizeof(ComponentType), __FILE__, __LINE__);
    *componentBits = COMPONENT_NONE;
    
    EntityId* idKey = (EntityId*)Arena_alloc(registry->arena, sizeof(EntityId), __FILE__, __LINE__);
    *idKey = id;
    
    Table_put(registry->entityTable, idKey, componentBits);
    
    return id;
}

void Entity_destroy(EntityRegistry* registry, EntityId id) {
    Table_remove(registry->entityTable, &id);
}

bool Entity_has_component(EntityRegistry* registry, EntityId id, ComponentType type) {
    ComponentType* bits = (ComponentType*)Table_get(registry->entityTable, &id);
    if (!bits) return false;
    return (*bits & type) != 0;
}

void Entity_add_component_type(EntityRegistry* registry, EntityId id, ComponentType type) {
    ComponentType* bits = (ComponentType*)Table_get(registry->entityTable, &id);
    if (bits) {
        *bits |= type;
    }
}

void Entity_remove_component_type(EntityRegistry* registry, EntityId id, ComponentType type) {
    ComponentType* bits = (ComponentType*)Table_get(registry->entityTable, &id);
    if (bits) {
        *bits &= ~type;
    }
}

int EntityId_cmp(const void *x, const void *y) {
    const EntityId *a = (const EntityId*)x;
    const EntityId *b = (const EntityId*)y;
    if (a->high != b->high) return a->high < b->high ? -1 : 1;
    if (a->low != b->low) return a->low < b->low ? -1 : 1;
    return 0;
}

unsigned EntityId_hash(const void *key) {
    const EntityId *id = (const EntityId*)key;
    return hash_uuid(*id);
}

