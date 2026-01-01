# Gramarye Game Systems

Documentation for all game systems in the main gramarye repository.

## System Overview

The game uses a system-based architecture where each system handles a specific aspect of the game:

1. **GameSystem** - Main game loop and state management
2. **InputSystem** - Input polling and command queue
3. **MovementSystem** - Entity movement and collision
4. **CameraSystem** - Camera following and transformations
5. **TileEditSystem** - Tile placement and editing
6. **RenderSystem** - Rendering entities and UI
7. **ChunkManagerSystem** - Chunk dirty tracking (from gramarye-chunk-controller)
8. **ChunkRenderSystem** - Chunk rendering (from gramarye-chunk-renderer)

## GameSystem

**Location**: `src/systems/game_system.c`, `include/systems/game_system.h`

Main game system that coordinates all other systems and manages game state.

### Responsibilities

- Initialize all subsystems (ECS, renderer, chunk systems, event bus)
- Manage game loop frame updates
- Coordinate system execution order
- Handle input command processing

### Frame Update Order

The `GameSystem_frame()` function executes systems in this order:

1. **Poll Input**: `InputSystem_poll_and_publish()` - Poll input and generate commands
2. **Process Commands**: Drain input command queue, defer tile placement until after camera update
3. **Process Tile Updates**: `ChunkManagerSystem_process_updates()` - Apply queued tile updates, mark chunks dirty
4. **Update Chunk Renderer**: `ChunkRenderSystem_update()` - Load/unload chunks, render dirty chunks
5. **Update Camera**: `CameraSystem_follow_player()` - Follow player, compute aspect fit, clamp to bounds
6. **Apply Deferred Placement**: Apply tile placement with up-to-date camera
7. **Render**: `RenderSystem_render()` - Render chunks, entities, UI

### Initialization

```c
GameSystem* game = GameSystem_create(arena, MAP_SIZE, TILE_SIZE, logicalSize, renderer, inputProvider);
```

Initializes:
- Atlas and tilemap
- ECS with component types (Position, Health, Sprite)
- Player entity with components
- Event bus
- Tile update queue
- Chunk manager system
- Chunk render system
- Camera system
- Input system

## InputSystem

**Location**: `src/systems/input_system.c`, `include/systems/input_system.h`

Handles input polling and command generation.

### Features

- Polls input from InputProvider (backend-agnostic)
- Generates input commands (Move, Zoom, PlaceTile, ToggleDebug)
- Command queue for deferred processing
- Single-threaded (threading code is commented out for future use)

### Input Commands

- `Cmd_Move`: Player movement (dx, dy)
- `Cmd_Zoom`: Camera zoom (wheel delta)
- `Cmd_PlaceTile`: Tile placement at mouse position
- `Cmd_ToggleDebug`: Toggle debug mode

### Usage

```c
InputSystem* input = InputSystem_create(arena, inputProvider);
InputSystem_poll_and_publish(input);  // Poll and generate commands
InputCommand cmd;
while (InputSystem_pop(input, &cmd)) {
    // Process command
}
```

## MovementSystem

**Location**: `src/systems/movement_system.c`, `include/systems/movement_system.h`

Handles entity movement and tile-based collision.

### Features

- Discrete movement (one tile per step)
- Tile collision checking
- Boundary checking (map bounds)

### Movement Logic

1. Calculate new position (current + delta)
2. Check map bounds
3. Check tile walkability
4. Update position if valid

### Walkability

Currently all tiles are walkable. The `is_tile_walkable()` function can be extended to check tile properties.

## CameraSystem

**Location**: `src/systems/camera_system.c`, `include/systems/camera_system.h`

Manages camera following, zoom, and transformations.

### Features

- Follow player entity
- Zoom with mouse wheel
- Aspect fit computation
- Camera clamping to map bounds
- World-to-screen and screen-to-world transformations

### Camera Functions

