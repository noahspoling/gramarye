#ifndef SPRITE_H
#define SPRITE_H

#include "entity.h"
#include "arena.h"
#include "table.h"
#include "atlas.h"

typedef struct {
    Atlas* atlas;
    int tile_id;
} Sprite;

// Sprite component registry
typedef struct SpriteRegistry {
    Arena_T arena;
    Table_T sprites;  // EntityId -> Sprite*
} SpriteRegistry;

// Create a new sprite registry
SpriteRegistry* SpriteRegistry_new(Arena_T arena);

// Add sprite component to entity
void Sprite_add(SpriteRegistry* registry, EntityId entity, Atlas* atlas, int tile_id);

// Get sprite component for entity
Sprite* Sprite_get(SpriteRegistry* registry, EntityId entity);

// Set sprite tile_id
void Sprite_set_tile(SpriteRegistry* registry, EntityId entity, int tile_id);

// Remove sprite component from entity
void Sprite_remove(SpriteRegistry* registry, EntityId entity);

#endif // SPRITE_H

