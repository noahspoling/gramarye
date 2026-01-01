# Gramarye Game Components

Documentation for game-specific components used in the main gramarye repository.

## Component Overview

The game uses components from gramarye-components and gramarye-component-functions. This document describes how components are used in the game.

## Registered Components

The game registers these component types in `GameSystem_create()`:

1. **Position** - 2D integer position (x, y)
2. **Health** - Health bar value (current, max)
3. **Sprite** - Sprite reference (atlas, tile_id)

## Position Component

**Type**: `Position` (from gramarye-components/core/position.h)

**Structure**:
```c
typedef struct {
    int x;
    int y;
} Position;
```

**Usage**:
- Player position tracking
- Entity positions in tilemap
- Camera following target

**Functions** (from gramarye-component-functions):
- `Position_add(ECS*, EntityId, ComponentTypeId, int x, int y)`
- `Position_get(ECS*, EntityId, ComponentTypeId) -> Position*`
- `Position_set(ECS*, EntityId, ComponentTypeId, int x, int y)`
- `Position_remove(ECS*, EntityId, ComponentTypeId)`

**Example**:
```c
Position_add(ecs, player, positionTypeId, startX, startY);
Position* pos = Position_get(ecs, player, positionTypeId);
Position_set(ecs, player, positionTypeId, newX, newY);
```

## Health Component

**Type**: `BarValue` (from gramarye-components/core/bar_value.h)

**Structure**:
```c
typedef struct {
    float value;
    float maxValue;
} BarValue;
```

**Usage**:
- Player health tracking
- Entity health values
- Health bar rendering (future)

**Functions** (from gramarye-component-functions):
- `Health_add(ECS*, EntityId, ComponentTypeId, float maxHealth)`
- `Health_get(ECS*, EntityId, ComponentTypeId) -> BarValue*`
- `Health_remove(ECS*, EntityId, ComponentTypeId)`

**Example**:
```c
Health_add(ecs, player, healthTypeId, 100.0f);
BarValue* health = Health_get(ecs, player, healthTypeId);
```

## Sprite Component

**Type**: `Sprite` (from gramarye-components/textures/sprite.h)

**Structure**:
```c
typedef struct {
    Atlas* atlas;
    int tile_id;
} Sprite;
```

**Usage**:
- Entity visual representation
- Sprite rendering
- Animation (via tile_id changes)

**Functions** (from gramarye-component-functions):
- `Sprite_add(ECS*, EntityId, ComponentTypeId, Atlas*, int tile_id)`
- `Sprite_get(ECS*, EntityId, ComponentTypeId) -> Sprite*`
- `Sprite_set_tile(ECS*, EntityId, ComponentTypeId, int tile_id)`
- `Sprite_remove(ECS*, EntityId, ComponentTypeId)`

**Example**:
```c
Sprite_add(ecs, player, spriteTypeId, atlas, 4);
Sprite* sprite = Sprite_get(ecs, player, spriteTypeId);
Sprite_set_tile(ecs, player, spriteTypeId, newTileId);
```

## Atlas

**Type**: `Atlas` (from gramarye-component-functions/textures/atlas.h)

**Structure**:
```c
struct Atlas {
    Texture2D texture;      // raylib texture
    Atlas_Table rects;      // Table of rectangles
    int rectCount;
    int capacity;
};
```

**Usage**:
- Texture atlas for sprites
- Tile rendering
- Sprite source rectangles

**Functions**:
- `Atlas_new(int initialCapacity) -> Atlas*`
- `Atlas_setTexture(Atlas*, char* path)`
- `Atlas_addRect(Atlas*, int index, Rectangle rect)`
- `Atlas_getRect(const Atlas*, int index) -> Rectangle`
- `Atlas_free(Atlas*)`

**Example**:
```c
Atlas* atlas = Atlas_new(400);
Atlas_setTexture(atlas, "resources/spritesheet-export.png");
Rectangle rect = {0.0f, 0.0f, 16.0f, 16.0f};
Atlas_addRect(atlas, 0, rect);
Rectangle sourceRect = Atlas_getRect(atlas, 0);
```

## Component Registration

Components are registered in `init_entities()`:

```c
s->positionTypeId = ECS_register_component_type(s->ecs, "Position", sizeof(Position));
s->healthTypeId = ECS_register_component_type(s->ecs, "Health", sizeof(BarValue));
s->spriteTypeId = ECS_register_component_type(s->ecs, "Sprite", sizeof(Sprite));
```

Component type IDs are stored in GameState for use throughout the game.

## Player Entity

The player entity is created with all three components:

```c
EntityId player = Entity_create(entityRegistry);

Position_add(ecs, player, positionTypeId, startX, startY);
Health_add(ecs, player, healthTypeId, 100.0f);
Sprite_add(ecs, player, spriteTypeId, atlas, 4);
```

## Adding New Components

To add a new component:

1. **Define Component Struct** (in gramarye-components if shared, or locally if game-specific)
2. **Add Component Functions** (in gramarye-component-functions if shared)
3. **Register Component Type** in `init_entities()`
4. **Store Type ID** in GameState
5. **Use Component** in systems

Example:

```c
// 1. Define struct (in gramarye-components or locally)
typedef struct {
    float speed;
} Velocity;

// 2. Add functions (in gramarye-component-functions or locally)
void Velocity_add(ECS* ecs, EntityId entity, ComponentTypeId typeId, float speed) {
    Velocity vel = {speed};
    ECS_add_component(ecs, entity, typeId, &vel);
}

// 3. Register in init_entities()
s->velocityTypeId = ECS_register_component_type(s->ecs, "Velocity", sizeof(Velocity));

// 4. Store in GameState
typedef struct GameState {
    // ... existing fields ...
    ComponentTypeId velocityTypeId;
} GameState;

// 5. Use in systems
Velocity* vel = (Velocity*)ECS_get_component(ecs, entity, velocityTypeId);
```

## Component Access Patterns

### Direct ECS Access

```c
Position* pos = (Position*)ECS_get_component(ecs, entity, positionTypeId);
```

### Component Function Access

```c
Position* pos = Position_get(ecs, entity, positionTypeId);
```

Prefer component functions when available - they provide a cleaner API and handle type checking.

## Component Relationships

Components can reference other game objects:

- **Sprite** references **Atlas** (pointer to atlas)
- Components can reference ECS entities (via EntityId)
- Components can reference game systems (via pointers in GameState)

Avoid circular dependencies and keep component structs simple (data only, no logic).

