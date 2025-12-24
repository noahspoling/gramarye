#include "sprite.h"
#include "mem.h"

SpriteRegistry* SpriteRegistry_new(Arena_T arena) {
    SpriteRegistry* registry = (SpriteRegistry*)Arena_alloc(arena, sizeof(SpriteRegistry), __FILE__, __LINE__);
    registry->arena = arena;
    registry->sprites = Table_new(1024, EntityId_cmp, EntityId_hash);
    return registry;
}

void Sprite_add(SpriteRegistry* registry, EntityId entity, Atlas* atlas, int tile_id) {
    Sprite* sprite = (Sprite*)Arena_alloc(registry->arena, sizeof(Sprite), __FILE__, __LINE__);
    sprite->atlas = atlas;
    sprite->tile_id = tile_id;
    
    EntityId* idKey = (EntityId*)Arena_alloc(registry->arena, sizeof(EntityId), __FILE__, __LINE__);
    *idKey = entity;
    
    Table_put(registry->sprites, idKey, sprite);
}

Sprite* Sprite_get(SpriteRegistry* registry, EntityId entity) {
    return (Sprite*)Table_get(registry->sprites, &entity);
}

void Sprite_set_tile(SpriteRegistry* registry, EntityId entity, int tile_id) {
    Sprite* sprite = Sprite_get(registry, entity);
    if (sprite) {
        sprite->tile_id = tile_id;
    }
}

void Sprite_remove(SpriteRegistry* registry, EntityId entity) {
    Table_remove(registry->sprites, &entity);
}

