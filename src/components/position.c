#include "position.h"

void Position_add(ECS* ecs, EntityId entity, ComponentTypeId typeId, int x, int y) {
    Position pos;
    pos.x = x;
    pos.y = y;
    ECS_add_component(ecs, entity, typeId, &pos);
}

Position* Position_get(ECS* ecs, EntityId entity, ComponentTypeId typeId) {
    return (Position*)ECS_get_component(ecs, entity, typeId);
}

void Position_set(ECS* ecs, EntityId entity, ComponentTypeId typeId, int x, int y) {
    Position* pos = Position_get(ecs, entity, typeId);
    if (pos) {
        pos->x = x;
        pos->y = y;
    }
}

void Position_remove(ECS* ecs, EntityId entity, ComponentTypeId typeId) {
    ECS_remove_component(ecs, entity, typeId);
}

