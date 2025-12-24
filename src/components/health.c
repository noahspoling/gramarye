#include "health.h"
#include "mem.h"

HealthRegistry* HealthRegistry_new(Arena_T arena) {
    HealthRegistry* registry = (HealthRegistry*)Arena_alloc(arena, sizeof(HealthRegistry), __FILE__, __LINE__);
    registry->arena = arena;
    registry->healths = Table_new(1024, EntityId_cmp, EntityId_hash);
    return registry;
}

void Health_add(HealthRegistry* registry, EntityId entity, float maxHealth) {
    BarValue* health = (BarValue*)Arena_alloc(registry->arena, sizeof(BarValue), __FILE__, __LINE__);
    health->value = maxHealth;
    health->maxValue = maxHealth;
    
    EntityId* idKey = (EntityId*)Arena_alloc(registry->arena, sizeof(EntityId), __FILE__, __LINE__);
    *idKey = entity;
    
    Table_put(registry->healths, idKey, health);
}

BarValue* Health_get(HealthRegistry* registry, EntityId entity) {
    return (BarValue*)Table_get(registry->healths, &entity);
}

void Health_remove(HealthRegistry* registry, EntityId entity) {
    Table_remove(registry->healths, &entity);
}