- `CameraSystem_apply_zoom()`: Apply zoom delta from mouse wheel
- `CameraSystem_follow_player()`: Update camera to follow player position
- `CameraSystem_compute_fit()`: Compute aspect fit for current window size
- `CameraSystem_clamp()`: Clamp camera to stay within map bounds

### Camera Structure

```c
typedef struct {
    Vector2 pos;           // World position in pixels
    float zoom;            // Zoom factor (1.0 = 1:1)
    float minZoom;
    float maxZoom;
    float speed;           // Pan speed (unused currently)
    Vector2 logicalSize;   // Render texture logical size
} Camera2DEx;
```

## TileEditSystem

**Location**: `src/systems/tile_edit_system.c`, `include/systems/tile_edit_system.h`

Handles tile placement and editing.

### Features

- Convert screen click to tile coordinates
- Queue tile updates (deferred processing)
- Supports negative coordinates (infinite maps)

### Tile Placement Flow

1. Convert mouse screen position to world coordinates
2. Convert world coordinates to tile coordinates
3. Queue tile update command
4. ChunkManagerSystem processes queue and applies update

### Usage

```c
TileEditSystem_place_tile_at_mouse(state, aspectFit, mousePos);
```

This queues a tile update that will be processed in the next frame by ChunkManagerSystem.

## RenderSystem

**Location**: `src/systems/render_system.c`, `include/systems/render_system.h`

Handles rendering of entities and debug overlays.

### Rendering Order

1. **Chunk Rendering**: Render tilemap chunks via ChunkRenderSystem
2. **Debug Overlays**: Render debug visualizations (last click, etc.)
3. **Entity Rendering**: Render entities (player, NPCs, etc.)

### Entity Rendering

Entities are rendered by:
1. Getting Position and Sprite components
2. Converting world position to screen position using camera
3. Getting sprite source rectangle from Atlas
4. Drawing sprite using raylib (currently - should use renderer interface)

### Debug Rendering

- Last click visualization: Red rectangle at last clicked tile
- Only rendered when debug mode is enabled

## ChunkManagerSystem

**From**: gramarye-chunk-controller

Manages tile updates and chunk dirty tracking.

### Responsibilities

- Process tile update queue
- Apply updates to tilemap
- Mark affected chunks as dirty
- Publish events (EVENT_TILE_UPDATED, EVENT_CHUNK_DIRTY)

### Usage

```c
ChunkManagerSystem_process_updates(&state->chunkManager, &state->tileUpdateQueue);
```

Called once per frame to process all queued tile updates.

## ChunkRenderSystem

**From**: gramarye-chunk-renderer

Handles chunk-based rendering of tilemaps.

### Responsibilities

- Load/unload chunks based on observer positions
- Render dirty chunks to render textures
- Render visible chunks to screen
- Handle click-to-tile conversion

### Usage

```c
// Update (load/unload, render dirty chunks)
ChunkRenderSystem_update(&state->chunkRenderer, state->ecs, state->positionTypeId, &state->chunkManager);

// Render visible chunks
ChunkRenderSystem_render(&state->chunkRenderer, state->ecs, state->positionTypeId, &state->cam, &aspectFit);
```

## System Communication

### Event Bus

Systems communicate via gramarye-event-bus:

- **EVENT_TILE_UPDATED**: Published when a tile is updated
- **EVENT_CHUNK_DIRTY**: Published when a chunk is marked dirty

### Direct Function Calls

Most system communication is via direct function calls:

- GameSystem calls other systems in order
- Systems access GameState directly
- No message passing between systems (except events)

## Adding New Systems

To add a new system:

1. Create system header in `include/systems/`
2. Create system implementation in `src/systems/`
3. Add system update call in `GameSystem_frame()`
4. Document system in this README

Example:

```c
// include/systems/my_system.h
void MySystem_update(GameState* state, float dt);

// src/systems/my_system.c
void MySystem_update(GameState* state, float dt) {
    // System logic
}

// src/systems/game_system.c
void GameSystem_frame(GameSystem* g, float dt) {
    // ... existing systems ...
    MySystem_update(&g->state, dt);
}
```

