#include "health.h"

void Health_add(ECS* ecs, EntityId entity, ComponentTypeId typeId, float maxHealth) {
    BarValue health;
    health.value = maxHealth;
    health.maxValue = maxHealth;
    ECS_add_component(ecs, entity, typeId, &health);
}

BarValue* Health_get(ECS* ecs, EntityId entity, ComponentTypeId typeId) {
    return (BarValue*)ECS_get_component(ecs, entity, typeId);
}

void Health_remove(ECS* ecs, EntityId entity, ComponentTypeId typeId) {
    ECS_remove_component(ecs, entity, typeId);
}

