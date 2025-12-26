#ifndef HEALTH_H
#define HEALTH_H

#include "gramarye_ecs/ecs.h"
#include "gramarye_ecs/entity.h"
#include "gramarye_ecs/component.h"
#include "bar_value.h"

// Add health component to entity
void Health_add(ECS* ecs, EntityId entity, ComponentTypeId typeId, float maxHealth);

// Get health component for entity
BarValue* Health_get(ECS* ecs, EntityId entity, ComponentTypeId typeId);

// Remove health component from entity
void Health_remove(ECS* ecs, EntityId entity, ComponentTypeId typeId);

#endif // HEALTH_H

